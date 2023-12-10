// common.h

#ifndef COMMON_H
#define COMMON_H

typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;

void outb(uint16 port, uint8 value);
uint8 inb(uint16 port);
uint16 inw(uint16 port);

void* memcpy(uint8 *dest, const uint8 *src, uint32 len);
void memset(uint8 *dest, uint8 value, uint32 len);
long strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

#endif // COMMON_H
