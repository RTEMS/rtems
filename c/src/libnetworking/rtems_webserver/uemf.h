/*
 * uemf.h -- Go Ahead Micro Embedded Management Framework Header
 *
 * Copyright (c) Go Ahead Software, Inc., 1995-1999
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

#ifndef _h_UEMF
#define _h_UEMF 1

/******************************** Description *********************************/

/* 
 *	Go Ahead Web Server header. This defines the Web public APIs
 */

/******************************* Per O/S Includes *****************************/

#if WIN
	#include	<direct.h>
	#include	<io.h>
	#include	<sys/stat.h>
	#include	<limits.h>
	#include	<tchar.h>
	#include	<windows.h>
	#include	<winnls.h>
	#include	<time.h>
	#include	<sys/types.h>
	#include	<stdio.h>
	#include	<stdlib.h>
#endif

#if CE
	#include	<limits.h>
	#include	<tchar.h>
	#include	<windows.h>
	#include	<winnls.h>
	#include	"CE/wincompat.h"
#endif

#if NW
	#include	<stdio.h>
#endif

#if UNIX
	#include	<stdio.h>
#endif

#if LINUX || __rtems__
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<sys/param.h>
	#include	<limits.h>
	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<unistd.h>
	#include	<sys/socket.h>
	#include	<sys/select.h>
	#include	<netinet/in.h>
	#include 	<arpa/inet.h>
	#include 	<netdb.h>
	#include	<time.h>
#endif

#if LYNX
	#include	<limits.h>
	#include	<stdarg.h>
	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<unistd.h>
	#include	<socket.h>
	#include	<netinet/in.h>
	#include 	<arpa/inet.h>
	#include 	<netdb.h>
	#include	<time.h>
#endif

#if UW
	#include	<stdio.h>
#endif

#if VXW486
	#include	<vxWorks.h>
	#include	<sockLib.h>
	#include	<selectLib.h>
	#include	<inetLib.h>
	#include	<ioLib.h>
	#include	<stdio.h>
	#include	<stat.h>
	#include	<time.h>
	#include	<usrLib.h>
#endif

#if QNX4
	#include	<sys/types.h>
	#include	<stdio.h>
	#include	<sys/socket.h>
	#include	<sys/select.h>
	#include	<netinet/in.h>
	#include 	<arpa/inet.h>
	#include 	<netdb.h>
#endif

/********************************** Includes **********************************/

#include	<ctype.h>
#include	<stdarg.h>
#include	<string.h>

/********************************** Unicode ***********************************/
/* 
 *	Constants and limits. Also FNAMESIZE and PATHSIZE are currently defined 
 *	in param.h to be 128 and 512
 */
#define TRACE_MAX			(4096 - 48)
#define VALUE_MAX_STRING	(4096 - 48)
#define SYM_MAX				(512)
#define XML_MAX				4096			/* Maximum size for tags/tokens */
#define BUF_MAX				4096			/* General sanity check for bufs */

/*
 *	Type for unicode systems
 */
#if UNICODE

/*
 *	To convert strings to UNICODE. We have a level of indirection so things
 *	like T(__FILE__) will expand properly.
 */
#define T(x)		__TXT(x)
#define __TXT(s)	L ## s
typedef unsigned short char_t;
typedef unsigned short uchar_t;

/*
 *	Text size of buffer macro. A buffer bytes will hold (size / char size) 
 *	characters. 
 */
#define	TSZ(x)		(sizeof(x) / sizeof(char_t))

/*
 *	How many ASCII bytes are required to represent this UNICODE string?
 */
#define	TASTRL(x)		((wcslen(x) + 1) * sizeof(char_t))


#define gmain		wmain

#define gsprintf	swprintf
#define gprintf		wprintf
#define gfprintf	fwprintf
#define gsscanf		swscanf
#define gvsprintf	vswprintf

#define gstrcpy		wcscpy
#define gstrncpy	wcsncpy
#define gstrncat	wcsncat
#define gstrlen		wcslen
#define gstrcat		wcscat
#define gstrcmp		wcscmp
#define gstrncmp	wcsncmp
#define gstricmp	wcsicmp
#define gstrchr		wcschr
#define gstrrchr	wcsrchr
#define gstrtok		wcstok
#define gstrnset	wcsnset
#define gstrstr		wcsstr

