// for interrupt handlers

#include "common.h"
#include "def8259.h"
#include "isr.h"
#include "monitor.h"

isr_t interrupt_handlers[256];

void isr_handler(register_t regs) {
  if (interrupt_handlers[regs.int_no] != 0) {
    interrupt_handlers[regs.int_no](regs);
  } else {
    monitor_write("receive interrupt: ");
    monitor_write_dec(regs.int_no);
    monitor_put('\n');
  }
}
void register_interrupt_handler(uint8 n, isr_t handler) {
  interrupt_handlers[n] = handler;
}

void irq_handler(register_t regs) {
  if (interrupt_handlers[regs.int_no] != 0) {
    interrupt_handlers[regs.int_no](regs);
  }
  if (regs.int_no >= 40) {
    monitor_write("receive interrupt: ");
    monitor_write_dec(regs.int_no);
    monitor_put('\n');

    PIC_SendEOI(regs.int_no - 0x20);
  }
}
