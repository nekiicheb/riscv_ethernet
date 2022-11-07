#!/bin/bash
../qemu_7/qemu-7.1.0/build/qemu-system-riscv64 \
-M microchip-icicle-kit \
-net nic,model=cadence_gem \
-net user,hostfwd=tcp::8888-:7777 \
-net dump,file=vm0.pcap \
-display none \
-kernel ../softconsole_wb/mpfs-mac-simple-test/Debug/mpfs-mac-simple-test.elf   
