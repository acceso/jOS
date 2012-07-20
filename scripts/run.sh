#!/bin/sh

BASE="/home/jose/wip/jOS"

sudo su -c "mount ${BASE}/root.img /mnt/tmp/ -o loop"
sudo su -c "cp ${BASE}/jOS/jOS /mnt/tmp/"
sudo su -c "umount /mnt/tmp"


if [ "$1" = "bochs" ]; then
	# nota: compilar bochs más o menos así:
	# ./configure --enable-x86-64 --enable-smp --enable-long-phy-address --enable-acpi --enable-pci --enable-configurable-msrs --enable-fast-function-calls --enable-host-specific-asms --enable-show-ips --enable-debugger --enable-x86-debugger --enable-iodebug --enable-debugger-gui --enable-all-optimizations --enable-vmx --enable-logging --enable-readline --enable-disasm --prefix=/opt/bochs-20100511/ 

	/opt/bochs/bin/bochs -qf $BASE/jOS/scripts/jOSbochsrc


else # default to qemu:

	# control+alt+2 enters monitor mode
	#EXTRA="-S" # wait for gdb
	# to get 1gb page tables: -cpu kvm64,+pdpe1gb ,
	# it seems unimplemented, see: target-i386/cpuid.c on qemu source :(
	qemu-system-x86_64 -name jOS -s $EXTRA -fda ${BASE}/root.img \
		-drive if=ide,index=0,media=disk,file=${BASE}/referencia.img \
		-boot a -no-reboot -m 1024 $*
fi



