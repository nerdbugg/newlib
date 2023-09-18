#define START "main"

__asm__(
".section .sdata,\"aw\"\n"
".text\n"
".global _start\n"
".type " START "_asm,%function\n"
START "_asm:\n"
".weak __global_pointer$\n"
".hidden __global_pointer$\n"
".option push\n"
".option norelax\n\t"
"lla gp, __global_pointer$\n"
".option pop\n\t"
"mv a0, sp\n"
".weak _DYNAMIC\n"
".hidden _DYNAMIC\n\t"
"lla a1, _DYNAMIC\n\t"
"andi sp, sp, -16\n\t"
"call " START "\n\t"
"tail _exit"
);

