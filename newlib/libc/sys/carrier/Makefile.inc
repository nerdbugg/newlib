AM_CPPFLAGS_%C% = -I$(srcdir)/libc/machine/riscv

libc_a_SOURCES += %D%/crt0.c
if MAY_SUPPLY_SYSCALLS
libc_a_SOURCES += %D%/syscalls.c %D%/tramps.S
endif

