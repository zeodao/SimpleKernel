#include "kHeap.h"
#include "monitor.h"

extern uint32 end;
uint32 placement_address = (uint32)&end;

void kmalloc_setInput(uint32 end){
  placement_address = end;
}

uint32 kmalloc_int(uint32 sz, uint32 align, uint32* phy) {
  if (align == 1 && placement_address & 0x00000FFF) {
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }
  if (phy){
    *phy = placement_address;
  }
  uint32 temp = placement_address;
  placement_address += sz;
  return temp;
}

uint32 kmalloc(uint32 sz) {
  return kmalloc_int(sz, 0, 0);
}

uint32 kmalloc_a(uint32 sz) {
  return kmalloc_int(sz, 1, 0);
}

uint32 kmalloc_p(uint32 sz, uint32* phys) {
  return kmalloc_int(sz, 0, phys);
}

uint32 kmalloc_ap(uint32 sz, uint32* phys) {
  return kmalloc_int(sz, 1, phys);
}
