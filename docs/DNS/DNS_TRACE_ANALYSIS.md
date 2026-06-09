# DNS Resolution Trace Analysis: `build/vmrp mythroad/gghjt.mrp` → `rop.skymobiapp.com`

## Executive Summary

When executing `build/vmrp mythroad/gghjt.mrp`, the application makes a network request to `rop.skymobiapp.com`. The DNS resolution for this domain is intercepted and redirected to `127.0.0.1` via a built-in DNS mapping table. This document traces the complete flow from initialization through socket connection.

---

## 1. DEFAULT DNS MAPPING CONFIGURATION

### Location: `src/vmrp.c:28`
```c
#define VMRP_DEFAULT_DNS_MAP "wap.skmeg.com->159.75.119.124;rop.skymobiapp.com->127.0.0.1"
```

**Key Points:**
- `rop.skymobiapp.com` is mapped to `127.0.0.1` (localhost)
- This is the **default** mapping; can be overridden by:
  1. CLI argument: `--dns-map` (highest priority)
  2. Environment variable: `VMRP_DNS_MAP`
  3. Configuration API call: `vmrp_api_set_dns_map()`
  4. Default (lowest priority)

### Selection Priority: `src/vmrp.c:216-233`
```c
static const char *selectStartupDnsMap(const char *dns_map_arg, VmrpDnsMapSource *source) {
    if (dns_map_arg) {                    // CLI --dns-map
        *source = VMRP_DNS_MAP_SOURCE_CLI;
        return dns_map_arg;
    }
    if (startup_config.dns_map_set &&     // Config API
        startup_config.dns_map_source == VMRP_DNS_MAP_SOURCE_CONFIG) {
        *source = VMRP_DNS_MAP_SOURCE_CONFIG;
        return startup_config.dns_map;
    }
    const char *env_dns_map = getenv("VMRP_DNS_MAP");  // Environment variable
    if (env_dns_map != NULL) {
        *source = VMRP_DNS_MAP_SOURCE_ENV;
        return env_dns_map;
    }
    *source = VMRP_DNS_MAP_SOURCE_DEFAULT;
    return VMRP_DEFAULT_DNS_MAP;          // Hardcoded default
}
```

---

## 2. DNS MAP INITIALIZATION FLOW

### Step 1: VMRP Main Entry Point
**File:** `src/vmrp.c:235-282` (`prepareVmrpArgs()`)

1. Parses command-line arguments (lines 243-244)
2. Selects which DNS map to use based on priority (line 269)
3. Applies the DNS map (line 270)

### Step 2: Apply DNS Map Configuration
**File:** `src/vmrp.c:198-214`

```c
static int applyStartupDnsMap(const char *map, VmrpDnsMapSource source) {
    if (my_configureDnsMap(map) != MR_SUCCESS) return MR_FAILED;
    // ...stores the map in startup_config
    startup_config.dns_map_set = 1;
    startup_config.dns_map_source = source;
    return MR_SUCCESS;
}
```

Calls `my_configureDnsMap()` which is implemented in `network.c`.

### Step 3: Parse DNS Map String
**File:** `src/network.c:125-159` (`my_configureDnsMap()`)

```c
int32 my_configureDnsMap(const char* map) {
    // Splits by ';', ',', or '\n'
    // For each entry, calls parseDnsMapEntry()
}
```

For the default map: `"wap.skmeg.com->159.75.119.124;rop.skymobiapp.com->127.0.0.1"`

Results in DNS map table with 2 entries:
```
dnsMap[0]: original="wap.skmeg.com"     fake="159.75.119.124"
dnsMap[1]: original="rop.skymobiapp.com" fake="127.0.0.1"
```

### Step 4: Parse Individual DNS Map Entry
**File:** `src/network.c:90-123` (`parseDnsMapEntry()`)

```c
static int parseDnsMapEntry(char* entry) {
    // Expects format: "original->fake" or "original=fake"
    // Trims whitespace
    // Normalizes domain names (lowercase, no trailing dots)
    // Stores in dnsMap[] array (max 32 entries)
}
```

Normalization details (lines 73-88):
- Trims leading/trailing whitespace
- Removes trailing dots
- Converts to lowercase

---

## 3. RUNTIME DNS RESOLUTION FLOW

### When MRP Application Calls `mr_getHostByName()`

#### Call Chain:
```
MRP App Code
    ↓
ARM Instruction (SWI/Function Call)
    ↓
arm_ext_executor.c:1549 (Case 83 in SWI handler)
    ↓
mr_getHostByName() → mythroad/dsm.c:1030
    ↓
getHostByName() → mythroad/dsm.c:999
    ↓
dsmInFuncs->getHostByName() → native_dsm_funcs.c:178 (native_getHostByName)
    ↓
my_getHostByName() → network.c:536
    ↓
my_getHostByNameSync() → network.c:480
```

