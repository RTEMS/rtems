/* tfs.h:
 *	Header file for TFS transactions, used by both application and monitor.
 * 
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#ifndef _tfs_h
#define _tfs_h

#define TFSINFOSIZE		23		/* Max size of info string (mod4-1). */

#ifndef TFSNAMESIZE				/* This specifies the maximum size of a file */
#define TFSNAMESIZE		23		/* name that can be used in TFS. */
#endif							/* This MUST be some value mod4 - 1. */

#ifndef TFS_CHANGELOG_FILE		/* Information used for change-log */
#define TFS_CHANGELOG_SIZE	0	/* facility within tfs. */
#define TFS_CHANGELOG_FILE	".tfschlog"
#endif

#ifndef SYMFILE					/* This specifies the default filename */
#define SYMFILE		"symtbl"	/* used by the monitor for the symbol */
#endif							/* table. */

#define MINUSRLEVEL		0		/* Minimum user level supported. */
#define MAXUSRLEVEL		3		/* Maximum user level supported. */

#ifndef TFS_RESERVED
#define TFS_RESERVED	4		/* Number of "reserved" entries (ulong) */
#endif							/* in the TFS header. */


/* Flags: */
#define TFS_EXEC	0x00000001	/* 'e': Executable script. */
#define TFS_BRUN	0x00000002	/* 'b': To be executed at boot. */
#define TFS_QRYBRUN	0x00000004	/* 'B': To be executed at boot if */
								/*      query passes. */
#define TFS_SYMLINK	0x00000008	/* 'l': Symbolic link file. */
#define TFS_EBIN	0x00000010	/* 'E': Executable binary (coff/elf/a.out). */
#define TFS_CPRS	0x00000040	/* 'c': File is compressed. */
#define TFS_IPMOD	0x00000080	/* 'i': File is in-place modifiable. */
#define TFS_UNREAD	0x00000100	/* 'u':	File is not even readable if the */
								/*		user-level requirement is not met; */
								/*		else, it is read-only. */
#define TFS_ULVLMSK	0x00000600	/*	User level mask defines 4 access levels: */
#define TFS_ULVL0	0x00000000	/* '0'	level 0 */
#define TFS_ULVL1	0x00000200	/* '1'	level 1 */
#define TFS_ULVL2	0x00000400	/* '2'	level 2 */
#define TFS_ULVL3	0x00000600	/* '3'	level 3 */
#define TFS_NSTALE	0x00000800	/* File is NOT stale, invisible to user.
								 * When this bit is clear, the file is 
								 * considered stale (see notes in tfsadd()). 
								 * See notes in tfsclose() for this.
								 */
#define TFS_ACTIVE	0x00008000	/* Used to indicate that file is not deleted. */

#define TFS_ULVLMAX	TFS_ULVL3
#define TFS_USRLVL(f)	((f->flags & TFS_ULVLMSK) >> 9)

/* Open modes */
#define	TFS_RDONLY		0x00010000	/* File is opened for reading. */
#define	TFS_CREATE		0x00020000	/* File is to be created. Error if file */
									/* with the same name already exists. */
#define	TFS_APPEND		0x00040000	/* Append to existing file.  If OR'ed */
									/* with TFS_CREATE, then create if */
									/* necessary. */
#define	TFS_ALLFFS		0x00080000	/* File is created with all FFs. */
#define	TFS_CREATERM	0x00100000	/* File is to be created. If file with */
									/* same name already exists, then allow */
									/* tfsadd() to remove it if necessary. */

/* The function tfsrunrc() will search through the current file set and */
/* if the file defined by TFS_RCFILE exists, it will be executed. */
/* If this file exists, it will NOT be run by tfsrunboot(). */
#define TFS_RCFILE		"monrc"

/* Requests that can be made to tfsctrl(): */
#define	TFS_ERRMSG		1
#define	TFS_MEMUSE		2
#define	TFS_MEMDEAD		3
#define	TFS_DEFRAG		4
#define	TFS_TELL		5
#define	TFS_UNOPEN		7
#define	TFS_FATOB		8
#define	TFS_FBTOA		9
#define	TFS_MEMAVAIL	10
#define	TFS_TIMEFUNCS	11
#define	TFS_DOCOMMAND	12
#define	TFS_INITDEV		13
#define	TFS_CHECKDEV	14
#define	TFS_DEFRAGDEV	15
#define	TFS_DEFRAGOFF	16
#define	TFS_DEFRAGON	17
#define	TFS_HEADROOM	18
#define	TFS_FCOUNT		19

