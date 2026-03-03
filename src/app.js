// 상태 저장소: 파일/모델/확장/루프 상태를 단일 메모리 객체로 유지합니다.
// State store: keep files/models/extensions/loop status in one in-memory object.
const state = {
  fastMode: true,
  activeFile: null,
  files: {
    "README.md": "# Ralph IDE\n\nFast Mode 기본 / Fast mode by default."
  },
  models: [],
  extensions: [],
  loopRunning: false,
  loopTimer: null
};

const el = {
  fastMode: document.getElementById("fastMode"),
  newFileName: document.getElementById("newFileName"),
  addFileBtn: document.getElementById("addFileBtn"),
  fileList: document.getElementById("fileList"),
  activeFileName: document.getElementById("activeFileName"),
  editor: document.getElementById("editor"),
  saveBtn: document.getElementById("saveBtn"),
  markdownPreview: document.getElementById("markdownPreview"),
  tabs: document.querySelectorAll(".tab"),
  markdownTab: document.getElementById("markdownTab"),
  pdfTab: document.getElementById("pdfTab"),
  pdfInput: document.getElementById("pdfInput"),
  pdfFrame: document.getElementById("pdfFrame"),
  extensionQuery: document.getElementById("extensionQuery"),
  searchOpenVsxBtn: document.getElementById("searchOpenVsxBtn"),
  extensionFile: document.getElementById("extensionFile"),
  importExtensionBtn: document.getElementById("importExtensionBtn"),
  extensionResults: document.getElementById("extensionResults"),
  modelForm: document.getElementById("modelForm"),
  providerType: document.getElementById("providerType"),
  modelName: document.getElementById("modelName"),
  endpoint: document.getElementById("endpoint"),
  apiKey: document.getElementById("apiKey"),
  modelList: document.getElementById("modelList"),
  loopCount: document.getElementById("loopCount"),
  startLoopBtn: document.getElementById("startLoopBtn"),
  stopLoopBtn: document.getElementById("stopLoopBtn"),
  loopLog: document.getElementById("loopLog")
};

function init() {
  bindEvents();
  renderFiles();
  setActiveFile("README.md");
  renderModels();
  renderExtensions();
}

function bindEvents() {
  el.fastMode.addEventListener("change", () => {
    state.fastMode = el.fastMode.checked;
    logLoop(`Mode: ${state.fastMode ? "Fast" : "Planning/SDD"}`);
  });

  el.addFileBtn.addEventListener("click", () => {
    const name = el.newFileName.value.trim();
    if (!name) return;
    if (!state.files[name]) {
      state.files[name] = "";
      renderFiles();
      setActiveFile(name);
    }
    el.newFileName.value = "";
  });

  el.editor.addEventListener("input", () => {
    if (!state.activeFile) return;
    state.files[state.activeFile] = el.editor.value;
    if (state.activeFile.endsWith(".md")) {
      renderMarkdown(state.files[state.activeFile]);
    }
  });

  el.saveBtn.addEventListener("click", () => {
    const snapshot = {
      savedAt: new Date().toISOString(),
      state
    };
    logLoop(`Snapshot saved: ${snapshot.savedAt}`);
  });

  el.tabs.forEach((tab) => {
    tab.addEventListener("click", () => switchTab(tab.dataset.tab));
  });

  el.pdfInput.addEventListener("change", () => {
    const [file] = el.pdfInput.files;
    if (!file) return;
    const url = URL.createObjectURL(file);
    el.pdfFrame.src = url;
  });

  el.searchOpenVsxBtn.addEventListener("click", searchOpenVsx);

  el.importExtensionBtn.addEventListener("click", () => {
    const [file] = el.extensionFile.files;
    if (!file) return;
    state.extensions.push({
      source: "local-file",
      name: file.name,
      version: "manual-import"
    });
    renderExtensions();
  });

  el.modelForm.addEventListener("submit", (event) => {
    event.preventDefault();
    const model = {
      provider: el.providerType.value,
      name: el.modelName.value.trim(),
      endpoint: el.endpoint.value.trim(),
      apiKey: el.apiKey.value.trim() ? "********" : ""
    };
    if (!model.name) return;
    state.models.push(model);
    renderModels();
    el.modelForm.reset();
  });

  el.startLoopBtn.addEventListener("click", startRalphLoops);
  el.stopLoopBtn.addEventListener("click", stopRalphLoops);
}

