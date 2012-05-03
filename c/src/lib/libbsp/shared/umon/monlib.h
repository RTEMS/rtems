/*
 *  monlib.h - Used by both the monitor and the application that
 *             may reside on top of the monitor
 *
 *  Based upon code from MicroMonitor 1.17 from http://www.umonfw.com/
 *  which includes this notice:
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

#ifndef _MONLIB_H_
#define _MONLIB_H_

#include <umon/tfs.h>
#include <umon/cli.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int monConnect(int (*monptr)(int,void *,void *,void *),
		void (*lock)(void),void (*unlock)(void));
extern void mon_getargv(int *argc,char ***argv);
extern void mon_intsrestore(unsigned long oldval);
extern void mon_appexit(int exit_value);
extern void mon_free(char *buffer);
extern void mon_profiler(void *pdata);
extern void mon_bbc(char *filename, int linenum);
extern void mon_warmstart(unsigned long mask);
extern void mon_delay(long msec);
extern void mon_printpkt(char *buf, int size, int incoming);
extern void mon_printmem(char *mem, int size, int ascii);


extern int mon_com(int cmd,void *arg1,void *arg2,void *arg3);
extern int mon_timer(int cmd, void * arg);
extern int mon_setenv(char *varname,char *value);
extern int mon_putchar(char c);
extern int mon_getchar(void);
extern int mon_gotachar(void);
extern int mon_getbytes(char *buf,int count,int block);
extern int mon_restart(int restart_value);
extern int mon_tfsinit(void);
extern int mon_tfsunlink(char *filename);
extern int mon_tfslink(char *source, char *target);
extern int mon_tfsrun(char **arglist,int verbosity);
extern int mon_tfsfstat(char *filename,struct tfshdr *tfp);
extern int mon_tfseof(int file_descriptor);
extern int mon_tfstruncate(int file_descriptor,long length);
extern int mon_tfsread(int file_descriptor,char *buffer,int size);
extern int mon_tfswrite(int file_descriptor,char *buffer,int size);
extern int mon_tfsopen(char *filename,long mode,char *buffer);
extern int mon_tfsclose(int file_descriptor,char *info);
extern int mon_tfsseek(int file_descriptor,int offset,int whence);
extern int mon_tfsgetline(int file_descriptor,char *buffer,int bufsize);
extern int mon_tfsipmod(char *name,char *buffer,int offset,int size);
extern int mon_addcommand(struct monCommand *command_list,char *);
extern int mon_docommand(char *cmdline,int verbosity);
extern int mon_getline(char *buffer,int max,int ledit);
extern int mon_decompress(char *src,int srcsize,char *dest);
extern int mon_heapextend(char *base,int size);
extern int mon_pcicfgwrite(int interface,int bus,int dev,int func,int reg,
		unsigned long val);
extern int mon_tfsadd(char *filename, char *info, char *flags,
		unsigned char *src, int size);
extern int mon_i2cwrite(int interface, int bigaddr, unsigned char *data,
		int len);
extern int mon_i2cread(int interface, int bigaddr, unsigned char *data,
		int len);
extern int mon_sendenetpkt(char *pkt, int len);
extern int mon_recvenetpkt(char *pkt, int len);
extern int mon_flashoverride(void *flashinfo, int get, int bank);
extern int mon_flasherase(int snum);
extern int mon_flashwrite(char *dest,char *src, int bytecnt);
extern int mon_flashinfo(int snum,int *size, char **base);
extern int mon_watchdog(void);
extern int mon_timeofday(int cmd, void *arg);

extern char *mon_getsym(char *symname, char *buf, int bufsize);
extern char *mon_getenv(char *varname);
extern char *mon_getenvp(void);
extern char *mon_version(void);
#ifdef MALLOC_DEBUG
extern char *mon_malloc(int size,char *file, int line);
extern char *mon_realloc(char *buf,int size,char *file, int line);
#else
extern char *mon_malloc(int size);
extern char *mon_realloc(char *buf,int size);
#endif

extern long mon_tfsctrl(int command,long arg1,long arg2);
extern long mon_tfstell(int file_descriptor);
extern long mon_portcmd(int cmd, void *arg);

extern unsigned short mon_xcrc16(char *buffer,long length);

extern unsigned long mon_intsoff(void);

extern unsigned long mon_pcicfgread(int interface,int bus,int dev,
		int func,int reg);

extern unsigned long mon_pcictrl(int interface, int cmd,
		unsigned long arg1, unsigned long arg2);

extern unsigned long mon_i2cctrl(int interface, int cmd,
		unsigned long arg1, unsigned long arg2);

extern unsigned long mon_assignhandler(long hnum,
		unsigned long arg1,unsigned long arg2);

extern struct tfshdr *mon_tfsnext(struct tfshdr *tfp);
extern struct tfshdr *mon_tfsstat(char *filename);

#if SHOWVARARGS
extern void mon_memtrace(char *fmt, ...);
extern int mon_printf(char *fmt, ...);
extern int mon_cprintf(char *fmt, ...);
extern int mon_sprintf(char *,char *fmt, ...);
#else
extern void mon_memtrace();
extern int mon_printf();
extern int mon_cprintf();
extern int mon_sprintf();
#endif

#ifdef __cplusplus
}
#endif


/* defines used by monConnect():
 */
