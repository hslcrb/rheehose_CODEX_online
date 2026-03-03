# CONTRIBUTING / 기여 가이드

Ralph IDE에 기여해 주셔서 감사합니다. 이 문서는 한국어/영어 병행 기준으로 작성됩니다.  
Thank you for contributing to Ralph IDE. This document is written in Korean/English parallel format.

## 1. 목표와 철학 / Goals and Philosophy

- Ralph IDE는 에이전틱 AI 기반 개발을 위한 Fast Mode 중심 워크플로우를 제공합니다.  
  Ralph IDE provides a Fast Mode-centered workflow for agentic AI-driven development.
- Ralph Loops는 코어 기능이며, 점검/감사/리뷰를 루프 수준에서 강제합니다.  
  Ralph Loops is a core feature that enforces inspection/audit/review at loop level.
- Markdown/PDF 렌더링은 기본 제공 기능입니다(확장 의존 최소화).  
  Markdown/PDF rendering is a built-in feature (minimal extension dependency).

## 2. 시작하기 / Getting Started

1. 저장소를 포크하고 브랜치를 생성합니다.  
   Fork the repository and create a feature branch.
2. 변경 범위를 작게 유지하고, 의도와 영향을 명확히 문서화합니다.  
   Keep change scope small and document intent and impact clearly.
3. 가능한 경우 실행 가능한 검증(예: `node src/app.js`)을 수행합니다.  
   Run executable verification where possible (e.g., `node src/app.js`).

## 3. 코드 작성 규칙 / Code Authoring Rules

- 모든 코드 주석은 한국어와 영어를 함께 작성합니다.  
  All code comments must include both Korean and English.
- Fast Mode를 기본 동작으로 유지합니다.  
  Preserve Fast Mode as the default behavior.
- Ralph Loops의 audit/review 강제 흐름을 약화시키지 않습니다.  
  Do not weaken Ralph Loops audit/review enforcement.
- 외부 확장 연동 시 Open VSX 및 직접 파일 설치 경로를 함께 고려합니다.  
  For extension integration, account for both Open VSX and direct file install paths.

## 4. AI 모델 연동 규칙 / AI Model Integration Rules

- 로컬 모델(Ollama, LM Studio)과 클라우드 API Key 모델을 동등한 1급 경로로 지원합니다.  
  Support local models (Ollama, LM Studio) and cloud API-key models as first-class paths.
- 공급자 추가 시 보안 민감정보(API Key)는 환경변수 사용을 기본으로 합니다.  
  For new providers, use environment variables for sensitive data (API keys).
- 모델 라우팅 변경은 회귀 위험을 명확히 기재합니다.  
  Clearly document regression risks for model-routing changes.

## 5. 문서와 고지 / Documentation and Notices

- README, CONTRIBUTING, NOTICE는 한국어/영어 병행 문서를 유지합니다(단 LICENSE 전문 제외).  
  Maintain bilingual Korean/English docs for README, CONTRIBUTING, NOTICE (except LICENSE full text).
- 외부 소프트웨어 사용, 상표, 라이선스 영향이 생기면 NOTICE를 갱신합니다.  
  Update NOTICE when external software, trademark, or license impacts arise.

## 6. 커밋/PR 체크리스트 / Commit/PR Checklist

- [ ] 변경 목적이 명확한가? / Is the change purpose clear?
- [ ] 설계 선택의 이유가 설명되었는가? / Are design choices explained?
- [ ] 검증 방법과 결과를 재현 가능하게 남겼는가? / Are validation steps and results reproducible?
- [ ] 문서(README/NOTICE/CONTRIBUTING) 영향이 반영되었는가? / Are doc impacts reflected?
- [ ] 한국어/영어 병행 작성 규칙을 충족하는가? / Does bilingual authoring requirement hold?

## 7. 커뮤니케이션 / Communication

- 이슈 및 PR에서는 사실 기반으로 간결하게 서술하되, 결정 근거를 반드시 남깁니다.  
  In issues and PRs, be concise and factual, and always include rationale.
- 리뷰 코멘트는 수정 제안 + 이유 + 기대 결과를 함께 제공합니다.  
  In review comments, include proposed change + reason + expected outcome.
