# riscv64-unknown-elf-gcc -fno-common -fno-builtin-printf -mcmodel=medany -Wl,--start-group -lc_nano -lgloss_htif -Wl,--end-group -lgcc -static -nostartfiles -dT htif.ld -c hello.c 

#../../binaries/llvm/bin/clang -fno-common -fno-builtin-printf -mcmodel=medany hello.c --sysroot=../../binaries/llvm/riscv64-unknown-elf/

#../../binaries/llvm/bin/clang -O2 hello.o -L/path/to/libgloss-htif -lgloss-htif -o hello

../../binaries/llvm/bin/clang --target=riscv64 -march=rv64gc -mabi=lp64d -c hello.c -o hello.o --sysroot=../../binaries/llvm/riscv64-unknown-elf/
#../../binaries/llvm/bin/clang --target=riscv64 -march=rv64gc -mabi=lp64d -S hello.c -o hello.s
../../binaries/llvm/bin/ld.lld -o hello.elf hello.o --entry=main --emulation=elf64lriscv