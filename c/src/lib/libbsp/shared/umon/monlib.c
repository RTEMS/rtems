/*
 *  monlib.c -
 *  This file is part of the monitor code, but it is actually linked into
 *  the application.  It is built with (but not linked with) the monitor,
 *  then the monlib.o file is linked with the application.
 *  The only requirement on the application is that it know where the address
 *  of the monCom function is in the monitor's space.
 *  The monCom function will be accessible in some "well-known" way (processor
 *  and platform dependent) so that this will not be a problem.
 *
 *  This monlib.c file is a replacement for the older mechanism that was
 *  a bit more error-prone...  A table of function pointers existed at some
 *  well-known location in the monitor, and the content of that table was
 *  assumed to also be "well-known".  This new version only assumes that the
 *  pointer to monCom is well-known; everything else will work based on the
 *  fact that the monitor and application will share the monlib.h header
 *  file.
 *
 **************************************************************************
 *  General notice:
 *  This code is part of a boot-monitor package developed as a generic base
 *  platform for embedded system designs.  As such, it is likely to be
 *  distributed to various projects beyond the control of the original
 *  author.  Please notify the author of any enhancements made or bugs found
 *  so that all may benefit from the changes.  In addition, notification back
 *  to the author will allow the new user to pick up changes that may have
 *  been made by other users after this version of the code was distributed.
 *
 *  Note1: the majority of this code was edited with 4-space tabs.
 *  Note2: as more and more contributions are accepted, the term "author"
 *         is becoming a mis-representation of credit.
 *
 *  Original author:    Ed Sutter
 *  Email:              esutter@alcatel-lucent.com
 *  Phone:              908-582-2351
 **************************************************************************
 *
 *  Ed Sutter has been informed that this code is being used in RTEMS.
 *
 *  This code was reformatted by Joel Sherrill from OAR Corporation and
 *  Fernando Nicodemos <fgnicodemos@terra.com.br> from NCB - Sistemas
 *  Embarcados Ltda. (Brazil) to be more compliant with RTEMS coding
 *  standards and to eliminate C++ style comments.
 */

#include <umon/monlib.h>

static int		(*_tfsseek)(int,int,int);
static int 		(*_tfsgetline)(int,char *,int);
static int		(*_tfsipmod)(char *,char *,int,int);
static int		(*_tfsinit)(void);
static int		(*_tfsadd)(char *,char *,char *,unsigned char *,int);
static int		(*_tfsunlink)(char *);
static int		(*_tfsrun)(char **,int);
static int 		(*_tfsread)(int,char *,int);
static int		(*_tfswrite)(int,char *,int);
static int		(*_tfsopen)(char *,long,char *);
static int		(*_tfsclose)(int,char *);
static int		(*_printf)(
  char *, int,int,int,int,int,int,int,int,int,int,int,int);
static int		(*_cprintf)(
  char *, int,int,int,int,int,int,int,int,int,int,int,int);
static int		(*_sprintf)(
  char *, char *, int,int,int,int,int,int,int,int,int,int,int,int);
static int		(*_monrestart)(int);
static int		(*_rputchar)(unsigned char c);
static int		(*_getchar)(void);
static int		(*_gotachar)(void);
static int		(*_getbytes)(char *,int,int);
static int		(*_addcommand)(struct monCommand *,char *);
static int		(*_docommand)(char *,int);
static int		(*_getline)(char *,int,int);
static int		(*_tfsfstat)(char *,struct tfshdr *);
static int		(*_tfseof)(int);
static int		(*_decompress)(char *,int,char *);
static int		(*_tfstruncate)(int,long);
static int		(*_heapextend)(char *,int);
static int		(*_tfslink)(char *,char *);
static int		(*_pcicfgwrite)(int,int,int,int,int,unsigned long);
static int		(*_i2cwrite)(int,int,unsigned char *,int);
static int		(*_i2cread)(int,int,unsigned char *,int);
static int 		(*_flashwrite)(char *,char *,int);
static int		(*_flasherase)(int);
static int		(*_flashinfo)(int,int *,char **);
static int		(*_flashoverride)(void *,int,int);
static int		(*_sendenet)(char *,int);
static int		(*_recvenet)(char *,int);
static int		(*_printpkt)(char *,int,int);
static int		(*_setenv)(char *,char *);
static int		(*_watchdog)(void);
static int		(*_timeofday)(int,void *);
static int		(*_montimer)(int cmd, void *arg);

