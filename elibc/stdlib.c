// =============================================================================
//  Program : stdlib.c
//  Author  : Chun-Jen Tsai
//  Date    : Dec/09/2019
// -----------------------------------------------------------------------------
//  Description:
//  This is the minimal stdlib library for Aquila.
// -----------------------------------------------------------------------------
//  Revision information:
//
//  Apr/01/2021, by Po-Wei Ho:
//     Fixed two bugs in malloc(). The first bug is that the for-loop index 'ptr'
//     was sometimes updated without clearing the used/unused flag.
//     The second bug is that 'curr_top' can somtimes point to itself.
//
// -----------------------------------------------------------------------------
//  License information:
//
//  This software is released under the BSD-3-Clause Licence,
//  see https://opensource.org/licenses/BSD-3-Clause for details.
//  In the following license statements, "software" refers to the
//  "source code" of the complete hardware/software system.
//
//  Copyright 2019,
//                    Embedded Intelligent Systems Lab (EISL)
//                    Deparment of Computer Science
//                    National Chiao Tung Uniersity
//                    Hsinchu, Taiwan.
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
// =============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long ulong;

extern ulong __heap_start; /* declared in the linker script */
extern ulong __heap_size;  /* declared in the linker script */
static ulong heap_top = (ulong) &__heap_start;
static ulong heap_size = (ulong) &__heap_size;
static ulong *curr_top = (ulong *) 0xFFFFFFF0, *heap_end = (ulong *) 0xFFFFFFF0;

void *malloc(size_t n)
{
    void *return_ptr;
    ulong *ptr, temp;
    int r;

    if (curr_top == heap_end) // first time to call malloc()?
    {
        curr_top = (ulong *) heap_top;
        heap_end = (ulong *) ((heap_top + heap_size) & 0xFFFFFFF0);
        *curr_top = (ulong) heap_end;
    }

    // Search for a large-enough free memory block (FMB).
    return_ptr = NULL;
    for (ptr = curr_top; ptr < heap_end; ptr = (ulong *) (*ptr & 0xFFFFFFFE))
    {
        if ((*ptr & 1) == 0 && (*ptr - (ulong) ptr > n))
        {
            temp = ((ulong) ptr) & 0xFFFFFFFE;
            return_ptr = (void *) (temp + sizeof(ulong));

            // Update the FMB link list structure.
            r = n % sizeof(ulong);
            temp = n + sizeof(ulong) + ((r)? 4-r : 0);
            curr_top = ptr + temp/sizeof(ulong);
            if (curr_top != (ulong *) *ptr)
                *curr_top = *ptr;
            *ptr = (ulong) curr_top | 1;
            break;
        }
    }

    if (return_ptr != NULL) return return_ptr;

    // Search again for a FMB from heap_top to curr_top
    for (ptr = (ulong *) heap_top; ptr < curr_top; ptr = (ulong *) (*ptr & 0xFFFFFFFE))
    {
        if ((*ptr & 1) == 0 && (*ptr - (ulong) ptr > n))
        {
            temp = ((ulong) ptr) & 0xFFFFFFFE;
            return_ptr = (void *) (temp + sizeof(ulong));

            // Update the FMB link list structure.
            r = n % sizeof(ulong);
            temp = n + sizeof(ulong) + ((r)? 4-r : 0);
            curr_top = ptr + temp/sizeof(ulong);
            if (curr_top != (ulong *) *ptr)
                *curr_top = *ptr;
            *ptr = (ulong) curr_top | 1;
            break;
        }
    }

    return return_ptr;
}

void free(void *mptr)
{
    ulong *ptr, *next;

    ptr = ((ulong *) mptr) - 1;
    *ptr = *ptr & 0xFFFFFFFE; // Free the FMB.
    next = (ulong *) *ptr;
    if ((*next & 1) == 0)
    {
        *ptr = *next; // Merge with the next FMB.
        curr_top = ptr;
    }
}

void *calloc(size_t n, size_t size)
{
    void *mptr;
    mptr = malloc(n*size);
    memset(mptr, 0, n*size);
    return mptr;
}

