// for interrupt handlers

#include "common.h"
#include "isr.h"
#include "monitor.h"

void isr_handler(register_t regs){
  monitor_write("receive interrupt: ");
  monitor_write_dec(regs.int_no);
  monitor_put('\n');
}
