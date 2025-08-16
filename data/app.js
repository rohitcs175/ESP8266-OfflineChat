(function () {
  const d = document;
  const joinBtn = d.getElementById("joinBtn");
  const nameInput = d.getElementById("name");
  const chatSection = d.getElementById("chatSection");
  const msgsEl = d.getElementById("msgs");
  const msgInput = d.getElementById("msg");
  const sendBtn = d.getElementById("send");
  const stateEl = d.getElementById("state");

  let ws = null;
  let myName = localStorage.getItem("sc8266_name") || "";

  // Fill name if stored
  if (myName) nameInput.value = myName;

  function setState(text, ok) {
    stateEl.textContent = text;
    stateEl.className = ok ? "pill ok" : "pill";
  }

  function escapeHTML(s) {
    return s.replace(/[&<>"']/g, (c) => ({
      "&": "&amp;",
      "<": "&lt;",
      ">": "&gt;",
      '"': "&quot;",
      "'": "&#39;"
    }[c]));
  }

  function appendLine(line) {
    const div = d.createElement("div");
    div.className = line.startsWith("[system]") ? "line sys" : "line";
    div.innerHTML = escapeHTML(line);
    msgsEl.appendChild(div);
    msgsEl.scrollTop = msgsEl.scrollHeight;
  }

  function connectWS() {
    try {
      ws = new WebSocket(`ws://${location.host}/ws`);
    } catch (e) {
      setState("ws error", false);
      return;
    }

    ws.onopen = () => {
      setState("connected", true);
      if (myName) {
        // Announce join
        ws.send(`JOIN|${myName}`);
        chatSection.classList.remove("hidden");
      }
    };

    ws.onclose = () => {
      setState("disconnected", false);
      // Attempt quick reconnect
      setTimeout(connectWS, 1500);
    };

    ws.onerror = () => {
      setState("ws error", false);
    };

    ws.onmessage = (ev) => {
      if (typeof ev.data === "string") {
        appendLine(ev.data);
      }
    };
  }

  joinBtn.addEventListener("click", () => {
    const n = nameInput.value.trim().slice(0, 20);
    if (!n) {
      alert("Enter a nickname");
      return;
    }
    myName = n;
    localStorage.setItem("sc8266_name", myName);
    if (ws && ws.readyState === 1) {
      ws.send(`JOIN|${myName}`);
      chatSection.classList.remove("hidden");
    } else {
      connectWS();
    }
  });

  function sendMsg() {
    const text = msgInput.value.trim().slice(0, 240);
    if (!text || !myName) return;
    if (ws && ws.readyState === 1) {
      ws.send(`MSG|${myName}|${text}`);
      msgInput.value = "";
      msgInput.focus();
    }
  }

  sendBtn.addEventListener("click", sendMsg);
  msgInput.addEventListener("keydown", (e) => {
    if (e.key === "Enter") sendMsg();
  });

  // Auto-connect
  connectWS();
})();
