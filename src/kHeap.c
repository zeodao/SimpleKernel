#include "kHeap.h"
#include "common.h"
#include "ordered_array.h"
#include "paging.h"
#include "monitor.h"

extern uint32 end;
extern page_directory_t *kernel_directory;
uint32 placement_address = (uint32)&end;
heap_t *kHeap;

void kmalloc_setInput(uint32 end) { placement_address = end; }

uint32 kmalloc_int(uint32 sz, uint32 align, uint32 *phy) {
  if (kHeap){
    return (uint32)alloc(sz, (uint8)align, kHeap);
  }
  if (align == 1 && placement_address & 0x00000FFF) {
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }
  if (phy) {
    *phy = placement_address;
  }
  uint32 temp = placement_address;
  placement_address += sz;
  return temp;
}

uint32 kmalloc(uint32 sz) { return kmalloc_int(sz, 0, 0); }

uint32 kmalloc_a(uint32 sz) { return kmalloc_int(sz, 1, 0); }

uint32 kmalloc_p(uint32 sz, uint32 *phys) { return kmalloc_int(sz, 0, phys); }

uint32 kmalloc_ap(uint32 sz, uint32 *phys) { return kmalloc_int(sz, 1, phys); }

void kfree(void* p){
  ASSERT(kHeap != 0);
  free(p, kHeap);
}

/**
 * @Synopsis FindSmallestHole
 *
 * @Param size
 * @Param pageAlign
 * @Param heap
 *
 * @Returns
 */
static int32 FindSmallestHole(uint32 size, uint8 pageAlign, heap_t *heap) {
  // smallest fit hole for malloc
  uint32 iter = 0;
  while (iter < heap->index.size) {
    header_t *header = (header_t *)LookupOrderedArray(iter, &heap->index);
    if (pageAlign > 0) {
      uint32 location = (uint32)header;
      int32 offset = 0;
      if (((location + sizeof(header_t)) & 0x00000FFF) != 0) {
        offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
      }
      int32 holeSize = (int32)header->size - offset;
      if (holeSize >= (int32)size) {
        break;
      }
    } else if (header->size >= size) {
      // first hole larger than alloc size
      break;
    }
    iter++;
  }

  if (iter == heap->index.size) {
    return -1;
  } else {
    return iter;
  }
}