### Key Files in Call Chain:

| File | Line | Function | Purpose |
|------|------|----------|---------|
| `src/arm_ext_executor.c` | 1549 | SWI case 83 | Extracts hostname from r0, calls mr_getHostByName |
| `src/mythroad/dsm.c` | 1030 | `mr_getHostByName()` | MRP wrapper for ext-called DNS lookup |
| `src/mythroad/dsm.c` | 999 | `getHostByName()` | Common implementation for mythroad and ext calls |
| `src/native_dsm_funcs.c` | 178 | `native_getHostByName()` | Native implementation bridge |
| `src/network.c` | 536 | `my_getHostByName()` | Main DNS resolution entry point |
| `src/network.c` | 480 | `my_getHostByNameSync()` | **Core DNS mapping logic** |

---

## 4. CORE DNS MAPPING LOGIC

### File: `src/network.c:480-517`

```c
static int32 my_getHostByNameSync(const char* name) {
    int32 ret = MR_FAILED;
    char mappedName[VMRP_DNS_NAME_MAX + 1];
    
    // *** KEY STEP: Get mapped name from DNS table ***
    const char* lookupName = getDnsLookupName(name, mappedName, sizeof(mappedName));
    //                                         ^^^^
    //                                    Original name passed in
    
    // Then perform actual DNS resolution on the MAPPED name
    struct addrinfo *result, *res;
    printf("getaddrinfo of %s\n", lookupName);
    if (getaddrinfo(lookupName, NULL, NULL, &result) != 0) {
        printf("getaddrinfo failed!\n");
        return ret;
    }
    
    // Extract IPv4 address
    for (res = result; res; res = res->ai_next) {
        if (res->ai_family == AF_INET) {
            struct in_addr* addr = &((struct sockaddr_in*)res->ai_addr)->sin_addr;
            printf("--- IPv4 address: %s\n", inet_ntoa(*addr));
            ret = ntohl((*addr).s_addr);
            break;
        }
    }
    freeaddrinfo(result);
    return ret;
}
```

### DNS Lookup Mapping Function
**File:** `src/network.c:161-174` (`getDnsLookupName()`)

```c
static const char* getDnsLookupName(const char* name, char* mappedName, 
                                     size_t mappedNameSize) {
    char normalized[VMRP_DNS_NAME_MAX + 1];
    
    // 1. Normalize input name
    if (copyNormalizedDomain(normalized, sizeof(normalized), name) != MR_SUCCESS) {
        return name;  // Fall back to original
    }
    
    // 2. Search backwards through dnsMap[] for a match
    for (int i = dnsMapCount - 1; i >= 0; i--) {
        if (strcmp(dnsMap[i].original, normalized) == 0) {
            // 3. If found, return the fake/mapped name
            snprintf(mappedName, mappedNameSize, "%s", dnsMap[i].fake);
            printf("dns map: %s -> %s\n", name, mappedName);  // Debug output
            return mappedName;
        }
    }
    
    // 4. If no match, return original name
    return name;
}
```

---

## 5. CONCRETE EXAMPLE: `rop.skymobiapp.com`

### Input:
- MRP application calls: `mr_getHostByName("rop.skymobiapp.com", callback)`

### Processing:

1. **Normalization:**
   - Input: `"rop.skymobiapp.com"`
   - Normalized: `"rop.skymobiapp.com"` (already lowercase, no trailing dots)

2. **DNS Map Lookup:**
   - Search `dnsMap[]` table (backwards from index 1 to 0)
   - Check `dnsMap[1].original` = `"rop.skymobiapp.com"` ✓ **MATCH!**
   - Get mapped name: `dnsMap[1].fake` = `"127.0.0.1"`

3. **System DNS Resolution:**
   - Call `getaddrinfo("127.0.0.1", ...)`
   - Returns IPv4 address: `127.0.0.1` = `0x7F000001`

4. **Return Value:**
   - Return as 32-bit integer: `0x7F000001` (network byte order converted to host byte order)
   - This represents IPv4 address `127.0.0.1`

5. **Socket Connection:**
   - MRP app calls: `mr_connect(socket_handle, 0x7F000001, port, ...)`
   - Connection attempt to `127.0.0.1:port` instead of the original server

---

## 6. NETWORK SOCKET OPERATIONS

Once DNS returns an IP, the connection is made:

