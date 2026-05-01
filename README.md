# WaveifySignal 

**WaveifySignal** is a hardware-to-web visualizer that transforms raw physical capacitance into a fluid 3D wave simulation. By piping live electrostatic data from an **ESP32** into a **Three.js** engine, the project creates an interface where human proximity deforms a high-density mesh in real-time.

---

### Visual Demo

<img width="800" height="450" alt="wave" src="https://github.com/user-attachments/assets/9349af4f-43ae-420d-b0e9-3f6ecafbeba8" />

*The grid reacts to physical proximity, visualizing the invisible electrostatic field.*

---

### Features
*   **Polygon Mesh**: A 130x130 grid (16,900 vertices) providing fluid-like motion.
*   **Pipeline**: The ESP32 is clocked at **240MHz** to handle high-frequency `touchRead` cycles with minimal latency.
*   **Radial Wave Physics**: Vertex displacement is calculated using the 3D distance formula relative to the central origin point.
*   **HUD**: A slide-out menu featuring live sparkline diagnostics for raw signal and pulse frequency.

---

###  Tech Stack
*   **Hardware**: ESP32 (WROOM-32) & a female to male jumper cable as the antenna.
*   **Frontend**: Three.js (WebGL), JavaScript (ES6+), CSS3.
*   **Backend**: C++ / Arduino (Embedded Web Server).

---

### Setup & Installation
1.  **Firmware**: Flash the `.ino` file to your ESP32.
2.  **Network**: Update the `ssid` and `password` variables to match your local WiFi.
3.  **Launch**: Open the Serial Monitor at `115200` baud to find the local IP address.
4.  **Visualize**: Navigate to that IP in any modern browser to start the stream.

---

### Future Roadmap
*   **GLSL Integration**: Migrating wave calculations to custom shaders to support ultra-high polygon counts.
*   **WebSocket Migration**: Moving from HTTP fetch cycles to WebSockets for near-zero latency.
*   **Multi-Node Arrays**: Supporting multiple capacitive pins to visualize complex wave interference patterns.
---


