# gitex architecture (C11)

## Core principles
1. **Content-addressed storage**: 모든 스냅샷 결과는 SHA-256으로 주소화.
2. **Type-specific tracking**:
   - XML: semantic normalization 후 저장.
   - ZIP 기반 XML 문서(docx/hwpx/xlsx): ZIP local entry를 직접 순회해 XML을 추출/정규화.
   - Binary: fixed chunk hash로 delta 통계 생성.
3. **Tracking state decoupled from filesystem**:
   - 파일이 존재해도 index에서 `excluded`면 추적 안 됨.
   - `reset --discard`는 파일 미삭제.

## Object model (gitex 고유)
- Blob object: 원본/의미 스냅샷 데이터
- Manifest blob(tree 유사): `path|blob_hash|kind`
- Commit meta:
  - tree
  - parent
  - branch
  - time
  - message

## Modules
- `src/hash.c`: SHA-256
- `src/object_store.c`: 객체 저장/조회
- `src/index.c`: 추적 메타데이터 직렬화
- `src/xml_semantic.c`: XML 의미 정규화
- `src/zip_xml.c`: ZIP + XML 집계
- `src/binary_diff.c`: 청크 기반 diff 통계
- `src/commands.c`: VCS 명령 구현
- `src/tui.c`: ncurses 상태 뷰
