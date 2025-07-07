# 📘 internal-order-book

A modern **C++17 backend matching engine** designed to process **market and limit orders** from internal hedge fund strategies. Built with **lock-free, memory-safe design**, exposed via **gRPC**, and engineered for performance. This project simulates real-world order matching in latency-sensitive environments.

> ✅ No external database required — current version uses in-memory storage and file-based persistence stubs.

---

## 🚀 Features

- **C++17** with strong RAII and object-oriented architecture
- **Lock-free ingestion** using `boost::lockfree::queue`
- **gRPC API** interface for submitting orders and fetching system stats
- **Single-threaded matching engine** ensures determinism and low contention
- **Memory-safe queueing** using `std::unique_ptr` for ownership transfer
- **Metrics tracking**: orders/sec, peak throughput, uptime
- **CLI and signal-based lifecycle management**
- **CMake-based** build system
- **Google Test** for unit testing
- **File-based persistence** stubbed for future trade logging

---

## 🛠️ Build

```bash
mkdir build
cd build
cmake ..
make
```

---

## ▶️ Run

```bash
./internal-order-book
```

To run the gRPC server with a custom port or host:

```bash
./internal-order-book -p 8080 -h localhost
```

---

## 🧪 Test

```bash
cd build
ctest
```

---

## 🧱 Project Structure

```
.
├── include/             # Public headers
├── src/                 # Matching engine, gRPC service, and core logic
├── test/                # Google Test unit tests
├── protos/              # gRPC proto definitions (e.g. orderbook_service.proto)
├── lib/                 # Optional third-party dependencies
└── build/               # CMake output
```

---

## 📡 gRPC Endpoints

- `SubmitOrder`: Submit market or limit orders
- `HealthCheck`: Check service status and uptime
- `GetPerformanceStats`: View system QPS and peak throughput
- `GetBestBid` / `GetBestAsk`: (stubbed) Best market price per side
- `CancelOrder`, `GetOrdersAtPrice`: (stubbed) Order management endpoints

---

## 📈 Planned Enhancements

- [ ] Persistent trade logger (`TradeLogger`)
- [ ] Order cancellation and modification support
- [ ] Multi-symbol sharded matching engines
- [ ] Streaming APIs for order fills (gRPC server-side streaming)
- [ ] REST or WebSocket gateway for external dashboards