#define gfopen		_wfopen
#define gopen		_wopen
#define gcreat		_wcreat
#define gfgets		fgetws
#define gfputs		fputws
#define gunlink		_wunlink
#define grename		_wrename
#define gtmpnam		_wtmpnam
#define gtempnam	_wtempnam
#define gfindfirst	_wfindfirst
#define gfinddata_t	_wfinddata_t
#define gfindnext	_wfindnext
#define gfindclose	_findclose
#define gstat		_wstat
#define gaccess		_waccess

typedef struct _stat gstat_t;

#define gmkdir		_wmkdir
#define gchdir		_wchdir
#define grmdir		_wrmdir
#define gremove		_wremove
#define ggetcwd		_wgetcwd

#define gtolower	towlower
#define gtoupper	towupper
#define gisspace	iswspace
#define gisdigit	iswdigit
#define gisxdigit	iswxdigit
#define gisalnum	iswalnum
#define gisalpha	iswalpha
#define gisupper	iswupper
#define gislower	iswlower
#define gatoi(s)	wcstol(s, NULL, 10)

#define gctime		_wctime
#define ggetenv		_wgetenv
#define gexecvp		_wexecvp

#else /* ! UNICODE */

#define T(s) 		s
#define	TSZ(x)		(sizeof(x))
#define	TASTRL(x)	(strlen(x) + 1)
typedef char char_t;
#if WIN
typedef unsigned char uchar_t;
#endif

#define gsprintf	sprintf
#define gprintf		printf
#define gfprintf	fprintf
#define gsscanf		sscanf
#define gvsprintf	vsprintf

#define gstrcpy		strcpy
#define gstrncpy	strncpy
#define gstrncat	strncat
#define gstrlen		strlen
#define gstrcat		strcat
#define gstrcmp		strcmp
#define gstrncmp	strncmp
#define gstricmp	stricmp
#define gstrchr		strchr
#define gstrrchr	strrchr
#define gstrtok		strtok
#define gstrnset	strnset
#define gstrstr		strstr

#define gfopen		fopen
#define gopen		open
#define gcreat		creat
#define gfgets		fgets
#define gfputs		fputs
#define gunlink		unlink
#define grename		rename
#define gtmpnam		tmpnam
#define gtempnam	tempnam
#define gfindfirst	_findfirst
#define gfinddata_t	_finddata_t
#define gfindnext	_findnext
#define gfindclose	_findclose
#define gstat		stat
#define gaccess		access

typedef struct stat gstat_t;

#define gmkdir		mkdir
#define gchdir		chdir
#define grmdir		rmdir
#define gremove		remove
#define ggetcwd		getcwd

#define gtolower	tolower
#define gtoupper	toupper
#define gisspace	isspace
#define gisdigit	isdigit
#define gisxdigit	isxdigit
#define gisalnum	isalnum
#define gisalpha	isalpha
#define gisupper	isupper
#define gislower	islower
#define gatoi		atoi

#define gctime		ctime
#define ggetenv		getenv
#define gexecvp		execvp
#ifndef VXW486
#define gmain		main
#endif
#endif

/********************************** Defines ***********************************/

#define FNAMESIZE			256			/* Max length of file names */

#define E_MAX_ERROR			4096
/*
 * Error types
 */
#define	E_ASSERT			0x1			/* Assertion error */
#define	E_LOG				0x2			/* Log error to log file */
#define	E_USER				0x3			/* Error that must be displayed */

#define E_L				T(__FILE__), __LINE__
#define E_ARGS_DEC		char_t *file, int line
#define E_ARGS			file, line

#if ASSERT
	#define a_assert(C)		if (C) ; else error(E_L, E_ASSERT, T("%s"), #C)
#else
	#define a_assert(C)		if (1) ; else
#endif

#define VALUE_VALID			{ {0}, integer, 1 }
#define VALUE_INVALID		{ {0}, undefined, 0 }

/*
 *	Allocation flags 
 */
#define VALUE_ALLOCATE		0x1

#define value_numeric(t)	(t == integer)
#define value_str(t) 		(t >= string || t <= bytes)
#define value_ok(t) 		(t > undefined && t <= symbol)

/*
 *	These values are not prefixed so as to aid code readability
 */
#ifndef UW
#pragma pack(2)
#endif

typedef enum {
	undefined	= 0,
	integer		= 1,
	string 		= 2,
	bytes 		= 3,
	errmsg 		= 4
} value_type_t;

