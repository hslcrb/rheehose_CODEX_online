// 한국어: Ralph IDE 핵심 상태를 담는 객체입니다.
// English: This object stores Ralph IDE core state.
const state = {
  mode: "fast",
  loopTimer: null,
  loopIteration: 0,
  modelConfig: null,
  extensions: [],
};

const elements = {
  modeSelect: document.getElementById("modeSelect"),
  providerSelect: document.getElementById("providerSelect"),
  endpointInput: document.getElementById("endpointInput"),
  modelInput: document.getElementById("modelInput"),
  apiKeyInput: document.getElementById("apiKeyInput"),
  saveModelConfig: document.getElementById("saveModelConfig"),
  modelStatus: document.getElementById("modelStatus"),
  openVsxUrl: document.getElementById("openVsxUrl"),
  extensionQuery: document.getElementById("extensionQuery"),
  searchExtensions: document.getElementById("searchExtensions"),
  extensionResults: document.getElementById("extensionResults"),
  vsixInput: document.getElementById("vsixInput"),
  importVsix: document.getElementById("importVsix"),
  localExtensions: document.getElementById("localExtensions"),
  markdownInput: document.getElementById("markdownInput"),
  markdownOutput: document.getElementById("markdownOutput"),
  pdfInput: document.getElementById("pdfInput"),
  pdfFrame: document.getElementById("pdfFrame"),
  loopCount: document.getElementById("loopCount"),
  infiniteLoop: document.getElementById("infiniteLoop"),
  startLoop: document.getElementById("startLoop"),
  stopLoop: document.getElementById("stopLoop"),
  loopLog: document.getElementById("loopLog"),
};

function setupTabs() {
  const tabButtons = document.querySelectorAll(".tabs button");
  const tabs = {
    editor: document.getElementById("editorTab"),
    markdown: document.getElementById("markdownTab"),
    pdf: document.getElementById("pdfTab"),
  };

  tabButtons.forEach((button) => {
    button.addEventListener("click", () => {
      tabButtons.forEach((b) => b.classList.remove("active"));
      Object.values(tabs).forEach((tab) => tab.classList.remove("active"));
      button.classList.add("active");
      tabs[button.dataset.tab].classList.add("active");
    });
  });
}

function setupModeSwitch() {
  elements.modeSelect.addEventListener("change", (event) => {
    state.mode = event.target.value;
    const warnings = [];

    // 한국어: Fast 모드를 기본으로 유지하도록 사용자에게 안내합니다.
    // English: Guide users to keep Fast mode as the default.
    if (state.mode !== "fast") {
      warnings.push("Fast 모드가 에이전트 개발에 권장됩니다. / Fast mode is recommended for agentic development.");
    }

    elements.modelStatus.textContent = `Mode: ${state.mode}\n${warnings.join("\n")}`;
  });
}

function setupModelConfig() {
  elements.saveModelConfig.addEventListener("click", () => {
    state.modelConfig = {
      provider: elements.providerSelect.value,
      endpoint: elements.endpointInput.value,
      model: elements.modelInput.value,
      apiKeyMasked: elements.apiKeyInput.value ? "***configured***" : "not-set",
    };

    localStorage.setItem("ralph:model-config", JSON.stringify(state.modelConfig));
    elements.modelStatus.textContent = JSON.stringify(state.modelConfig, null, 2);
  });

  const saved = localStorage.getItem("ralph:model-config");
  if (saved) {
    const parsed = JSON.parse(saved);
    elements.providerSelect.value = parsed.provider || "ollama";
    elements.endpointInput.value = parsed.endpoint || "";
    elements.modelInput.value = parsed.model || "";
    elements.modelStatus.textContent = JSON.stringify(parsed, null, 2);
  }
}

async function searchOpenVsx() {
  const baseUrl = elements.openVsxUrl.value.replace(/\/$/, "");
  const query = encodeURIComponent(elements.extensionQuery.value.trim() || "ai");
  const endpoint = `${baseUrl}/-/search?query=${query}`;

  const resultNode = elements.extensionResults;
  resultNode.innerHTML = "<li>검색 중... / Searching...</li>";

  try {
    const response = await fetch(endpoint);
    if (!response.ok) {
      throw new Error(`Open VSX request failed (${response.status})`);
    }

    const data = await response.json();
    const extensions = data.extensions || [];

    if (extensions.length === 0) {
      resultNode.innerHTML = "<li>결과 없음 / No results</li>";
      return;
    }

    resultNode.innerHTML = "";
    extensions.slice(0, 10).forEach((ext) => {
      const li = document.createElement("li");
      li.textContent = `${ext.namespace}.${ext.name} (${ext.version || "n/a"})`;
      resultNode.appendChild(li);
    });
  } catch (error) {
    resultNode.innerHTML = `<li>실패 / Failed: ${error.message}</li>`;
  }
}

