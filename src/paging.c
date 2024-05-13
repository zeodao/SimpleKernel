#include "common.h"
#include "monitor.h"
#include "paging.h"
#include "kHeap.h"

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

void page_fault(register_t regs) {
  // get fault address from cr2
  uint32 faulting_address;
  asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

  // get info
  int present = !(regs.err_code & 0x1);
  int rw = (regs.err_code & 0x2);
  int user = (regs.err_code & 0x4);
  int reserved = (regs.err_code & 0x8);
  int id = (regs.err_code & 0x10);

  monitor_write("Page fault! ( ");
  if (present) {
    monitor_write("present ");
  }
  if (rw) {
    monitor_write("read-only ");
  }
  if (user) {
    monitor_write("user_mode ");
  }
  if (reserved) {
    monitor_write("reserved ");
  }
  if (id) {
    monitor_write("fetch instruction ");
  }
  monitor_write(") at ");
  monitor_write_hex(faulting_address);
  monitor_write("\n");
  PANIC("Page fault");
}

// frame allocation
uint32 *frames;
uint32 nframes; // every bit in uint32 will be used to present this frame is
                // valid or not

page_directory_t *kernel_directory=0;

page_directory_t *current_directory=0;

static void set_frame(uint32 frame_addr) {
  uint32 frame = frame_addr / 0x1000; // shift right 12 bit
  uint32 idx = INDEX_FROM_BIT(frame);
  uint32 off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32 frame_addr) {
  uint32 frame = frame_addr / 0x1000; // shift right 12 bit
  uint32 idx = INDEX_FROM_BIT(frame);
  uint32 off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

static int test_frame(uint32 frame_addr) {
  uint32 frame = frame_addr / 0x1000; // shift right 12 bit
  uint32 idx = INDEX_FROM_BIT(frame);
  uint32 off = OFFSET_FROM_BIT(frame);
  return (frames[idx] | (0x1 << off)); 
};

static uint32 first_frame() {
  uint32 i, j;
  for (i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
    if (frames[i] != 0xFFFFFFFF) {
      for (j = 0; j < 32; ++j) {
        uint32 toTest = 0x1 << j;
        if (!(frames[i] & toTest)) {
          return i * 4 * 8 + j;
        }
      }
    }
  }
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
  if (page->frame != 0) {
    return; // allocated
  }
  uint32 idx = first_frame();
  if (idx == (uint32)(-1)) {
    PANIC("No free frames");
  }
  set_frame(idx * 0x1000);
  page->present = 1;
  page->rw = is_writeable ? 1 : 0;
  page->user = is_kernel ? 0 : 1;
  page->frame = idx;
}

void free_frame(page_t *page, int is_kernel, int is_writeable) {
  uint32 frame;
  if (!(frame = page->frame)) {
    return;
  }
  clear_frame(frame);

  page->frame = 0;
}

void initialise_paging(){

  register_interrupt_handler(14, &page_fault);

  // 16 MB of RAM 4k page
  uint32 mem_end_page = 0x10000000;


  nframes = mem_end_page / 0x1000; // 4kpage size
  frames = (uint32*)kmalloc(INDEX_FROM_BIT(nframes));
  memset(frames, 0, INDEX_FROM_BIT(nframes)); // page index size
  
  kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
  memset(kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;

  // in this case 
  int i = 0;
  while (i < placement_address) {
    alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
    i += 0x1000;
  }

  switch_page_directory(kernel_directory);
  uint32 cr0;
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000;// enable paging
  asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void switch_page_directory(page_directory_t *newPage){
  current_directory = newPage;
  asm volatile("mov %0, %%cr3":: "r"(&newPage->tablesPhysical));
}


page_t *get_page(uint32 address, int make, page_directory_t* dir){
  address /= 0x1000; // page index

  uint32 table_idx = address / 1024; // one table contain 1024 poge
  if (dir->tables[table_idx]){
    return &dir->tables[table_idx]->pages[address%1024];
  }
  if (make){
    uint32 tmp;
    dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
    memset(dir->tables[table_idx], 0, 0x1000);
    dir->tablesPhysical[table_idx] = tmp | 0x7;
    return &dir->tables[table_idx]->pages[address%1024];
  }else{
    return 0;
  }
}

