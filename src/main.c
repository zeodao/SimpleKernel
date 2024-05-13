
#include "common.h"
#include "kHeap.h"
#include "monitor.h"
#include "descriptor_tables.h"
#include "paging.h"
//#include "timer.h"

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

int pageTest(){
  initialise_paging();
  monitor_write("Hello World!\n");
  uint32 *ptr = (uint32*)0xA0000000;
  uint32 doPageFault = *ptr;
  return 0;
}

int main(struct multiboot *mboot_ptr) {
  init_descriptor_tables();
  //monitor_clear();
  //printTest();
  // PIC is markable, to avoid setting
  return pageTest();
  //asm volatile("sti");
  //init_timer(50);
  return 0;
}
