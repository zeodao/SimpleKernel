#include "monitor.h"

// vga frmae buffer info
uint16 *video_memory = (uint16 *)0xB8000;

uint8 cursorX = 0;
uint8 cursorY = 0;

static void move_cursor() {
  // assume 80 char wide
  uint16 cursorLocation = cursorY * 80 + cursorX;
  outb(0x3D4, 14);
  outb(0x3D5, cursorLocation >> 8); // high byte
  outb(0x3D4, 15);
  outb(0x3D5, cursorLocation); // low byte
}

// scrol screen one line
static void scroll() {
  uint8 attributeByte =
      (0 << 4 /*black background*/) | (15 & 0x0f /*white frontcolor*/);
  uint16 blankInfo = 0x20 | (attributeByte << 8);
  // 80 * 25
  if (cursorY >= 25) {
    int i;
    for (i = 0 * 80; i < 24 * 80; i++) {
      video_memory[i] = video_memory[i + 80];
    }
    for (i = 24 * 80; i < 25 * 80; i++) {
      video_memory[i] = blankInfo;
    }
    cursorY = 24;
  }
}

void monitor_put(char c) {
  uint8 backColor = 0;
  uint8 frontColor = 15;
  uint16 attributeW = ((backColor << 4) | (frontColor & 0x0f)) << 8;
  // ->
  if (c == 0x08 && cursorX) {
    cursorX--;
  }
  // tab
  else if (c == 0x09) {
    cursorX = (cursorX + 4) & ~(4 - 1);
  }

  else if (c == '\r') {
    cursorX = 0;
  }

  else if (c == '\n') {
    cursorX = 0;
    cursorY++;
  }

  else if (c >= ' ') {
    uint16 *location = video_memory + (cursorY * 80) + cursorX;
    *location = attributeW | c;
    cursorX++;
  }

  if (cursorX >= 80) {
    cursorX = 0;
    cursorY++;
  }

  scroll();

  move_cursor();
}

void monitor_clear() {
  uint8 attributeByte =
      (0 << 4 /*black background*/) | (15 & 0x0f /*white frontcolor*/);
  uint16 blankInfo = 0x20 | (attributeByte << 8);

  for (int i = 0; i < 25 * 80; i++) {
    video_memory[i] = blankInfo;
  }

  cursorX = 0;
  cursorY = 0;
  move_cursor();
}

void monitor_write(char *c) {
  int i = 0;
  while (c[i]) {
    monitor_put(c[i++]);
  }
}

void monitor_write_hex(uint32 n) {
  monitor_put('0');
  monitor_put('x');
  char cH;
  for (unsigned long i = 0; i < (sizeof(uint32) * 2); i++, n <<= 4) {
    cH = (0xf0000000 & n) >> (sizeof(uint32) * 8 - 4);
    cH += cH > 10 ? 'A' - 10 : '0';
    monitor_put(cH);
  }
}

void monitor_write_dec(uint32 n) {
  // uint32 max -> 10^9
  int pos = 9;
  uint32 lim;
  char dig;
  int digists = 0;
  do {
    for (dig = 0, lim = 1; dig < pos; dig++){
      lim *= 10;
    }
    // digist to jump leading zero
    // !pos to handle 0 value
    if (digists || n >=lim || !pos){
      for(dig = 0 ; n >=lim ;dig++){
        n -= lim;
      }
      digists++;
      dig += '0';
      monitor_put(dig);
    }
  }while (pos--);
}
