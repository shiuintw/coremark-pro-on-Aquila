//
// Created by shiuintw on 2026/4/18.
//
/*
 * al_stubs.c - Stub implementations for bare-metal (no file IO, no SMP)
 */
#include <stddef.h>
#include <stdarg.h>

typedef void ee_FILE;

/* al_file stubs */
int      al_fclose(ee_FILE *fp)   { (void)fp; return 0; }
int      al_ferror(ee_FILE *fp)   { (void)fp; return 0; }
int      al_feof(ee_FILE *fp)     { (void)fp; return 1; }
void     al_clearerr(ee_FILE *fp) { (void)fp; }
int      al_fileno(ee_FILE *fp)   { (void)fp; return -1; }
int      al_fflush(ee_FILE *fp)   { (void)fp; return 0; }
int      al_vfprintf(ee_FILE *fp, const char *fmt, va_list ap) { (void)fp; (void)fmt; (void)ap; return 0; }
int      al_fseek(ee_FILE *fp, long offset, int whence) { (void)fp; (void)offset; (void)whence; return -1; }
long     al_ftell(ee_FILE *fp)    { (void)fp; return -1; }
int      al_putc(int c, ee_FILE *fp)  { (void)fp; return c; }
int      al_getc(ee_FILE *fp)     { (void)fp; return -1; }
int      al_ungetc(int c, ee_FILE *fp) { (void)c; (void)fp; return -1; }
char    *al_fgets(char *s, int n, ee_FILE *fp) { (void)s; (void)n; (void)fp; return NULL; }
int      al_fputs(const char *s, ee_FILE *fp)  { (void)s; (void)fp; return -1; }
size_t   al_fread(void *buf, size_t size, size_t count, ee_FILE *fp)  { (void)buf; (void)size; (void)count; (void)fp; return 0; }
size_t   al_fwrite(const void *buf, size_t size, size_t count, ee_FILE *fp) { (void)buf; (void)size; (void)count; (void)fp; return 0; }
ee_FILE *al_fopen(const char *f, const char *m)  { (void)f; (void)m; return NULL; }
ee_FILE *al_fdopen(int fd, const char *m)        { (void)fd; (void)m; return NULL; }
ee_FILE *al_freopen(const char *f, const char *m, ee_FILE *fp) { (void)f; (void)m; (void)fp; return NULL; }
ee_FILE *al_tmpfile(void)    { return NULL; }
char    *al_mktemp(char *t)  { (void)t; return NULL; }
int      al_stat(const char *p, void *b)  { (void)p; (void)b; return -1; }
int      al_fstat(int fd, void *b)        { (void)fd; (void)b; return -1; }
int      al_unlink(const char *p)         { (void)p; return -1; }
int      al_rename(const char *o, const char *n) { (void)o; (void)n; return -1; }