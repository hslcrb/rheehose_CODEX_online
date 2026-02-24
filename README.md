# gitex

git + experience = **gitex**. 순수 C로 작성한 콘텐츠 중심 버전관리/TUI 프로토타입입니다.

## 구현된 방향
- Git과 독립된 저장소(`.gitex`) 구조
- content-addressed object 저장 (SHA-256)
- 문서 타입별 추적 전략 분리
  - XML: 의미 정규화 후 저장
  - DOCX/XLSX/HWPX 등 ZIP+XML: ZIP 내부 XML을 파싱/복원(inflate)하여 의미 스냅샷 생성
  - 바이너리: 청크 해시 기반 diff 리포트
- `reset --discard <file>`는 파일 삭제 대신 인덱스 상태를 `excluded`로 변경
- 기본 명령: init/add/status/commit/log/diff/branch/checkout/merge/reset/stash/tag/revert/tui

## 빌드
```bash
make
```

## 사용 예시
```bash
./gitex init
./gitex add .
./gitex status
./gitex commit -m "first"
./gitex log
./gitex diff old.mp4 new.mp4
./gitex reset --discard draft.docx
./gitex tui
```

## 내부 레이아웃
- `.gitex/objects`: 객체 저장소
- `.gitex/index`: 추적 메타데이터 (`tracked/excluded`)
- `.gitex/refs/*`: 브랜치 포인터
- `.gitex/commits/*.meta`: 커밋 메타
- `.gitex/tags/*`: 태그 포인터
- `.gitex/stash/*`: stash용 예약
