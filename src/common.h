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

#define PANIC(msg) panic(msg, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

extern void panic(const char * message, const char * file, uint32 line);
extern void panic_assert(const char * file, uint32 line, const char* desc);

void* memcpy(uint8 *dest, const uint8 *src, uint32 len);
void memset(uint8 *dest, uint8 value, uint32 len);
long strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

#endif // COMMON_H
