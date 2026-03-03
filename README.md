# Ralph IDE (에이전트 중심 AI 개발 IDE)

## 개요 (Overview)
Ralph IDE는 VS Code 스타일의 사용자 경험을 목표로 하는 경량 참조 구현입니다. 에이전트 중심(AI-driven) 개발에 최적화된 워크플로우를 기본으로 제공하며, 별도 확장 설치 없이 Markdown 및 PDF 렌더링을 내장합니다.

Ralph IDE is a lightweight reference implementation targeting a VS Code-like user experience. It is optimized for agentic AI-driven workflows and includes built-in Markdown and PDF rendering without requiring extra extensions.

## 핵심 기능 (Core Features)
- **Fast Mode 우선 설계**: SDD(명세/계획) 중심 모드보다 즉시 실행 가능한 Fast Mode를 기본 동작으로 채택.
- **내장 렌더러**: Markdown 미리보기 + PDF 뷰어를 코어 기능으로 직접 제공.
- **Open VSX 연동**: Open VSX 검색/조회 기반 확장 탐색 경로를 제공.
- **로컬 확장 파일 설치**: `.vsix` 등 확장 파일 직접 임포트 지원.
- **AI 모델 라우팅**: Ollama / LM Studio(로컬), API Key 기반 클라우드 모델을 단일 인터페이스로 관리.
- **Ralph Loops 내장**: 확장 설치 없이 반복 기반 점검/감사/리뷰 루프를 코어 엔진에서 직접 제공.

- **Fast Mode first**: Prioritizes immediate execution over spec/planning-heavy SDD flows.
- **Built-in renderers**: Native Markdown preview and PDF viewer in core.
- **Open VSX integration**: Extension discovery path backed by Open VSX API.
- **Local extension import**: Direct install workflow for extension package files.
- **AI model routing**: Unified management for local (Ollama/LM Studio) and cloud API-key models.
- **Ralph Loops built-in**: Native iterative inspection/audit/review control at software level.

## 빠른 시작 (Quick Start)
> 설치 명령 없이 동작하도록 정적 파일 기반으로 작성되었습니다.

> Implemented as static files so it can run without installation commands.

1. `index.html` 파일을 브라우저로 엽니다.
2. 좌측 패널에서 파일을 작성하고, 중앙 편집기에서 코드를 입력합니다.
3. 우측 패널의 `Markdown Preview` 또는 `PDF Viewer`를 사용합니다.
4. AI 설정 패널에서 Local/Cloud 모델을 등록합니다.
5. Ralph Loops에서 반복 횟수(숫자 또는 `infinite`)를 설정하고 실행합니다.

1. Open `index.html` in a browser.
2. Create files in the left panel and write code in the center editor.
3. Use `Markdown Preview` or `PDF Viewer` in the right panel.
4. Register local/cloud model settings in AI configuration panel.
5. Set Ralph Loops iteration (`number` or `infinite`) and run.

## 프로젝트 구조 (Project Structure)
- `index.html`: IDE 레이아웃 및 UI 구성 요소.
- `styles.css`: VS Code 스타일 기반 UI 테마.
- `src/app.js`: 에디터 상태, 렌더러, 확장/AI/루프 코어 로직.
- `NOTICE`: 외부 소프트웨어 고지 및 라이선스 통지.
- `CONTRIBUTING.md`: 기여 가이드라인.
- `LICENSE`: Apache License 2.0.

- `index.html`: IDE layout and UI elements.
- `styles.css`: VS Code-inspired theme.
- `src/app.js`: Editor state, renderers, extension/AI/loop core logic.
- `NOTICE`: External software and license notices.
- `CONTRIBUTING.md`: Contribution guidelines.
- `LICENSE`: Apache License 2.0.

## Ralph Loops 설명 (Ralph Loops Details)
Ralph Loops는 제품 개발이 산발적 태스크로 분산되지 않도록, 단일 제품 품질을 향한 연속적 점검 루프를 강제합니다.

Ralph Loops enforce continuous quality loops that keep work centered on a single product outcome, rather than fragmented tasks.

- 반복 횟수는 정수 또는 무한(`infinite`)을 허용합니다.
- 각 루프에서 `inspection -> auditing -> review` 단계를 순차 수행합니다.
- 루프 로그를 통해 팀은 변경 이력과 품질 게이트 통과 여부를 추적할 수 있습니다.

- Iteration count supports integers or `infinite`.
- Each loop executes `inspection -> auditing -> review` in sequence.
- Loop logs allow teams to track changes and quality-gate progression.

## 라이선스 (LICENSE)
Licensed under the Apache License, Version 2.0. See `LICENSE`.

## 기여 (CONTRIBUTION)
자세한 내용은 `CONTRIBUTING.md`를 참고하세요.

See `CONTRIBUTING.md` for detailed contribution policies.

## 고지 (NOTICE)
외부 소프트웨어 고지 및 저작권/상표 통지는 `NOTICE` 파일을 참고하세요.

See `NOTICE` for external software notices and attribution details.
