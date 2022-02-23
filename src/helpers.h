#ifndef HELPERS
#define HELPERS

#define BGB_BREAKPOINT() __asm__("ld b, b") // opcode used as breakpoint in BGB
#define CRASH_POINT() __asm__(" .db #0xDD") // invalid opcode, will lock up the system

// SDCC doesn't have "bool" defined, for some reason?? Pretty sure it's supposed to???
#define bool uint8_t
#define true 1
#define false 0

#endif//HELPERS