static int8 LessThan_header_t(void *a, void *b) {
  return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

heap_t *CreateHeap(uint32 start, uint32 end, uint32 maxSize, uint8 supervisor,
                   uint8 readonly) {
  heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
  // align check
  ASSERT(start % 0x1000 == 0);
  ASSERT(end % 0x1000 == 0);
  // init Index
  heap->index = PlaceOrderedArray((void *)start, HEAP_INDEX_SIZE, &LessThan_header_t);

  start += sizeof(type_t) * HEAP_INDEX_SIZE;

  if ((start & 0x00000FFF) != 0) {
    start &= 0xFFFFF000;
    start += 0x00001000;
  }

  // update heap info
  heap->startAddress = start;
  heap->endAddress = end;
  heap->maxAddress = maxSize;
  heap->supervisor = supervisor;
  heap->readonly = readonly;

  // make first hole large
  header_t *hole = (header_t *)start;
  hole->size = end - start;
  hole->magic = HEAP_MAGIC;
  hole->is_hole = 1;
  InsertOrderedArray((void *)hole, &heap->index);
  return heap;
}

static void Expand(uint32 newSize, heap_t *heap) {
  // Check
  ASSERT(newSize > heap->endAddress - heap->startAddress);

  // align to page
  if ((newSize & 0x00000FFF) != 0) {
    newSize &= 0xFFFFF000;
    newSize += 0x00001000;
  }

  // check new size valid
  ASSERT(heap->startAddress + newSize <= heap->maxAddress);

  // expand from last page
  uint32 oldSize = heap->endAddress - heap->startAddress;
  uint32 i = oldSize;
  while (i < newSize) {
    alloc_frame(get_page(heap->startAddress + i, 1, kernel_directory),
                heap->supervisor ? 1 : 0, heap->readonly ? 0 : 1);
    i += 0x1000;
  }
  heap->endAddress = heap->startAddress + newSize;
}

static uint32 Contract(uint32 newSize, heap_t *heap) {
  ASSERT(newSize < heap->endAddress - heap->startAddress);
  // align to page
  if (newSize & 0x00000FFF) {
    newSize &= 0xFFFFF000;
    newSize += 0x00001000;
  }

  if (newSize < HEAP_MIN_SIZE) {
    newSize = HEAP_MIN_SIZE;
  }
  uint32 oldSize = heap->endAddress - heap->startAddress;
  uint32 i = oldSize - 0x1000;

  while (newSize < i) {
    free_frame(get_page(heap->startAddress + i, 1, kernel_directory));
    i -= 0x1000;
  }
  heap->endAddress = heap->startAddress + newSize;
  return newSize;
}

void *alloc(uint32 size, uint8 pageAlign, heap_t *heap) {
  uint32 newSize = size + sizeof(header_t) + sizeof(footer_t);

  int32 iter = FindSmallestHole(newSize, pageAlign, heap);

  if (iter == -1) {
    // cannot find fit holes, save prev data
    //
    // monitor_write("hole not Find!\n");
    // PrintArray(&(heap->index));
    uint32 oldLen = heap->endAddress - heap->startAddress;
    uint32 oldEnd = heap->endAddress;

    Expand(oldLen + newSize, heap);
    uint32 newLen = heap->endAddress - heap->startAddress;

    iter = 0;

    // idx is
    int32 idx = -1;
    uint32 value = 0x0;

    while (iter < heap->index.size) {
      // find last
      uint32 tmp = (uint32)LookupOrderedArray(iter, &heap->index);
      if (tmp > value) {
        value = tmp;
        idx = iter;
      }
      iter++;
    }

    // If we didn't find ANY headers, we need to add one.
    if (idx == -1) {
      header_t *header = (header_t *)oldEnd;
      header->magic = HEAP_MAGIC;
      header->size = newLen - oldLen;
      header->is_hole = 1;
      footer_t *footer = (footer_t *)(oldEnd + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
      InsertOrderedArray((void *)header, &heap->index);
    } else {
      // The last header needs adjusting.
      header_t *header = LookupOrderedArray(idx, &heap->index);
      header->size += newLen - oldLen;
      // Rewrite the footer.
      footer_t *footer =
          (footer_t *)((uint32)header + header->size - sizeof(footer_t));
      footer->header = header;
      footer->magic = HEAP_MAGIC;
    }
    // We now have enough space. Recurse, and call the function again.
    return alloc(size, pageAlign, heap);
  }
  // found fit hole
  // monitor_write("hole Find!");
  header_t *oriHoleHeader = (header_t *)LookupOrderedArray(iter, &heap->index);
  uint32 oriHolePos = (uint32)oriHoleHeader;
  uint32 oriHoleSize = oriHoleHeader->size;
  if (oriHoleSize - newSize < sizeof(header_t) + sizeof(footer_t)) {
    // if hole is bigger than alloc size but not big enough to make another hole
    // change alloc size to hole size
    size += oriHoleSize - newSize;
    newSize = oriHoleSize;
  }

  if (pageAlign && (oriHolePos & 0x00000FFF)) {
    // need align but currnet is not align
    uint32 newLocation =
        oriHolePos + 0x1000 - (oriHolePos & 0x00000FFF) - sizeof(header_t);
    // update splited prev hole
    header_t *hole_header = (header_t *)oriHolePos;
    hole_header->size = 0x1000 /* page size */ -
                        (oriHolePos & 0x00000FFF) /* align size */ -
                        sizeof(header_t) /* remove hole size */;
    hole_header->magic = HEAP_MAGIC;
    hole_header->is_hole = 1;
    // update splited prev footer
    footer_t *prev_footer = (footer_t *)((uint32)(newLocation) +
                                         hole_header->size - sizeof(footer_t));
    prev_footer->magic = HEAP_MAGIC;
    prev_footer->header = hole_header;

    oriHolePos = newLocation;
    oriHoleSize = oriHoleSize - hole_header->size;

  } else {
    // simple remove hole
    RemoveOrderedArray(iter, &heap->index);
  }
  // build cur block
  header_t *blockHeader = (header_t *)oriHolePos;
  blockHeader->magic = HEAP_MAGIC;
  blockHeader->is_hole = 0;
  blockHeader->size = newSize;

  footer_t *blockFooter = (footer_t *)(oriHolePos + sizeof(header_t) + size);
  blockFooter->magic = HEAP_MAGIC;
  blockFooter->header = blockHeader;

  // create a new hole
  if (oriHoleSize - newSize > 0) {
    header_t *hole_header =
        (header_t *)(oriHoleSize + sizeof(header_t) + size + sizeof(footer_t));
    hole_header->magic = HEAP_MAGIC;
    hole_header->is_hole = 1;
    hole_header->size = oriHoleSize - newSize;
    footer_t *hole_footer = (footer_t *)((uint32)hole_header + oriHoleSize -
                                         newSize - sizeof(footer_t));
    if ((uint32)hole_footer < heap->endAddress) {
      hole_footer->magic = HEAP_MAGIC;
      hole_footer->header = hole_header;
    }
    // Put the new hole in the index;
    InsertOrderedArray((void *)hole_header, &heap->index);
  }
  return (void *)((uint32)blockHeader + sizeof(header_t));
}

void free(void *p, heap_t *heap) {
  if (p == 0) {
    return;
  }

  // get header and footer
  header_t *header = (header_t *)((uint32)p - sizeof(header_t));
  footer_t *footer =
      (footer_t *)((uint32)header + header->size - sizeof(footer_t));

  // sanity check
  ASSERT(header->magic == HEAP_MAGIC);
  ASSERT(footer->magic == HEAP_MAGIC);

  // to a hole
  header->is_hole = 1;

  uint8 do_add = 1;

  // unify left
  footer_t *leftFooter = (footer_t *)((uint32)header - sizeof(footer_t));
  if (leftFooter->magic == HEAP_MAGIC && leftFooter->header->is_hole) {
    uint32 curSize = header->size;
    header = leftFooter->header;
    header->size += curSize;
    footer->header = header;
    do_add = 0;
  }

  // unify right
  header_t *rightHeader = (header_t *)((uint32)header + header->size);
  if (rightHeader->magic == HEAP_MAGIC && rightHeader->is_hole) {
    header->size += rightHeader->size;
    footer = (footer_t *)((uint32)(rightHeader) + rightHeader->size -
                          sizeof(footer_t));
    footer->header = header;

    uint32 iter = 0;
    while (iter < heap->index.size &&
           (LookupOrderedArray(iter, &heap->index) != (void *)rightHeader)) {
      iter++;
    }
    RemoveOrderedArray(iter, &heap->index);
  }

  // concate
  if ((uint32)footer + sizeof(footer_t) == heap->endAddress) {
    uint32 oldLen = heap->endAddress - heap->startAddress;
    uint32 newLen = Contract((uint32)header - heap->startAddress, heap);
    if (header->size - (oldLen - newLen) > 0) {
      // this head is exist
      header->size -= oldLen - newLen;
      footer = (footer_t *)((uint32)header + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
    } else {
      uint32 iter = 0;
      while ((iter < heap->index.size) &&
             (LookupOrderedArray(iter, &heap->index)) != (void *)header) {
        iter++;
      }
      if (iter < heap->index.size) {
        RemoveOrderedArray(iter, &heap->index);
      }
    }
  }

  if (do_add == 1){
    InsertOrderedArray((void*) header, &heap->index);
  }

}
