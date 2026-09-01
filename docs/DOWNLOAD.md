# MDS2450 다운로드/실행

이 프로젝트는 **Bootloader가 SDRAM과 Clock을 이미 초기화한 상태**에서 `0x30000000`으로 프로그램을 올려 실행하는 방식이다.

기존 실습 자료의 실행 주소는 `0x30000000`이다.

## 방식 A: 기존 USB DNW 경로

Bootloader에서:

```text
dnw 0x30000000
```

호스트에서 `output/MDS2450.bin`을 전송하고:

```text
go 0x30000000
```

## 방식 B: Bootloader Kermit 경로

Bootloader가 `loadb`를 지원하면:

```text
loadb 0x30000000
```

Kermit 송신 도구로 `output/MDS2450.bin`을 전송한 뒤:

```text
go 0x30000000
```

Ubuntu 26.04에서 **실제 전송 프로그램은 현재 MDS2450에 탑재된 Bootloader 명령과 USB download protocol 확인이 필요하다.**
빌드와 UART 모니터링은 본 패키지로 독립적으로 수행할 수 있다.
