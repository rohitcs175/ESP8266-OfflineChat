# ESP8266‑OfflineChat

**A tiny offline chat room hosted on an ESP8266.** Users connect to the ESP8266’s Wi‑Fi hotspot and chat in a web page. No internet or external server needed.

> **One‑line:** ESP8266 AP + WebSocket + SPIFFS = local Wi‑Fi chat.

---

## ✨ What this project does

* Creates its **own Wi‑Fi hotspot (AP mode)** named `SecretChat_8266` (password `chat8266`).
* Serves a chat web app (`index.html`, `app.js`, `style.css`) from the board’s **SPIFFS** storage.
* Uses **WebSocket** (`/ws`) for real‑time messages between users.
* Shows the last **50 messages** to new users (stored in RAM; cleared on reboot).
* Health check endpoint: `GET /health` → `OK`.

---

## 🧠 How it works (simple)

1. ESP8266 starts in **AP mode** so phones/laptops can connect to it.
2. It runs an HTTP server and a WebSocket server on **port 80**.
3. The web page (from SPIFFS) opens a WebSocket to `ws://<device>/ws`.
4. When you send a message, the ESP8266 **broadcasts** it to everyone.
5. A small **in‑memory history** lets newcomers see recent chat.

---

## 🛠 Hardware

* ESP8266 board (NodeMCU 1.0 / Wemos D1 mini recommended)
* Reliable USB cable + power (phone charger or PC)

## 💻 Software

* **Arduino IDE** (1.8.x or 2.x)
* **ESP8266 Board package** (via Boards Manager)
* **Libraries:**

  * `ESPAsyncWebServer`
  * `ESPAsyncTCP`
  * `FS` / `SPIFFS` (included with ESP8266 core)
* **SPIFFS Uploader plugin** (to upload files in the `data/` folder to flash)

> If you already use LittleFS elsewhere, this project specifically uses **SPIFFS**.

---

## 📦 Folder structure

```
ESP8266-OfflineChat/
├─ ESP8266_LocalChat_AP.ino        # main firmware
└─ data/                            # files served to the browser (SPIFFS)
   ├─ index.html
   ├─ app.js
   └─ style.css
```

---

## ⚙️ Quick Setup (Step‑by‑Step)

### 1) Install ESP8266 support in Arduino IDE

1. **Arduino IDE → Preferences**
2. In **Additional Boards Manager URLs** add:

   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. **Tools → Board → Boards Manager…** → search **ESP8266** → **Install**.

### 2) Install required libraries

Use **Sketch → Include Library → Manage Libraries…** and install:

* **ESPAsyncWebServer**
* **ESPAsyncTCP**

> Tip: If library manager doesn’t show them, install from their GitHub ZIP via **Sketch → Include Library → Add .ZIP Library…**

### 3) Add the SPIFFS uploader (one‑time)

Install the **ESP8266FS (SPIFFS) Uploader** plugin so you can send the `data/` folder to flash. After a correct install and IDE restart, you will see a menu item:

* **Tools → ESP8266 Sketch Data Upload**

### 4) Open the project

* Open `ESP8266_LocalChat_AP.ino` in Arduino IDE.

### 5) Select the board & port

* **Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP‑12E Module)** (or your model)
* **Tools → Port** → pick the COM port of your ESP8266

Recommended tool settings (adjust to your board):

* Upload Speed: `921600` (use `115200` if unstable)
* Flash Size: your default (commonly `4M (1M SPIFFS)`)
* CPU Frequency: default

### 6) Put web files in SPIFFS

Make sure the `data/` folder (containing `index.html`, `app.js`, `style.css`) sits **next to** the `.ino` file.

Then upload the SPIFFS image:

* **Tools → ESP8266 Sketch Data Upload**
  (Wait for “SPIFFS Image Uploaded” message.)

### 7) Upload the firmware

* Click **Upload** (right‑arrow) to flash `ESP8266_LocalChat_AP.ino`.
* After upload, **Tools → Serial Monitor** at **115200** to see logs.

---

## 🚀 Use it

1. Power the ESP8266. It creates a Wi‑Fi network:

   * **SSID:** `SecretChat_8266`
   * **Password:** `chat8266`
2. Connect your phone/laptop to this Wi‑Fi.
3. Open a browser and go to **`http://192.168.4.1/`** (captive portals may also auto‑open).
4. Enter a **nickname** → **Join**.
5. Start chatting! Messages appear instantly for everyone connected.

> New users see the last **50 messages** when they connect.

---

## 🔧 Configuration

Open `ESP8266_LocalChat_AP.ino` and edit:

```cpp
// AP (hotspot) settings
const char* AP_SSID     = "SecretChat_8266";
const char* AP_PASSWORD = "chat8266"; // empty string = open AP

// Message history size
const size_t MAX_HISTORY = 50; // increase/decrease as you like
```

Other notes:

* WebSocket path is fixed to **`/ws`**.
* Static files are served from **SPIFFS root** and `index.html` is the **default**.
* Health check is **`/health`** → returns `OK`.

---

## 🧪 Test checklist

* [ ] Device boots, Serial shows AP started
* [ ] Phone sees `SecretChat_8266`
* [ ] `http://192.168.4.1/` loads the chat UI
* [ ] Joining sends a system message
* [ ] Sending messages updates all connected clients

---

## 🛟 Troubleshooting

**Browser doesn’t open automatically**
Open `http://192.168.4.1/` manually (some OSs don’t auto‑trigger captive portal).

**Can’t find SPIFFS upload menu**
Recheck SPIFFS uploader installation and restart the IDE.

**Upload succeeds but page is blank**
Confirm `data/` is next to the `.ino` and contains `index.html`. Re‑upload SPIFFS.

**Random reboots or disconnects**
Use a better power source/cable; try a lower upload speed; avoid noisy USB hubs.

**Multiple users, slow updates**
ESP8266 is tiny; keep message bursts reasonable; reduce `MAX_HISTORY` if needed.

---

## 🔒 Privacy & Scope

* Works **offline**; messages stay within the ESP8266 hotspot.
* Messages are held **in RAM only** and vanish on reboot.
* This is a **local demo/tool**, not end‑to‑end encrypted chat.

---

## 📜 License

Recommend **MIT License** (simple and permissive). Add a `LICENSE` file of your choice.

---

## 🙏 Credits

* Built with ESP8266 Arduino core, **ESPAsyncWebServer**, **ESPAsyncTCP**, and **SPIFFS**.

---

## TL;DR

Flash the firmware → upload `data/` via SPIFFS → connect to `SecretChat_8266` → open `http://192.168.4.1/` → join and chat, **no internet needed**.
