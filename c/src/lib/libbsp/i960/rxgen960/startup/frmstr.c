/*-------------------------------------*/
/* frmstr.c                            */
/* Last change : 14.10.94              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#include "frmstr.h"
/*-------------------------------------*/
  /* How to treat the rest.
   */
#define FOR_CONSOLE	1
#define DO_LONG         1

  /* To Store a byte.
   */
#ifdef _STORE_BYTE
#  define	STORE_BYTE(a, b)	(store_byte(a, b))
   long	store_byte(void);
#else
#  define 	STORE_BYTE(a, b)	(* (a) = (b))
#endif
  /* Some decalrations.
   */
static void geta(ArgType *, int);
static const char * gnum(const char *, ArgType * );
static char * i_compute(unsigned val, int, char *);
#ifdef DO_LONG
static char * l_compute(long, int, char *);
#endif
static ArgType * nextarg;
  /* And macros.
   */
#define wsize(par) 	((sizeof par) / sizeof(ArgType))
#define signbit(par) 	(1L<<(sizeof par * 8 - 1))

int format_string(const char * fmt, ArgType * args, char * buffer)
{
  char * s;
# ifdef DO_LONG
  long l;
  int lflag;
# else
# define lflag 0
# endif
# ifdef DO_FLOAT
  double dbl;
# endif
  ArgType inte;
  ArgType_U uint;
  ArgType width, ndigit;
  int i, j, c, rjust, ndfnd, zfill;
  const char * oldfmt;
  char * s1, buf[64];

  nextarg = args;
  while (c = * fmt ++) {
    if (c != '%') {
#     ifdef FOR_CONSOLE
      if (c == '\n') STORE_BYTE(buffer ++, '\r');
#     endif
      STORE_BYTE(buffer ++, c);
      continue;
    }
#   ifdef DO_LONG
    lflag = 0 ;
#   endif
    j = 10 ;
    rjust = 0;
    if (* fmt == '-') {
      fmt ++;
      rjust ++;
    }
    zfill = ' ';
    if (* fmt == '0') {
      fmt ++;
      zfill = '0';
    }
    fmt = gnum(fmt, & width);
    ndigit = 0; ndfnd = 0;
    if (* fmt == '.') {
      fmt ++; oldfmt = fmt;
      fmt = gnum(fmt, & ndigit);
      ndfnd = (int)(fmt != oldfmt);
    }
    s = s1 = buf;
#   ifdef DO_LONG
    if (* fmt == 'l' || * fmt == 'L') {
      fmt ++; lflag ++;
    }
#   endif
    switch (c = * fmt ++) {
      default:
#       ifdef FOR_CONSOLE
        if (c == '\n') STORE_BYTE(buffer ++, '\r');
#       endif
        STORE_BYTE(buffer ++, c);
        continue;
      case 's':
        geta((ArgType *) & s1, wsize(s1));
        s = s1;
        do {
	  if (s == 0) break;
	  if (* s == 0)  
	    break;
	  s ++;
        } while (-- ndigit);
        break;
      case 'b':
        j = 2;
      case 'u':
        getu:
      
        if (! lflag) {
	  geta(& inte, wsize(inte));
	  goto i_unsignd;
        }
#     ifdef DO_LONG
      case 'U':
        getlu:
      
        geta((ArgType *) & l, wsize(l));
        goto l_unsignd;
      case 'B':
        j = 2 ;
        goto getlu;
      case 'X':
        j = 16;
        goto getlu;
      case 'O':
        j = 8;
        goto getlu ;
      case 'D':
        l_signed:
      
        geta((ArgType *) & l, wsize(l));
        if (l < 0) {
          STORE_BYTE(s ++, '-');
          l = -l;
        }
        goto do_l;
      
        l_unsignd:

        if (l && ndigit)
	  STORE_BYTE(s ++, '0');

        do_l:
      
        s = l_compute(l, j, s);
        break;
#     endif
      case 'x':
        j = 16;
        goto getu;
      case 'o':
        j = 8;
        goto getu;
      case 'd':
        if (lflag) goto l_signed;
        geta(& inte, wsize(inte));
        if (inte < 0) {
	  STORE_BYTE(s ++, '-');
	  inte = - inte;
        }
        goto do_i;
      
        i_unsignd:
      
        if (inte && ndigit)
	  STORE_BYTE(s ++, '0');

        do_i:
      
	s = i_compute(inte, j, s);
        break;
      case 'c':
	geta ((ArgType *) & uint, wsize(uint));
	for (i = sizeof uint - 1; i >= 0; i --) {
	  if (STORE_BYTE(s, uint % 256)) s ++;
	  uint /= 256 ;
	}
	break;
#     ifdef DO_FLOAT
      case 'e':
        geta((ArgType *) & dbl, wsize(dbl));
        s = _pscien(dbl, s, ndigit, ndfnd);
        break;
      case 'f':
        geta((ArgType *) &dbl,wsize(dbl));
        s = _pfloat(dbl, s, ndigit, ndfnd);
        break;
#     endif
      case 'r':
        geta((ArgType *) & nextarg, wsize(nextarg));
        geta((ArgType *) & oldfmt, wsize(fmt));
        fmt = oldfmt;
        continue;
    }
    j = s - s1;
    if ((c = width - j) > 0)  {
      if (rjust == 0)  {
	do STORE_BYTE(buffer ++, zfill);
	while (-- c);
      }
    }  
    while (-- j >= 0)
      STORE_BYTE(buffer ++, * s1 ++);
    while (-- c >= 0)
      STORE_BYTE(buffer ++, zfill);
  }
  STORE_BYTE(buffer, 0);
  return 0;
}
static void geta(ArgType * p, int size)
{
  if ((ArgType *) & p - (ArgType *) & size > 0)  {
    p += size;
    while (size --)  {
	* -- p = * nextarg --;
    }
  }
  else  {
    while (size --)  {
      * p ++ = * nextarg ++ ;
    }
  }
}
static const char * gnum(const char * f, ArgType * ip)
{
  ArgType i;
  int c;

  if (* f == '*')  {
    geta(ip, wsize(i)) ;
    f ++;
  }
  else  {
    i = 0;
    while ((c = * f - '0') >= 0 && c <= 9) {
      i = i * 10 + c;
      f ++;
    }
    * ip = i;
  }
  return f;
}
static char * i_compute(unsigned int val, int base, char * s)
{
  int c;

  c = val % base;
  val /= base;
  if (val)
    s = i_compute(val, base, s);
  STORE_BYTE(s ++, c>9 ? c-10+'a' : c+'0');
  return s;
}
#ifdef DO_LONG
static char *l_compute(long l1,int d, char * s)
{
  int c;
  long l2;

  if (l1 < 0)  {
    c = l1 & 1;
    l2 = ((l1>>1) & ~signbit(l1));
    l1 = l2 / (d>>1);
    c += (l2%(d>>1))<<1;
  } 
  else  {
    c = l1 % d;
    l1 = l1 / d;
  }
  if (l1)
    s = l_compute(l1, d, s);
  STORE_BYTE(s ++, c>9 ? c-10+'A' : c+'0');
  return s;
}
#endif
#ifdef _STORE_BYTE
long store_byte(char * cp, long c)
{
  long shift, reg, * ptr;

  shift = ((long) cp & 3) * 8;
  ptr = (long *) ((long) cp & ~3);
  reg = * ptr;
  reg &= ~(0xff << shift);
  reg |= c << shift;
  * ptr = reg;

  return c;
}
#endif

