# Ralph IDE

Ralph IDE는 에이전트 중심(Agentic) AI 개발을 위해 설계된 경량 IDE 프로토타입입니다.  
Ralph IDE is a lightweight IDE prototype designed for agentic AI-driven development.

## 프로젝트 목표 / Project Goals
- Fast Mode를 기본 실행 모드로 제공하여 즉시 실행 가능한 개발 흐름을 지원합니다.  
  Provide Fast Mode as the default execution mode for immediate development workflows.
- Markdown/PDF 렌더링을 확장 설치 없이 기본 제공하여 문서 기반 개발을 가속화합니다.  
  Deliver built-in Markdown/PDF rendering without requiring extension installation.
- Open VSX 연동과 VSIX 직접 설치를 통해 확장 생태계 호환성을 제공합니다.  
  Offer extension ecosystem compatibility via Open VSX integration and direct VSIX installation.
- 로컬 모델(Ollama, LM Studio)과 클라우드 API 모델을 하나의 UI에서 구성합니다.  
  Configure local models (Ollama, LM Studio) and cloud API-based models from one UI.
- Ralph Loops를 코어 기능으로 내장하여 지속적 점검/감사/리뷰 루프를 수행합니다.  
  Integrate Ralph Loops as a core capability to execute continuous inspect/audit/review loops.

## 핵심 기능 / Core Features
1. **Fast 모드 우선 / Fast-first mode**
   - 기본값이 Fast Mode이며, Planning/Spec(SDD) 모드도 선택 가능하게 구성했습니다.  
     The default mode is Fast Mode, while Planning/Spec (SDD) modes remain selectable.

2. **내장 렌더러 / Built-in renderers**
   - Markdown 편집 및 실시간 렌더링을 기본 제공.  
     Native Markdown editing and live rendering included.
   - PDF 파일 업로드 후 즉시 미리보기 제공.  
     Instant PDF preview after upload.

3. **확장성 / Extensibility**
   - Open VSX API 검색 UI 제공.  
     UI for searching Open VSX API.
   - VSIX 파일 직접 가져오기 및 로컬 메타데이터 저장 제공.  
     Direct VSIX file import and local metadata persistence.

4. **AI 모델 연결 / AI model connectivity**
   - Ollama, LM Studio, Cloud API 모델 설정 저장 기능 제공.  
     Store configurations for Ollama, LM Studio, and cloud API models.

5. **Ralph Loops 내장 / Built-in Ralph Loops**
   - 반복 횟수 지정 또는 무한 반복 지원.  
     Supports finite iteration count or infinite loops.
   - 각 반복에서 inspect → audit → review → improve 흐름을 로깅.  
     Logs inspect → audit → review → improve flow each iteration.

## 실행 방법 / How to Run
1. 저장소 루트에서 정적 파일 서버를 실행합니다.  
   Run a static file server from the repository root.
2. `index.html`을 브라우저에서 엽니다.  
   Open `index.html` in a browser.

예시 / Example:
```bash
python3 -m http.server 8000
```

## 문서 안내 / Documentation Map
- `CONTRIBUTING.md`: 기여 정책 및 개발 워크플로 / Contribution policy and development workflow
- `NOTICE`: Apache 2.0 NOTICE 및 외부 소프트웨어 고지 / Apache 2.0 NOTICE and third-party notices
- `LICENSE`: Apache License 2.0 전문 / Full Apache License 2.0 text

## CONTRIBUTION / 기여
상세한 기여 가이드는 `CONTRIBUTING.md`를 참고하세요.  
For detailed contribution guidance, see `CONTRIBUTING.md`.

## NOTICE / 고지
상세 고지는 `NOTICE` 파일을 참고하세요.  
For detailed notices, see the `NOTICE` file.

## LICENSE
This project is licensed under the Apache License, Version 2.0. See `LICENSE`.