/*
 *	In UW, bit fields default to unsigned unless explicitly defined as signed.
 *	Unfortunately, enum become ints, but not explicitly signed. Thus, if using
 *	an enum type in a bit field, it becomes unsigned, but we need signed. So
 *	for UW we declare value_type_t to be a signed int to make this all work.
 */
typedef struct {

	union {
		long	integer;
		char_t	*string;
		char_t	*bytes;
		char_t	*errmsg;
		void	*symbol;
	} value;

#if UW
	signed int		type		: 8;
#else
	value_type_t	type		: 8;
#endif

	unsigned int	valid 		: 1;
	unsigned int	user_def_1 	: 1;
	unsigned int	allocated 	: 1;		/* String was balloced */
} value_t;

/*
 *	Extract a string from the value depending whether inline or via pointer
 */
#define value_strget(v) \
	(((v)->type == bytes) ? (v)->value.bytes : (v)->value.string)

#ifndef UW 
#pragma pack()
#endif

/******************************************************************************/
/*
 *	A ring queue allows maximum utilization of memory for data storage and is
 *	ideal for input/output buffering. This module provides a highly effecient
 *	implementation and a vehicle for dynamic strings.
 *
 *	WARNING:  This is a public implementation and callers have full access to
 *	the queue structure and pointers.  Change this module very carefully.
 *
 *	This module follows the open/close model.
 *
 *	Operation of a ringq where rq is a pointer to a ringq :
 *
 *		rq->buflen contains the size of the buffer.
 *		rq->buf will point to the start of the buffer.
 *		rq->servp will point to the first (un-consumed) data byte.
 *		rq->endp will point to the next free location to which new data is added
 *		rq->endbuf will point to one past the end of the buffer.
 *
 *	Eg. If the ringq contains the data "abcdef", it might look like :
 *
 *	+-------------------------------------------------------------------+
 *  |   |   |   |   |   |   |   | a | b | c | d | e | f |   |   |   |   |
 *	+-------------------------------------------------------------------+
 *    ^                           ^                       ^               ^
 *    |                           |                       |               |
 *  rq->buf                    rq->servp               rq->endp      rq->enduf
 *     
 *	The queue is empty when servp == endp.  This means that the queue will hold
 *	at most rq->buflen -1 bytes.  It is the fillers responsibility to ensure
 *	the ringq is never filled such that servp == endp.
 *
 *	It is the fillers responsibility to "wrap" the endp back to point to
 *	rq->buf when the pointer steps past the end. Correspondingly it is the
 *	consumers responsibility to "wrap" the servp when it steps to rq->endbuf.
 *	The ringqPutc and ringqGetc routines will do this automatically.
 */

/*
 *	Ring queue buffer structure
 */
typedef struct {
	unsigned char	*buf;				/* Holding buffer for data */
	unsigned char	*servp;				/* Pointer to start of data */
	unsigned char	*endp;				/* Pointer to end of data */
	unsigned char	*endbuf;			/* Pointer to end of buffer */
	int				buflen;				/* Length of ring queue */
	int				maxsize;			/* Maximum size */
	int				increment;			/* Growth increment */
} ringq_t;

/*
 *	Block allocation definitions
 */
#define B_L				__FILE__, __LINE__
#define B_ARGS_DEC		char *file, int line
#define B_ARGS			file, line

/*
 *	Block classes are: 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 
 *					   16384, 32768, 65536 
 */
#define B_SHIFT			4					/* Convert size to class */
#define B_ROUND			((1 << (B_SHIFT)) - 1)
#define B_MAX_CLASS		13					/* Maximum class number + 1 */
#define B_MALLOCED		0x80000000			/* Block was malloced */
#define B_DEFAULT_MEM	(64 * 1024)			/* Default memory allocation */
#define B_MAX_FILES		(512)				/* Maximum number of files */
#define B_FILL_CHAR		(0x77)				/* Fill byte for buffers */
#define B_FILL_WORD		(0x77777777)		/* Fill word for buffers */
#define B_MAX_BLOCKS	(64 * 1024)			/* Maximum allocated blocks */

/*
 *	Flags. The integer value is used as an arbitrary value to fill the flags.
 */

#define B_USE_MALLOC		0x1				/* Okay to use malloc if required */
#define B_USER_BUF			0x2				/* User supplied buffer for mem */
#define B_INTEGRITY			0x8124000		/* Integrity value */
#define B_INTEGRITY_MASK	0xFFFF000		/* Integrity mask */

