// tool/skymobi-pay-server.go
//
// netpay.mrp（skymobi 付费 SDK）在启动时会向 rop.skymobiapp.com:80 发起
// HTTP POST /payOneAsTlv，body 是一段 TLV（Type-Length-Value）二进制，携带
// 设备信息(IMEI/IMSI)、应用信息(包名/版本)、时间戳和 MD5 签名，用于校验
// 付费/授权。真机上服务器返回校验结果，netpay 据此放行游戏。该服务器已下线，
// gxdzc 等游戏因此卡在等待回复处（见 docs 分析）。
//
// 本程序是一个本地假服务器。DNS 解析已在模拟器内部完成重定向（见
// src/network.c 的 my_getHostByNameSync：rop.skymobiapp.com -> 127.0.0.1，
// 无需改 /etc/hosts），netpay 的请求会直接落到本机。服务器做两件事：
//   1) 完整解析并打印请求的 HTTP 头与 TLV body —— 用于逆向 netpay 实际发送
//      的字段，进而推导出能让它放行的"成功"响应格式。
//   2) 返回一段可配置的 TLV 响应（默认是 best-effort 的"成功"占位，需要根据
//      netpay 对响应的解析行为继续打磨）。
//
// 用法：
//   go run tool/skymobi-pay-server.go
//   sudo PORT=80 go run tool/skymobi-pay-server.go
//   # netpay 写死连 80 端口。若不想用 root，可改连别的端口：先用
//   #   VMRP_PAY_HOST=127.0.0.1 启动 vmrp（IP 重定向），再配合端口转发，
//   #   例如  socat TCP-LISTEN:80,fork,reuseaddr TCP:127.0.0.1:8080
//   #   然后  PORT=8080 go run tool/skymobi-pay-server.go
//
// 环境变量：
//   PORT            监听端口，默认 80（skymobi 请求里写死的是 80 端口）
//   RESP_TLV_FILE   指定一个文件，其内容作为响应 body 原样返回（覆盖默认响应）
//   RESP_HTTP_FILE  指定一个文件，其内容作为完整 HTTP 响应原样返回（连头部）
//   LOG_RAW         设为 "1" 时额外打印每个请求收到的原始 body hex

package main

import (
	"encoding/binary"
	"encoding/hex"
	"fmt"
	"io"
	"net"
	"net/http"
	"os"
	"strings"
	"sync/atomic"
)

var connSeq uint64

// ---------- TLV 编解码工具 ----------
// skymobi payOneAsTlv 记录格式：4 字节 type(大端) + 4 字节 length(大端) +
// length 字节 value。

type tlvRecord struct {
	Type  uint32
	Len   uint32
	Value []byte
}

func u32be(v uint32) []byte {
	b := make([]byte, 4)
	binary.BigEndian.PutUint32(b, v)
	return b
}

func tlvEncode(typ uint32, value []byte) []byte {
	buf := make([]byte, 0, 8+len(value))
	buf = append(buf, u32be(typ)...)
	buf = append(buf, u32be(uint32(len(value)))...)
	buf = append(buf, value...)
	return buf
}

// scanTlv 在 body 里做容错 TLV 扫描（选记录数最多的对齐方式），纯日志用途。
func scanTlv(body []byte) (start int, records []tlvRecord, end int) {
	bestStart := -1
	var bestRecords []tlvRecord
	bestEnd := 0

	limit := len(body)
	if limit > 64 {
		limit = 64
	}

	for s := 0; s < limit; s++ {
		var recs []tlvRecord
		off := s
		for off+8 <= len(body) {
			typ := binary.BigEndian.Uint32(body[off:])
			length := binary.BigEndian.Uint32(body[off+4:])
			if int(length) > len(body)-(off+8) {
				break
			}
			val := make([]byte, length)
			copy(val, body[off+8:off+8+int(length)])
			recs = append(recs, tlvRecord{Type: typ, Len: length, Value: val})
			off += 8 + int(length)
		}

		reachedEnd := off == len(body)
		var better bool
		if reachedEnd {
			better = bestEnd != len(body) || len(recs) > len(bestRecords)
		} else {
			better = bestEnd != len(body) && len(recs) > len(bestRecords)
		}
		if better {
			bestStart = s
			bestRecords = recs
			bestEnd = off
		}
	}
	return bestStart, bestRecords, bestEnd
}

func preview(data []byte, max int) string {
	n := len(data)
	if n > max {
		n = max
	}
	slice := data[:n]

	// hex
	hexStr := hex.EncodeToString(slice)
	var hexParts []string
	for i := 0; i < len(hexStr); i += 2 {
		end := i + 2
		if end > len(hexStr) {
			end = len(hexStr)
		}
		hexParts = append(hexParts, hexStr[i:end])
	}
	hexOut := strings.Join(hexParts, " ")
	if len(data) > max {
		hexOut += " …"
	}

	// ascii
	var ascii []byte
	for _, b := range slice {
		if b >= 0x20 && b <= 0x7e {
			ascii = append(ascii, b)
		} else {
			ascii = append(ascii, '.')
		}
	}

	return fmt.Sprintf("%s  |%s|", hexOut, string(ascii))
}