int atoi(char *s)
{
    int value, sign;

    /* skip leading while characters */
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '-') sign = -1, s++;
    else sign = 1;
    if (*s >= '0' && *s <= '9') value = (*s - '0');
    else return 0;
    s++;
    while (*s != 0)
    {
       if (*s >= '0' && *s <= '9')
       {
           value = value * 10 + (*s - '0');
           s++;
       }
       else return 0;
    }

    return value * sign;
}

int abs(int n)
{
    int j;

    if (n >= 0) j = n; else j = -n;

	return j;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void exit(int status)
{
    printf("\n-----------------------------------------------------------------------\n");
    printf("Program exit with a status code %d\n", status);
    printf("Press <reset> on the FPGA board to reboot the cpu ...\n\n");

    // If Aquila is running in a waveform simulator, we can use putchar(03)
    // to inform the simulator to end simulation if exit() has been called.
    // However, you need a UART module that invokes $finish() when a 0x03 code
    // has been sent to the UART device in simulation mode.
    putchar(03);

    while (1);
}
#pragma GCC pop_options

static int rand_seed = 27182;

void srand(unsigned int seed)
{
    rand_seed = (long) seed;
}

int rand(void)
{
    return(((rand_seed = rand_seed * 214013L + 2531011L) >> 16) & 0x7fff);
}

// added by cyhsu
char *getenv(const char *key)
{
    (void)key;
    return NULL;
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr) return malloc(size);
    if (size == 0) { free(ptr); return NULL; }
    void *new_ptr = malloc(size);
    if (new_ptr) {
        unsigned char *d = new_ptr, *s = ptr;
        for (size_t i = 0; i < size; i++) d[i] = s[i];
        free(ptr);
    }
    return new_ptr;
}

long strtol(const char *nptr, char **endptr, int base)
{
    const char *s = nptr;
    long result = 0;
    int neg = 0;

    while (*s == ' ' || *s == '\t' || *s == '\n') s++;
    if (*s == '-') { neg = 1; s++; }
    else if (*s == '+') { s++; }

    if (base == 0) {
        if (*s == '0') {
            s++;
            if (*s == 'x' || *s == 'X') { base = 16; s++; }
            else { base = 8; }
        } else { base = 10; }
    } else if (base == 16 && *s == '0' && (*(s+1) == 'x' || *(s+1) == 'X')) {
        s += 2;
    }

    while (*s) {
        int digit;
        if (*s >= '0' && *s <= '9') digit = *s - '0';
        else if (*s >= 'a' && *s <= 'z') digit = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'Z') digit = *s - 'A' + 10;
        else break;
        if (digit >= base) break;
        result = result * base + digit;
        s++;
    }
    if (endptr) *endptr = (char *)s;
    return neg ? -result : result;
}

double strtod(const char *nptr, char **endptr)
{
    const char *s = nptr;
    double result = 0.0;
    int neg = 0;

    while (*s == ' ' || *s == '\t') s++;
    if (*s == '-') { neg = 1; s++; }
    else if (*s == '+') { s++; }

    while (*s >= '0' && *s <= '9') {
        result = result * 10.0 + (*s - '0');
        s++;
    }
    if (*s == '.') {
        s++;
        double frac = 0.1;
        while (*s >= '0' && *s <= '9') {
            result += (*s - '0') * frac;
            frac *= 0.1;
            s++;
        }
    }
    if (*s == 'e' || *s == 'E') {
        s++;
        int eneg = 0, exp = 0;
        if (*s == '-') { eneg = 1; s++; }
        else if (*s == '+') { s++; }
        while (*s >= '0' && *s <= '9') {
            exp = exp * 10 + (*s - '0');
            s++;
        }
        double epow = 1.0;
        for (int i = 0; i < exp; i++) epow *= 10.0;
        if (eneg) result /= epow;
        else result *= epow;
    }
    if (endptr) *endptr = (char *)s;
    return neg ? -result : result;
}