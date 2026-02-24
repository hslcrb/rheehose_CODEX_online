# NTC (New Technology Compiler)

NTC는 **GCC 100% 대체**를 목표로 하는 컴파일러 프로젝트이며, 현재 저장소의 구현은 **전부 C 언어**로 작성되어 있습니다.

## 현재 구현 범위

- 단일 실행 파일 `ntc` (C11)
- GCC/Clang 자동 감지 후 드라이버 동작
- GCC 스타일 핵심 옵션 처리
  - `-c`, `-E`, `-S`, `-o`, `-O*`, `-g`, `-std=*`, `-I`, `-D`, `-W*`, `-f*`, `-m*`, `-L`, `-l`, `-Wl,*`
- 멀티소스 병렬 컴파일 (`-j`)
- 소스 내용 + 플래그 + 컴파일러 기준 캐시 (`.ntc-cache`)

## 빌드

```bash
make
```

## 사용 예시

```bash
./ntc hello.c -O2 -o hello
./ntc -c hello.c -O3 -o hello.o
./ntc a.c b.c -j 8 -O2 -o app
```

## 테스트

```bash
make test
```

## 프로젝트 방향

- 최종 목표: GCC 기능 100% 대체
- 본 구현은 드라이버/캐시/병렬화 기반을 C로 고정하여, 이후 프론트엔드/최적화/백엔드를 순차적으로 확장하는 구조입니다.
