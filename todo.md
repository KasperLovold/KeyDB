# funnydb Development Roadmap

---

## Phase 1: Core Functionality (Done)
- [x] TCP server with socket handling
- [x] Thread pool for concurrency
- [x] Command parser
- [x] Basic commands: `GET`, `SET`

---

## Phase 2: Essential Features

### Command Extensions
- [X] `DEL <key>` – delete a key
- [X] `EXISTS <key>` – check if key exists
- [X] `KEYS` – list all keys
- [X] `FLUSHALL` – delete all keys
- [X] `INCR <key>` – increment integer values
- [ ] `PING` – health check response (`PONG`)

---

### Expiry Support
- [ ] `SET mykey value EX 10` – expire after 10 seconds
- [ ] Lazy delete on access if expired
- [ ] Background cleanup thread

---

### RESP Protocol (Redis Protocol)
- [ ] Parse RESP commands (arrays, bulk strings)
- [ ] Send responses in RESP format:
    - Simple strings: `+OK\r\n`
    - Errors: `-ERR <msg>\r\n`
    - Bulk strings: `$6\r\nfoobar\r\n`
    - Arrays: `*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n`

🔗 [RESP Spec](https://redis.io/docs/reference/protocol-spec/)

---

## Phase 3: Persistence

### Data Persistence
- [ ] Periodic snapshot (RDB-like)
- [ ] Append-only log (AOF-style)
- [ ] Load data on startup

---

## Phase 4: Advanced Networking

- [ ] Keep connections open (reusable clients)
- [ ] Command pipelining (batching)
- [ ] Handle malformed requests gracefully
- [ ] Implement `AUTH <password>` command

---

## Phase 5: Monitoring & CLI

- [ ] `INFO` command – return server metrics (uptime, command count, memory)
- [ ] CLI tool: `keycli SET mykey value`
- [ ] Logging system for diagnostics

---

## Phase 6: Data Types

- [ ] `HSET` / `HGET` – Hash maps
- [ ] `LPUSH` / `LPOP` – Lists
- [ ] `SADD` / `SMEMBERS` – Sets
- [ ] `ZADD` / `ZRANGE` – Sorted sets

---

## Phase 7: Deployment

- [ ] Configurable via CLI args or config file (port, thread count, etc.)
- [ ] Build & package as binary (`funnydb`)
- [ ] Dockerfile for containerization