#define GETMONFUNC_PUTCHAR				1
#define GETMONFUNC_GETCHAR				2
#define GETMONFUNC_GOTACHAR				3
#define GETMONFUNC_GETBYTES				4
#define GETMONFUNC_PRINTF				5
#define GETMONFUNC_CPRINTF				6
#define GETMONFUNC_SPRINTF				7
#define GETMONFUNC_RESTART				8
#define GETMONFUNC_GETENV				9
#define GETMONFUNC_SETENV				10
#define GETMONFUNC_TFSINIT				11
#define GETMONFUNC_TFSADD				12
#define GETMONFUNC_TFSUNLINK			13
#define GETMONFUNC_TFSRUN				14
#define GETMONFUNC_TFSNEXT				15
#define GETMONFUNC_TFSSTAT				16
#define GETMONFUNC_TFSREAD				17
#define GETMONFUNC_TFSWRITE				18
#define GETMONFUNC_TFSOPEN				19
#define GETMONFUNC_TFSCLOSE				20
#define GETMONFUNC_TFSSEEK				21
#define GETMONFUNC_TFSGETLINE			22
#define GETMONFUNC_TFSIPMOD				23
#define GETMONFUNC_TFSCTRL				24
#define GETMONFUNC_ADDCOMMAND			25
#define GETMONFUNC_DOCOMMAND			26
#define GETMONFUNC_GETARGV				27
#define GETMONFUNC_CRC16				28
#define GETMONFUNC_CRC32				29
#define GETMONFUNC_PIOGET				30	/* NA (removed as of 1.0) */
#define GETMONFUNC_PIOSET				31	/* NA (removed as of 1.0) */
#define GETMONFUNC_PIOCLR				32	/* NA (removed as of 1.0) */
#define GETMONFUNC_INTSOFF				33
#define GETMONFUNC_INTSRESTORE			34
#define GETMONFUNC_APPEXIT				35
#define GETMONFUNC_MALLOC				36
#define GETMONFUNC_FREE					37
#define GETMONFUNC_GETLINE				38
#define GETMONFUNC_TFSFSTAT				39
#define GETMONFUNC_TFSEOF				40
#define GETMONFUNC_DECOMPRESS			41
#define GETMONFUNC_TFSTRUNCATE			42
#define GETMONFUNC_HEAPXTEND			43
#define GETMONFUNC_PROFILER				44
#define GETMONFUNC_TFSLINK				45
#define GETMONFUNC_BBC					46
#define GETMONFUNC_MEMTRACE				47
#define GETMONFUNC_TFSTELL				48
#define GETMONFUNC_VERSION				49
#define GETMONFUNC_WARMSTART			50
#define GETMONFUNC_PCICFGREAD			51
#define GETMONFUNC_PCICFGWRITE			52
#define GETMONFUNC_PCICONTROL			53
#define GETMONFUNC_I2CREAD				54
#define GETMONFUNC_I2CWRITE				55
#define GETMONFUNC_I2CCONTROL			56
#define GETMONFUNC_MONDELAY				57
#define GETMONFUNC_GETENVP				58
#define GETMONFUNC_REALLOC				59
#define GETMONFUNC_SENDENETPKT			60
#define GETMONFUNC_RECVENETPKT			61
#define GETMONFUNC_GETSYM				62
#define GETMONFUNC_PRINTPKT				63
#define GETMONFUNC_FLASHWRITE			64
#define GETMONFUNC_FLASHERASE			65
#define GETMONFUNC_FLASHINFO			66
#define GETMONFUNC_ASSIGNHDLR			67
#define GETMONFUNC_WATCHDOG				68
#define GETMONFUNC_PRINTMEM				69
#define GETMONFUNC_PORTCMD				70
#define GETMONFUNC_TIMEOFDAY			71
#define GETMONFUNC_TIMER				72
#define GETMONFUNC_FLASHOVRRD			73

#define CACHEFTYPE_DFLUSH				200
#define CACHEFTYPE_IINVALIDATE			201

#define	CHARFUNC_PUTCHAR				300
#define	CHARFUNC_GETCHAR				301
#define	CHARFUNC_GOTACHAR				302
#define	CHARFUNC_RAWMODEON				303
#define	CHARFUNC_RAWMODEOFF				304

#define ASSIGNFUNC_GETUSERLEVEL			400


/* Defines used by mon_warmstart():
 */
#define WARMSTART_IOINIT			0x00000001
#define WARMSTART_BSSINIT			0x00000002
#define WARMSTART_RUNMONRC			0x00000004
#define WARMSTART_MONHEADER			0x00000008
#define WARMSTART_TFSAUTOBOOT		0x00000010
#define WARMSTART_BOARDINFO			0x00000020
#define WARMSTART_ALL				0xffffffff
#endif