/*
 *	The symbol table record for each symbol entry
 */

typedef struct sym_t {
	struct sym_t	*forw;					/* Pointer to next hash list */
	value_t			name;					/* Name of symbol */
	value_t			content;				/* Value of symbol */
	int				arg;					/* Parameter value */
} sym_t;

typedef int sym_fd_t;						/* Returned by symOpen */

/*
 *	Socket flags 
 */
#define SOCKET_EOF				0x1			/* Seen end of file */
#define SOCKET_CONNECTING		0x2			/* Connect in progress */
#define SOCKET_BROADCAST		0x4			/* Broadcast mode */
#define SOCKET_PENDING			0x8			/* Message pending on this socket */
#define SOCKET_FLUSHING			0x10		/* Background flushing */
#define SOCKET_LISTENING		0x20		/* Server socket listening */

/*
 *	Socket error values
 */
#define SOCKET_WOULDBLOCK		1			/* Socket would block on I/O */
#define SOCKET_RESET			2			/* Socket has been reset */
#define SOCKET_NETDOWN			3			/* Network is down */
#define SOCKET_AGAIN			4			/* Issue the request again */
#define SOCKET_INTR				5			/* Call was interrupted */
#define SOCKET_INVAL			6			/* Invalid */

/*
 *	Handler event masks
 */
#define SOCKET_READABLE			0x2			/* Make socket readable */ 
#define SOCKET_WRITABLE			0x4			/* Make socket writable */
#define SOCKET_EXCEPTION		0x8			/* Interested in exceptions */

#define SOCKET_BUFSIZ			512			/* Underlying buffer size */

typedef void 	(*socketHandler_t)(int sid, int mask, int data);
typedef int		(*socketAccept_t)(int sid, char *ipaddr, int port);

/*
 *	Script engines
 */
#define EMF_SCRIPT_JSCRIPT			0		/* javascript */
#define EMF_SCRIPT_TCL	 			1		/* tcl */
#define EMF_SCRIPT_EJSCRIPT 		2		/* Ejscript */
#define EMF_SCRIPT_MAX	 			3

#define	MAXINT		INT_MAX
#define BITSPERBYTE 8
#define BITS(type)	(BITSPERBYTE * (int) sizeof(type))

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

/******************************* Per O/S Defines ******************************/

#if VXW486 || LINUX || __rtems__ || LYNX
#ifndef O_BINARY
#define O_BINARY 		0
#endif
#define	SOCKET_ERROR	-1
#endif

#if WIN
#undef R_OK
#define R_OK	4
#undef W_OK
#define W_OK	2
#undef X_OK
#define X_OK	1
#undef F_OK
#define F_OK	0
#endif

/********************************* Prototypes *********************************/

extern void 	 bclose();
extern int 		 bopen(void *buf, int bufsize, int flags);
#if NO_BALLOC
#undef B_L
#define B_L x
#define balloc(x, num) malloc(num)
#define bfree(x, p) free(p)
#define bfreeSafe(x, p) \
	if (p) { bfree(x, p); } else
#define brealloc(x, p, num) realloc(p, num)
extern char *bstrdupANoBalloc(char *s);
#define bstrdupA(x, s) bstrdupANoBalloc(s)
#if UNICODE
extern char_t *bstrdupNoBalloc(char_t *s);
#define bstrdup(x, s) bstrdupNoBalloc(s)
#define gstrdup(x, s) bstrdupNoBalloc(s)
#else /* Not UNICODE */
#define bstrdup(x, s) bstrdupANoBalloc(s)
#define gstrdup(x, s) bstrdupANoBalloc(s)
#endif /* UNICODE */

#else /* BALLOC */
extern void		*balloc(B_ARGS_DEC, int size);
extern void 	 bfree(B_ARGS_DEC, void *mp);
extern void 	*brealloc(B_ARGS_DEC, void *buf, int newsize);
extern void 	 bstats(int handle, void (*writefn)(int fd, char_t *fmt, ...));
extern char_t 	*bstrdup(B_ARGS_DEC, char_t *s);
extern void		 bfreeSafe(B_ARGS_DEC, void* mp);
#define gstrdup(B_ARGS, s)	bstrdup(B_ARGS, s)
#if UNICODE
extern char *bstrdupA(B_ARGS_DEC, char *s);
#else
#define bstrdupA bstrdup
#endif /* UNICODE */
#endif /* BALLOC */

