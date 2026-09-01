# Windows/WSL 기반 MDS2450 개발 환경 설정

이 문서는 Windows 11의 WSL2에서 MDS2450 바이너리를 빌드하고, TFTP로 내려받고, USB 시리얼로 결과를 확인하기 위해 적용한 설정을 요약한다.

## 네트워크 구성

| 장치 | IPv4 주소 |
|---|---|
| Windows `이더넷 5` | `192.168.20.90/24` |
| MDS2450 | `192.168.20.246/24` |

Windows 어댑터 주소는 관리자 PowerShell에서 다음과 같이 설정했다.

```powershell
netsh interface ipv4 set address name=53 source=static address=192.168.20.90 mask=255.255.255.0 gateway=192.168.20.1 store=persistent
```

WSL에서 LAN의 요청을 직접 받을 수 있도록 `%USERPROFILE%\.wslconfig`를 생성했다.

```ini
[wsl2]
networkingMode=mirrored
```

설정을 반영하려면 PowerShell에서 WSL을 완전히 종료한 뒤 다시 실행한다.

```powershell
wsl --shutdown
wsl -d Ubuntu-26.04
```

WSL 안에서 적용 여부와 타깃 연결을 확인한다.

```bash
wslinfo --networking-mode       # mirrored
ping -c 3 192.168.20.246
```

## Windows 방화벽

MDS2450에서 WSL TFTP 서버로 들어오는 UDP 69번만 허용했다. 아래 명령은 관리자 PowerShell에서 실행한다.

```powershell
New-NetFirewallHyperVRule `
  -Name "WSL-TFTP-MDS2450" `
  -DisplayName "WSL TFTP for MDS2450" `
  -Direction Inbound `
  -VMCreatorId "{40E0AC32-46A5-438A-A0B2-2B479E8F2E90}" `
  -Protocol UDP `
  -LocalPorts 69 `
  -RemoteAddresses 192.168.20.246 `
  -Action Allow
```

## WSL TFTP 서버

```bash
sudo apt update
sudo apt install -y tftpd-hpa tcpdump
sudo systemctl enable --now tftpd-hpa
```

`/etc/default/tftpd-hpa`의 설정은 다음과 같다.

```text
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/srv/tftp"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="--secure"
```

빌드한 최신 바이너리를 TFTP 루트에 복사한다.

```bash
sudo install -o tftp -g tftp -m 0644 \
  output/MDS2450.bin /srv/tftp/MDS2450.bin
```

서버 상태를 확인한다.

```bash
systemctl is-active tftpd-hpa
ss -lun | grep ':69'
ls -lh /srv/tftp/MDS2450.bin
```

문제가 있으면 패킷을 확인한다.

```bash
sudo tcpdump -ni any 'host 192.168.20.246 and udp'
```

## COM4 시리얼 연결

COM4는 FTDI FT232R(`VID:PID 0403:6001`) 장치다. Windows에 `usbipd-win`을 설치했다.

```powershell
winget install --interactive --exact dorssel.usbipd-win
usbipd list
```

확인된 BUSID가 `2-5`일 때 최초 한 번 관리자 PowerShell에서 공유한다.

```powershell
usbipd bind --busid 2-5
```

일반 PowerShell에서 WSL에 연결한다.

```powershell
usbipd attach --wsl --busid 2-5
```

WSL에서는 `/dev/ttyUSB0`으로 나타난다. 사용자를 `dialout` 그룹에 추가한 뒤 WSL 터미널을 다시 연다.

```bash
sudo usermod -aG dialout "$USER"
picocom --baud 115200 --flow none --parity none \
  --databits 8 --stopbits 1 /dev/ttyUSB0
```

현재 환경에는 attach와 `picocom` 실행을 묶은 명령도 설치했다.

```bash
mds2450-console
```

`picocom` 종료 키는 `Ctrl+A`, `Ctrl+X`다. WSL이 종료되면 USB 연결도 해제되므로 다시 attach해야 한다.

## 빌드와 다운로드

MDS2450의 부트로더 콘솔은 UART0이 아니라 **UART1**(`0x50004000`)을 사용한다. 프로젝트 기본값도 `UART_CHANNEL=1`로 설정했다.

```bash
cd ~/works/mds2450-perf-lab-ubuntu26/mds2450-perf-lab-ubuntu26
./scripts/build_all.sh
sudo install -o tftp -g tftp -m 0644 \
  output/MDS2450.bin /srv/tftp/MDS2450.bin
```

MDS2450 부트로더에서 다운로드하고 실행한다.

```text
setenv ipaddr 192.168.20.246
setenv serverip 192.168.20.90
tftp 30000000 MDS2450.bin
go 30000000
```

다음 메시지가 나오면 다운로드가 성공한 것이다.

```text
Bytes transferred = 2825 (0xb09)
```

부트로더의 `ping 192.168.20.90`이 Windows ICMP 방화벽 때문에 실패하더라도 TFTP 전송이 성공하면 실행에는 문제가 없다.
