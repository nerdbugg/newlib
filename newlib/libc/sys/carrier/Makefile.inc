AM_CPPFLAGS_%C% = -I$(srcdir)/libc/machine/riscv

libc_a_SOURCES += 
if MAY_SUPPLY_SYSCALLS
libc_a_SOURCES += %D%/syscalls.c
endif

