# Source alignment notes

이 프로젝트는 기존 교육자료의 다음 요소와 맞춘다.

- `MDS2450.bin`을 `0x30000000`에서 실행하는 ARMControl 실습 흐름
- ARM926EJ CP15 C1: bit0 MMU, bit2 D-cache, bit12 I-cache
- CP15 C2: Level-1 Translation Table Base
- 1MB Section Descriptor: AP/Domain/fixed bit4/C/B/type `10`
- 세 영역:
  - `0x30000000~0x31FFFFFF`
  - `0x32000000~0x33EFFFFF`
  - `0x33F00000~0x33FFFFFF`
- 대표 CASE2: Area1/3 `RW_CB`, Area2 `RW_NCNB`
- Timer0: prescaler 255, divider 1/16, Timer Count 기반 측정
- UART0 / GPIO / Timer register addresses는 기존 S3C2450 디바이스 제어 자료를 기반으로 한다.

현대 GCC로 빌드하기 위해 startup/linker/CP15 assembly는 새로 작성했다.
