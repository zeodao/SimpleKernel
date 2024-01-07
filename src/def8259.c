#include "def8259.h"
#include "common.h"

void io_wait(){
  outb(0x80,0);
}

void PIC_SendEOI(uint8 irq){
  if (irq >= 8){
    outb(PIC2_CMD, PIC_EOI);
  }
  outb(PIC1_CMD, PIC_EOI);
}

void PIC_Remap(uint8 offset1, uint8 offset2){
  //uint8 mA = inb(PIC1_DATA), mB = inb(PIC2_DATA);

  // no io_wait for now
  // cascade
  outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
  outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
  // offset
  outb(PIC1_DATA, offset1);
  outb(PIC2_DATA, offset2);
  // cascade
  outb(PIC1_DATA, 0x01 << 2); // PIC2 at position2 of PIC1
  outb(PIC2_DATA, 0x02); // identify
  // 8086 mode
  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086); 

  //outb(PIC1_DATA, mA);
  //outb(PIC2_DATA, mB);
}

