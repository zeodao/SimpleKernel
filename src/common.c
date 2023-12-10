#include "common.h"

void outb(uint16 port, uint8 value) {
  asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

uint8 inb(uint16 port) {
  uint8 ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
  return ret;
}

uint16 inw(uint16 port) {
  uint16 ret;
  asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
  return ret;
}

void *memcpy(uint8 *dest, const uint8 *src, uint32 len) {
  if (dest == src || len == 0) {
    return dest;
  }
  while (len--) {
    *dest++ = *src++;
  }
  return dest;
}

void memset(uint8 *dest, uint8 value, uint32 len) {
  while (len--) {
    *dest++ = value;
  }
}

long strlen(const char *str){
  const char * strEnd;
  for (strEnd = str; *strEnd != '\0'; strEnd++) {
  
  }
  return strEnd - str;
}

int strcmp(const char *str1, const char *str2) {
  char c1, c2;
  for (;;) {
    c1 = *str1++;
    c2 = *str2++;
    if (c1 != c2) {
      return c1 > c2 ? 1 : -1;
    }
    if (!c1) {
      break;
    }
  }
  return 0;
}

char *strcpy(char *dest, const char *src) {
  if (dest == src) {
    return dest;
  }
  char *ret = dest;
  while ((*dest++ = *src++) != '\0') {
  }
  return ret;
}

char *strcat(char *dest, const char *src) {
  char *ret = dest;
  while (*dest) {
    dest++;
  }
  while ((*dest++ = *src++) != '\0') {
  }
  return ret;
}