/* struct tfshdr:
 *	It is in FLASH as part of the file system to record the attributes of
 *	the file at the time of creation.
 */
struct tfshdr {
	unsigned short	hdrsize;		/* Size of this header.					*/
	unsigned short	hdrvrsn;		/* Header version #.					*/
	long	filsize;				/* Size of the file.					*/
	long	flags;					/* Flags describing the file.			*/
	unsigned long filcrc; 			/* 32 bit CRC of file.					*/
	unsigned long hdrcrc;			/* 32 bit CRC of the header.			*/
	unsigned long	modtime;		/* Time when file was last modified.	*/
	struct	tfshdr	*next;			/* Pointer to next file in list.		*/
	char	name[TFSNAMESIZE+1];	/* Name of file.						*/
	char	info[TFSINFOSIZE+1];	/* Miscellaneous info field.			*/
#if TFS_RESERVED
	unsigned long	rsvd[TFS_RESERVED];
#endif
};

#define TFSHDRSIZ	sizeof(struct tfshdr)

/* TFS error returns. */
#define TFS_OKAY				0
#define TFSERR_NOFILE			-1
#define TFSERR_NOSLOT			-2
#define TFSERR_EOF				-3
#define TFSERR_BADARG			-4
#define TFSERR_NOTEXEC			-5
#define TFSERR_BADCRC			-6
#define TFSERR_FILEEXISTS		-7
#define TFSERR_FLASHFAILURE		-8
#define TFSERR_WRITEMAX			-9
#define TFSERR_RDONLY			-10
#define TFSERR_BADFD			-11
#define TFSERR_BADHDR			-12
#define TFSERR_CORRUPT			-13
#define TFSERR_MEMFAIL			-14
#define TFSERR_NOTIPMOD			-16
#define TFSERR_MUTEXFAILURE		-17
#define TFSERR_FLASHFULL		-18
#define TFSERR_USERDENIED		-19
#define TFSERR_NAMETOOBIG		-20
#define TFSERR_FILEINUSE		-21
#define TFSERR_NOTCPRS			-22
#define TFSERR_NOTAVAILABLE		-23
#define TFSERR_BADFLAG			-24
#define TFSERR_CLEANOFF			-25
#define TFSERR_FLAKEYSOURCE		-26
#define TFSERR_BADEXTENSION		-27
#define TFSERR_MIN				-100

/* TFS seek options. */
#define TFS_BEGIN		1
#define TFS_CURRENT		2
#define TFS_END			3

/* Macros: */
#define TFS_DELETED(fp)		(!((fp)->flags & TFS_ACTIVE))
#define TFS_FILEEXISTS(fp)	((fp)->flags & TFS_ACTIVE)
#define TFS_ISCPRS(fp)		((fp)->flags & TFS_CPRS)
#define TFS_ISEXEC(fp)		((fp)->flags & TFS_EXEC)
#define TFS_ISBOOT(fp)		((fp)->flags & TFS_BRUN)
#define TFS_ISLINK(fp)		((fp)->flags & TFS_SYMLINK)
#define TFS_STALE(fp) 		(!((fp)->flags & TFS_NSTALE))
#define TFS_FLAGS(fp)		((fp)->flags)
#define TFS_NAME(fp)		((fp)->name)
#define TFS_SIZE(fp)		((fp)->filsize)
#define TFS_TIME(fp)		((fp)->modtime)
#define TFS_INFO(fp)		((fp)->info)
#define TFS_NEXT(fp)		((fp)->next)
#define TFS_CRC(fp)			((fp)->filcrc)
#define TFS_ENTRY(fp)		((fp)->entry)
#define TFS_BASE(fp)		((char *)(fp)+(fp)->hdrsize)

typedef struct tfshdr TFILE;
#endif