#if !LINUX && !__rtems__
extern char_t*	basename(char_t* name);
#endif

extern void		*emfCreateTimer(int delay, void (*routine)(long arg), long arg);
extern void 	 emfDeleteTimer(void *id);
extern int		 emfInstGet();
extern void		 emfInstSet(int inst);
extern void		 error(E_ARGS_DEC, int flags, char_t *fmt, ...);

extern int		 hAlloc(void*** map);
extern int		 hFree(void*** map, int handle);
extern int		 hAllocEntry(void ***list, int *max, int size);

extern int	 	 ringqOpen(ringq_t *rq, int increment, int maxsize);
extern void 	 ringqClose(ringq_t *rq);
extern int 		 ringqLen(ringq_t *rq);

extern int 		 ringqPutc(ringq_t *rq, char_t c);
extern int	 	 ringqInsertc(ringq_t *rq, char_t c);
extern int	 	 ringqPutstr(ringq_t *rq, char_t *str);
extern int 		 ringqGetc(ringq_t *rq);

extern int		gvsnprintf(char_t **s, int n, char_t *fmt, va_list arg);
extern int		gsnprintf(char_t **s, int n, char_t *fmt, ...);

#if UNICODE
extern int 		ringqPutcA(ringq_t* rq, char c);
extern int	 	ringqInsertcA(ringq_t* rq, char c);
extern int	 	ringqPutstrA(ringq_t* rq, char* str);
extern int 		ringqGetcA(ringq_t* rq);
#else
#define ringqPutcA ringqPutc
#define ringqInsertcA ringqInsertc
#define ringqPutstrA ringqPutstr
#define ringqGetcA ringqGetc
#endif

extern int 		 ringqPutBlk(ringq_t *rq, unsigned char *buf, int len);
extern int 		 ringqPutBlkMax(ringq_t *rq);
extern void 	 ringqPutBlkAdj(ringq_t *rq, int size);
extern int 		 ringqGetBlk(ringq_t *rq, unsigned char *buf, int len);
extern int 		 ringqGetBlkMax(ringq_t *rq);
extern void 	 ringqGetBlkAdj(ringq_t *rq, int size);
extern void 	 ringqFlush(ringq_t *rq);

extern int		 scriptSetVar(int engine, char_t *var, char_t *value);
extern int		 scriptEval(int engine, char_t *cmd, char_t **rslt, int chan);

extern void		 socketClose();
extern void		 socketCloseConnection(int sid);
extern void		 socketCreateHandler(int sid, int mask, socketHandler_t 
					handler, int arg);
extern void		 socketDeleteHandler(int sid);
extern int		 socketEof(int sid);
extern int		 socketFlush(int sid, int block);
extern int		 socketGets(int sid, char_t** buf);
extern int		 socketInputBuffered(int sid);
extern int		 socketOpen();
extern int 		 socketOpenConnection(char *host, int port, 
					socketAccept_t accept, int flags);
extern void 	 socketProcess();
extern int		 socketRead(int sid, char *buf, int len);
extern int 		 socketReady();
extern int		 socketWrite(int sid, char *buf, int len);
extern int		 socketWriteString(int sid, char_t *buf);
extern int 		 socketSelect();

extern char_t	*strlower(char_t *string);
extern char_t	*strupper(char_t *string);

extern char_t	*stritoa(int n, char_t *string, int width);

extern sym_fd_t	 symOpen(int hash_size);
extern void		 symClose(sym_fd_t sd, void (*cleanup)(sym_t *sp));
extern sym_t	*symLookup(sym_fd_t sd, char_t *name);
extern sym_t	*symEnter(sym_fd_t sd, char_t *name, value_t v, int arg);
extern int		 symDelete(sym_fd_t sd, char_t *name);
extern void 	 symWalk(sym_fd_t sd, void (*fn)(sym_t *symp));

extern void		 trace(int lev, char_t *fmt, ...);
 
extern value_t 	 valueInteger(long value);
extern value_t	 valueString(char_t *value, int flags);
extern value_t	 valueErrmsg(char_t *value);
extern void 	 valueFree(value_t *v);

extern char_t 	*ascToUni(char_t *ubuf, char *str, int nBytes);
extern char 	*uniToAsc(char *buf, char_t *ustr, int nBytes);
extern char_t	*ballocAscToUni(char * cp);
extern char		*ballocUniToAsc(char_t * unip, int ulen);

#endif /* _h_UEMF */

/******************************************************************************/
