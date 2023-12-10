
#include "monitor.h"

struct multiboot;

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

int main(struct multiboot *mboot_ptr) {
  monitor_clear();
  printTest();
  return 0;
}
