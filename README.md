# 🛰️ Alex – Lunar Rescue Robot

**Alex** is a remotely operated robotic system designed for a simulated lunar search-and-rescue mission at Moonbase CEG, located in the Sea of Tranquillity. The mission scenario involved rescuing injured astronauts in a hazardous, obstacle-filled environment within a strict 8-minute operation window.

Alex autonomously maps its surroundings, identifies astronauts in need using a colour sensor, and performs one of two actions:
- **Deliver medpacks** to green-tagged astronauts
- **Retrieve and transport** red-tagged astronauts to a designated safe zone

---

## 🚀 System Overview

Alex is powered by a dual-board system:
- **Raspberry Pi** (high-level control):
  - SLAM-based navigation using LiDAR
  - Map visualization and remote operation
  - ROS2-based task coordination
- **Arduino Mega** (low-level control):
  - Real-time motor control
  - Colour sensor classification
  - Servo control for arm and medpack dispenser

Boards communicate via **serial protocol** to coordinate actions.

---

## 🧠 Features

- 🔎 **SLAM Navigation** with LiDAR (2D mapping + obstacle avoidance)
- 🧠 **Astronaut Classification** using a colour sensor
- 🤖 **Front Robotic Arm** to pick up red astronauts
- 💊 **Rear Medpack Dispenser** to aid green astronauts
- 🧭 **Remote Operation** with real-time environment feedback
- ⚙️ **Low-level C/C++ programming** for embedded control on Arduino

---