function setupExtensionManager() {
  elements.searchExtensions.addEventListener("click", searchOpenVsx);

  elements.importVsix.addEventListener("click", async () => {
    const [file] = elements.vsixInput.files;
    if (!file) {
      alert("VSIX 파일을 선택하세요. / Please choose a VSIX file.");
      return;
    }

    // 한국어: 환경 제약으로 VSIX 압축 해제를 생략하고 기본 메타데이터를 기록합니다.
    // English: Due to environment constraints, skip VSIX unpack and persist basic metadata.
    const extension = {
      fileName: file.name,
      size: file.size,
      installedAt: new Date().toISOString(),
      source: "direct-vsix-import",
    };

    state.extensions.push(extension);
    localStorage.setItem("ralph:extensions", JSON.stringify(state.extensions));
    renderLocalExtensions();
  });

  const savedExtensions = localStorage.getItem("ralph:extensions");
  if (savedExtensions) {
    state.extensions = JSON.parse(savedExtensions);
    renderLocalExtensions();
  }
}

function renderLocalExtensions() {
  elements.localExtensions.innerHTML = "";
  state.extensions.forEach((ext) => {
    const item = document.createElement("li");
    item.textContent = `${ext.fileName} · ${(ext.size / 1024).toFixed(1)}KB · ${ext.installedAt}`;
    elements.localExtensions.appendChild(item);
  });
}

function simpleMarkdownToHtml(markdown) {
  return markdown
    .replace(/^### (.*$)/gim, "<h3>$1</h3>")
    .replace(/^## (.*$)/gim, "<h2>$1</h2>")
    .replace(/^# (.*$)/gim, "<h1>$1</h1>")
    .replace(/\*\*(.*?)\*\*/gim, "<strong>$1</strong>")
    .replace(/\*(.*?)\*/gim, "<em>$1</em>")
    .replace(/`(.*?)`/gim, "<code>$1</code>")
    .replace(/\n$/gim, "<br />");
}

function setupMarkdown() {
  const defaultText = `# Ralph IDE\n\n- 내장 마크다운 렌더링\n- Built-in Markdown rendering\n- Fast mode 중심 UX`; 
  elements.markdownInput.value = defaultText;
  elements.markdownOutput.innerHTML = simpleMarkdownToHtml(defaultText);

  elements.markdownInput.addEventListener("input", () => {
    elements.markdownOutput.innerHTML = simpleMarkdownToHtml(elements.markdownInput.value);
  });
}

function setupPdf() {
  elements.pdfInput.addEventListener("change", () => {
    const [file] = elements.pdfInput.files;
    if (!file) return;

    const url = URL.createObjectURL(file);
    elements.pdfFrame.src = url;
  });
}

function setupRalphLoops() {
  elements.startLoop.addEventListener("click", () => {
    const isInfinite = elements.infiniteLoop.checked;
    const maxIterations = Number(elements.loopCount.value || "1");

    clearInterval(state.loopTimer);
    state.loopIteration = 0;
    elements.loopLog.textContent = "";

    state.loopTimer = setInterval(() => {
      state.loopIteration += 1;
      const line = `Iteration ${state.loopIteration}: inspect → audit → review → improve`;
      elements.loopLog.textContent += `${line}\n`;

      // 한국어: 무한 반복이 아니면 사용자 지정 횟수에 도달할 때 멈춥니다.
      // English: Stop at user-defined count unless infinite mode is enabled.
      if (!isInfinite && state.loopIteration >= maxIterations) {
        clearInterval(state.loopTimer);
      }
    }, 800);
  });

  elements.stopLoop.addEventListener("click", () => {
    clearInterval(state.loopTimer);
    elements.loopLog.textContent += "Stopped by user / 사용자 중지\n";
  });
}

setupTabs();
setupModeSwitch();
setupModelConfig();
setupExtensionManager();
setupMarkdown();
setupPdf();
setupRalphLoops();
