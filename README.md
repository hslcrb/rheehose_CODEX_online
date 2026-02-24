# gitex

`gitex`는 XML 기반 문서(docx/xlsx/hwpx 포함)와 바이너리 파일을 콘텐츠 중심으로 추적하기 위한 순수 C 기반 버전관리 TUI/CLI 프로토타입입니다.

## 주요 설계

- `.gitex` 내부에 독립 저장소 구성 (Git과 분리)
- 콘텐츠 주소 기반 객체 저장소(`objects`)
- 추적 상태와 파일 존재를 분리
  - `gitex reset --discard <path>`는 파일을 삭제하지 않고 추적에서만 제외
- XML diff 토큰 분석 / 바이너리 chunk hash diff
- Git 유사 명령 제공:
  - init, add, commit, status, diff, log
  - branch, checkout, merge, reset
  - stash, tag, revert

## 빌드

```bash
make
# ncurses TUI 포함
make USE_NCURSES=1
```

## 빠른 사용 예시

```bash
./gitex init
./gitex add example.xml image.bin
./gitex commit -m "first"
./gitex status
./gitex diff a.bin b.bin
./gitex reset --discard example.xml
```
