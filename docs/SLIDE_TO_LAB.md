# Day1 PPT ↔ 코드 매핑

| PPT | 주제 | 코드/명령 |
|---|---|---|
| 12 | ARMControl 프로젝트 | 이 패키지 전체 구조 |
| 13 | Build 파이프라인 | `make clean && make CASE=2 OPT=O2` |
| 14 | Timer 성능 측정 | `src/timer.c`, `src/main.c::run_one()` |
| 17 | CP15 | `src/cp15.S` |
| 18 | MMU 초기화 | `src/mmu.c::mmu_enable_case()` |
| 19 | Section Descriptor | `include/mmu.h` |
| 20 | 3개 메모리 영역 | `src/mmu.c::mmu_build_identity_table()` |
| 21 | C/B Attribute | `RW_NCNB`, `RW_CB` in `include/mmu.h` |
| 22 | CP15 C2 | `cp15_set_ttbr()` |
| 23 | CP15 C1 | `cp15_get_control()/set_control()` |
| 24 | CASE1 | `make case1` |
| 25 | CASE2 | `make case2` |
| 26 | CASE3 | `make case3` |
| 27 | 5회/Median | UART `CSV`/`MEDIAN` output + `capture_serial.py` |
| 28 | 결과 해석 | integer/read/write/stride benchmark 비교 |

## 수업 권장 실행 순서

```bash
make clean
make case1
# MDS2450에 output/MDS2450.bin 다운로드 → go 0x30000000

make case2
# 다운로드/실행/로그 저장

make case3
# 다운로드/실행/로그 저장
```

컴파일러 최적화 비교:

```bash
make CASE=2 OPT=O0
make CASE=2 OPT=O2
make CASE=2 OPT=O3
```
