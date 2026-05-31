// GEN By Claude
#ifndef _CTYPE_H
#define _CTYPE_H

#define _U  0x01  /* upper */
#define _L  0x02  /* lower */
#define _N  0x04  /* digit */
#define _S  0x08  /* space */
#define _P  0x10  /* punct */
#define _C  0x20  /* cntrl */
#define _X  0x40  /* hex */
#define _B  0x80  /* blank */

extern const unsigned char _ctype_[];

#define isalpha(c)  (_ctype_[(unsigned char)(c)+1] & (_U|_L))
#define isupper(c)  (_ctype_[(unsigned char)(c)+1] & _U)
#define islower(c)  (_ctype_[(unsigned char)(c)+1] & _L)
#define isdigit(c)  (_ctype_[(unsigned char)(c)+1] & _N)
#define isxdigit(c) (_ctype_[(unsigned char)(c)+1] & (_N|_X))
#define isspace(c)  (_ctype_[(unsigned char)(c)+1] & _S)
#define ispunct(c)  (_ctype_[(unsigned char)(c)+1] & _P)
#define isalnum(c)  (_ctype_[(unsigned char)(c)+1] & (_U|_L|_N))
#define iscntrl(c)  (_ctype_[(unsigned char)(c)+1] & _C)
#define isprint(c)  (_ctype_[(unsigned char)(c)+1] & (_P|_U|_L|_N|_B))
#define isgraph(c)  (_ctype_[(unsigned char)(c)+1] & (_P|_U|_L|_N))
#define tolower(c)  (isupper(c) ? (c)+('a'-'A') : (c))
#define toupper(c)  (islower(c) ? (c)-('a'-'A') : (c))

#endif
