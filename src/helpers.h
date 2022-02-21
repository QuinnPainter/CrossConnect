#ifndef HELPERS
#define HELPERS

#define BGB_BREAKPOINT() __asm__("ld b, b") // opcode used as breakpoint in BGB
#define CRASH_POINT() __asm__(" .db #0xDD") // invalid opcode, will lock up the system

#endif//HELPERS