static char		*(*_getenv)(char *);
static char		*(*_version)(void);
static char		*(*_getenvp)(void);
#ifdef MALLOC_DEBUG
static char		*(*_malloc)(int,char *,int);
static char		*(*_realloc)(char *buf,int,char *,int);
#else
static char		*(*_malloc)(int);
static char		*(*_realloc)(char *,int);
#endif
static char		*(*_getsym)(char *,char *,int);

static void		(*_intsrestore)(unsigned long);
static void		(*_appexit)(int);
static void		(*_free)(char *);
static void		(*_getargv)(int *,char ***);
static void		(*_profiler)(void *);
static void		(*_bbc)(char *,int);
static void		(*_memtrace)(
  char *, int,int,int,int,int,int,int,int,int,int,int,int);
static void		(*_appwarmstart)(unsigned long);
static void		(*_mondelay)(long);
static void		(*_printmem)(char *,int,int);

static long		(*_tfsctrl)(int,long,long);
static long		(*_tfstell)(int);
static long		(*_portcmd)(int,void *);

static struct	tfshdr *(*_tfsnext)(struct tfshdr *);
static struct	tfshdr *(*_tfsstat)(char *);

static unsigned long	(*_i2cctrl)(int,int,unsigned long,unsigned long);
static unsigned long	(*_pcicfgread)(int,int,int,int,int);
static unsigned long	(*_pcictrl)(int,int,unsigned long,unsigned long);
static unsigned long	(*_crc32)(unsigned char *,unsigned long);
static unsigned long	(*_intsoff)(void);
static unsigned long	(*_assign_handler)(long,unsigned long,unsigned long);

static unsigned short	(*_xcrc16)(unsigned char *,unsigned long);


static void (*_monlock)(void);
static void (*_monunlock)(void);
static int	(*_moncom)(int,void *,void *, void *);

/**************************************************************************
 *
 * The following macros support the default monitor lock/unlock mechanism when
 * they point to monLock and monUnlock.  If something other than the default
 * is to be used, then simply redefine them here.  Refer to the monitor
 * app note that discusses multi-tasking access to the monitor API for more
 * information.
 *
 * TFS_MONLOCK/UNLOCK:
 * Lock/unlock for functions that access TFS flash space:
 */
#define TFS_MONLOCK			monLock
#define TFS_MONUNLOCK		monUnlock

/* ENV_MONLOCK/UNLOCK:
 * Lock/unlock for functions that access monitor shell variables:
 */
#define ENV_MONLOCK			monLock
#define ENV_MONUNLOCK		monUnlock

/* CONSOLE_MONLOCK/UNLOCK:
 * Lock/unlock for functions in the monitor that deal with console output.
 */
#define CONSOLE_MONLOCK		monLock
#define CONSOLE_MONUNLOCK	monUnlock

/* HEAP_MONLOCK/UNLOCK:
 * Lock/unlock for functions in the monitor that deal with the heap.
 */
#define HEAP_MONLOCK		monLock
#define HEAP_MONUNLOCK		monUnlock

/* BLOCKING_MONLOCK/UNLOCK:
 * Lock/unlock for functions in the monitor that block waiting for
 * console input.
 */
#define BLOCKING_MONLOCK	monLock
#define BLOCKING_MONUNLOCK	monUnlock

/* GENERIC_MONLOCK/UNLOCK:
 * Lock/unlock for all functions not covered by the above macros.
 */
#define GENERIC_MONLOCK		monLock
#define GENERIC_MONUNLOCK	monUnlock

/**************************************************************************
 *
 * monConnect():
 *	This must be the first call by the application code to talk to the
 *	monitor.  It is expecting three incoming function pointers:
 *
 *	mon:	Points to the monitor's _moncom function;
 *			This is a "well-known" address because the monitor and
 *			application code (two separately linked binaries) must
 *			know it.
 *	lock:	Points to a function in the application code that will be
 *			used by the monitor as a lock-out function (some kind of
 *			semaphore in the application).
 *	unlock:	Points to a function in the application code that will be
 *			used by the monitor as an un-lock-out function (undo whatever
 *			lock-out mechanism was done by lock).
 */
