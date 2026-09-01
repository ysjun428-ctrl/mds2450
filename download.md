
MDS2450은 보통 시리얼 콘솔로 부트로더(U-Boot/Bootroad)에 진입한 뒤, PC의 TFTP 서버에서 실행 파일·커널 이미지를 RAM으로 내려받아 실행합니다. 앞서 picocom 연결을 시도하셨으므로, 먼저 UART 콘솔이 안정적으로 보이는 상태를 만든 뒤 아래 절차를 진행하면 됩니다. 공개된 MDS2450 교육 자료에서는 보드 RAM 시작 주소를 0x30000000, UART 콘솔을 ttySAC1,115200n81로 사용하며, TFTP 기반 다운로드 예시를 제시합니다.github
전체 흐름
text
Ubuntu PC
  └─ Ethernet/TFTP server ───────── MDS2450 Ethernet

Ubuntu PC
  └─ USB-UART ───────────────────── MDS2450 UART console
                                      │
                                      ├─ bootloader 진입
                                      ├─ TFTP로 파일을 RAM에 수신
                                      └─ go 또는 bootm으로 실행
용도별로 명령이 달라집니다.
목적
내려받는 파일
보드 명령
실행 명령
Bare-metal LED/app
MDS2450.bin 등
tftp 30000000 MDS2450.bin
go 30000000
Linux 커널 일회성 부팅
zImage
tftp c0008000 zImage
bootm c0008000
U-Boot 교체
u-boot.bin
tftp c0008000 u-boot.bin
검증 후 NAND 기록
Root filesystem NAND 기록
*.cramfs
tftp c0000000 fs...
NAND erase/write

주의: NAND erase/write는 기존 부트로더·커널·파일시스템을 지울 수 있으므로, 처음에는 반드시 RAM 다운로드와 실행(tftp + go 또는 bootm)만 시험하세요.
1. 시리얼 콘솔 접속
USB-UART 연결은 다음이어야 합니다.
text
USB-UART TX  → MDS2450 RX
USB-UART RX  → MDS2450 TX
USB-UART GND → MDS2450 GND
Ubuntu에서 포트를 확인합니다.
bash
dmesg -wT
ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
일반적으로 다음으로 접속합니다.
bash
sudo picocom -b 115200 /dev/ttyUSB0
MDS2450 자료의 기본 UART 속도는 115200 bps입니다.github
picocom을 먼저 열고 MDS2450의 전원을 켜거나 RESET 버튼을 누릅니다. 자동 부팅 카운트다운이 나오면 아무 키나 눌러 부트로더 프롬프트로 멈춥니다.
정상이라면 대략 다음 중 하나가 보입니다.
text
MDS2450#
또는
text
bootroad#
먼저 환경을 백업해 두세요.
text
printenv
가능하면 이 출력 전체를 PC에 보관하는 것이 좋습니다. 기존 네트워크, bootcmd, NAND 주소를 보존해야 복구가 쉬워집니다.
2. Ubuntu에 TFTP 서버 구성
Ubuntu 20.04/22.04/24.04 기준으로 tftpd-hpa를 쓰는 편이 간단합니다.
bash
sudo apt update
sudo apt install -y tftpd-hpa tftp-hpa
TFTP 루트 디렉터리를 만들고 권한을 설정합니다.
bash
sudo mkdir -p /srv/tftp
sudo chown -R tftp:tftp /srv/tftp
sudo chmod 755 /srv/tftp
설정 파일을 엽니다.
bash
sudo nano /etc/default/tftpd-hpa
아래처럼 설정합니다.
bash
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/srv/tftp"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="--secure --verbose"
서비스를 재시작하고 상태를 봅니다.
bash
sudo systemctl restart tftpd-hpa
sudo systemctl enable tftpd-hpa
sudo systemctl status tftpd-hpa --no-pager
UDP 69 포트가 열려 있는지 확인합니다.
bash
sudo ss -lunp | grep ':69'
UFW를 사용 중이라면 TFTP를 허용합니다.
bash
sudo ufw allow 69/udp
교육용 예제에서도 PC를 TFTP 서버, MDS2450을 TFTP 클라이언트로 구성하여 파일을 전송합니다.whatmam.tistory
3. PC와 보드 IP 설정
PC 유선 NIC와 MDS2450 Ethernet 포트를 연결합니다. 가능하면 스위치/공유기 대신 PC와 보드를 직접 연결해서 먼저 검증하는 것이 편합니다. 최신 NIC는 자동 MDI-X를 지원하는 경우가 대부분이지만, 링크 LED가 반드시 켜지는지 확인하세요.
먼저 Ubuntu의 유선 인터페이스명을 확인합니다.
bash
ip -br link
ip -br addr
예를 들어 유선 NIC가 enp3s0라면, PC를 192.168.10.107/24로 설정합니다.
bash
sudo ip addr flush dev enp3s0
sudo ip addr add 192.168.10.107/24 dev enp3s0
sudo ip link set enp3s0 up
ip -br addr show enp3s0
부트로더에서 다음과 같이 설정합니다.
text
setenv serverip 192.168.10.107
setenv ipaddr 192.168.10.117
setenv netmask 255.255.255.0
일부 Bootroad/U-Boot 버전은 set 문법을 사용합니다.
text
set serverip 192.168.10.107
set ipaddr 192.168.10.117
set netmask 255.255.255.0
현재 사용하는 부트로더가 어떤 문법인지 모르면 먼저 help, ?, printenv를 입력해 보세요. 공개된 Bootroad 예시에는 setenv serverip, setenv ipaddr, saveenv가 사용됩니다.whatmam.tistory
설정이 RAM에서만 유효한지 먼저 확인합니다.
text
printenv
통신이 확인되고 재부팅 뒤에도 유지할 필요가 있을 때만 저장합니다.
text
saveenv
saveenv는 환경 영역을 Flash에 기록하므로, 처음에는 실행하지 않고 RAM 설정으로 시험하는 편이 안전합니다. MDS2450 U-Boot 자료도 setenv, printenv, saveenv를 각각 설정·확인·Flash 저장 명령으로 설명합니다.github
4. Bare-metal 바이너리 다운로드·실행
LED 예제, 자체 제작한 bare-metal 프로그램 등 MDS2450.bin을 실행하는 경우입니다.
PC에서 파일 준비
bash
cp ./MDS2450.bin /srv/tftp/
sudo chown tftp:tftp /srv/tftp/MDS2450.bin
chmod 644 /srv/tftp/MDS2450.bin

