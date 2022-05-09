#ifndef FIREWORKS_H
#define FIREWORKS_H

void startFireworks() __preserves_regs(b, c, d, e, h, l);
void updateFireworks();
void clearFireworks() __preserves_regs(b, c, d);

#endif//FIREWORKS_H