int
monConnect(int (*mon)(int,void *,void *,void *),
	void (*lock)(void), void (*unlock)(void))
{
	int rc = 0;

	/* Assign incoming lock and unlock functions... */
	_monlock = lock;
	_monunlock = unlock;

	/* If the mon pointer is non-zero, then make the mon_ connections... */
	if (mon) {

		_moncom = mon;

		/* Make the connections between "mon_" functions that are			*/
		/* symbolically	accessible by the application and the corresponding	*/
		/* functions that exists in the monitor.							*/
		rc += _moncom(GETMONFUNC_PUTCHAR,&_rputchar,0,0);
		rc += _moncom(GETMONFUNC_GETCHAR,&_getchar,0,0);
		rc += _moncom(GETMONFUNC_GOTACHAR,&_gotachar,0,0);
		rc += _moncom(GETMONFUNC_GETBYTES,&_getbytes,0,0);
		rc += _moncom(GETMONFUNC_PRINTF,&_printf,0,0);
		rc += _moncom(GETMONFUNC_CPRINTF,&_cprintf,0,0);
		rc += _moncom(GETMONFUNC_SPRINTF,&_sprintf,0,0);
		rc += _moncom(GETMONFUNC_RESTART,&_monrestart,0,0);
		rc += _moncom(GETMONFUNC_GETENV,&_getenv,0,0);
		rc += _moncom(GETMONFUNC_SETENV,&_setenv,0,0);
		rc += _moncom(GETMONFUNC_TFSINIT,&_tfsinit,0,0);
		rc += _moncom(GETMONFUNC_TFSADD,&_tfsadd,0,0);
		rc += _moncom(GETMONFUNC_TFSUNLINK,&_tfsunlink,0,0);
		rc += _moncom(GETMONFUNC_TFSRUN,&_tfsrun,0,0);
		rc += _moncom(GETMONFUNC_TFSNEXT,&_tfsnext,0,0);
		rc += _moncom(GETMONFUNC_TFSSTAT,&_tfsstat,0,0);
		rc += _moncom(GETMONFUNC_TFSREAD,&_tfsread,0,0);
		rc += _moncom(GETMONFUNC_TFSWRITE,&_tfswrite,0,0);
		rc += _moncom(GETMONFUNC_TFSOPEN,&_tfsopen,0,0);
		rc += _moncom(GETMONFUNC_TFSCLOSE,&_tfsclose,0,0);
		rc += _moncom(GETMONFUNC_TFSSEEK,&_tfsseek,0,0);
		rc += _moncom(GETMONFUNC_TFSGETLINE,&_tfsgetline,0,0);
		rc += _moncom(GETMONFUNC_TFSIPMOD,&_tfsipmod,0,0);
		rc += _moncom(GETMONFUNC_TFSCTRL,&_tfsctrl,0,0);
		rc += _moncom(GETMONFUNC_ADDCOMMAND,&_addcommand,0,0);
		rc += _moncom(GETMONFUNC_DOCOMMAND,&_docommand,0,0);
		rc += _moncom(GETMONFUNC_GETARGV,&_getargv,0,0);
		rc += _moncom(GETMONFUNC_CRC16,&_xcrc16,0,0);
		rc += _moncom(GETMONFUNC_CRC32,&_crc32,0,0);
		rc += _moncom(GETMONFUNC_INTSOFF,&_intsoff,0,0);
		rc += _moncom(GETMONFUNC_INTSRESTORE,&_intsrestore,0,0);
		rc += _moncom(GETMONFUNC_APPEXIT,&_appexit,0,0);
		rc += _moncom(GETMONFUNC_MALLOC,&_malloc,0,0);
		rc += _moncom(GETMONFUNC_FREE,&_free,0,0);
		rc += _moncom(GETMONFUNC_GETLINE,&_getline,0,0);
		rc += _moncom(GETMONFUNC_TFSFSTAT,&_tfsfstat,0,0);
		rc += _moncom(GETMONFUNC_TFSEOF,&_tfseof,0,0);
		rc += _moncom(GETMONFUNC_DECOMPRESS,&_decompress,0,0);
		rc += _moncom(GETMONFUNC_TFSTRUNCATE,&_tfstruncate,0,0);
		rc += _moncom(GETMONFUNC_HEAPXTEND,&_heapextend,0,0);
		rc += _moncom(GETMONFUNC_PROFILER,&_profiler,0,0);
		rc += _moncom(GETMONFUNC_TFSLINK,&_tfslink,0,0);
		rc += _moncom(GETMONFUNC_BBC,&_bbc,0,0);
		rc += _moncom(GETMONFUNC_MEMTRACE,&_memtrace,0,0);
		rc += _moncom(GETMONFUNC_TFSTELL,&_tfstell,0,0);
		rc += _moncom(GETMONFUNC_VERSION,&_version,0,0);
		rc += _moncom(GETMONFUNC_WARMSTART,&_appwarmstart,0,0);
		rc += _moncom(GETMONFUNC_PCICFGREAD,&_pcicfgread,0,0);
		rc += _moncom(GETMONFUNC_PCICFGWRITE,&_pcicfgwrite,0,0);
		rc += _moncom(GETMONFUNC_PCICONTROL,&_pcictrl,0,0);
		rc += _moncom(GETMONFUNC_I2CREAD,&_i2cread,0,0);
		rc += _moncom(GETMONFUNC_I2CWRITE,&_i2cwrite,0,0);
		rc += _moncom(GETMONFUNC_I2CCONTROL,&_i2cctrl,0,0);
		rc += _moncom(GETMONFUNC_MONDELAY,&_mondelay,0,0);
		rc += _moncom(GETMONFUNC_GETENVP,&_getenvp,0,0);
		rc += _moncom(GETMONFUNC_REALLOC,&_realloc,0,0);
		rc += _moncom(GETMONFUNC_SENDENETPKT,&_sendenet,0,0);
		rc += _moncom(GETMONFUNC_RECVENETPKT,&_recvenet,0,0);
		rc += _moncom(GETMONFUNC_GETSYM,&_getsym,0,0);
		rc += _moncom(GETMONFUNC_PRINTPKT,&_printpkt,0,0);
		rc += _moncom(GETMONFUNC_FLASHWRITE,&_flashwrite,0,0);
		rc += _moncom(GETMONFUNC_FLASHERASE,&_flasherase,0,0);
		rc += _moncom(GETMONFUNC_FLASHINFO,&_flashinfo,0,0);
		rc += _moncom(GETMONFUNC_ASSIGNHDLR,&_assign_handler,0,0);
		rc += _moncom(GETMONFUNC_WATCHDOG,&_watchdog,0,0);
		rc += _moncom(GETMONFUNC_PRINTMEM,&_printmem,0,0);
		rc += _moncom(GETMONFUNC_PORTCMD,&_portcmd,0,0);
		rc += _moncom(GETMONFUNC_TIMEOFDAY,&_timeofday,0,0);
		rc += _moncom(GETMONFUNC_TIMER,&_montimer,0,0);
		rc += _moncom(GETMONFUNC_FLASHOVRRD,&_flashoverride,0,0);
	}
	return(rc);
}

