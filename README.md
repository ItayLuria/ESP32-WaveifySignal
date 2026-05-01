# WaveifySignal 

**WaveifySignal** is a hardware-to-web visualizer that transforms raw physical capacitance into a 3D wave simulation. By piping live electrostatic data from an **ESP32** into a **Three.js** engine, the project creates an interface where human proximity deforms a high-density mesh in real-time.

---

## 
 Demonstration

### Real-Time preview
 the grid is reacting to physical proximity by visualizing the invisible electrostatic field:
<img src="assets/demo.gif" width="100%" style="border-radius: 10px;" alt="WaveifySignal Real-time Interaction" />

---

## Features & Technical Core

The system utilizes a high-density 130x130 grid (16,900 vertices) to provide smooth, fluid-like motion.

### 1. Real-Time Pipeline
- **Optimization:** The ESP32 is clocked at **240MHz** to handle high-frequency `touchRead` cycles with minimal latency.
- **Connectivity:** Standalone HTTP server architecture for low-latency data streaming.

### 2. Radial Wave Physics
- **Mechanism:** Vertex displacement is calculated using the 3D distance formula relative to the central origin point.
- **Visuals:** Dynamic vertex color interpolation based on signal intensity.

### 3. 2D Graphs Menu
- **Diagnostics:** Real-time sparkline graphs monitoring "Capacitance Raw" and "Impulse Frequency".

---

##  Hardware Configuration

To build this device, connect your touch interface to the following pinout:

| Component | ESP32 Pin | Details |
|-----------|-----------|---------|
| **Touch Signal** | GPIO 4 | Capacitive Input |
| **CPU Clock** | Internal | 240MHz Processing |

---

##  Setup Instructions

1. **Firmware:** Flash the `.ino` code to your ESP32.
2. **Network:** Update the `ssid` and `password` variables to match your local WiFi.
3. **Flash:** Upload the code via Arduino IDE.
4. **Visualize:** Open the Serial Monitor at `115200` baud to find the local IP address and navigate to it in any modern browser.

---

## Future Roadmap
- **GLSL Integration:** Migrating wave calculations to custom shaders to support ultra-high polygon counts.
- **WebSocket Migration:** Moving from HTTP fetch cycles to WebSockets for near-zero latency.
- **Multi-Node Arrays:** Supporting multiple capacitive pins to visualize complex wave interference patterns.

---<img width="800" height="450" alt="wave" src="https://github.com/user-attachments/assets/fb554223-4e27-4293-9dbf-f19b54a27bba" />
