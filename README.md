# gitex

`gitex`는 Git의 사용자 경험을 확장(experience)하는 콘텐츠 중심 버전관리 실험 프로젝트입니다.

## 특징
- 순수 C11 기반 CLI/TUI.
- `.docx`, `.xlsx`, `.hwpx` 같은 ZIP+XML 포맷을 별도 타입으로 분리 추적.
- XML 구조 변화(노드/속성 변화량) 기반 diff.
- 이미지/동영상 등 바이너리를 청크 해시 기반으로 diff.
- 파일 존재 여부와 추적 상태를 분리:
  - `add --discard <path>`는 파일을 지우지 않고 추적만 제외합니다.
- Git과 독립적인 `.gitex` 메타데이터 저장소를 사용.

## 빌드
```bash
make
# ncurses TUI 활성화
make USE_NCURSES=1
```

## 주요 명령
```bash
gitex init
gitex add <file>...
gitex add --discard <file>
gitex commit -m "message"
gitex status
gitex diff [file]
gitex log
gitex branch <name>
gitex checkout <name>
gitex stash [list]
gitex tag <name>
gitex tui
```

## 내부 저장 모델
- `objects/`: 콘텐츠 주소 기반 객체 저장.
- `index`: 추적 메타데이터(상태/타입/해시/mtime/size).
- `refs/heads`, `refs/tags`, `HEAD`: 커밋 그래프 진입점.

## 방어적 설계 메모
- `reset --hard`, `merge`, `revert`는 현재 스캐폴딩 단계에서 위험 작업을 의도적으로 제한.
- 파괴적 동작보다 메타데이터 전환을 우선해 데이터 손실 가능성을 낮춤.
