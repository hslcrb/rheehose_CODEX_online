# CONTRIBUTING / 기여 가이드

## 목적 / Purpose
이 문서는 Ralph IDE 프로젝트에 기여하는 모든 참여자가 동일한 품질 기준, 협업 프로세스, 법적 요구사항을 따를 수 있도록 설계되었습니다.

This document ensures all contributors follow the same quality standards, collaboration process, and legal requirements for Ralph IDE.

## 기본 원칙 / Core Principles
1. **단일 제품 집중 / Single-product focus**
   - 모든 변경은 Ralph IDE의 일관된 제품 경험 강화에 기여해야 합니다.
   - All changes must strengthen one coherent Ralph IDE product experience.
2. **Fast Mode 우선 / Fast Mode first**
   - 설계 문서 중심 절차보다 실행 가능한 결과를 먼저 제공합니다.
   - Favor executable outcomes over heavyweight spec-first processes.
3. **내장 기능 우선 / Built-in capability first**
   - Markdown/PDF/Ralph Loops 같은 핵심 기능은 코어 코드로 유지합니다.
   - Keep core capabilities (Markdown/PDF/Ralph Loops) in built-in code rather than external dependency layers.
4. **이중 언어 유지 / Bilingual consistency**
   - 문서, 이슈 설명, 코드 주석은 한국어와 영어를 함께 제공합니다.
   - Provide both Korean and English for documentation, issue descriptions, and code comments.

## 기여 절차 / Contribution Workflow
1. **이슈 확인 / Review issues**
   - 기존 이슈/로드맵과 중복 여부를 먼저 확인합니다.
   - Check existing issues/roadmap to avoid duplicates.
2. **브랜치 생성 / Create branch**
   - `feat/*`, `fix/*`, `docs/*`, `refactor/*` 네이밍을 사용합니다.
   - Use branch names like `feat/*`, `fix/*`, `docs/*`, `refactor/*`.
3. **구현 / Implement**
   - 변경 범위를 명확히 제한하고, 커밋 단위를 작게 유지합니다.
   - Keep scope explicit and commits small.
4. **검증 / Validate**
   - 실행 가능한 검증(정적 점검/테스트/수동 검증)을 수행합니다.
   - Perform executable checks (lint/tests/manual validation).
5. **PR 제출 / Submit PR**
   - 변경 목적, 영향도, 검증 결과를 명시합니다.
   - State motivation, impact, and validation evidence.

## 코드 스타일 / Code Style
- 주석은 한국어+영어 병기. / Write comments in Korean + English.
- 의미 있는 함수명/변수명 사용. / Use meaningful names.
- 하드코딩 최소화. / Minimize hardcoded values.
- 핵심 동작(루프 제어, 모델 라우팅)은 함수로 분리. / Isolate core behavior (loop control/model routing) into functions.

## 커밋 메시지 규칙 / Commit Message Rules
- 권장 형식: `type(scope): summary`
- 예시 / Examples:
  - `feat(core): add built-in Ralph loop controller`
  - `docs(legal): add bilingual NOTICE and CONTRIBUTING`

## Pull Request 체크리스트 / Pull Request Checklist
- [ ] 변경 내용이 단일 목적에 집중되어 있는가?
- [ ] Fast Mode 동작이 유지되는가?
- [ ] Markdown/PDF 내장 기능이 손상되지 않았는가?
- [ ] Ralph Loops 반복/무한 동작이 검증되었는가?
- [ ] 로컬/클라우드 모델 설정 흐름이 유지되는가?
- [ ] 문서/주석이 한국어+영어로 작성되었는가?

- [ ] Is the change focused on one clear outcome?
- [ ] Is Fast Mode behavior preserved?
- [ ] Are built-in Markdown/PDF capabilities intact?
- [ ] Are finite/infinite Ralph Loops validated?
- [ ] Is local/cloud model configuration flow preserved?
- [ ] Are docs/comments bilingual (Korean + English)?

## 법적 및 라이선스 요구사항 / Legal and Licensing Requirements
- 모든 기여 코드는 Apache License 2.0 호환이어야 합니다.
- 신규 외부 라이브러리/자산 도입 시 라이선스 호환성과 NOTICE 반영 여부를 반드시 확인하세요.
- 저작권/상표/특허 관련 민감한 자료는 PR 본문에서 출처와 권리를 명확히 고지하세요.

- All contributed code must be compatible with Apache License 2.0.
- For any new external library/asset, verify compatibility and update NOTICE.
- Clearly disclose source and rights for copyright/trademark/patent-sensitive materials.

## 보안 가이드 / Security Guidance
- 실제 API Key, 토큰, 민감정보를 커밋하지 않습니다.
- 샘플 값은 `example`, `mock`, `placeholder` 형태를 사용합니다.
- 보안 취약점 제보는 공개 이슈가 아니라 비공개 채널(프로젝트 관리자가 지정한 경로)로 전달합니다.

- Never commit real API keys/tokens/secrets.
- Use `example`, `mock`, or `placeholder` values.
- Report vulnerabilities via private channels designated by project maintainers.