### File: `src/network.c:280-309` (`my_connect()`)

```c
int32 my_connect(int32 s, int32 ip, uint16 port, int32 type) {
    uIntMap* obj = uIntMap_search(&sockets, (uint32_t)s);
    mSocket* data = (mSocket*)obj->data;
    
    // ... handle CMWAP proxy mode ...
    
    if (type == MR_SOCKET_NONBLOCK) {
        // Async connection: create thread
        connectData_t* d = malloc(sizeof(connectData_t));
        d->s = data;
        d->ip = ip;           // ← 0x7F000001 for rop.skymobiapp.com
        d->port = port;
        int ret = pthread_create(&d->th, NULL, my_connectAsync, d);
        return MR_WAITING;
    }
    return my_connectSync(data->s, ip, port);
}
```

### Actual Connection: `src/network.c:243-257` (`my_connectSync()`)

```c
static int32 my_connectSync(SOCKET_T s, int32 ip, uint16 port) {
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(port);
    clientService.sin_addr.s_addr = htonl(ip);  // Convert 0x7F000001 → 127.0.0.1
    
    printf("my_connect('%s', %d)\n", inet_ntoa(clientService.sin_addr), port);
    // ^ Logs: "my_connect('127.0.0.1', port)"
    
    if (connect(s, (struct sockaddr*)&clientService, sizeof(clientService)) != 0) {
        printf("my_connect(0x%X) fail\n", ip);
        return MR_FAILED;
    }
    printf("my_connect(0x%X) suc\n", ip);  // 0x7F000001
    return MR_SUCCESS;
}
```

---

## 7. ASYNC DNS RESOLUTION (WITH CALLBACK)

If MRP app provides a callback function:

**File:** `src/network.c:536-559` (`my_getHostByName()`)

```c
int32 my_getHostByName(uc_engine* uc, const char* name, MR_GET_HOST_CB cb, 
                       void* userData) {
    if (cb != NULL) {
        // Async path: create thread
        getHostByNameAsyncData_t* data = malloc(sizeof(getHostByNameAsyncData_t));
        data->name = malloc(len + 1);
        strcpy(data->name, name);         // Store "rop.skymobiapp.com"
        data->cb = cb;
        data->userData = userData;
        data->uc = uc;
        
        int ret = pthread_create(&data->th, NULL, my_getHostByNameAsync, data);
        if (ret != 0) {
            return MR_FAILED;
        }
        return MR_WAITING;  // Tell app to wait for callback
    }
    // Sync path: return immediately
    return my_getHostByNameSync(name);
}
```

**File:** `src/network.c:520-528` (`my_getHostByNameAsync()`)

```c
static void* my_getHostByNameAsync(void* arg) {
    getHostByNameAsyncData_t* data = (getHostByNameAsyncData_t*)arg;
    int32 r = my_getHostByNameSync(data->name);  // Do DNS lookup
    printf("my_getHostByNameAsync(): 0x%X\n", r);
    bridge_dsm_network_cb(data->uc, (uint32_t)data->cb, r, (uint32_t)data->userData);
    // Invokes callback with resolved IP (0x7F000001)
    free(data->name);
    free(data);
    return NULL;
}
```

---

## 8. CMWAP PROXY MODE SPECIAL HANDLING

**File:** `src/network.c:618-635` (`my_send()` in CMWAP mode)

When `isCMWAP = TRUE` (from `my_initNetwork()` with mode="cmwap"):

```c
if (isCMWAP) {  // CMWAP proxy mode
    if (data->realState == MR_WAITING) {
        if (data->sendCounter == 1) {  // First data send
            char tmp[256];
            char host[256];
            uint16_t port;
            
            my_readLine((char*)buf, tmp, sizeof(tmp));
            // Extract "host:port" from HTTP CONNECT header
            if (parseHostPort(tmp, host, sizeof(host), &port) == MR_FAILED) {
                return MR_FAILED;
            }
            
            // *** DNS lookup happens HERE in CMWAP mode ***
            int32 ip = my_getHostByNameSync(host);
            // ^ Resolves "rop.skymobiapp.com" → 0x7F000001
            
            if (ip == MR_FAILED) {
                return MR_FAILED;
            }
            
            // Connect to the resolved IP
            if (my_connect(s, ip, port, MR_SOCKET_NONBLOCK) == MR_FAILED) {
                return MR_FAILED;
            }
        }
        return 0;  // Waiting for real connection
    }
}
```

---

## 9. DATA STRUCTURES

### DNS Map Entry: `src/network.c:56-59`