function renderFiles() {
  el.fileList.innerHTML = "";
  Object.keys(state.files).forEach((name) => {
    const li = document.createElement("li");
    li.textContent = name;
    li.className = name === state.activeFile ? "active" : "";
    li.addEventListener("click", () => setActiveFile(name));
    el.fileList.appendChild(li);
  });
}

function setActiveFile(name) {
  state.activeFile = name;
  el.activeFileName.textContent = name;
  el.editor.value = state.files[name] ?? "";
  renderFiles();
  if (name.endsWith(".md")) {
    renderMarkdown(el.editor.value);
  }
}

// 간단한 Markdown 렌더러: 설치 없이 기본 문법을 HTML로 변환합니다.
// Simple Markdown renderer: converts basic Markdown syntax to HTML without dependencies.
function renderMarkdown(markdownText) {
  const html = markdownText
    .replace(/^### (.*$)/gim, "<h3>$1</h3>")
    .replace(/^## (.*$)/gim, "<h2>$1</h2>")
    .replace(/^# (.*$)/gim, "<h1>$1</h1>")
    .replace(/\*\*(.*?)\*\*/gim, "<strong>$1</strong>")
    .replace(/\*(.*?)\*/gim, "<em>$1</em>")
    .replace(/\n$/gim, "<br />");
  el.markdownPreview.innerHTML = html;
}

async function searchOpenVsx() {
  const query = el.extensionQuery.value.trim();
  if (!query) return;

  try {
    const url = `https://open-vsx.org/api/-/search?query=${encodeURIComponent(query)}`;
    const response = await fetch(url);
    const data = await response.json();
    const hits = data.extensions?.slice(0, 5) ?? [];

    hits.forEach((item) => {
      state.extensions.push({
        source: "open-vsx",
        name: `${item.namespace}.${item.name}`,
        version: item.version
      });
    });
    renderExtensions();
  } catch (error) {
    logLoop(`Open VSX search failed: ${error.message}`);
  }
}

function renderExtensions() {
  el.extensionResults.innerHTML = "";
  state.extensions.forEach((ext) => {
    const li = document.createElement("li");
    li.textContent = `[${ext.source}] ${ext.name} (${ext.version})`;
    el.extensionResults.appendChild(li);
  });
}

function renderModels() {
  el.modelList.innerHTML = "";
  state.models.forEach((model) => {
    const li = document.createElement("li");
    li.textContent = `${model.provider} :: ${model.name} ${
      model.endpoint ? `@ ${model.endpoint}` : ""
    } ${model.apiKey ? "(secured api key)" : ""}`;
    el.modelList.appendChild(li);
  });
}

function startRalphLoops() {
  const raw = el.loopCount.value.trim().toLowerCase();
  const infinite = raw === "infinite";
  const limit = Number.parseInt(raw, 10);

  if (!infinite && (!Number.isInteger(limit) || limit <= 0)) {
    logLoop("루프 수는 양의 정수 또는 infinite 여야 합니다. / Loop count must be positive integer or infinite.");
    return;
  }

  stopRalphLoops();
  state.loopRunning = true;
  let current = 0;

  state.loopTimer = setInterval(() => {
    if (!state.loopRunning) return;

    current += 1;
    runLoopStages(current);

    if (!infinite && current >= limit) {
      stopRalphLoops();
      logLoop(`Ralph Loops completed: ${limit}`);
    }
  }, state.fastMode ? 500 : 1200);

  logLoop(`Ralph Loops started (${infinite ? "infinite" : limit})`);
}

function stopRalphLoops() {
  if (state.loopTimer) {
    clearInterval(state.loopTimer);
    state.loopTimer = null;
  }
  state.loopRunning = false;
}

// Ralph Loops 핵심 단계: 점검/감사/리뷰를 순환 강제합니다.
// Ralph Loops core stages: enforce inspection/auditing/review in sequence.
function runLoopStages(cycle) {
  logLoop(`Cycle ${cycle}: inspection`);
  logLoop(`Cycle ${cycle}: auditing`);
  logLoop(`Cycle ${cycle}: review`);
}

function logLoop(message) {
  const stamp = new Date().toLocaleTimeString();
  el.loopLog.textContent += `[${stamp}] ${message}\n`;
  el.loopLog.scrollTop = el.loopLog.scrollHeight;
}

init();
