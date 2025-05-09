# 🌐 C Network Tools

This repository is a collection of **low-level network utilities written in C**, designed to help understand and implement foundational network protocols and tools **without relying on external libraries or high-level APIs**.

It aims to build core networking skills using **raw sockets**, **manual protocol handling**, and **custom packet construction**.

---

## 📦 Project Structure

| Folder                  | Description                                                                 |
|--------------------------|-----------------------------------------------------------------------------|
| `Fingerprinting`         | Tools to identify systems, banners, and open ports (active/passive scan).   |
| `Network_Mapping`        | DNS lookup, WHOIS queries, and traceroute-style implementations.            |
| `Server-Client Relationship` | Basic TCP/UDP socket programming for custom client/server applications. |

---

## 🧠 Key Features (Implemented)

✅ **DNS Lookup**  
→ Send and parse DNS query packets manually to resolve domain names.

✅ **WHOIS Query**  
→ Connect to WHOIS servers and retrieve domain ownership/registration data.

✅ **Traceroute-like Utility**  
→ Send packets with incremental TTL and analyze ICMP replies to trace routes.

✅ **Zone Transfer (AXFR)**  
→ Connect to DNS servers and attempt zone transfers (for educational purposes).

✅ **Custom Server-Client Architecture**  
→ Build minimal TCP/UDP echo and messaging servers using raw socket APIs.

---

## 🔭 Planned Features

| Category                      | Planned Enhancements                                                  |
|-------------------------------|------------------------------------------------------------------------|
| 🔍 Nmap-style Scanning        | Open port and service discovery using TCP/UDP probes.                 |
| 📡 Protocol Emulation         | FTP, SMTP, HTTP request parsing and minimal response logic.           |
| 🧠 DNS Expansion              | Reverse DNS lookups, MX record retrieval, and custom resolver logic. |
| 🧪 Packet Analysis            | Raw packet sniffing with custom filters (no libpcap).                 |
| 📶 ARP/ICMP Tools             | ARP spoofing, ping utilities, and spoofed ICMP packet creation.      |

---

## 🚀 How to Build and Run

+---------+      TTL=1       +--------+      TTL=2       +---------+
| Your PC | ---------------> | Router | ---------------> | Gateway |
+---------+     (ICMP)       +--------+     (ICMP)       +---------+
     |                          ↑                          ↑
     └-------> Receives ICMP TTL Exceeded <---------------┘


1. Clone the repository:
```bash
git clone https://github.com/abkarada/C_Network_Tools.git
cd C_Network_Tools
gcc Network_Mapping/dns_lookup.c -o dns_lookup
./dns_lookup example.com
