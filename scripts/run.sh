#!/bin/sh

BASE="/home/jose/wip/jOS"

sudo su -c "mount ${BASE}/root.img /mnt/tmp/ -o loop"
sudo su -c "cp ${BASE}/jOS/jOS /mnt/tmp/"
sudo su -c "umount /mnt/tmp"


if [ "$1" = "bochs" ]; then
	# nota: compilar bochs más o menos así:
	# ./configure --enable-x86-64 --enable-smp --enable-long-phy-address --enable-acpi --enable-pci --enable-1g-pages --enable-configurable-msrs --enable-fast-function-calls --enable-host-specific-asms --enable-show-ips --enable-debugger --enable-x86-debugger --enable-iodebug --enable-debugger-gui --enable-all-optimizations --enable-vmx --enable-logging --enable-readline --enable-disasm --prefix=/opt/bochs-20100402/ 

	/opt/bochs/bin/bochs -qf $BASE/jOS/scripts/jOSbochsrc


else # qemu por defecto:

	# control+alt+2 para entrar en modo monitor
	#EXTRA="-S" # para que espere a gdb
	qemu-system-x86_64 -name jOS $DEBUG -s $EXTRA -fda ${BASE}/root.img -hda ${BASE}/referencia.img -boot a -no-reboot -m 512 $*
fi






