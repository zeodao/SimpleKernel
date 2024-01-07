#ifndef DEF8259_H
#define DEF8259_H

#include "common.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_CMD PIC1
#define PIC2_CMD PIC2
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)

#define ICW1_ICW4 (0x01 << 0)
#define ICW1_SINGLE (0x01 << 1)
#define ICW1_INTERVAL4 (0x01 << 2)
#define ICW1_LEVEL (0x01 << 3)
#define ICW1_INIT (0x10)

#define ICW4_8086 (0x01 << 0)
#define ICW4_AUTO (0x01 << 1)
#define ICW4_BUF_SALVE (0x01 << 4)
#define ICW4_BUF_MASTER (0x0C)
#define ICW4_SFNM (0x10)

#define PIC_EOI 0x20

void PIC_SendEOI(uint8 irq);

void PIC_Remap(uint8 offset1, uint8 offset2);

#endif //DEF8259_H
