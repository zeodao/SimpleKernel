#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"
#include "ordered_array.h"

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_MIN_SIZE 0x70000


/**
 * @Synopsis size information of block/hole's head or foot
 */
typedef struct {
  uint32 magic; // using to judge size
  uint8 is_hole;// using or not 
  uint32 size;  // block size
} header_t;

typedef struct {
  uint32 magic;
  header_t *header;
} footer_t;

typedef struct {
  ordered_array_t index;
  uint32 startAddress;
  uint32 endAddress;
  uint32 maxAddress;
  uint8 supervisor;
  uint8 readonly;
} heap_t;

heap_t *CreateHeap(uint32 start, uint32 end, uint32 maxSize, uint8 supervisor, uint8 readonly);

void  *alloc(uint32 size, uint8 pageAlign, heap_t* heap);

void free(void *p, heap_t* heap);

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

void kfree(void* p);

#endif // KHEAP_H
