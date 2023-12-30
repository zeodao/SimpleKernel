#include "common.h"

typedef struct registers {
  uint32 ds;
  uint32 edi,esi,edp,esp,ebx,edx,ecx,eax;   // using by pusha
  uint32 int_no, err_code;                  // to judge which int
  uint32 eip, cs, eflags, useresp, ss;      // push by process
} register_t;
