# 기여 가이드 / Contribution Guide

Ralph IDE 프로젝트에 기여해 주셔서 감사합니다.  
Thank you for contributing to the Ralph IDE project.

## 1) 목적 / Purpose
- 본 문서는 Ralph IDE의 코드, 문서, 설계 개선에 참여하는 방법을 정의합니다.  
  This document defines how to contribute code, documentation, and design improvements to Ralph IDE.
- 본 프로젝트는 에이전트 중심 개발 생산성 향상과 신뢰 가능한 반복 개선 루프 구현을 목표로 합니다.  
  This project aims to improve agentic development productivity and implement reliable iterative improvement loops.

## 2) 언어 정책 / Language Policy
- **문서와 코드 주석은 한국어와 영어를 함께 작성**해야 합니다.  
  **Documentation and code comments must be written in both Korean and English.**
- 사용자 UI 텍스트도 가능하면 이중 언어를 유지합니다.  
  UI text should also remain bilingual whenever feasible.

## 3) 기여 유형 / Contribution Types
- 버그 수정 / Bug fixes
- 성능 개선 / Performance improvements
- UI/UX 개선 / UI/UX enhancements
- 문서 개선 / Documentation updates
- 보안/안정성 개선 / Security and reliability improvements
- Ralph Loops, 모델 연동, 렌더링 파이프라인 고도화 / Ralph Loops, model integration, rendering pipeline enhancements

## 4) 개발 원칙 / Development Principles
1. Fast Mode 기본 경험을 훼손하지 않습니다.  
   Do not degrade the default Fast Mode experience.
2. Markdown/PDF 기본 기능은 확장 의존 없이 유지합니다.  
   Keep Markdown/PDF baseline functionality independent from extensions.
3. Open VSX 및 VSIX 직접 설치 경로는 명확하고 안전하게 유지합니다.  
   Keep Open VSX and direct VSIX paths clear and safe.
4. Ralph Loops는 코어 기능으로 유지하며, 반복 제어의 신뢰성을 보장합니다.  
   Preserve Ralph Loops as a core feature and ensure reliable iteration controls.

## 5) 작업 절차 / Workflow
1. 이슈를 생성하거나 기존 이슈를 확인합니다.  
   Create an issue or review an existing one.
2. 브랜치를 생성하고 작업 단위를 작게 유지합니다.  
   Create a branch and keep change sets small.
3. 커밋 메시지는 변경 의도를 명확히 설명합니다.  
   Use clear commit messages that explain intent.
4. 가능한 검증(실행/테스트/린트)을 수행하고 결과를 공유합니다.  
   Run available validation (run/test/lint) and share outcomes.
5. PR에는 변경 요약, 리스크, 향후 과제를 기록합니다.  
   Include summary, risks, and follow-up items in the PR.

## 6) 코드 스타일 / Code Style
- 단순하고 읽기 쉬운 구조를 우선합니다.  
  Prefer simple and readable structures.
- 주석은 “왜(why)”를 설명하고, 구현 세부는 코드로 표현합니다.  
  Comments should explain “why,” while code expresses implementation details.
- 새 기능에는 최소한의 상태 모델과 오류 처리 흐름을 제공합니다.  
  New features should include a minimal state model and error handling flow.

## 7) 보안 지침 / Security Guidance
- API 키는 평문 저장을 피하고 마스킹/비노출 원칙을 따릅니다.  
  Avoid plaintext API key storage and follow masking/non-disclosure principles.
- 사용자 업로드 파일(VSIX/PDF)은 신뢰하지 않고 검증 가능한 범위에서만 처리합니다.  
  Treat user-uploaded files (VSIX/PDF) as untrusted and process only within safe bounds.

## 8) 라이선스 및 고지 / License and Notice
- 기여 코드는 Apache 2.0 라이선스를 따릅니다.  
  Contributed code is licensed under Apache 2.0.
- 외부 소프트웨어를 추가하는 경우 `NOTICE`를 업데이트해야 합니다.  
  If third-party software is added, the `NOTICE` file must be updated.

## 9) PR 체크리스트 / PR Checklist
- [ ] 문서와 코드 주석이 한국어/영어로 작성되었는가? / Are docs and code comments bilingual?
- [ ] Fast Mode 기본 동작을 유지하는가? / Is Fast Mode default behavior preserved?
- [ ] Markdown/PDF 기본 렌더링이 동작하는가? / Does Markdown/PDF built-in rendering work?
- [ ] Ralph Loops 반복/무한 모드가 동작하는가? / Do Ralph Loops finite/infinite modes work?
- [ ] NOTICE/README/CONTRIBUTING 업데이트가 필요한가? / Are NOTICE/README/CONTRIBUTING updates needed?
