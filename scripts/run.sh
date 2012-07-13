#!/bin/sh

BASE="/home/jose/wip/jOS"

sudo su -c "mount ${BASE}/root.img /mnt/tmp/ -o loop"
sudo su -c "cp ${BASE}/jOS/jOS /mnt/tmp/"
sudo su -c "umount /mnt/tmp"


# nota: compilar bochs más o menos así:
# ./configure --enable-x86-64 --enable-smp --enable-long-phy-address --enable-debugger \
#  --enable-debugger-gui --enable-all-optimizations --enable-readline --enable-disasm --prefix=/opt/bochs-201...




# control+alt+2 para entrar en modo monitor
#EXTRA="-S" # para que espere a gdb
qemu-system-x86_64 -name jOS $DEBUG -s $EXTRA -fda ${BASE}/root.img -boot a -no-reboot -m 512 $*
#/opt/bochs/bin/bochs -qf $BASE/jOS/scripts/jOSbochsrc


