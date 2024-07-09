
#include "common.h"
#include "descriptor_tables.h"
#include "kHeap.h"
#include "monitor.h"
#include "paging.h"
// #include "timer.h"

struct multiboot;
extern uint32 bss;

void printTest() {
  monitor_write("Hello World!\n");
  monitor_write_hex(0x60000);
  monitor_put('\n');
  monitor_write_dec(20241212);
  monitor_put('\n');
  monitor_write_dec(4294967295);
  monitor_put('\n');
  monitor_write_dec(0);
  monitor_put('\n');
}

int pageTest() {
  initialise_paging();
  monitor_write("Hello World!\n");
  uint32 *ptr = (uint32 *)0xA0000000;
  uint32 doPageFault = *ptr;
  return 0;
}

int heapTest() {
  uint32 a = kmalloc(8);
  initialise_paging();
  uint32 b = kmalloc(8);
  uint32 c = kmalloc(8);
  monitor_write("a: ");
  monitor_write_hex(a);
  monitor_write(", b: ");
  monitor_write_hex(b);
  monitor_write("\nc: ");
  monitor_write_hex(c);

  kfree((void*)c);
  kfree((void*)b);
  uint32 d = kmalloc(12);
  monitor_write(", d: ");
  monitor_write_hex(d);
  return 0;
}

int main(struct multiboot *mboot_ptr) {
  init_descriptor_tables();
  // monitor_clear();
  // printTest();
  return heapTest();
  return 0;
}
