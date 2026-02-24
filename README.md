# gitex

`gitex`는 Git의 경험(experience)을 확장해 XML 기반 문서 포맷(docx/hwpx/xlsx)과 바이너리(이미지/동영상)를 **내용 단위**로 추적하는 순수 C 기반 TUI/CLI 버전관리 도구입니다.

## 핵심 차별점
- 파일 존재와 추적 상태를 분리 (`discard`는 파일 삭제가 아닌 untrack 처리)
- XML/ZIP 기반 문서를 구조적으로 비교할 수 있는 확장 지점 제공
- 바이너리를 청크 해시 기반으로 비교하는 diff 파이프라인
- Git과 독립적인 `.gitex` 저장소 구조

## 빌드
```bash
make
```

## 기본 사용
```bash
./gitex init
./gitex add docs/spec.xml
./gitex commit "first"
./gitex status
./gitex diff docs/spec.xml
./gitex discard docs/spec.xml
./gitex tui
```

## 저장소 구조
- `.gitex/objects`: 콘텐츠 객체 저장
- `.gitex/index`: 추적 메타데이터
- `.gitex/commits.log`: 커밋 로그
- `.gitex/HEAD`, `.gitex/refs`: 브랜치 포인터 확장 지점

## 구현 상태
- 동작 구현: `init/add/status/commit/log/diff/discard/tui`
- 스텁 제공: `branch/checkout/merge/reset/stash/tag/revert`

상세 설계는 `docs/design.md`를 참고하세요.