#define	SPC	01
#define	STP	02

#define NULL 	0
#define EOF	0
#define	SHORT	0
#define	REGULAR	1
#define	LONG	2
#define	INT	0
#define	FLOAT	1

static int new_c(void); 
static void unnew_c(char);
static int _innum(int ** ptr, int type, int len, int size, int * eofptr);
static int _instr(char * ptr, int type, int len, int * eofptr);
static const char * _getccl(const char *);
static int vme_isupper(char);
static int vme_tolower(char);
static int vme_isdigit(char);

static char _sctab[128] = {
	0,0,0,0,0,0,0,0,
	0,SPC,SPC,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	SPC,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};
static const char * line;
static char * linep;

int unformat_string(const char * fmt, int ** argp, const char * buffer)
{
  int ch;
  int nmatch, len, ch1;
  int ** ptr, fileended, size;

  line = buffer;
  linep = (char*)line;
  
  nmatch = 0;
  fileended = 0;
  for (;;) switch (ch = * fmt ++)  {
    case '\0': 
      return (nmatch);
    case '%': 
      if ((ch = * fmt ++) == '%')
        goto def;
      ptr = 0;
      if (ch != '*')
	ptr = argp ++;
      else
	ch = * fmt ++;
      len = 0;
      size = REGULAR;
      while (vme_isdigit(ch))  {
	len = len*10 + ch - '0';
	ch = * fmt ++;
      }
      if (len == 0)
	len = 30000;
      if (ch == 'l')  {
	ch = * fmt ++;
	size = LONG;
      }
      else if (ch == 'h')  {
	size = SHORT;
	ch = * fmt ++;
      }
      else if (ch=='[')
	fmt = _getccl(fmt);
	if (vme_isupper(ch)) {
	  ch = vme_tolower(ch);
	  size = LONG;
	}
	if (ch == '\0')
	  return -1;
	if (_innum(ptr, ch, len, size, & fileended) && ptr)
	  nmatch ++;
	if (fileended)
	  return nmatch? nmatch: -1;
	break;
    case ' ':
    case '\n':
    case '\t': 
      while ((ch1 = new_c())==' ' || ch1=='\t' || ch1=='\n')
	;
      if (ch1 != EOF)
	unnew_c(ch1);
      break;
    default:
     
    def:
    
      ch1 = new_c();
      if (ch1 != ch)  {
	if (ch1==EOF)
	  return -1 ;
	unnew_c(ch1);
	return nmatch;
      }
   }
}
static int new_c()             
{
  char c;
  
  if (linep)  {
    c = * linep ++;
    return c;
  }
  else  {
    return 0;
  }
}
static void unnew_c(char ch)
{
  if (linep > line)
    * (-- linep) = ch;
}
static int _innum(int ** ptr, int type, int len, int size, int * eofptr)
{
# ifdef DO_FLOAT
  extern double atof();
# endif  
  char * np;
  char numbuf[64];
  int c, base;
  int expseen, scale, negflg, c1, ndigit;
  long lcval;

  if (type=='c' || type=='s' || type=='[')
    return _instr(ptr? * (char **) ptr: (char *) NULL, type, len, eofptr);
  lcval = 0;
  ndigit = 0;
  scale = INT;
  if (type=='e'||type=='f')
    scale = FLOAT;
  base = 10;
  if (type=='o')
    base = 8;
  else if (type=='x')
    base = 16;
  np = numbuf;
  expseen = 0;
  negflg = 0;
  while ((c = new_c())==' ' || c=='\t' || c=='\n');
  if (c=='-') {
    negflg ++;
    * np ++ = c;
    c = new_c();
    len --;
  }
  else if (c=='+') {
    len --;
    c = new_c();
  }
  for ( ; -- len >= 0; * np ++ = c, c = new_c()) {
    if (vme_isdigit(c)
	|| base==16 && ('a'<=c && c<='f' || 'A'<=c && c<='F')) {
      ndigit ++;
      if (base==8)
	lcval <<=3;
      else if (base==10)
	lcval = ((lcval<<2) + lcval)<<1;
      else
	lcval <<= 4;
      c1 = c;
      if ('0'<=c && c<='9')
        c -= '0';
      else if ('a'<=c && c<='f')
	c -= 'a'-10;
      else
	c -= 'A'-10;
      lcval += c;
      c = c1;
      continue;
    }
    else if (c=='.')  {
      if (base!=10 || scale==INT)
	break;
      ndigit ++;
      continue;
    }
    else if ((c=='e'||c=='E') && expseen==0)  {
      if (base!=10 || scale==INT || ndigit==0)
	break;
      expseen ++;
      * np ++ = c;
      c = new_c();
      if (c!='+'&&c!='-'&&('0'>c||c>'9'))
	break;
    }
    else
      break;
  }
  if (negflg)
    lcval = -lcval;
  if (c != EOF) {
    unnew_c(c);
    * eofptr = 0;
  }
  else
    * eofptr = 1;
  if (ptr==NULL || np==numbuf)
    return 0;
  * np ++ = 0;
  switch ((scale<<4) | size)  {
#   ifdef DO_FLOAT

    case (FLOAT<<4) | SHORT:
    case (FLOAT<<4) | REGULAR:
      ** (float **) ptr = atof(numbuf);
      break;

    case (FLOAT<<4) | LONG:
      ** (double **) ptr = atof(numbuf);
      break;
#   endif
    case (INT<<4) | SHORT:
      ** (short **) ptr = lcval;
      break;

    case (INT<<4) | REGULAR:
      ** (int **) ptr = lcval;
      break;

    case (INT<<4) | LONG:
      ** (long **) ptr = lcval;
      break;
  }
  return 1;
}
static int _instr(char * ptr, int type, int len, int * eofptr)
{
  int ch;
  char * optr;
  int ignstp;

  * eofptr = 0;
  optr = ptr;
  if (type=='c' && len==30000)
    len = 1;
  ignstp = 0;
  if (type=='s')
    ignstp = SPC;
  while (_sctab[ch = new_c()] & ignstp)
    if (ch==EOF)
      break;
  ignstp = SPC;
  if (type=='c')
    ignstp = 0;
  else if (type=='[')
    ignstp = STP;
  while (ch!=EOF && (_sctab[ch]&ignstp)==0)  {
    if (ptr)
      * ptr ++ = ch;
    if (-- len <= 0)
      break;
    ch = new_c();
  }
  if (ch != EOF)  {
    if (len > 0)
      unnew_c(ch);
    * eofptr = 0;
  }
  else
    * eofptr = 1;
  if (ptr && ptr!=optr) {
    if (type!='c')
      * ptr ++ = '\0';
    return 1;
  }
  return 0;
}  
static const char * _getccl(const char * s)
{
  int c, t;

  t = 0;
  if (* s == '^') {
    t ++;
    s ++;
  }
  for (c = 0; c < 128; c++)
    if (t)
      _sctab[c] &= ~STP;
    else
      _sctab[c] |= STP;
    while (((c = * s ++)&0177) != ']') {
    if (t)
      _sctab[c++] |= STP;
    else
      _sctab[c++] &= ~STP;
    if (c==0)
      return -- s;
  }
  return s;
}
static int vme_isupper(char ch)
{
  if( ch >= 'A' & ch <= 'Z')
    return 1;
  else
    return 0;
}
static int vme_tolower(char ch)
{
  return 'a' + 'A' - ch;
}
static vme_isdigit(char ch)
{
  if (ch >= '0' & ch <= '9')
    return 1;
  else
    return 0;
}
/*-------------*/
/* End of file */
/*-------------*/

