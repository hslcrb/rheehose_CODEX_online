# CONTRIBUTION / 기여 가이드

이 문서는 Ralph IDE 프로젝트에 기여하는 방법을 설명합니다.  
This document describes how to contribute to the Ralph IDE project.

## 1. 기본 원칙 / Core Principles

1. **이중 언어 원칙 / Bilingual-first policy**
   - LICENSE 섹션을 제외한 문서 텍스트는 한국어와 영어를 함께 제공합니다.
   - Except for the LICENSE section, documentation text must be provided in both Korean and English.
   - 코드 주석도 한국어+영어를 함께 작성합니다.
   - Code comments must also include both Korean and English.

2. **Fast 모드 우선 / Fast mode priority**
   - 기본 사용자 경험은 Fast 모드를 중심으로 설계합니다.
   - The default UX should prioritize Fast mode.
   - Planning/SDD 관련 기능은 보조 옵션이어야 하며 기본값이 되어서는 안 됩니다.
   - Planning/SDD features are optional and must not override the default.

3. **Ralph Loops 내장성 / Ralph Loops as core capability**
   - Ralph Loops는 확장 기능이 아니라 코어 기능으로 유지해야 합니다.
   - Ralph Loops must remain a core feature, not an extension.
   - 반복 실행 시 검사(Inspect)-감사(Audit)-리뷰(Review) 흐름을 유지해야 합니다.
   - Iterative execution must preserve the inspect-audit-review flow.

4. **확장 생태계 상호운용 / Extension ecosystem interoperability**
   - Open VSX 검색 연동과 VSIX 직접 가져오기 흐름을 유지해야 합니다.
   - Keep Open VSX search integration and direct VSIX import workflow intact.

## 2. 개발 및 변경 절차 / Development and Change Process

1. 이슈를 명확히 정의합니다.
   / Define the issue clearly.
2. 가능한 경우 작은 단위로 변경합니다.
   / Prefer small, reviewable changes.
3. UI/동작 변경 시 README와 관련 문서를 함께 업데이트합니다.
   / Update README and related docs with UI/behavior changes.
4. 외부 소프트웨어를 추가/변경하면 NOTICE를 반드시 갱신합니다.
   / Update NOTICE whenever external software is added/changed.
5. 커밋 메시지는 변경 의도를 명확히 작성합니다.
   / Write commit messages that clearly explain intent.

## 3. 코드 스타일 / Code Style

- 주석은 한국어/영어 병기.
  / Comments must be bilingual (Korean/English).
- 과도한 추상화보다 명시적이고 읽기 쉬운 구현을 우선.
  / Prefer explicit readability over unnecessary abstraction.
- 사용자 설정(예: 모델 엔드포인트/API 키)은 로컬 저장소에 저장하되, 보안 경고를 문서화.
  / Store user settings locally but document security cautions.

## 4. 테스트 가이드 / Testing Guidelines

- 정적 실행 확인: `python -m http.server 4173` 후 UI 동작 수동 검증
  / Static run check: launch `python -m http.server 4173` and validate UI behavior manually.
- 필수 확인 항목 / Required checks:
  - 탭 전환 (Code/Markdown/PDF)
    / Tab switching (Code/Markdown/PDF)
  - Markdown 실시간 렌더링
    / Real-time Markdown rendering
  - PDF 파일 로딩 및 프리뷰
    / PDF loading and preview
  - Open VSX 검색 응답 처리
    / Open VSX search response handling
  - VSIX 파일 가져오기 리스트 반영
    / VSIX import reflected in extension list
  - Ralph Loop 반복 횟수/무한 모드 상태 표시
    / Ralph Loop finite/infinite mode status display

## 5. 문서화 체크리스트 / Documentation Checklist

PR 또는 변경 제출 전에 아래를 확인하세요.  
Before submitting changes or PRs, confirm the following.

- [ ] README가 최신 동작을 반영하는가?
  / Does README reflect current behavior?
- [ ] NOTICE 외부 소프트웨어 항목이 최신인가?
  / Is NOTICE up to date for external software?
- [ ] 코드 주석이 한국어+영어 병기인가?
  / Are code comments bilingual?
- [ ] Fast 모드 기본값이 유지되는가?
  / Is Fast mode still the default?
- [ ] Ralph Loops가 코어 로직에 통합되어 있는가?
  / Are Ralph Loops integrated as core logic?

## 6. 커뮤니티 행동 지침 / Community Conduct

- 상호 존중과 건설적 피드백을 준수합니다.
  / Follow mutual respect and constructive feedback.
- 설계 비판은 사람 아닌 코드/결정에 집중합니다.
  / Focus criticism on code/decisions, not people.
- 논쟁 이슈는 재현 가능한 근거(로그, 화면, 단계)를 함께 제시합니다.
  / Provide reproducible evidence (logs/screens/steps) for disputed issues.

감사합니다. Ralph IDE를 함께 개선해 주세요.  
Thank you for helping improve Ralph IDE.
