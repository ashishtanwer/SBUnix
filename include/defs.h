#ifndef _DEFS_H
#define _DEFS_H

#define NULL 0
#define asm __asm__

typedef unsigned long __uint64_t;
typedef __uint64_t uint64_t;
typedef long __int64_t;
typedef __int64_t int64_t;
typedef unsigned int __uint32_t;
typedef __uint32_t uint32_t;
typedef int __int32_t;
typedef __int32_t int32_t;
typedef unsigned short __uint16_t;
typedef __uint16_t uint16_t;
typedef unsigned char __uint8_t;
typedef __uint8_t uint8_t;
//typedef char __int8_t;
//typedef __int8_t int8_t;

#define	KERNBASE 0xFFFFFFFF80000000UL
extern uint64_t kernmemvirt;
extern uint64_t physbasevirt;
extern uint64_t physfreevirt;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long long longlong_t;
typedef unsigned long QWORD;

//extern void *memset(void *dest, char val, size_t count);

#endif