/* ignorelock:
 * Used as a back-door to disable the monLock()/monUnlock() stuff.
 * This is useful if the application CLI falls through to the monitor's
 * CLI and you are using the "call" command in the monitor to execute some
 * function that has a mon_xxx function in it.  In this case, the fact that
 * the application has fallen through to the monitor means that the lock
 * is already active, so when the function tries to call some other mon_xxx
 * function it won't be able to because of the lock already being set.
 *
 * With these functions in the application space, the user can do the
 * following:
 *	call %DisableLock
 *  call %Func_with_monXXX_in_it
 *  call %EnableLock
 *
 * Note that this is NOT to be used by application code, it is simply a
 * back-door mechanism to allow "call" from the CLI to invoke functions
 * that have mon_XXX functionality in them.
 */
static int ignorelock = 0;

static void
DisableMonLock(void)
{
	ignorelock = 2;
}

static void
EnableMonLock(void)
{
	ignorelock = 0;
}

/* monLock() & monUnlock():
 * Used by all of the wrapper functions below this point to call
 * the function pointed to by _monlock & _monunlock function pointers
 * (if set).
 * These functions must test both the function pointer and the state
 * of the ignorelock variable.  The function DisableMonLock() sets the
 * ignorelock variable to 2 because it is being executed through "call"
 * which means that the lock is active.
 */
static void
monLock(void)
{
	if (_monlock) {
		switch(ignorelock) {
			case 1:
				break;
			case 2:
				ignorelock--;
				break;
			default:
				_monlock();
				break;
		}
	}
}