ls -lh /srv/tftp/MDS2450.bin
보드에서 다운로드
text
tftp 30000000 MDS2450.bin
성공 시 보통 다음과 유사하게 수신 바이트와 전송 속도가 나옵니다.
text
TFTP from server 192.168.10.107; our IP address is 192.168.10.117
Filename 'MDS2450.bin'.
Load address: 0x30000000
Loading: ###########
done
Bytes transferred = ...
수신 뒤 실행합니다.
text
go 30000000
이 방식은 파일을 RAM에만 올려 실행하므로 전원을 끄거나 리셋하면 사라집니다. MDS2450 교육 예시도 tftp 30000000 MDS2450.bin 다음에 go 30000000으로 실행합니다.whatmam.tistory
5. Linux zImage 다운로드·부팅
Linux 커널을 TFTP로 받아 일회성 부팅할 때는, 사용 중인 부트로더 환경의 bootcmd와 로드 주소를 먼저 확인합니다.
text
printenv
공개된 MDS2450 U-Boot 예시에는 아래와 같은 설정이 있습니다.
text
bootcmd=tftp c0008000 zImage; bootm c0008000
따라서 PC의 TFTP 루트에 zImage를 두고:
bash
cp ./zImage /srv/tftp/zImage
sudo chown tftp:tftp /srv/tftp/zImage
chmod 644 /srv/tftp/zImage
MDS2450 프롬프트에서 실행합니다.
text
tftp c0008000 zImage
bootm c0008000
또는 기존 환경이 이 구조라면:
text
setenv bootcmd 'tftp c0008000 zImage; bootm c0008000'
run bootcmd
다만 bootm은 보통 U-Boot image header가 있는 uImage에 맞는 명령이고, 순수 zImage에는 보드의 오래된 U-Boot 포팅에서 특수 처리되었거나 별도 설정이 들어갔을 수 있습니다. 따라서 현재 printenv에 이미 tftp ... zImage; bootm ... 형태가 있다면 그 기존 주소와 명령을 그대로 따르세요. MDS2450 공개 환경 예시도 c0008000으로 zImage를 받은 뒤 bootm c0008000을 수행합니다.github
NFS root filesystem까지 함께 쓰는 예시는 다음과 같은 구조입니다.
text
setenv bootargs 'root=/dev/nfs rw nfsroot=192.168.10.107:/srv/nfs/rootfs ip=192.168.10.117:192.168.10.107:192.168.10.1:255.255.255.0::eth0:off console=ttySAC1,115200n81'
tftp c0008000 zImage
bootm c0008000
여기서 /srv/nfs/rootfs는 Ubuntu에서 실제로 NFS export한 rootfs 경로여야 하며, TFTP와 NFS는 별도 서비스입니다.
6. TFTP 실패 시 진단
TFTP error: Access violation
TFTP 루트가 다르거나 파일 권한/소유자가 맞지 않는 경우가 많습니다.
bash
sudo journalctl -u tftpd-hpa -f
ls -ld /srv/tftp
ls -l /srv/tftp
파일은 TFTP 루트 아래에 두고 읽기 가능해야 합니다.
bash
sudo chown tftp:tftp /srv/tftp/MDS2450.bin
sudo chmod 644 /srv/tftp/MDS2450.bin
과거 xinetd 기반 TFTP 예시도 /tftpboot 생성 뒤 권한 부족으로 Access violation이 발생할 수 있음을 언급합니다.whatmam.tistory
TFTP timeout 또는 Retry count exceeded
다음을 순서대로 확인하세요.
text
printenv
ping 192.168.10.107
ping이 부트로더에 없을 수도 있습니다. 이 경우 Ubuntu에서 TFTP 로그를 실시간으로 보세요.
bash
sudo journalctl -u tftpd-hpa -f
주요 원인입니다.
serverip가 Ubuntu PC IP와 다름
ipaddr가 PC와 같은 서브넷이 아님
PC NIC가 Wi-Fi이고 실제 TFTP는 유선 NIC에서 나가도록 설정되지 않음
인터페이스가 eth0가 아니라 enp3s0, enx... 등인데 잘못 설정함
보드와 PC 사이 LAN 링크가 없음
UFW 또는 다른 방화벽이 UDP 69/임시 UDP 포트를 차단
TFTP 루트에 파일이 없음 또는 파일명이 대소문자까지 일치하지 않음
패킷을 직접 확인하려면 Ubuntu에서 다음을 실행한 뒤 보드에서 tftp를 다시 수행합니다.
bash
sudo tcpdump -ni enp3s0 'udp port 69 or arp'
ARP 요청도 안 보이면: 케이블·링크·보드 IP·보드 Ethernet 초기화 문제
ARP는 보이나 TFTP 요청이 없으면: 부트로더 네트워크 설정 문제
TFTP 요청은 보이나 데이터가 안 가면: 방화벽·TFTP 서비스·서버 응답 경로 문제
NAND 기록은 마지막에
NAND Flash로 영구 반영하는 작업은 이미지 종류와 보드별 NAND partition 주소를 정확히 알아야 합니다. 예를 들어 공개된 다른 2450 계열 자료에는 cramfs 파일시스템을 다음처럼 기록하는 예가 있습니다.
text
tftp c0000000 fs.mv2450.cramfs
nand erase 400000 4300000
nand write c0000000 400000 4300000
하지만 이 주소와 크기는 해당 보드의 NAND 레이아웃에 종속적입니다. 사용 중인 MDS2450의 U-Boot, kernel, rootfs partition을 확인하지 않은 채 그대로 실행하면 다른 영역을 지워 부팅 불능 상태가 될 수 있습니다.mvcpu.co
마찬가지로 한 공개 MDS2450 자료에는 U-Boot를 위한 nand erase 0 40000 및 쓰기 예시가 있지만, 부트로더 영역은 특히 복구 난이도가 높습니다. 현재 보드에서 이미 정상 부팅되는 상태라면 U-Boot NAND write는 수행하지 마세요.github
최소 실행 명령
현재 목적이 LED/bare-metal .bin 파일을 다운로드해서 실행하는 것이라면, 아래만 맞추면 됩니다.
bash
# Ubuntu PC
sudo apt install -y tftpd-hpa
sudo mkdir -p /srv/tftp
sudo cp MDS2450.bin /srv/tftp/
sudo chown -R tftp:tftp /srv/tftp
sudo systemctl restart tftpd-hpa
text
# MDS2450 Bootroad/U-Boot
setenv serverip 192.168.10.107
setenv ipaddr 192.168.10.117
setenv netmask 255.255.255.0

tftp 30000000 MDS2450.bin
go 30000000
