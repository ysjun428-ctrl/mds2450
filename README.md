# MDS2450 Performance Lab — Ubuntu 26.04 Cross Build

첨부 Day1 강의 교안의 MDS2450 파트를 **Ubuntu 26.04 + Arm GNU bare-metal toolchain**으로 재구성한 실습 코드다.

교안은 MDS2450에서 Timer 기반 성능 측정, ARM926EJ CP15 C1/C2, 1MB Level-1 Section Descriptor, `0x30000000~0x33FFFFFF`의 세 메모리 영역, CASE1/2/3 Cache/Write Buffer 비교를 수행한다. 이 프로젝트가 그 실습을 직접 실행하도록 구성되어 있다.

## 전제

- Target: MDS2450 / Samsung S3C2450 / ARM926EJ-S
- Bootloader가 SDRAM과 Clock을 이미 초기화한다.
- 프로그램 load/run address: `0x30000000`
- SDRAM: `0x30000000~0x33FFFFFF` 64 MiB 가정
- Console: UART0, 115200, **bootloader UART 설정 재사용**이 기본
- Timer0 register map 사용

## 1. Ubuntu 26.04 설치

```bash
./scripts/install_ubuntu26.sh
```

수동 설치:

```bash
sudo apt update
sudo apt install -y gcc-arm-none-eabi binutils-arm-none-eabi make gdb-multiarch picocom python3-serial
```

## 2. Build

```bash
make info
make CASE=2 OPT=O2
```

생성:

```text
output/MDS2450.bin
output/MDS2450_case2_O2.elf
output/MDS2450_case2_O2.bin
output/MDS2450_case2_O2.map
output/MDS2450_case2_O2.lst
```

## 3. CASE 실습

```bash
make case0   # MMU/Cache off
make case1   # 모든 SDRAM NC/NB
make case2   # Area1+3 C/B, Area2 NC/NB
make case3   # 모든 SDRAM C/B
```

전체:

```bash
make cases
```

## 4. 보드 실행

기존 Bootloader download 흐름을 사용한다.

```text
dnw 0x30000000
... MDS2450.bin transfer ...
go 0x30000000
```

또는 Bootloader가 `loadb`를 지원하면 Kermit 방식 사용.

자세한 내용: `docs/DOWNLOAD.md`

## 5. UART

```bash
./scripts/console.sh /dev/ttyUSB0 115200
```

자동 CSV 추출:

```bash
python3 scripts/capture_serial.py --dev /dev/ttyUSB0 --baud 115200
```

## 6. 코드 구조

```text
src/start.S   : stack/BSS 초기화, main 진입
src/cp15.S    : CP15 C1/C2, TLB/Cache 제어
src/mmu.c     : 4096-entry Level-1 identity mapping + CASE1/2/3
src/timer.c   : Timer0 free-running 성능 counter
src/uart.c    : UART0 polling output
src/bench.c   : integer/read/write/stride workloads
src/main.c    : 5회 반복 + Median + CSV 출력
include/mmu.h : Section Descriptor C/B Attribute
```

## 7. 수업에서 바로 쓰는 명령

```bash
# CASE1
make case1
# download/run, result capture

# CASE2
make case2
# download/run, result capture

# CASE3
make case3
# download/run, result capture
```

## 8. 중요한 확인 사항

1. **UART 포트**: 기존 자료는 UART0가 Host Console에 연결된 것으로 설명한다. 실제 보드 배선이 다른 경우 `s3c2450.h`를 조정해야 한다.
2. **Timer clock**: 결과는 `tick` 상대값이므로 PCLK 절대값에 덜 의존한다. 원 실습은 PCLK 66 MHz 예시를 사용한다.
3. **TCFG1 주소**: 본 코드는 Samsung 계열의 canonical PWM timer map인 `0x51000004`를 사용한다. 교육 문서 표에 offset 오기가 있는 경우 실제 S3C2450 manual/기존 `2450addr.h`로 최종 확인한다.
4. **Benchmark Area2**: 기본 `0x32000000`을 사용한다. LCD framebuffer가 실제 동작 중이면 다른 미사용 SDRAM 주소로 변경한다.
5. **실행 모델**: 이 binary는 NAND boot image가 아니라, **이미 초기화된 SDRAM에 download해서 `go 0x30000000`으로 실행하는 lab image**다.

## 9. PPT 대응

`docs/SLIDE_TO_LAB.md`를 사용한다.
