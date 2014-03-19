#ifndef _UTIL_H
#define _UTIL_H

#include <defs.h>
//int  atoi( char *s);
uint64_t power(uint64_t x, int e);
uint64_t octalToDecimal(uint64_t octal);
//int strlen(const char *s);

int strnlen(const char *s, longlong_t size);

char * strcpy(char *dst, const char *src);
char * strncpy(char *dst, const char *src, longlong_t size);
longlong_t strlcpy(char *dst, const char *src, longlong_t size);

//int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint32_t n);

char * strchr(const char *s, char c);
char * strfind(const char *s, char c);

//void *  memset(void *dst, uint64_t c, longlong_t len);
/* no memcpy - use memmove instead */
void * memmove(void *dst, const void *src, longlong_t len);
void * memcpy(void *dst, const void *src, longlong_t n);
int memcmp(const void *s1, const void *s2, longlong_t len);
void * memfind(const void *s, int c, longlong_t len);

long strtol(const char *s, char **endptr, int base);
char * strtrim_start(char *dst, int trim);

int matchString(char *s, char *t);
int atoi(char *s);
int strlen(const char *s);

int strcmp(char *s1, char *s2);
int pow(int x, int y);
int getSize(char *p);
int top(float x);
#endif