```c
typedef struct {
    char original[VMRP_DNS_NAME_MAX + 1];  // "rop.skymobiapp.com"
    char fake[VMRP_DNS_NAME_MAX + 1];      // "127.0.0.1"
} DnsMapEntry;

static DnsMapEntry dnsMap[VMRP_DNS_MAP_MAX];  // Array of up to 32 entries
static int dnsMapCount = 0;
```

### Socket Structure: `src/network.c:42-48`

```c
typedef struct {
    SOCKET_T s;                    // Actual socket descriptor
    uint32_t sendCounter;          // Number of sends on this socket
    int32_t realState;             // Real connection state
    int32_t state;                 // Apparent state (CMWAP mode may differ)
    int32_t cmwapProxyAck;         // Flag for CMWAP proxy response
} mSocket;

static struct rb_root sockets = RB_ROOT;  // Red-black tree of all sockets
```

### Network Callback Data (Async): `src/network.c:398-404`

```c
typedef struct {
    MR_INIT_NETWORK_CB cb;
    void* userData;
    uc_engine* uc;
    pthread_t th;
} initNetworkAsyncData_t;
```

And for DNS:

```c
typedef struct {
    char* name;                    // "rop.skymobiapp.com"
    MR_GET_HOST_CB cb;             // Callback function pointer
    void* userData;                // User data passed to callback
    uc_engine* uc;                 // Unicorn engine context
    pthread_t th;                  // Thread for async resolution
} getHostByNameAsyncData_t;        // lines 471-477
```

---

## 10. COMPLETE CALL CHAIN DIAGRAM

```
┌─────────────────────────────────────────────────────────┐
│ MRP App Bytecode                                         │
│ (mr_getHostByName("rop.skymobiapp.com", callback))      │
└────────────┬────────────────────────────────────────────┘
             │ ARM SWI Instruction #83
             ↓
┌────────────────────────────────────────────────────────┐
│ arm_ext_executor.c:1549 (SWI handler case 83)          │
│ - Extracts hostname from r0                             │
│ - Extracts callback from r1                             │
│ - Calls: mr_getHostByName(host, cb)                     │
└────────────┬────────────────────────────────────────────┘
             │
             ↓
┌────────────────────────────────────────────────────────┐
│ mythroad/dsm.c:1030 (mr_getHostByName)                 │
│ - Delegates to getHostByName(ptr, cb, isExtCB=1)       │
└────────────┬────────────────────────────────────────────┘
             │
             ↓
┌────────────────────────────────────────────────────────┐
│ mythroad/dsm.c:999 (getHostByName)                     │
│ - Allocates networkData_st                              │
│ - Calls: dsmInFuncs->getHostByName()                    │
└────────────┬────────────────────────────────────────────┘
             │
             ↓
┌────────────────────────────────────────────────────────┐
│ native_dsm_funcs.c:178 (native_getHostByName)          │
│ - Calls: my_getHostByName(NULL, name, cb, userData)    │
└────────────┬────────────────────────────────────────────┘
             │
             ↓
┌────────────────────────────────────────────────────────┐
│ network.c:536 (my_getHostByName)                       │
│ - If callback: create thread for async resolution       │
│ - Else: call my_getHostByNameSync()                    │
└────────────┬────────────────────────────────────────────┘
             │
      ┌──────┴──────┐
      │             │
      ↓ (async)     ↓ (sync)
┌──────────────┐   ┌──────────────────────────────────────┐
│ Create       │   │ network.c:480                         │
│ pthread for  │   │ (my_getHostByNameSync)                │
│ async        │   │ **CORE DNS MAPPING FUNCTION**         │
│ resolution   │   │                                       │
└──────────────┘   │ 1. Call getDnsLookupName()            │
                   │    Input: "rop.skymobiapp.com"        │
                   │    Output: "127.0.0.1" (from dnsMap)  │
                   │                                       │
                   │ 2. Call getaddrinfo("127.0.0.1")      │
                   │    Returns: struct addrinfo *         │
                   │                                       │
                   │ 3. Extract IPv4 address               │
                   │    Return: 0x7F000001 (127.0.0.1)     │
                   └──────────────────────────────────────┘
                             │
                             ↓
                   ┌──────────────────────────────────────┐
                   │ Return 0x7F000001 to callback/app     │
                   │ or invoke callback thread             │
                   └──────────────────────────────────────┘
```

---

## 11. CONFIGURATION AND OVERRIDE MECHANISMS

### 1. **Default (Hardcoded)**
- Location: `src/vmrp.c:28`
- Value: `"wap.skmeg.com->159.75.119.124;rop.skymobiapp.com->127.0.0.1"`

