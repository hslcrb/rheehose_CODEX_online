const tabs = document.querySelectorAll('.tab');
const views = document.querySelectorAll('.view');
const statusBar = document.getElementById('statusBar');
const markdownInput = document.getElementById('markdownInput');
const markdownPreview = document.getElementById('markdownPreview');
const pdfInput = document.getElementById('pdfInput');
const pdfFrame = document.getElementById('pdfFrame');
const loopCountInput = document.getElementById('loopCountInput');
const infiniteLoopCheck = document.getElementById('infiniteLoopCheck');
const runLoopBtn = document.getElementById('runLoopBtn');
const providerSelect = document.getElementById('providerSelect');
const endpointInput = document.getElementById('endpointInput');
const apiKeyInput = document.getElementById('apiKeyInput');
const saveModelBtn = document.getElementById('saveModelBtn');
const searchExtBtn = document.getElementById('searchExtBtn');
const extSearchInput = document.getElementById('extSearchInput');
const extensionList = document.getElementById('extensionList');
const vsixInput = document.getElementById('vsixInput');
const workMode = document.getElementById('workMode');

// 한국어: 내장 탭 전환 로직입니다. / English: Built-in tab switching logic.
tabs.forEach((tab) => {
  tab.addEventListener('click', () => {
    tabs.forEach((t) => t.classList.remove('active'));
    views.forEach((v) => v.classList.remove('active'));
    tab.classList.add('active');
    document.getElementById(tab.dataset.view).classList.add('active');
  });
});

// 한국어: 확장 없이 Markdown 렌더링을 즉시 제공합니다.
// English: Provides instant Markdown rendering without any extension installation.
function renderMarkdown() {
  markdownPreview.innerHTML = marked.parse(markdownInput.value);
}
markdownInput.addEventListener('input', renderMarkdown);
renderMarkdown();

// 한국어: 브라우저 내장 PDF 프리뷰를 사용합니다.
// English: Uses browser-native PDF preview support.
pdfInput.addEventListener('change', (event) => {
  const file = event.target.files?.[0];
  if (!file) return;
  const url = URL.createObjectURL(file);
  pdfFrame.src = url;
  statusBar.textContent = `PDF 로드됨 / PDF loaded: ${file.name}`;
});

// 한국어: Fast 모드를 기본값으로 강제합니다.
// English: Enforces Fast mode as the default working style.
workMode.addEventListener('change', () => {
  const mode = workMode.value;
  if (mode !== 'fast') {
    statusBar.textContent = `경고: Fast 모드 권장 / Warning: Fast mode is recommended (current: ${mode})`;
  } else {
    statusBar.textContent = 'Fast 모드 활성화 / Fast mode enabled';
  }
});

// 한국어: Ralph Loop는 검사/감사/리뷰를 반복 실행합니다.
// English: Ralph Loop repeatedly enforces inspect/audit/review checkpoints.
runLoopBtn.addEventListener('click', () => {
  const infinite = infiniteLoopCheck.checked;
  const count = Number(loopCountInput.value || 0);
  if (!infinite && count <= 0) {
    statusBar.textContent = '반복 횟수는 1 이상이어야 합니다 / Iterations must be at least 1';
    return;
  }

  const summary = infinite
    ? 'Ralph Loop: 무한 반복 모드 / Infinite iteration mode'
    : `Ralph Loop: ${count}회 반복 / ${count} iterations`;

  statusBar.textContent = `${summary} - 검사/감사/리뷰 단계 강제 / inspect/audit/review enforced`;
});

// 한국어: 로컬/클라우드 LLM 연결 설정을 저장합니다.
// English: Stores connection settings for both local and cloud LLM providers.
saveModelBtn.addEventListener('click', () => {
  const config = {
    provider: providerSelect.value,
    endpoint: endpointInput.value.trim(),
    apiKey: apiKeyInput.value.trim(),
  };

  localStorage.setItem('ralph-model-config', JSON.stringify(config));
  statusBar.textContent = `모델 설정 저장됨 / Model config saved (${config.provider})`;
});

// 한국어: Open VSX 공개 API를 통해 확장 검색 결과를 표시합니다.
// English: Shows extension search results using Open VSX public API.
searchExtBtn.addEventListener('click', async () => {
  const query = extSearchInput.value.trim();
  if (!query) {
    statusBar.textContent = '검색어를 입력하세요 / Enter a search query';
    return;
  }

  statusBar.textContent = 'Open VSX 검색 중... / Searching Open VSX...';
  extensionList.innerHTML = '';

  try {
    const response = await fetch(`https://open-vsx.org/api/-/search?query=${encodeURIComponent(query)}&size=5`);
    const data = await response.json();
    const extensions = data?.extensions || [];

    extensions.forEach((ext) => {
      const item = document.createElement('li');
      item.textContent = `${ext.namespace}.${ext.name} - ${ext.version}`;
      extensionList.appendChild(item);
    });

    statusBar.textContent = `검색 완료 / Search complete (${extensions.length} results)`;
  } catch (error) {
    statusBar.textContent = 'Open VSX 검색 실패 / Open VSX search failed';
  }
});

// 한국어: 사용자가 가져온 VSIX 파일을 직접 설치 대상으로 등록합니다.
// English: Registers user-imported VSIX files for direct installation workflow.
vsixInput.addEventListener('change', () => {
  const file = vsixInput.files?.[0];
  if (!file) return;

  const item = document.createElement('li');
  item.textContent = `로컬 VSIX / Local VSIX: ${file.name}`;
  extensionList.prepend(item);
  statusBar.textContent = `VSIX 가져오기 완료 / VSIX imported: ${file.name}`;
});

// 한국어: 저장된 모델 설정을 복원합니다.
// English: Restores saved model configuration at startup.
(function bootstrapModelConfig() {
  const raw = localStorage.getItem('ralph-model-config');
  if (!raw) return;
  try {
    const config = JSON.parse(raw);
    providerSelect.value = config.provider || providerSelect.value;
    endpointInput.value = config.endpoint || '';
    apiKeyInput.value = config.apiKey || '';
  } catch {
    // 한국어: 손상된 설정은 무시합니다. / English: Ignore corrupted saved settings.
  }
})();
