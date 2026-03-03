# Ralph IDE / 랄프 IDE

Ralph IDE는 VS Code 스타일의 개발 경험을 목표로 하며, 에이전트 중심(Agentic) AI 개발을 기본값으로 설계한 오픈소스 IDE 청사진입니다.  
Ralph IDE is an open-source IDE blueprint targeting a VS Code-like developer experience, with agentic AI-driven development as a first-class default.

## Vision / 비전

- **하나의 제품에 지속적으로 집중**: Ralph Loops를 코어 엔진에 내장하여 반복 실행, 점검, 감사, 리뷰를 강제합니다.  
  **Continuous focus on one product**: Ralph Loops is built into the core engine to enforce iteration, inspection, audit, and review.
- **Fast Mode 우선**: Spec-Driven Development/Planning 중심 흐름보다 Fast Mode를 기본 동작으로 제공합니다.  
  **Fast Mode first**: Fast Mode is the default workflow instead of SDD/planning-centric flows.
- **확장 없는 기본 생산성**: Markdown/PDF 렌더링을 기본 제공하여 별도 확장 설치 없이 사용 가능합니다.  
  **No-extension baseline productivity**: Markdown/PDF rendering is built in and available without extra extensions.

## Built-in Core Features / 기본 내장 기능

1. **Agentic AI Workspace / 에이전틱 AI 워크스페이스**
   - 작업 컨텍스트를 모델 호출에 연결하고, 변경 이력을 루프 단위로 기록합니다.  
     Connects working context to model calls and logs changes per loop.

2. **Fast Mode Engine / Fast 모드 엔진**
   - 빠른 실행/검증 사이클을 기본으로 하며, 계획 문서를 강제하지 않습니다.  
     Uses rapid execution/validation cycles by default without mandatory planning artifacts.

3. **Ralph Loops (Integrated) / Ralph Loops(코어 통합)**
   - 루프 횟수는 `무한(infinite)` 또는 `임의 숫자(any finite number)`를 지원합니다.  
     Loop count supports `infinite` or `any finite number`.
   - 각 루프 시작/종료 시 감사 훅(audit hook)과 검토 훅(review hook)을 트리거합니다.  
     Triggers audit and review hooks at the start/end of each loop.

4. **Built-in Markdown + PDF Rendering / Markdown + PDF 기본 렌더링**
   - Markdown은 기본 파서 경로로 렌더링합니다.  
     Markdown is rendered through a built-in parser path.
   - PDF는 브라우저/웹뷰 네이티브 렌더러 또는 PDF.js 통합 경로를 통해 표시합니다.  
     PDF is displayed using native browser/webview rendering or an integrated PDF.js path.

5. **Extension Ecosystem / 확장 생태계**
   - Open VSX 레지스트리 메타데이터를 조회할 수 있습니다.  
     Supports querying Open VSX registry metadata.
   - 사용자가 `.vsix`/확장 패키지 파일을 직접 import/install할 수 있습니다.  
     Users can directly import/install `.vsix`/extension package files.

6. **LLM Provider Abstraction / LLM 공급자 추상화**
   - 로컬 모델: Ollama, LM Studio  
     Local models: Ollama, LM Studio
   - 클라우드 모델: API Key 기반 공급자(예: OpenAI 호환 엔드포인트)  
     Cloud models: API key-based providers (e.g., OpenAI-compatible endpoints)

## Repository Layout / 저장소 구조

- `src/app.js`: IDE 구성과 부트스트랩. / IDE composition and bootstrap.
- `src/core/ralphLoops.js`: 루프 오케스트레이션 엔진. / Loop orchestration engine.
- `src/core/modelProviders.js`: 로컬/클라우드 LLM 연결 추상화. / Local/cloud LLM provider abstraction.
- `src/core/rendering.js`: Markdown/PDF 렌더 파이프라인. / Markdown/PDF render pipeline.
- `src/core/extensionManager.js`: Open VSX + 직접 파일 설치 처리. / Open VSX + direct file install.
- `src/config/defaultConfig.js`: 기본 동작(Fast Mode 포함) 설정. / Default behavior (including Fast Mode).

## Quick Start (No Install) / 빠른 시작(설치 없이)

```bash
node src/app.js
```

위 명령은 샘플 데모 실행으로, 핵심 아키텍처 동작을 콘솔에 보여줍니다.  
The command runs a sample demo and prints core architecture behavior to the console.

## CONTRIBUTION / 기여 안내

Ralph IDE는 단순한 기능 추가보다 **일관성, 감사 가능성, 다국어 문서 품질(한국어/영어)**을 중시합니다.  
Ralph IDE values **consistency, auditability, and high-quality bilingual documentation (Korean/English)** over raw feature volume.

### 1) 개발 원칙 / Development Principles

- 모든 코드 주석은 한국어/영어 병기로 작성합니다.  
  All code comments must be bilingual (Korean/English).
- Fast Mode 기본값을 유지하고, 변경 시 근거를 PR에 명시합니다.  
  Keep Fast Mode as default; justify any changes in the PR.
- Ralph Loops 감사/검토 훅을 우회하는 변경은 허용하지 않습니다.  
  Changes bypassing Ralph Loops audit/review hooks are not allowed.
- 신규 기능은 로컬 LLM + 클라우드 LLM 양쪽 사용 시나리오를 고려해야 합니다.  
  New features must consider both local and cloud LLM usage scenarios.

### 2) 커밋/PR 규칙 / Commit and PR Rules

- 작고 검증 가능한 단위로 커밋합니다.  
  Commit in small, verifiable increments.
- PR 본문에 다음 내용을 포함합니다.  
  Include the following in every PR body:
  - 변경 목적 / Change purpose
  - 설계 결정 / Design decisions
  - 검증 방법 / Validation method
  - 호환성 영향 / Compatibility impact

### 3) 문서 기준 / Documentation Standards

- README, CONTRIBUTING, NOTICE는 한국어/영어를 모두 포함해야 합니다(단, LICENSE 전문 제외).  
  README, CONTRIBUTING, and NOTICE must include both Korean and English (except LICENSE full text).
- 외부 소프트웨어 고지 변경 시 NOTICE를 반드시 갱신합니다.  
  Update NOTICE whenever external software notices change.

## NOTICE / 외부 고지

이 프로젝트는 Apache License 2.0을 따릅니다. 외부 구성요소 및 상표/라이선스 관련 고지는 `NOTICE` 파일을 참조하세요.  
This project is licensed under Apache License 2.0. See the `NOTICE` file for external component and trademark/license notices.

## LICENSE

Licensed under the Apache License, Version 2.0. See `LICENSE`.
