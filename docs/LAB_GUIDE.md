# Day 1 MDS2450 실습 가이드

## Lab 0. Ubuntu 26.04 Cross Toolchain

```bash
./scripts/install_ubuntu26.sh
make info
```

완료 기준: `arm-none-eabi-gcc`가 실행된다.

## Lab 1. Bare-metal Build

```bash
make clean
make CASE=0 OPT=O2
ls -lh output/MDS2450.bin
```

산출물:
- `.elf`: symbol/debug 정보 포함
- `.bin`: 보드 download용 raw binary
- `.map`: linker map
- `.lst`: source+disassembly

## Lab 2. UART / CP15 상태 확인

CASE0 binary를 실행한다. 예상 시작 로그:

```text
MDS2450 ARM926EJ Performance Lab
case=0 CASE0 MMU/CACHE OFF
cp15_c1_before=...
```

UART는 bootloader 설정을 기본으로 재사용한다. UART가 나오지 않으면 UART port/bootloader 설정 확인이 필요하다.

## Lab 3. Timer Baseline

`src/timer.c`는 원 교재의 Timer0 설정을 따른다.
- prescaler=255
- divider=1/16
- free-running 16-bit down counter

결과는 절대 시간보다 `ticks` 상대 비교에 사용한다.

## Lab 4. CASE1 - 모든 SDRAM NC/NB

```bash
make case1
```

`src/mmu.c`:

```c
map_sections(0x30000000u, 0x33F00000u, 0x30000000u, RW_NCNB);
```

실행 결과 5회를 저장한다.

## Lab 5. CASE2 - 원 교안 대표 설정

```bash
make case2
```

- Area1 `0x30000000~0x31FFFFFF`: C/B
- Area2 `0x32000000~0x33EFFFFF`: NC/NB
- Area3 `0x33F00000~0x33FFFFFF`: C/B

`mem_read`, `mem_write`는 Area2의 `0x32000000`을 사용하므로 CASE2에서 NC/NB 영향을 직접 관찰할 수 있다.

## Lab 6. CASE3 - 전체 SDRAM C/B

```bash
make case3
```

CASE2와 비교할 때 Area2 memory benchmark의 변화가 핵심이다.

## Lab 7. 5회 반복/Median

보드 프로그램 자체가 5회 수행하고 다음을 출력한다.

```text
CSV,2,mem_read,1,1234
...
MEDIAN,2,mem_read,1200
```

호스트 자동 저장:

```bash
python3 scripts/capture_serial.py --dev /dev/ttyUSB0 --baud 115200
```

## Lab 8. Compiler Optimization

```bash
make CASE=2 OPT=O0
make CASE=2 OPT=O2
make CASE=2 OPT=O3
```

비교 지표:
- binary size
- integer ticks
- memory ticks

`output/*.lst`에서 생성 assembly도 비교한다.

## 측정 주의

- UART 출력은 benchmark 구간 밖에서만 수행한다.
- Timer0 modulo 측정 범위는 약 65536 tick이다. 한 benchmark가 이 범위를 넘지 않도록 workload를 유지한다.
- Area2가 LCD framebuffer 등으로 실제 사용 중이면 `BENCH_REGION_ADDR`을 미사용 SDRAM 주소로 변경해야 한다.
- CASE3처럼 framebuffer/MMIO 성격의 메모리를 무조건 cacheable로 하는 것은 실제 제품의 정답이 아니다.

## Lab 9. Host에서 Median/개선율 자동 계산

CASE1/2/3 로그를 `results/*.csv`로 모은 뒤:

```bash
python3 scripts/analyze_results.py results/mds2450_*.csv
```

출력 예:

```text
case,benchmark,n,median_ticks,improvement_vs_case1_pct
1,mem_read,5,8200,0.00
2,mem_read,5,8100,1.22
3,mem_read,5,2100,74.39
```

실제 숫자는 장비와 workload에 따라 달라진다.
