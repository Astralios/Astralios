// NOTE: Inspired by nob's da_append macros
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define da_reserve(da, extra) \
   do {\
      if((da)->len + extra >= (da)->cap) {\
         (da)->cap = (da)->cap*2+extra;\
         (da)->items = realloc((da)->items, (da)->cap*sizeof(*(da)->items));\
         assert((da)->items && "Ran out of memory");\
      }\
   } while(0)
#define da_push(da, value) \
   do {\
        da_reserve(da, 1);\
        (da)->items[(da)->len++]=value;\
   } while(0)
