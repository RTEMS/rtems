/*
 *  C4x simulator IO interface routines based on code provided
 *  by Herman ten Brugge <Haj.Ten.Brugge@net.HCC.nl>
 *
 *  $Id$
 */

#include <stdio.h>

#define	SIM_OPEN	(0xf0)
#define	SIM_CLOSE	(0xf1)
#define	SIM_READ	(0xf2)
#define	SIM_WRITE	(0xf3)
#define	SIM_LSEEK	(0xf4)
#define	SIM_UNLINK	(0xf5)
#define	SIM_GETENV	(0xf6)
#define	SIM_RENAME	(0xf7)
#define	SIM_GETTIME	(0xf8)
#define	SIM_GETCLOCK	(0xf9)

typedef union _io {
  struct _open {
    int	fd    : 16;
    int	flags : 16;
  } open;
  struct _openr {
    int	result : 16;
  } openr;
  struct _close {
    int	fd     : 16;
  } close;
  struct _closer {
    int	result : 16;
  } closer;
  struct _read {
    int	fd    : 16;
    int	count : 16;
  } read;
  struct _readr {
    int	result : 16;
  } readr;
  struct _write {
    int	fd    : 16;
    int	count : 16;
  } write;
  struct _writer {
    int	result : 16;
  } writer;
  struct _lseek {
    int	fd         : 16;
    int	offsetlow  : 16;
    int	offsethigh : 16;
    int	orgin	   : 16;
  } lseek;
  struct _lseekr {
    int	result;
  } lseekr;
  struct _unlinkr {
    int	result : 16;
  } unlinkr;
  struct _renamer {
    int	result : 16;
  } renamer;
  struct _getenvr {
    int	result : 16;
  } getenvr;
  struct _gettimer {
    int	result;
  } gettimer;
  struct _getclockr {
    int	result;
  } getclockr;
  struct _common {
    int	word1;
    int	word2;
  } common;
} io;

static void to_sim(int command, io *param, char *data, int length);
static void call_sim(void);
static void from_sim(io *param, char *data);

void sim_exit(void)
{
  __asm__(" .global C$$EXIT");
  __asm__("C$$EXIT: nop");
  __asm__("nop");
}

int sim_open(const char *path, unsigned flags, int fno)
{
  io param;

  param.open.fd = fno;
  param.open.flags = flags;
  to_sim(SIM_OPEN,&param,(char *)path,strlen(path)+1);
  call_sim();
  from_sim(&param, NULL);
  return param.openr.result;
}

int sim_close(int fno)
{
  io param;

  param.close.fd = fno;
  to_sim(SIM_CLOSE,&param,NULL,0);
  call_sim();
  from_sim(&param, NULL);
  return param.closer.result;
}

int sim_read(int fno, char *buf, unsigned count)
{
  io param;

  param.read.fd = fno;
  param.read.count = count;
  to_sim(SIM_READ,&param,NULL,0);
  call_sim();
  from_sim(&param, buf);
  return param.readr.result;
}

int sim_write(int fno, const char *buf, unsigned count)
{
  io param;

  param.write.fd = fno;
  param.write.count = count;
  to_sim(SIM_WRITE,&param,(char *)buf,count);
  call_sim();
  from_sim(&param, NULL);
  return param.writer.result;
}

fpos_t sim_lseek(int fno, fpos_t offset, int origin)
{
  io param;

  param.lseek.fd = fno;
  param.lseek.offsetlow = offset & 0xffff;
  param.lseek.offsethigh = offset >> 16;
  to_sim(SIM_LSEEK,&param,NULL,0);
  call_sim();
  from_sim(&param, NULL);
  return param.lseekr.result;
}

int sim_unlink(const char *path)
{
  io param;

  to_sim(SIM_UNLINK,NULL,(char *)path,strlen(path)+1);
  call_sim();
  from_sim(&param, NULL);
  return param.unlinkr.result;
}

int sim_rename(const char *old, const char *new)
{
  int l;
  static char combined[200];
  io param;

  strcpy(combined,old);
  l = strlen(old)+1;
  strcpy(combined+l,new);
  l += strlen(new) + 1;
  to_sim(SIM_RENAME,NULL,combined,l);
  call_sim();
  from_sim(&param, NULL);
  return param.renamer.result;
}

char *sim_getenv(const char *str)
{
  io param;
  static char result[200];

  to_sim(SIM_GETENV,NULL,(char *)str,strlen(str)+1);
  call_sim();
  from_sim(&param, result);
  return param.getenvr.result ? result : NULL;
}

int sim_gettime(void)
{
  io param;

  to_sim(SIM_GETTIME,NULL,NULL,0);
  call_sim();
  from_sim(&param, NULL);
  return param.gettimer.result;
}

int sim_getclock(void)
{
  io param;

  to_sim(SIM_GETCLOCK,NULL,NULL,0);
  call_sim();
  from_sim(&param, NULL);
  return param.getclockr.result;
}

int _CIOBUF_[BUFSIZ+32];

static void to_sim(int command, io *param, char *data, int length)
{
  int i;
  int n;
  int v;
  int *ip = &_CIOBUF_[0];

  *ip++ = length;
  *ip++ = command;
  if (param) {
    *ip++ = param->common.word1;
    *ip++ = param->common.word2;
  }
  else {
    *ip++ = 0;
    *ip++ = 0;
  }
  n = length & ~3;
  for (i = 0 ; i < n ; i += 4) {
    v = *data++ & 0xff;
    v |= (*data++ & 0xff) << 8;
    v |= (*data++ & 0xff) << 16;
    v |= (*data++ & 0xff) << 24;
    *ip++ = v;
  }
  v = 0;
  for ( ; i < length ; i++) {
    v |= (*data++ & 0xff) << ((i & 3) << 3);
  }
  *ip = v;
}

static void call_sim(void)
{
  __asm__(" .global C$$IO$$");
  __asm__("C$$IO$$: nop");
}

static void from_sim(io *param, char *data)
{
  int i;
  int l;
  int n;
  int v;
  int *ip = &_CIOBUF_[0];

  l = *ip++;
  param->common.word1 = *ip++;
  param->common.word2 = *ip++;
  if (data != NULL) {
    n = l & ~3;
    for (i = 0 ; i < n ; i += 4) {
      v = *ip++;
      *data++ = v & 0xff;
      *data++ = (v >> 8) & 0xff;
      *data++ = (v >> 16) & 0xff;
      *data++ = (v >> 24) & 0xff;
    }
    v = *ip;
    for (; i < l ; i++) {
      *data++ = v >> ((i & 3) << 3);
    }
  }
}

#if 0
#include <fcntl.h>
sim_io_test()
{
sim_write(1, "howdy\n", 6);
}
#endif

/*
 *  Debug junk
 */
#if 0
void printk_wrapper(void)
{
  __asm__(" .global _printf");
  __asm__("_printf: bu	_printk");
}
#endif

#if 1
#ifdef _HAVE_STDC
#include <stdarg.h>
#else
#include <varargs.h>
#endif

int __svfscanf(
     register FILE *fp,
     char const *fmt0,
     va_list ap
)
{
  return 0;
}
#endif