func toAsciiSafe(data []byte) string {
	var sb strings.Builder
	for _, b := range data {
		if b >= 0x20 && b <= 0x7e {
			sb.WriteByte(b)
		} else {
			sb.WriteByte('.')
		}
	}
	return sb.String()
}

// ---------- 默认响应 ----------

func buildDefaultBody() []byte {
	if f := os.Getenv("RESP_TLV_FILE"); f != "" {
		data, err := os.ReadFile(f)
		if err != nil {
			fmt.Fprintf(os.Stderr, "读取 RESP_TLV_FILE(%s) 失败: %v\n", f, err)
			os.Exit(1)
		}
		return data
	}
	// 占位"成功"响应：result(0x044f?)=0 等，后续按 netpay 解析行为修订
	var body []byte
	body = append(body, tlvEncode(0x03f1, []byte("000000006"))...)
	body = append(body, tlvEncode(0x044f, []byte{0x00, 0x00, 0x00, 0x00})...)
	return body
}

// ---------- HTTP handler ----------

func handler(w http.ResponseWriter, r *http.Request) {
	id := atomic.AddUint64(&connSeq, 1)
	peer := r.RemoteAddr
	fmt.Printf("\n[#%d] connection from %s\n", id, peer)
	fmt.Printf("[#%d] %s %s HTTP/%d.%d\n", id, r.Method, r.URL.RequestURI(), r.ProtoMajor, r.ProtoMinor)
	for k, vs := range r.Header {
		for _, v := range vs {
			fmt.Printf("[#%d]   %s: %s\n", id, k, v)
		}
	}

	body, err := io.ReadAll(r.Body)
	if err != nil {
		fmt.Printf("[#%d] read body error: %v\n", id, err)
		http.Error(w, "read error", 500)
		return
	}

	if os.Getenv("LOG_RAW") == "1" {
		fmt.Printf("[#%d] raw body %dB: %s\n", id, len(body), preview(body, 96))
	}
	fmt.Printf("[#%d] body %dB: %s\n", id, len(body), preview(body, 96))

	// 解析并打印 TLV body（容错扫描）
	start, records, end := scanTlv(body)
	if len(records) > 0 {
		trailing := len(body) - end
		fmt.Printf("[#%d] TLV stream @body+%d..%d (%d records, %dB trailing):\n",
			id, start, end, len(records), trailing)
		for _, rec := range records {
			fmt.Printf("[#%d]   type=0x%04x len=%d value=%s  |%s|\n",
				id, rec.Type, rec.Len, hex.EncodeToString(rec.Value), toAsciiSafe(rec.Value))
		}
	} else {
		fmt.Printf("[#%d] (no TLV stream found — full body hex)\n", id)
		fmt.Println(hex.EncodeToString(body))
	}

	// RESP_HTTP_FILE: 直接用原始 socket 写完整 HTTP 响应（绕过 http 模块的头部处理）
	if f := os.Getenv("RESP_HTTP_FILE"); f != "" {
		raw, err := os.ReadFile(f)
		if err != nil {
			fmt.Printf("[#%d] read RESP_HTTP_FILE error: %v\n", id, err)
			http.Error(w, "file error", 500)
			return
		}
		fmt.Printf("[#%d] -> reply (raw file) %dB\n", id, len(raw))
		// hijack 连接直接写原始字节
		hj, ok := w.(http.Hijacker)
		if !ok {
			http.Error(w, "hijack not supported", 500)
			return
		}
		conn, buf, err := hj.Hijack()
		if err != nil {
			fmt.Printf("[#%d] hijack error: %v\n", id, err)
			return
		}
		buf.Write(raw)
		buf.Flush()
		conn.Close()
		return
	}

	respBody := buildDefaultBody()
	fmt.Printf("[#%d] -> reply %dB body\n", id, len(respBody))
	w.Header().Set("Content-Type", "application/x-tar")
	w.Header().Set("Connection", "close")
	w.WriteHeader(200)
	w.Write(respBody)
}

func main() {
	port := os.Getenv("PORT")
	if port == "" {
		port = "80"
	}

	addr := "0.0.0.0:" + port

	fmt.Printf("skymobi fake pay-server listening on %s\n", addr)
	fmt.Println("DNS 已在 src/network.c 内重定向 rop.skymobiapp.com -> 127.0.0.1，")
	fmt.Println("直接启动 vmrp 即可，netpay 的 POST /payOneAsTlv 会落到这里。")

	listener, err := net.Listen("tcp", addr)
	if err != nil {
		if isPermissionError(err) {
			fmt.Fprintf(os.Stderr, "监听 %s 失败：端口 <1024 需要 root。请用  sudo PORT=%s go run tool/skymobi-pay-server.go\n", port, port)
		} else if isAddrInUse(err) {
			fmt.Fprintf(os.Stderr, "端口 %s 已被占用。\n", port)
		} else {
			fmt.Fprintf(os.Stderr, "server error: %v\n", err)
		}
		os.Exit(1)
	}

	server := &http.Server{Handler: http.HandlerFunc(handler)}
	if err := server.Serve(listener); err != nil {
		fmt.Fprintf(os.Stderr, "server error: %v\n", err)
		os.Exit(1)
	}
}

func isPermissionError(err error) bool {
	return strings.Contains(err.Error(), "permission denied")
}

func isAddrInUse(err error) bool {
	return strings.Contains(err.Error(), "address already in use")
}
