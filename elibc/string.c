// =============================================================================
//  Program : string.c
//  Author  : Chun-Jen Tsai
//  Date    : Dec/09/2019
// -----------------------------------------------------------------------------
//  Description:
//  This is the minimal string library for aquila.
//  Extended by CY HSU for CoreMark-PRO bare-metal porting.
// =============================================================================
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// ===== helper macros from newlib =====
#define UNALIGNED(X, Y) (((long)X & (sizeof(long) - 1)) | ((long)Y & (sizeof(long) - 1)))

#if LONG_MAX == 2147483647L
#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
#elif LONG_MAX == 9223372036854775807L
#define DETECTNULL(X) (((X) - 0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif

// ===== mem functions =====

void *memcpy(void *d, const void *s, size_t n)
{
    unsigned char *dst = (unsigned char *)d;
    const unsigned char *src = (const unsigned char *)s;
    for (size_t i = 0; i < n; i++) dst[i] = src[i];
    return d;
}

void *memmove(void *d, const void *s, size_t n)
{
    unsigned char *dst = (unsigned char *)d;
    const unsigned char *src = (const unsigned char *)s;
    if (dst > src && dst < src + n) {
        for (size_t i = n; i > 0; i--) dst[i-1] = src[i-1];
    } else {
        for (size_t i = 0; i < n; i++) dst[i] = src[i];
    }
    return d;
}

void *memset(void *d, int v, size_t n)
{
    unsigned char *dst = (unsigned char *)d;
    while (n--) *(dst++) = (unsigned char)v;
    return d;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

// ===== strlen =====

size_t strlen(const char *s)
{
    size_t n = 0;
    while (*s++) n++;
    return n;
}

// ===== strcpy (from newlib, optimized) =====

char *strcpy(char *dst0, const char *src0)
{
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    if (!UNALIGNED(src, dst)) {
        aligned_dst = (long *)dst;
        aligned_src = (long *)src;
        while (!DETECTNULL(*aligned_src))
            *aligned_dst++ = *aligned_src++;
        dst = (char *)aligned_dst;
        src = (char *)aligned_src;
    }
    while ((*dst++ = *src++));
    return dst0;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    char *tmp = dst;
    while (*src && n) *(tmp++) = *(src++), n--;
    while (n--) *(tmp++) = 0;
    return dst;
}

// ===== strcat =====

char *strcat(char *dst, const char *src)
{
    char *tmp = dst;
    while (*tmp) tmp++;
    while (*src) *(tmp++) = *(src++);
    *tmp = 0;
    return dst;
}

char *strncat(char *dst, const char *src, size_t n)
{
    char *tmp = dst;
    while (*tmp) tmp++;
    while (*src && n) *(tmp++) = *(src++), n--;
    *tmp = 0;
    return dst;
}

// ===== strcmp (from newlib, optimized) =====

int strcmp(const char *s1, const char *s2)
{
    const unsigned long *a1, *a2;

    if (!UNALIGNED(s1, s2)) {
        a1 = (const unsigned long *)s1;
        a2 = (const unsigned long *)s2;
        while (*a1 == *a2) {
            if (DETECTNULL(*a1)) return 0;
            a1++; a2++;
        }
        s1 = (const char *)a1;
        s2 = (const char *)a2;
    }
    while (*s1 != '\0' && *s1 == *s2) { s1++; s2++; }
    return (*(const unsigned char *)s1) - (*(const unsigned char *)s2);
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0) return 0;
    while (--n && *s1 && *s1 == *s2) { s1++; s2++; }
    return (*(const unsigned char *)s1) - (*(const unsigned char *)s2);
}

// ===== search functions =====

char *strchr(const char *s, int c)
{
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    return (c == '\0') ? (char *)s : NULL;
}

char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == '\0') return (char *)s;
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle)
{
    if (!*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack, *n = needle;
        while (*h && *n && *h == *n) { h++; n++; }
        if (!*n) return (char *)haystack;
    }
    return NULL;
}

char *strpbrk(const char *s, const char *accept)
{
    for (; *s; s++) {
        for (const char *a = accept; *a; a++) {
            if (*s == *a) return (char *)s;
        }
    }
    return NULL;
}

size_t strspn(const char *s, const char *accept)
{
    size_t count = 0;
    for (; *s; s++) {
        const char *a;
        for (a = accept; *a; a++) {
            if (*s == *a) break;
        }
        if (!*a) break;
        count++;
    }
    return count;
}

size_t strcspn(const char *s, const char *reject)
{
    size_t count = 0;
    for (; *s; s++) {
        for (const char *r = reject; *r; r++) {
            if (*s == *r) return count;
        }
        count++;
    }
    return count;
}

char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *d = malloc(len);
    if (d) strcpy(d, s);
    return d;
}

int *__errno(void)
{
    static int _errno_val = 0;
    return &_errno_val;
}