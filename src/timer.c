
#include "timer.h"

#include "common.h"
#include "isr.h"
#include "monitor.h"

uint32 tick = 0;

static void timer_callback(register_t regs){
  monitor_write("Tick: ");
  monitor_write_dec(++tick);
  monitor_write("\n");
}

void init_timer(uint32 frequency){
  register_interrupt_handler(IRQ0, &timer_callback);

  uint32 divistor = 1193180 / frequency;
  // Pitch at 40~42 43 is commond port
  outb(0x43, 0x36);
  uint8 value = (uint8) (divistor & 0xFF);
  outb(0x40, value);
  value = (uint8) ((divistor >> 8) & 0xFF);
  outb(0x40, value);
}
