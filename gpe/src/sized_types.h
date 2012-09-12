#ifndef _SIZED_TYPES_H_
#define _SIZED_TYPES_H_

// (un)signed integers
typedef   signed char      I8;
typedef unsigned char      U8;
typedef   signed short int I16;
typedef unsigned short int U16;
typedef   signed int       I32;
typedef unsigned int       U32;

// (un)signed fast integers
typedef   signed long  int I32F;
typedef unsigned long  int U32F;

// floating point values
typedef float  F32;
typedef double F64;

// 64 bit integers & SIMD values
# ifdef _WIN32
typedef          __int64 I64;
typedef unsigned __int64 U64;
typedef __declspec(align(16)) F32 VF32[4];
# else
#  if __WORDSIZE == 64
typedef          long int I64;
typedef long unsigned int U64;
#  else
typedef          long long int I64;
typedef long long unsigned int U64;
#  endif
typedef F32 VF32[4] __attribute__(aligned (16)); 
# endif

#endif // _SIZED_TYPES_H_