### 2. **Environment Variable Override**
```bash
export VMRP_DNS_MAP="rop.skymobiapp.com->127.0.0.1;other.com->1.2.3.4"
vmrp mythroad/gghjt.mrp
```
- Selected in: `src/vmrp.c:226-230`

### 3. **Command-Line Override (Highest Priority)**
```bash
vmrp --dns-map "rop.skymobiapp.com->192.168.1.1" mythroad/gghjt.mrp
```
- Parsed in: `src/vmrp.c:158-164`
- Priority check: `src/vmrp.c:217-219`

### 4. **API Call Override**
```c
vmrp_api_set_dns_map("rop.skymobiapp.com->10.0.0.1");
```
- Function: `src/vmrp.c:212-214`
- API header: `src/include/vmrp_api.h`

---

## 12. KEY IMPLEMENTATION DETAILS

### DNS Normalization
**File:** `src/network.c:73-88`

```c
static int copyNormalizedDomain(char* dst, size_t dstSize, const char* src) {
    // 1. Skip leading whitespace
    // 2. Convert to lowercase
    // 3. Remove trailing whitespace and dots
    // 4. Fail if any whitespace in middle or if ≥dstSize
}
```

### Constants
- `VMRP_DNS_MAP_MAX = 32` (max entries in DNS map)
- `VMRP_DNS_NAME_MAX = 255` (max hostname length)
- `VMRP_DNS_MAP_LIMIT = 2048` (max string length for entire map)

### Platform-Specific
- Uses `getaddrinfo()` for IPv4/IPv6 resolution (modern approach)
- Also has fallback to `gethostbyname()` (commented out, lines 504-514)
- CMWAP proxy mode special case (lines 284-289)

---

## 13. SUMMARY TABLE

| Component | File | Line | Purpose |
|-----------|------|------|---------|
| Default DNS map | `vmrp.c` | 28 | Hardcoded default with `rop.skymobiapp.com->127.0.0.1` |
| Parse arguments | `vmrp.c` | 133-186 | Handle `--dns-map` CLI argument |
| Select DNS map | `vmrp.c` | 216-233 | Priority: CLI > Config > Env > Default |
| Apply DNS map | `vmrp.c` | 198-214 | Call `my_configureDnsMap()` |
| Configure DNS | `network.c` | 125-159 | Parse map string, split by `;,\n` |
| Parse entry | `network.c` | 90-123 | Parse `original->fake`, normalize domain |
| Lookup mapping | `network.c` | 161-174 | Search dnsMap[], return mapped name |
| DNS resolve (sync) | `network.c` | 480-517 | Core: map name → call getaddrinfo → return IP |
| DNS resolve (async) | `network.c` | 536-559 | Create thread for DNS resolution |
| Socket connect | `network.c` | 280-309 | Connect to resolved IP address |
| ARM SWI handler | `arm_ext_executor.c` | 1549 | Extract hostname, call mr_getHostByName |
| MRP wrapper | `mythroad/dsm.c` | 1025-1031 | mr_getHostByName, mythroad_getHostByName |
| Native bridge | `native_dsm_funcs.c` | 178-180 | native_getHostByName → my_getHostByName |

---

## 14. VERIFICATION & TESTING

### To trace DNS resolution in action:
```bash
# Enable verbose output (already in code)
vmrp --dns-map "rop.skymobiapp.com->127.0.0.1" mythroad/gghjt.mrp
```

### Expected console output:
```
getaddrinfo of 127.0.0.1
dns map: rop.skymobiapp.com -> 127.0.0.1
--- IPv4 address: 127.0.0.1
my_getHostByNameSync(): 0x7F000001
my_connect('127.0.0.1', <port>)
```

### To change the mapping:
```bash
# Use environment variable
VMRP_DNS_MAP="rop.skymobiapp.com->10.0.0.5" vmrp mythroad/gghjt.mrp

# Or CLI argument (highest priority)
vmrp --dns-map "rop.skymobiapp.com->10.0.0.5" mythroad/gghjt.mrp
```

---

## CONCLUSION

The VMRP emulator intercepts DNS resolution for `rop.skymobiapp.com` through a built-in DNS mapping table. By default, it's mapped to `127.0.0.1`. The resolution happens in `my_getHostByNameSync()` in `network.c`, which:

1. **Maps** the original hostname using `getDnsLookupName()`
2. **Resolves** the mapped hostname using `getaddrinfo()`
3. **Returns** the resolved IP address

This allows the emulator to redirect network traffic from the MRP application to localhost or any other configured address, enabling local testing and HTTP interception.
