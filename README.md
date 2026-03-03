# Ralph IDE

## 프로젝트 개요 / Project Overview
Ralph IDE는 VS Code 스타일의 인터페이스를 가진, 에이전트 중심 AI 개발에 최적화된 경량 IDE 프로토타입입니다.  
Ralph IDE is a lightweight IDE prototype with a VS Code-like interface, optimized for agentic AI-driven development.

이 저장소는 설치 없이 실행 가능한 브라우저 기반 구현으로 다음 요구사항을 기본 내장합니다.  
This repository contains a no-install browser implementation with the following built-in capabilities.

- Fast 모드를 기본 작업 모드로 제공 (Planning/SDD 모드보다 우선) / Fast mode as the default workflow (preferred over Planning/SDD)
- 확장 설치 없이 Markdown 렌더링 내장 / Built-in Markdown rendering without extension installation
- 확장 설치 없이 PDF 프리뷰 내장 / Built-in PDF preview without extension installation
- Open VSX 검색 연동 / Open VSX extension search integration
- 사용자의 VSIX 파일 직접 가져오기 / Direct user VSIX file import
- 로컬/클라우드 LLM 연결 설정 / Local and cloud LLM connection settings
  - 로컬: Ollama, LM Studio / Local: Ollama, LM Studio
  - 클라우드: API Key 기반 OpenAI/Anthropic/Custom / Cloud: API key-based OpenAI/Anthropic/Custom
- Ralph Loops 내장 (확장 필요 없음) / Ralph Loops built in (no extension required)
  - 무한 반복 또는 지정 반복 횟수 / Infinite loop or fixed iteration count
  - 소프트웨어 검사·감사·리뷰 단계 강제 / Enforced inspect/audit/review cycle

## 실행 방법 / How to Run
별도 설치 없이 정적 파일 서버로 실행할 수 있습니다.  
You can run it with a static server without additional installation.

```bash
python -m http.server 4173
```

브라우저에서 `http://localhost:4173` 접속.  
Open `http://localhost:4173` in your browser.

## 아키텍처 / Architecture
- `index.html`: 레이아웃, 모드/모델/확장/루프 UI  
  `index.html`: Layout and mode/model/extension/loop UI
- `styles.css`: VS Code 유사 다크 테마 스타일  
  `styles.css`: VS Code-like dark theme styles
- `app.js`: 핵심 동작 로직 (탭, Markdown/PDF, Open VSX, VSIX, 모델 설정, Ralph Loop)  
  `app.js`: Core behavior (tabs, Markdown/PDF, Open VSX, VSIX, model settings, Ralph Loop)

## CONTRIBUTION / 기여 안내
아래 상세 가이드는 `CONTRIBUTING.md`를 참고하세요.  
Please refer to `CONTRIBUTING.md` for full contribution guidelines.

핵심 원칙 / Core principles:
- 모든 문서와 코드 주석은 한국어+영어 병기 / All docs and code comments must be bilingual (Korean + English)
- Fast 모드 우선 / Fast mode first
- Ralph Loops의 검사·감사·리뷰 흐름 보존 / Preserve Ralph Loop inspect/audit/review flow
- Open VSX 및 VSIX 사용자 워크플로우 훼손 금지 / Do not break Open VSX and user VSIX workflows

## NOTICE / 고지
외부 소프트웨어 고지 및 저작권/상표/책임 제한 정보는 `NOTICE` 파일을 참고하세요.  
For external software notices, copyright/trademark attributions, and limitation statements, see the `NOTICE` file.

## LICENSE
Apache License 2.0 (`LICENSE` 파일 참조)