static void
monUnlock(void)
{
	if (_monunlock) {
		switch(ignorelock) {
			case 1:
				break;
			case 2:
				ignorelock--;
			default:
				_monunlock();
				break;
		}
	}
}

int
mon_com(int cmd, void *arg1, void *arg2, void *arg3)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _moncom(cmd,arg1,arg2,arg3);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_putchar(char c)
{
	int	ret;

	CONSOLE_MONLOCK();
	ret = _rputchar(c);
	CONSOLE_MONUNLOCK();
	return(ret);
}

int
mon_getchar(void)
{
	int	ret;

	BLOCKING_MONLOCK();
	ret = _getchar();
	BLOCKING_MONUNLOCK();
	return(ret);
}

int
mon_gotachar(void)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _gotachar();
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_getbytes(char *buf,int cnt,int block)
{
	int	ret;

	BLOCKING_MONLOCK();
	ret = _getbytes(buf,cnt,block);
	BLOCKING_MONUNLOCK();
	return(ret);
}

int
mon_printf(
  char *fmt,
  int   a1, int a2, int a3, int a4,  int a5,  int a6,
  int   a7, int a8, int a9, int a10, int a11, int a12
)
{
	int	ret;

	CONSOLE_MONLOCK();
	ret = _printf(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12);
	CONSOLE_MONUNLOCK();
	return(ret);
}

int
mon_cprintf(
  char *fmt,
  int   a1, int a2, int a3, int a4,  int a5,  int a6,
  int   a7, int a8, int a9, int a10, int a11, int a12
)
{
	int	ret;

	CONSOLE_MONLOCK();
	ret = _cprintf(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12);
	CONSOLE_MONUNLOCK();
	return(ret);
}

int
mon_sprintf(
  char *buf,
  char *fmt,
  int   a1, int a2, int a3, int a4,  int a5,  int a6,
  int   a7, int a8, int a9, int a10, int a11, int a12
)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _sprintf(buf,fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_restart(int val)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _monrestart(val);
	GENERIC_MONUNLOCK();
	return(ret);
}

char *
mon_getenvp(void)
{
	char *ret;

	ENV_MONLOCK();
	ret = _getenvp();
	ENV_MONUNLOCK();
	return(ret);
}

char *
mon_getenv(char *name)
{
	char *ret;

	ENV_MONLOCK();
	ret = _getenv(name);
	ENV_MONUNLOCK();
	return(ret);
}

int
mon_setenv(char *name,char *val)
{
	int ret;

	ENV_MONLOCK();
	ret = _setenv(name,val);
	ENV_MONUNLOCK();
	return(ret);
}

char *
mon_getsym(char *name,char *buf, int bufsize)
{
	char *ret;

	ENV_MONLOCK();
	ret = _getsym(name,buf,bufsize);
	ENV_MONUNLOCK();
	return(ret);
}

int
mon_tfsinit(void)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsinit();
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsadd(char *name, char *info, char *flags, unsigned char *src, int size)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsadd(name,info,flags,src,size);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfslink(char *src, char *target)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfslink(src,target);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsunlink(char *name)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsunlink(name);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsrun(char **name,int verbose)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsrun(name,verbose);
	TFS_MONUNLOCK();
	return(ret);
}

