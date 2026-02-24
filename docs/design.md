# gitex 아키텍처

## 1. 목표
- 콘텐츠 중심(content-addressed) 객체 저장
- 포맷별 추적 전략 분리
- 추적 상태를 파일 존재와 독립적으로 관리

## 2. 데이터 모델
- **Index Entry**: `{path, hash, type, tracked}`
- **Blob Object**: 파일 원본 콘텐츠 (SHA-256 키)
- **Commit Object**: 스냅샷 메타 + 파일 맵

## 3. 포맷별 전략
### XML 계열 (docx/hwpx/xlsx/xml)
- 현재 구현: XML 텍스트 토큰화 기반 시그니처 생성
- 확장 지점: ZIP 컨테이너 내부 XML 엔트리 파싱 후 노드 수준 semantic diff

### 바이너리 (이미지/동영상)
- 고정 청크(4KiB) 단위 SHA-256 시그니처 생성
- 시그니처 간 비교로 변화량을 해석
- 확장 지점: 롤링 해시/가변 청크(CDC) 기반 개선

## 4. 명령 흐름
- `init`: `.gitex` 스토리지 초기화
- `add`: 인덱스에 tracked=true로 등록 + blob 저장
- `commit`: tracked 엔트리 스냅샷 커밋 생성
- `status`: 워킹트리 vs 인덱스 비교
- `diff`: 포맷별 전략으로 의미 기반 차이 리포트
- `discard`: tracked=false 처리 (파일 삭제 없음)

## 5. 방어적 설계
- 모든 파일 I/O 결과 코드 검사
- 인덱스 동적 배열은 capacity 확장 시 검사
- 해시 계산/객체 저장 실패 시 즉시 중단

## 6. 향후 로드맵
1. ZIP 로우 레벨 파서 구현(중앙 디렉터리/로컬 헤더)
2. XML AST 기반 구조 diff 엔진
3. commit graph, branch refs, merge base 계산
4. TUI 고도화(ncurses 패널, hunk stage UI)
