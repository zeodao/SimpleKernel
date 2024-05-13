#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"


extern uint32 placement_address;


void kmalloc_setInput(uint32 end);

/*
 * page aligned
 */
uint32 kmalloc_a(uint32 sz);

uint32 kmalloc_p(uint32 sz, uint32 *phys);

/*
 * page aligned with phy address
 */
uint32 kmalloc_ap(uint32 sz, uint32 *phys);

uint32 kmalloc(uint32 sz);

#endif // KHEAP_H