struct tfshdr *
mon_tfsnext(struct tfshdr *fp)
{
	struct tfshdr *ret;

	TFS_MONLOCK();
	ret = _tfsnext(fp);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfstruncate(int tfd, long len)
{
	int ret;

	TFS_MONLOCK();
	ret = _tfstruncate(tfd,len);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfseof(int tfd)
{
	int ret;

	TFS_MONLOCK();
	ret = _tfseof(tfd);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsfstat(char *name, struct tfshdr *fp)
{
	int ret;

	TFS_MONLOCK();
	ret = _tfsfstat(name,fp);
	TFS_MONUNLOCK();
	return(ret);
}

struct tfshdr *
mon_tfsstat(char *name)
{
	struct tfshdr *ret;

	TFS_MONLOCK();
	ret = _tfsstat(name);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsread(int fd, char *buf, int cnt)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsread(fd,buf,cnt);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfswrite(int fd, char *buf, int cnt)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfswrite(fd,buf,cnt);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsopen(char *file,long flagmode,char *buf)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsopen(file,flagmode,buf);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsclose(int fd,char *info)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsclose(fd,info);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsseek(int fd, int offset, int whence)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsseek(fd,offset,whence);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsgetline(int fd,char *bp,int max)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsgetline(fd,bp,max);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_tfsipmod(char *name,char *buf,int offset,int size)
{
	int	ret;

	TFS_MONLOCK();
	ret = _tfsipmod(name,buf,offset,size);
	TFS_MONUNLOCK();
	return(ret);
}

long
mon_tfsctrl(int rqst,long arg1,long arg2)
{
	long	ret;

	TFS_MONLOCK();
	ret = _tfsctrl(rqst,arg1,arg2);
	TFS_MONUNLOCK();
	return(ret);
}

long
mon_tfstell(int fd)
{
	long	ret;

	TFS_MONLOCK();
	ret = _tfstell(fd);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_addcommand(struct monCommand *cmdlist, char *cmdlvl)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _addcommand(cmdlist,cmdlvl);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_docommand(char *cmdline,int verbose)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _docommand(cmdline,verbose);
	GENERIC_MONUNLOCK();
	return(ret);
}

void
mon_getargv(int *argc,char ***argv)
{
	GENERIC_MONLOCK();
	_getargv(argc,argv);
	GENERIC_MONUNLOCK();
}

unsigned short
mon_xcrc16(char *buf,long nbytes)
{
	unsigned short ret;

	GENERIC_MONLOCK();
	ret = _xcrc16((unsigned char *)buf,nbytes);
	GENERIC_MONUNLOCK();
	return(ret);
}

unsigned long
mon_intsoff(void)
{
	unsigned long ret;

	GENERIC_MONLOCK();
	ret = _intsoff();
	GENERIC_MONUNLOCK();
	return(ret);
}

void
mon_intsrestore(unsigned long msr)
{
	GENERIC_MONLOCK();
	_intsrestore(msr);
	GENERIC_MONUNLOCK();
}

void
mon_appexit(int val)
{
	GENERIC_MONLOCK();
	_appexit(val);
	GENERIC_MONUNLOCK();
}

#ifdef MALLOC_DEBUG
char *
mon_malloc(int size,char *fname,int fline)
{
	char *ret;

	HEAP_MONLOCK();
	ret = _malloc(size,fname,fline);
	HEAP_MONUNLOCK();
	return(ret);
}

char *
mon_realloc(char *buf, int size, char *fname, int fline)
{
	char *ret;

	HEAP_MONLOCK();
	ret = _realloc(buf,size, fname, fline);
	HEAP_MONUNLOCK();
	return(ret);
}
#else
char *
mon_malloc(int size)
{
	char *ret;

	HEAP_MONLOCK();
	ret = _malloc(size);
	HEAP_MONUNLOCK();
	return(ret);
}

char *
mon_realloc(char *buf, int size)
{
	char *ret;

	HEAP_MONLOCK();
	ret = _realloc(buf,size);
	HEAP_MONUNLOCK();
	return(ret);
}
#endif

void
mon_free(char *cp)
{
	HEAP_MONLOCK();
	_free(cp);
	HEAP_MONUNLOCK();
}

int
mon_getline(char *buf,int max,int ledit)
{
	int	ret;

	BLOCKING_MONLOCK();
	ret = _getline(buf,max,ledit);
	BLOCKING_MONUNLOCK();
	return(ret);
}

int
mon_decompress(char *src,int srcsize,char *dest)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _decompress(src,srcsize,dest);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_heapextend(char *base,int size)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _heapextend(base,size);
	GENERIC_MONUNLOCK();
	return(ret);
}

void
mon_bbc(char *filename, int lineno)
{
	_bbc(filename, lineno);
}

void
mon_profiler(void *pdata)
{
	_profiler(pdata);
}

void
mon_memtrace(
  char *fmt,
  int   a1, int a2, int a3, int a4,  int a5,  int a6,
  int   a7, int a8, int a9, int a10, int a11, int a12
)
{
	_memtrace(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12);
}

char *
mon_version(void)
{
	char *ret;

	GENERIC_MONLOCK();
	ret = _version();
	GENERIC_MONUNLOCK();
	return(ret);
}

void
mon_warmstart(unsigned long mask)
{
	GENERIC_MONLOCK();
	_appwarmstart(mask);
	GENERIC_MONUNLOCK();
}

int
mon_pcicfgwrite(int interface,int bus,int dev,int func,int reg,
	unsigned long val)
{
	int	retval;

	GENERIC_MONLOCK();
	retval = _pcicfgwrite(interface,bus,dev,func,reg,val);
	GENERIC_MONUNLOCK();
	return(retval);
}

unsigned long
mon_pcicfgread(int interface,int bus,int dev, int func,int reg)
{
	unsigned long retval;

	GENERIC_MONLOCK();
	retval = _pcicfgread(interface,bus,dev,func,reg);
	GENERIC_MONUNLOCK();
	return(retval);
}

unsigned long
mon_pcictrl(int interface, int cmd, unsigned long arg1, unsigned long arg2)
{
	unsigned long val;

	GENERIC_MONLOCK();
	val = _pcictrl(interface,cmd,arg1,arg2);
	GENERIC_MONUNLOCK();
	return(val);
}

unsigned long
mon_i2cctrl(int interface, int cmd, unsigned long arg1, unsigned long arg2)
{
	unsigned long val;

	GENERIC_MONLOCK();
	val = _i2cctrl(interface,cmd,arg1,arg2);
	GENERIC_MONUNLOCK();
	return(val);
}

int
mon_i2cwrite(int interface, int bigaddr, unsigned char *data, int len)
{
	int	val;

	GENERIC_MONLOCK();
	val = _i2cwrite(interface,bigaddr,data,len);
	GENERIC_MONUNLOCK();
	return(val);
}

int
mon_i2cread(int interface, int bigaddr, unsigned char *data, int len)
{
	int	val;

	GENERIC_MONLOCK();
	val = _i2cread(interface,bigaddr,data,len);
	GENERIC_MONUNLOCK();
	return(val);
}

void
mon_delay(long msec)
{
	GENERIC_MONLOCK();
	_mondelay(msec);
	GENERIC_MONUNLOCK();
}

int
mon_timer(int cmd, void *arg)
{
	int ret;

	GENERIC_MONLOCK();
	ret = _montimer(cmd, arg);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_sendenetpkt(char *pkt,int size)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _sendenet(pkt,size);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_recvenetpkt(char *pkt,int size)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _recvenet(pkt,size);
	GENERIC_MONUNLOCK();
	return(ret);
}

void
mon_printpkt(char *buf,int size, int incoming)
{
	GENERIC_MONLOCK();
	_printpkt(buf,size,incoming);
	GENERIC_MONUNLOCK();
}

int
mon_flashoverride(void *flashinfo,int get,int bank)
{
	int	ret;

	TFS_MONLOCK();
	ret = _flashoverride(flashinfo,get,bank);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_flashwrite(char *dest,char *src,int bytecnt)
{
	int	ret;

	TFS_MONLOCK();
	ret = _flashwrite(dest,src,bytecnt);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_flasherase(int snum)
{
	int	ret;

	TFS_MONLOCK();
	ret = _flasherase(snum);
	TFS_MONUNLOCK();
	return(ret);
}

int
mon_flashinfo(int snum, int *size, char **base)
{
	int	ret;

	TFS_MONLOCK();
	ret = _flashinfo(snum,size,base);
	TFS_MONUNLOCK();
	return(ret);
}

unsigned long
mon_assignhandler(long hnum, unsigned long arg1, unsigned long arg2)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _assign_handler(hnum,arg1,arg2);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_watchdog(void)
{
	int	ret;

	GENERIC_MONLOCK();
	ret = _watchdog();
	GENERIC_MONUNLOCK();
	return(ret);
}

void
mon_printmem(char *mem, int size, int ascii)
{
	GENERIC_MONLOCK();
	_printmem(mem,size,ascii);
	GENERIC_MONUNLOCK();
}

long
mon_portcmd(int cmd, void *arg)
{
	long ret;

	GENERIC_MONLOCK();
	ret = _portcmd(cmd,arg);
	GENERIC_MONUNLOCK();
	return(ret);
}

int
mon_timeofday(int cmd, void *arg)
{
	int ret;

	GENERIC_MONLOCK();
	ret = _timeofday(cmd,arg);
	GENERIC_MONUNLOCK();
	return(ret);
}
