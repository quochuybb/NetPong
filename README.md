# NetPong — Online Pong with Power-ups

> Classic Pong reimagined for the network: a lightweight client–server multiplayer Pong with special items/power-ups to make matches chaotic and fun — all implemented from scratch in modern C++ as a learning project.

<!-- Badges (replace with actual links if you add CI, license, etc.) -->
[![Build status](https://img.shields.io/badge/build-setup-lightgrey)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()

## Demo
*Add a GIF or screenshot here (e.g. `assets/demo.gif`).*

---

## About
NetPong is a small game project that implements an online multiplayer Pong using a client–server architecture. The project purpose is educational: I implemented the game logic, rendering, input handling, network layer, and netcode myself (no external game/net libraries). The gameplay adds special items/power-ups that spawn during matches to make online games more lively.

This repository is intentionally minimal and designed to show how networked games are structured and how to reason about synchronization, latency, and authoritative server design.

---

## Highlights / Features
- Real-time client–server multiplayer (custom networking code).
- Authoritative server model (server resolves collisions, scoring, item logic).
- Multiple fun **power-ups / items** (examples):
  - Ball speed burst
  - Paddle shrink / grow
  - Multi-ball
  - Invisibility (ball or paddle)
  - Temporary magnet / curve effect
- Custom netcode implemented from scratch — messaging, state sync, simple latency compensation.
- Written in modern C++ (no external game engines or networking libraries).
- Lightweight, easy to read and extend — great for learning.

---

## Tech / Design Notes
- Language: C++ (modern standard — specify in `CMakeLists.txt` or build scripts).
- Networking: custom socket-based client/server layer (plain sockets, message framing & serialization done by the project).
- Architecture: server is authoritative for game state; clients are responsible for input and prediction where appropriate.
- Serialization: simple binary or compact message format (see `net/`).
- Minimal dependencies: standard library + platform socket APIs (or small cross-platform wrappers included).

---

## Repo layout (example)
