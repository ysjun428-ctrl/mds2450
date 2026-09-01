# Windows Tftpd64 설치 및 방화벽 설정

이 문서는 Windows에서 Tftpd64를 설치하고 MDS2450 전용 방화벽 규칙을 추가하는 절차만 설명한다.

사용 주소:

| 장치 | IPv4 주소 |
|---|---|
| Windows TFTP 서버 | `192.168.20.90` |
| MDS2450 | `192.168.20.246` |

아래 명령은 **관리자 PowerShell**에서 실행한다.

## 1. Tftpd64 설치

Winget으로 설치한다.

```powershell
winget install --exact --id PJO2.tftpd64 `
  --accept-package-agreements `
  --accept-source-agreements
```

설치 여부를 확인한다.

```powershell
winget list --exact --id PJO2.tftpd64
```

PATH에서 `tftpd64` 명령을 찾지 못하면 전체 경로로 실행한다.

```powershell
Start-Process "C:\Program Files\Tftpd64\tftpd64.exe"
```

Tftpd64에서 다음 항목을 지정한다.

```text
Current Directory : C:\tftp
Server interfaces : 192.168.20.90
```

`Settings > GLOBAL`에서는 `TFTP Server`만 활성화하고, `Settings > TFTP`에서는 다음과 같이 설정한다.

```text
Base Directory   : C:\tftp
Security         : Read-Only
Local ports pool : 50000:50010
```

## 2. Windows 방화벽 설정

TFTP 요청 포트와 전송 포트 범위를 MDS2450에만 허용한다.

```powershell
New-NetFirewallRule `
  -DisplayName "MDS2450 TFTP Windows" `
  -Direction Inbound `
  -Action Allow `
  -Protocol UDP `
  -LocalPort 69,50000-50010 `
  -RemoteAddress 192.168.20.246 `
  -InterfaceAlias "이더넷 5" `
  -Profile Public
```

생성된 규칙을 확인한다.

```powershell
Get-NetFirewallRule -DisplayName "MDS2450 TFTP Windows" |
  Format-List DisplayName,Enabled,Direction,Action,Profile

Get-NetFirewallRule -DisplayName "MDS2450 TFTP Windows" |
  Get-NetFirewallPortFilter
```

Tftpd64 실행 후 UDP 69번 포트가 열렸는지 확인한다.

```powershell
Get-NetUDPEndpoint -LocalPort 69
```

## 방화벽 규칙 제거

더 이상 사용하지 않을 때는 관리자 PowerShell에서 제거한다.

```powershell
Remove-NetFirewallRule -DisplayName "MDS2450 TFTP Windows"
```
