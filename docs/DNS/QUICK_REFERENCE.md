# DNS Resolution Quick Reference - VMRP MRP Emulator

## The Answer in 30 Seconds

When `build/vmrp mythroad/gghjt.mrp` tries to access `rop.skymobiapp.com`:

1. **DNS is intercepted** by a hardcoded mapping table
2. **`rop.skymobiapp.com` → `127.0.0.1`** (localhost)
3. **Network call redirects** to localhost instead of the real server

---

## Key Locations

| What | Where | Line(s) |
|------|-------|---------|
| 🎯 **Mapping definition** | `src/vmrp.c` | 28 |
| 💡 **Core DNS logic** | `src/network.c` | 480-517 |
| 🔍 **Domain lookup** | `src/network.c` | 161-174 |
| 🔌 **Socket connect** | `src/network.c` | 243-257 |
| 🛠️ **Configuration** | `src/network.c` | 125-159 |

---

## Default DNS Map

```c
#define VMRP_DEFAULT_DNS_MAP "wap.skmeg.com->159.75.119.124;rop.skymobiapp.com->127.0.0.1"
```

Location: `src/vmrp.c:28`

**What it means:**
- `rop.skymobiapp.com` → `127.0.0.1`
- Queries to the real server go to localhost instead

---

## How It Works

```
MRP App: mr_getHostByName("rop.skymobiapp.com")
    ↓
Network Layer: getDnsLookupName("rop.skymobiapp.com")
    ↓
DNS Map Lookup: dnsMap[] → "127.0.0.1"
    ↓
System Call: getaddrinfo("127.0.0.1")
    ↓
Return: IP address 0x7F000001 (127.0.0.1)
    ↓
Socket Connect: connect(socket, 127.0.0.1:port)
```

---

## Core Function: `my_getHostByNameSync()`

**File:** `src/network.c:480-517`

```c
static int32 my_getHostByNameSync(const char* name) {
    // 1. Map hostname using DNS table
    const char* lookupName = getDnsLookupName(name, mappedName, sizeof(mappedName));
    // Returns: "127.0.0.1" if input was "rop.skymobiapp.com"
    
    // 2. Resolve mapped hostname
    getaddrinfo(lookupName, NULL, NULL, &result);
    
    // 3. Extract and return IP as 32-bit integer
    ret = ntohl((*addr).s_addr);
    return ret;  // Returns: 0x7F000001
}
```

---

## Call Chain

```
arm_ext_executor.c:1549 (SWI case 83)
    ↓
mythroad/dsm.c:1030 (mr_getHostByName)
    ↓
mythroad/dsm.c:999 (getHostByName)
    ↓
native_dsm_funcs.c:178 (native_getHostByName)
    ↓
network.c:536 (my_getHostByName)
    ↓
network.c:480 (my_getHostByNameSync) ← **DNS mapping happens here**
```

---

## DNS Map Configuration Priority

1. **CLI argument** (highest)
   ```bash
   vmrp --dns-map "rop.skymobiapp.com->10.0.0.5" mythroad/gghjt.mrp
   ```

2. **Configuration API**
   ```c
   vmrp_api_set_dns_map("rop.skymobiapp.com->10.0.0.5");
   ```

3. **Environment variable**
   ```bash
   export VMRP_DNS_MAP="rop.skymobiapp.com->10.0.0.5"
   ```

4. **Hardcoded default** (lowest)
   ```c
   #define VMRP_DEFAULT_DNS_MAP "rop.skymobiapp.com->127.0.0.1"
   ```

---

## DNS Resolution Example

**Input:** `mr_getHostByName("rop.skymobiapp.com", callback)`

**Process:**
1. Normalize: `"rop.skymobiapp.com"` → `"rop.skymobiapp.com"` (lowercase)
2. Search dnsMap[]
3. Find match: `dnsMap[1].original = "rop.skymobiapp.com"` ✓
4. Get mapped: `dnsMap[1].fake = "127.0.0.1"`
5. Call: `getaddrinfo("127.0.0.1")`
6. Return: `0x7F000001` (127.0.0.1 as 32-bit integer)

**Result:** Connection goes to localhost, not the real server

---

## Debug Output

When resolving DNS, you'll see:

```
dns map: rop.skymobiapp.com -> 127.0.0.1
getaddrinfo of 127.0.0.1
--- IPv4 address: 127.0.0.1
my_connect('127.0.0.1', 80)
```

---

## Key Data Structures

### DNS Mapping Table
```c
typedef struct {
    char original[256];  // e.g., "rop.skymobiapp.com"
    char fake[256];      // e.g., "127.0.0.1"
} DnsMapEntry;

static DnsMapEntry dnsMap[32];  // Max 32 entries
static int dnsMapCount = 0;
```

### Socket State
```c
typedef struct {
    SOCKET_T s;           // Real socket descriptor
    int32_t state;        // Connection state
    int32_t realState;    // Actual connection state
} mSocket;
```

---

## Important Files

| File | Purpose |
|------|---------|
| `src/vmrp.c` | DNS map definition & startup configuration |
| `src/network.c` | Core DNS & socket implementation |
| `src/arm_ext_executor.c` | ARM SWI handler for DNS calls |
| `src/native_dsm_funcs.c` | Native function bridge |
| `src/mythroad/dsm.c` | MRP wrapper functions |

---

## Functions at a Glance

| Function | File | Purpose |
|----------|------|---------|
| `my_configureDnsMap()` | network.c:125 | Parse DNS map string |
| `parseDnsMapEntry()` | network.c:90 | Parse individual entries |
| `getDnsLookupName()` | network.c:161 | **Lookup domain in table** |
| `my_getHostByNameSync()` | network.c:480 | **Core DNS resolution** |
| `my_getHostByName()` | network.c:536 | Entry point (sync/async) |
| `my_connect()` | network.c:280 | Connect to resolved IP |
| `my_connectSync()` | network.c:243 | Actual socket connection |

---

## Key Constants

```c
#define VMRP_DNS_MAP_MAX 32       // Max DNS entries
#define VMRP_DNS_NAME_MAX 255     // Max hostname length
#define VMRP_DNS_MAP_LIMIT 2048   // Max map string length
```

---

## How to Change the Mapping

### Option 1: CLI (Highest Priority)
```bash
vmrp --dns-map "rop.skymobiapp.com->192.168.1.100" mythroad/gghjt.mrp
```

### Option 2: Environment Variable
```bash
export VMRP_DNS_MAP="rop.skymobiapp.com->192.168.1.100"
vmrp mythroad/gghjt.mrp
```

### Option 3: API Call
```c
vmrp_api_set_dns_map("rop.skymobiapp.com->192.168.1.100");
```

---

## What It's NOT

- ❌ No custom DNS server
- ❌ No DNS protocol implementation  
- ❌ No network interception
- ❌ No packet sniffing

It's just a **hostname→IP lookup table** that redirects network traffic to localhost or a configured address.

---

## Summary

| Aspect | Answer |
|--------|--------|
| **Domain** | `rop.skymobiapp.com` |
| **Resolves to** | `127.0.0.1` (by default) |
| **Mechanism** | In-memory DNS mapping table |
| **Location** | `src/vmrp.c:28` (definition), `src/network.c:480` (resolution) |
| **Async?** | Yes (with optional callback) |
| **Customizable?** | Yes (CLI, env, API) |
| **Uses standard DNS?** | No, internal mapping + system `getaddrinfo()` |

