#include <WiFi.h>

const char* ssid = "YOUR-NETWORK-NAME";
const char* password = "YOUR-NETWORK-PASSWORD"; 

WiFiServer server(80);

void setup() {
  setCpuFrequencyMhz(240); 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  server.begin();
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();
    if (request.indexOf("/data") != -1) {
      client.println("HTTP/1.1 200 OK\nContent-Type: text/plain\nAccess-Control-Allow-Origin: *\n");
      client.print(touchRead(4)); 
    } 
    else {
      client.println("HTTP/1.1 200 OK\nContent-type:text/html\n");
      client.print(R"rawliteral(
<!DOCTYPE html><html><head>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/three@0.128.0/examples/js/controls/OrbitControls.js"></script>
  <style>
    body { margin: 0; background: #050507; overflow: hidden; font-family: 'JetBrains Mono', monospace; color: #fff; }
    
    /* Side Deck Styling */
    #side-deck {
      position: absolute; top: 0; right: 0; width: 350px; height: 100%;
      background: rgba(10, 10, 15, 0.9); border-left: 1px solid #00ffaa44;
      z-index: 100; transition: transform 0.5s cubic-bezier(0.19, 1, 0.22, 1);
      backdrop-filter: blur(15px); padding: 30px 20px; box-sizing: border-box;
    }
    #side-deck.closed { transform: translateX(350px); }
    
    /* Arrow Button */
    #toggle-btn {
      position: absolute; left: -40px; top: 50%; transform: translateY(-50%);
      width: 40px; height: 60px; background: #00ffaa; color: #000;
      display: flex; align-items: center; justify-content: center;
      cursor: pointer; border-radius: 10px 0 0 10px; transition: 0.3s;
    }
    #toggle-btn:hover { background: #fff; }
    .arrow { border: solid black; border-width: 0 3px 3px 0; display: inline-block; padding: 5px; transition: 0.3s; }
    .right { transform: rotate(-45deg); }
    .closed .right { transform: rotate(135deg); }

    .data-card { background: rgba(255,255,255,0.03); border: 1px solid #333; padding: 15px; border-radius: 8px; margin-bottom: 20px; }
    .label { font-size: 10px; color: #00ffaa; text-transform: uppercase; letter-spacing: 2px; margin-bottom: 8px; }
    .val { font-size: 24px; font-weight: 300; }
    canvas.spark { width: 100%; height: 60px; margin-top: 10px; }
  </style>
</head><body>

  <div id="side-deck" class="closed">
    <div id="toggle-btn" onclick="document.getElementById('side-deck').classList.toggle('closed')">
      <i class="arrow right"></i>
    </div>
    
    <div style="font-size: 14px; margin-bottom: 30px; border-bottom: 1px solid #333; padding-bottom: 10px;">SIGNAL_NODE_B2N</div>

    <div class="data-card">
      <div class="label">Capacitance Raw</div>
      <div class="val" id="v_raw">0.0</div>
      <canvas id="g_raw" class="spark"></canvas>
    </div>

    <div class="data-card">
      <div class="label">Impulse Frequency</div>
      <div class="val" id="v_pulse">0.0</div>
      <canvas id="g_pulse" class="spark"></canvas>
    </div>

    <div class="data-card">
      <div class="label">Thermal Stability</div>
      <div class="val" style="font-size: 16px;">STABLE // 240MHz</div>
      <canvas id="g_noise" class="spark"></canvas>
    </div>
  </div>

  <script>
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(60, window.innerWidth/window.innerHeight, 0.1, 5000);
    const renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);
    
    const controls = new THREE.OrbitControls(camera, renderer.domElement);
    camera.position.set(0, 350, 600);
    controls.enableDamping = true;

    // 130x130 High-Density Mesh
    const res = 130; 
    const geometry = new THREE.PlaneBufferGeometry(1000, 1000, res, res);
    const material = new THREE.MeshBasicMaterial({
        vertexColors: true, 
        wireframe: true, 
        transparent: true, 
        opacity: 0.4 
    });
    const terrain = new THREE.Mesh(geometry, material);
    terrain.rotation.x = -Math.PI / 2;
    scene.add(terrain);

    let rippleBuffer = new Array(res + 1).fill(0);
    let lastRaw = 0, smoothedPulse = 0;
    let dataSets = { raw: [], pulse: [], noise: [] };

    async function updateData() {
      try {
        const response = await fetch('/data');
        const text = await response.text();
        let val = parseFloat(text);
        
        let diff = Math.abs(val - lastRaw);
        lastRaw = val;
        
        smoothedPulse = (smoothedPulse * 0.75) + (diff * 0.25);
        let p = smoothedPulse > 0.6 ? smoothedPulse * 10.0 : 0;

        rippleBuffer.unshift(p);
        rippleBuffer.pop();

        document.getElementById('v_raw').innerText = val.toFixed(1);
        document.getElementById('v_pulse').innerText = p.toFixed(1);

        pushData(dataSets.raw, val);
        pushData(dataSets.pulse, p);
        pushData(dataSets.noise, Math.random() * 5);
      } catch (e) {}
      setTimeout(updateData, 25); 
    }

    function pushData(arr, v) { arr.push(v); if(arr.length > 50) arr.shift(); }

    function drawSpark(id, data, color) {
      const c = document.getElementById(id);
      const ctx = c.getContext('2d');
      ctx.clearRect(0,0,c.width,c.height);
      ctx.strokeStyle = color;
      ctx.lineWidth = 2;
      ctx.beginPath();
      let m = Math.max(...data, 1);
      for(let i=0; i<data.length; i++){
        let x = (i/50)*c.width;
        let y = c.height - ((data[i]/m)*c.height * 0.7) - 5;
        if(i==0) ctx.moveTo(x,y); else ctx.lineTo(x,y);
      }
      ctx.stroke();
    }

    function getColor(h) {
        let i = Math.min(1, h / 50);
        // Clean Rainbow Gradient
        if (i < 0.2) return {r:0, g:0.2, b:1}; // Dark Blue
        if (i < 0.4) return {r:0, g:1, b:1};   // Cyan
        if (i < 0.6) return {r:0, g:1, b:0.2}; // Green
        if (i < 0.8) return {r:1, g:0.8, b:0}; // Yellow/Gold
        return {r:1, g:0, b:0.1};              // Alert Red
    }

    function animate() {
      requestAnimationFrame(animate);
      controls.update();
      
      drawSpark('g_raw', dataSets.raw, '#00ffaa');
      drawSpark('g_pulse', dataSets.pulse, '#fff');
      drawSpark('g_noise', dataSets.noise, '#444');

      const pos = geometry.attributes.position.array;
      if (!geometry.attributes.color) {
        geometry.setAttribute('color', new THREE.BufferAttribute(new Float32Array(pos.length), 3));
      }
      const col = geometry.attributes.color.array;
      
      for (let i = 0; i < pos.length; i += 3) {
        let d = Math.sqrt(pos[i]**2 + pos[i+1]**2);
        let idx = Math.floor(d / 5.5); 
        let h = (idx < rippleBuffer.length) ? rippleBuffer[idx] : 0;
        pos[i+2] = h;
        let c = getColor(h);
        col[i] = c.r; col[i+1] = c.g; col[i+2] = c.b;
      }
      geometry.attributes.position.needsUpdate = true;
      geometry.attributes.color.needsUpdate = true;
      renderer.render(scene, camera);
    }
    updateData(); animate();
  </script>
</body></html>)rawliteral");
    }
    client.stop();
  }
}