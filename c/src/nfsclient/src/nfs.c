/* $Id$ */

/* NFS client implementation for RTEMS; hooks into the RTEMS filesystem */

/* Author: Till Straumann <strauman@slac.stanford.edu> 2002 */

/* 
 * Authorship
 * ----------
 * This software (NFS-2 client implementation for RTEMS) was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The NFS-2 client implementation for RTEMS was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 

#ifdef	HAVE_CONFIG_H
#include <config.h>
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <netdb.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <nfs_prot.h>
#include <mount_prot.h>

#include "rpcio.h"

#ifdef HAVE_CEXP_H
#include <cexpHelp.h>
#endif


/* Configurable parameters */

/* Estimated average length of a filename (including terminating 0).
 * This was calculated by doing
 *
 * 	find <some root> -print -exec basename '{}' \; > feil
 * 	wc feil
 *
 * AVG_NAMLEN = (num_chars + num_lines)/num_lines
 */
#define CONFIG_AVG_NAMLEN				10

#define CONFIG_NFS_SMALL_XACT_SIZE		800			/* size of RPC arguments for non-write ops */
/* lifetime of NFS attributes in a NfsNode;
 * the time is in seconds and the lifetime is
 * infinite if the symbol is #undef
 */
#define CONFIG_ATTR_LIFETIME			10/*secs*/

/*
 * The 'st_blksize' (stat(2)) value this nfs
 * client should report. If set to zero then the server's fattr data
 * is passed throught which is not necessary optimal.
 * Newlib's stdio uses 'st_blksize' (if built with HAVE_BLKSIZE defined)
 * to size the default buffer.
 * Due to the overhead of NFS it is probably better to use the maximum
 * size of an NFS read request (8k) rather than the optimal block
 * size on the server.
 * This value can be overridden at run-time by setting the global
 * variable 'nfsStBlksize'.
 * Thanks to Steven Johnson <sjohnson@sakuraindustries.com> for helping
 * working on this issue.
 */   
#define DEFAULT_NFS_ST_BLKSIZE			NFS_MAXDATA

/* dont change this without changing the maximal write size */
#define CONFIG_NFS_BIG_XACT_SIZE		UDPMSGSIZE	/* dont change this */

/* The real values for these are specified further down */
#define NFSCALL_TIMEOUT					(&_nfscalltimeout)
#define MNTCALL_TIMEOUT					(&_nfscalltimeout)
static struct timeval _nfscalltimeout = { 10, 0 };	/* {secs, us } */

/* More or less fixed constants; in particular, NFS3 is not supported */
#define DELIM							'/'
#define HOSTDELIM						':'
#define UPDIR							".."
#define UIDSEP							'@'
#define NFS_VERSION_2					NFS_VERSION

/* we use a dynamically assigned major number */
#define NFS_MAJOR						(nfsGlob.nfs_major)


/* NOTE: RTEMS (ss-20020301) uses a 'short st_ino' type :-( but the
 * NFS fileid is 32 bit. [Later versions of RTEMS have fixed this;
 * nfsInit() issues a warning if you run a version with 'short st_ino'.]
 *
 * As a workarount, we merge the upper 16bits of the fileid into the
 * minor device no. Hence, it is still possible to uniquely identify
 * a file by looking at its device number (major = nfs, minor = part
 * of the fileid + our 'nfs-id' identifier).
 *
 * This has an impact on performance, as e.g. getcwd() stats() all
 * directory entries when it believes it has crossed a mount point
 * (a.st_dev != b.st_dev).
 * 
 * OTOH, it also might cause node comparison failure! E.g. 'getcwd()'
 * assumes that two nodes residing in the same directory must be located
 * on the same device and hence compares 'st_ino' only.
 * If two files in the same directory have the same inode number
 * modulo 2^16, they will be considered equal (although their device
 * number doesn't match - getcwd doesn't look at it).
 *
 * Other software might or might not be affected.
 *
 * The only clean solution to this problem is bumping up the size of
 * 'ino_t' at least to 'long'.
 * Note that this requires _all_ software (libraries etc.) to be
 * recompiled.
 */

#define	NFS_MAKE_DEV_T_INO_HACK(node) \
		rtems_filesystem_make_dev_t( NFS_MAJOR, \
			(((node)->nfs->id)<<16) | (SERP_ATTR((node)).fileid >> 16) )

/* use our 'nfs id' and the server's fsid for the minor device number
 * this should be fairly unique
 */
#define	NFS_MAKE_DEV_T(node) \
		rtems_filesystem_make_dev_t( NFS_MAJOR, \
			(((node)->nfs->id)<<16) | (SERP_ATTR((node)).fsid & ((1<<16)-1)) )

#define  DIRENT_HEADER_SIZE ( sizeof(struct dirent) - \
			sizeof( ((struct dirent *)0)->d_name ) )


/* debugging flags */
#define DEBUG_COUNT_NODES	(1<<0)
#define DEBUG_TRACK_NODES	(1<<1)
#define DEBUG_EVALPATH		(1<<2)
#define DEBUG_READDIR		(1<<3)
#define DEBUG_SYSCALLS		(1<<4)

/* #define DEBUG	( DEBUG_SYSCALLS | DEBUG_COUNT_NODES ) */

#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif

#define MUTEX_ATTRIBUTES    (RTEMS_LOCAL           |   \
                            RTEMS_PRIORITY         |   \
                            RTEMS_INHERIT_PRIORITY |   \
                            RTEMS_BINARY_SEMAPHORE)

#define LOCK(s)		do {                               \
						rtems_semaphore_obtain((s),    \
									RTEMS_WAIT,        \
									RTEMS_NO_TIMEOUT); \
					} while (0) 

#define UNLOCK(s)	do { rtems_semaphore_release((s)); \
					} while (0)

/*****************************************
	Types with Associated XDR Routines
 *****************************************/

/* a string buffer with a maximal length.
 * If the buffer pointer is NULL, it is updated
 * with an appropriately allocated area.
 */
typedef struct strbuf {
	char	*buf;
	u_int	max;
} strbuf;

static bool_t
xdr_strbuf(XDR *xdrs, strbuf *obj)
{
	return xdr_string(xdrs, &obj->buf, obj->max);
}

/* Read 'readlink' results into a 'strbuf'.
 * This is convenient as it avoids  
 * one extra step of copying / lenght
 * checking.
 */
typedef struct readlinkres_strbuf {
	nfsstat	status;
	strbuf	strbuf;
} readlinkres_strbuf;

static bool_t
xdr_readlinkres_strbuf(XDR *xdrs, readlinkres_strbuf *objp)
{
	if ( !xdr_nfsstat(xdrs, &objp->status) )
		return FALSE;

	if ( NFS_OK == objp->status ) {
		if ( !xdr_string(xdrs, &objp->strbuf.buf, objp->strbuf.max) )
			return FALSE;
	}
	return TRUE;
}


/* DirInfoRec is used instead of dirresargs
 * to convert recursion into iteration. The
 * 'rpcgen'erated xdr_dirresargs ends up
 * doing nested calls when unpacking the
 * 'next' pointers.
 */

typedef struct DirInfoRec_ {
	readdirargs	readdirargs;
	/* clone of the 'readdirres' fields;
	 * the cookie is put into the readdirargs above
	 */
	nfsstat		status;
	char		*buf, *ptr;
	int			len;
	bool_t		eofreached;
} DirInfoRec, *DirInfo;

/* this deals with one entry / record */
static bool_t
xdr_dir_info_entry(XDR *xdrs, DirInfo di)
{
union	{
	char			nambuf[NFS_MAXNAMLEN+1];
	nfscookie		cookie;
}				dummy;
struct dirent	*pde = (struct dirent *)di->ptr;
u_int			fileid;
char			*name;
register int	nlen = 0,len,naligned = 0;
nfscookie		*pcookie;

	len = di->len;

	if ( !xdr_u_int(xdrs, &fileid) )
		return FALSE;

	/* we must pass the address of a char* */
	name = (len > NFS_MAXNAMLEN) ? pde->d_name : dummy.nambuf;

	if ( !xdr_filename(xdrs, &name) ) {
		return FALSE;
	}

	if (len >= 0) {
		nlen      = strlen(name);
		naligned  = nlen + 1 /* string delimiter */ + 3 /* alignment */;
		naligned &= ~3;
		len      -= naligned;
	}

	/* if the cookie goes into the DirInfo, we hope this doesn't fail
	 * - the caller ends up with an invalid readdirargs cookie otherwise...
	 */
	pcookie = (len >= 0) ? &di->readdirargs.cookie : &dummy.cookie;
	if ( !xdr_nfscookie(xdrs, pcookie) ) {
		return FALSE;
	}

	di->len = len;
	/* adjust the buffer pointer */
	if (len >= 0) {
		pde->d_ino    = fileid;
		pde->d_namlen = nlen;
		pde->d_off	  = di->ptr - di->buf;
		if (name == dummy.nambuf) {
			memcpy(pde->d_name, dummy.nambuf, nlen + 1);
		}
		pde->d_reclen = DIRENT_HEADER_SIZE + naligned;
		di->ptr      += pde->d_reclen;
	}

	return TRUE;
}

/* this routine loops over all entries */
static bool_t
xdr_dir_info(XDR *xdrs, DirInfo di)
{
DirInfo	dip;

	if ( !xdr_nfsstat(xdrs, &di->status) )
		return FALSE;

	if ( NFS_OK != di->status )
		return TRUE;

	dip = di;

	while (dip) {
		/* reserve space for the dirent 'header' - we assume it's word aligned! */
#ifdef DEBUG
		assert( DIRENT_HEADER_SIZE % 4 == 0 );
#endif
		dip->len -= DIRENT_HEADER_SIZE;

		/* we pass a 0 size - size is unused since
		 * we always pass a non-NULL pointer
		 */
		if ( !xdr_pointer(xdrs, (void*)&dip, 0 /* size */, (xdrproc_t)xdr_dir_info_entry) )
			return FALSE;
	}

	if ( ! xdr_bool(xdrs, &di->eofreached) )
		return FALSE;

	/* if everything fits into the XDR buffer but not the user's buffer,
	 * they must resume reading from where xdr_dir_info_entry() started
	 * skipping and 'eofreached' needs to be adjusted
	 */
	if ( di->len < 0 && di->eofreached )
		di->eofreached = FALSE;

	return TRUE;
}


/* a type better suited for node operations
 * than diropres.
 * fattr and fhs are swapped so parts of this
 * structure may be used as a diroparg which
 * is practical when looking up paths.
 */

/* Macro for accessing serporid fields
 */
#define SERP_ARGS(node) ((node)->serporid.serporid_u.serporid.arg_u)
#define SERP_ATTR(node) ((node)->serporid.serporid_u.serporid.attributes)
#define SERP_FILE(node) ((node)->serporid.serporid_u.serporid.file)


typedef struct serporidok {
	fattr					attributes;
	nfs_fh					file;
	union	{
		struct {
			filename	name;
		}					diroparg;
		struct {
			sattr		attributes;
		}					sattrarg;
		struct {
			u_int 		offset;
			u_int		count;
			u_int		totalcount;
		}					readarg;
		struct {
			u_int		beginoffset;
			u_int		offset;
			u_int		totalcount;
			struct {
				u_int data_len;
				char* data_val;
			}			data;
		}					writearg;
		struct {
			filename	name;
			sattr		attributes;
		}					createarg;
		struct {
			filename	name;
			diropargs	to;
		}					renamearg;
		struct {
			diropargs	to;
		}					linkarg;
		struct {
			filename	name;
			nfspath		to;
			sattr		attributes;
		}					symlinkarg;
		struct {
			nfscookie	cookie;
			u_int		count;
		}					readdirarg;
	}							arg_u;
} serporidok;

typedef struct serporid {
	nfsstat			status;
	union	{
		serporidok	serporid;
	}				serporid_u;
} serporid;

/* an XDR routine to encode/decode the inverted diropres 
 * into an nfsnodestat;
 *
 * NOTE: this routine only acts on
 *   - 'serporid.status'
 *   - 'serporid.file'
 *   - 'serporid.attributes'
 * and leaves the 'arg_u' alone.
 * 
 * The idea is that a 'diropres' is read into 'serporid'
 * which can then be used as an argument to subsequent
 * NFS-RPCs (after filling in the node's arg_u).
 */
static bool_t
xdr_serporidok(XDR *xdrs, serporidok *objp)
{
     if (!xdr_nfs_fh (xdrs, &objp->file))
         return FALSE;
     if (!xdr_fattr (xdrs, &objp->attributes))
         return FALSE;
    return TRUE;
}   

static bool_t
xdr_serporid(XDR *xdrs, serporid *objp)
{
     if (!xdr_nfsstat (xdrs, &objp->status))
         return FALSE;
    switch (objp->status) {
    case NFS_OK:
         if (!xdr_serporidok(xdrs, &objp->serporid_u.serporid))
             return FALSE;
        break;
    default:
        break;
    }
    return TRUE;
}

/*****************************************
	Data Structures and Types
 *****************************************/

/* 'time()' hack with less overhead; */

/* assume reading a long word is atomic */
#define READ_LONG_IS_ATOMIC

typedef uint32_t	TimeStamp;

static inline TimeStamp
nowSeconds(void)
{
register uint32_t	rval;
#ifndef READ_LONG_IS_ATOMIC
rtems_interrupt_level		l;

	rtems_interrupt_disable(level);
#endif

	rval = _TOD_Seconds_since_epoch;

#ifndef READ_LONG_IS_ATOMIC
	rtems_interrupt_enable(level);
#endif
	return rval;
}


/* Per mounted FS structure */
typedef struct NfsRec_ {
		/* the NFS server we're talking to.
		 */
	RpcUdpServer						 server;
		/* statistics; how many NfsNodes are
		 * currently alive.
		 */
	volatile int						 nodesInUse;
#if DEBUG & DEBUG_COUNT_NODES
		/* statistics; how many 'NfsNode.str'
		 * strings are currently allocated.
		 */
	volatile int						 stringsInUse;
#endif
		/* A small number who uniquely
		 * identifies a mounted NFS within
		 * this driver (i.e. this NfsRec).
		 * Each time a NFS is mounted, the
		 * global ID counter is incremented
		 * and its value is assigned to the
		 * newly created NfsRec.
		 */
	u_short								 id;
		/* Our RTEMS filesystem mt_entry
		 */ 
	rtems_filesystem_mount_table_entry_t *mt_entry;
		/* Next NfsRec on a linked list who
		 * is anchored at nfsGlob 
		 */
	struct NfsRec_						 *next;
		/* Who we pretend we are
		 */
	u_long								 uid,gid;
} NfsRec, *Nfs;

typedef struct NfsNodeRec_ {
		/* This holds this node's attributes
		 * (stats) and its nfs filehandle.
		 * It also contains room for nfs rpc
		 * arguments.
		 */
	serporid	serporid;
		/* The arguments we used when doing
		 * the 'lookup' call for this node.
		 * We need this information (especially
		 * the directory FH) for performing
		 * certain operations on this
		 * node (in particular: for unlinking
		 * it from a parent directory)
		 */
	diropargs		args;
		/* FS this node belongs to
		 */
	Nfs				nfs;
		/* A buffer for the string the
		 * args.name points to.
		 * We need this because args.name might
		 * temporarily point to strings on the
		 * stack. Duplicates are allocated from
		 * the heap and attached to 'str' so
		 * they can be released as appropriate.
		 */
	char		   *str;
		/* A timestamp for the stats
		 */
	TimeStamp		age;
} NfsNodeRec, *NfsNode;

/*****************************************
	Forward Declarations
 *****************************************/

static int nfs_readlink(
	rtems_filesystem_location_info_t  *loc,     	/* IN  */       
	char							  *buf,			/* OUT */
	size_t							  len
);

static int updateAttr(NfsNode node, int force);

/* Mask bits when setting attributes.
 * Only the 'arg' fields with their
 * corresponding bit set in the mask
 * will be used. The others are left
 * unchanged.
 * The 'TOUCH' bits instruct nfs_sattr()
 * to update the respective time
 * fields to the current time
 */
#define	SATTR_MODE		(1<<0)
#define	SATTR_UID		(1<<1)
#define	SATTR_GID		(1<<2)
#define	SATTR_SIZE		(1<<3)
#define	SATTR_ATIME		(1<<4)
#define	SATTR_TOUCHA	(1<<5)
#define	SATTR_MTIME		(1<<6)
#define	SATTR_TOUCHM	(1<<7)
#define SATTR_TOUCH		(SATTR_TOUCHM | SATTR_TOUCHA)

static int
nfs_sattr(NfsNode node, sattr *arg, u_long mask);

extern struct _rtems_filesystem_operations_table nfs_fs_ops;
static struct _rtems_filesystem_file_handlers_r	 nfs_file_file_handlers;
static struct _rtems_filesystem_file_handlers_r	 nfs_dir_file_handlers;
static struct _rtems_filesystem_file_handlers_r	 nfs_link_file_handlers;
static		   rtems_driver_address_table		 drvNfs;

int
nfsMountsShow(FILE*);

rtems_status_code
rtems_filesystem_resolve_location(char *buf, int len, rtems_filesystem_location_info_t *loc);


/*****************************************
	Inline Routines
 *****************************************/


/* * * * * * * * * * * * * * * * * *
	Trivial Operations on a NfsNode 
 * * * * * * * * * * * * * * * * * */

/* determine if a location 'l' is an NFS root node */
static inline int
locIsRoot(rtems_filesystem_location_info_t *l)
{
NfsNode me = (NfsNode) l->node_access;
NfsNode r;
	r = (NfsNode)l->mt_entry->mt_fs_root.node_access;
	return SERP_ATTR(r).fileid == SERP_ATTR(me).fileid &&
		   SERP_ATTR(r).fsid   == SERP_ATTR(me).fsid;
}

/* determine if a location 'l' is an NFS node */
static inline int
locIsNfs(rtems_filesystem_location_info_t *l)
{
	return l->ops == &nfs_fs_ops;
}

/* determine if two locations refer to the
 * same entity. We know that 'nfsloc' is a
 * location inside nfs. However, we needn't
 * know anything about 'anyloc'.
 */
static inline int
locAreEqual(
	rtems_filesystem_location_info_t *nfsloc,
	rtems_filesystem_location_info_t *anyloc
)
{
NfsNode na = (NfsNode) nfsloc->node_access;
NfsNode nb;

	if (!locIsNfs(anyloc))
		return 0;

	nb = (NfsNode) anyloc->node_access;

	if (na->nfs != nb->nfs)
		return 0;

	updateAttr(nb, 0);

	return SERP_ATTR(na).fileid == SERP_ATTR(nb).fileid &&
		   SERP_ATTR(na).fsid   == SERP_ATTR(nb).fsid;
}



/*****************************************
	Global Variables
 *****************************************/

/* These are (except for MAXNAMLEN/MAXPATHLEN) copied from IMFS */

static rtems_filesystem_limits_and_options_t
nfs_limits_and_options = {
   5, 				/* link_max */
   6, 				/* max_canon */
   7, 				/* max_input */
   NFS_MAXNAMLEN,	/* name_max */
   NFS_MAXPATHLEN,	/* path_max */
   2,				/* pipe_buf */
   1,				/* posix_async_io */
   2,				/* posix_chown_restrictions */
   3,				/* posix_no_trunc */
   4,				/* posix_prio_io */
   5,				/* posix_sync_io */
   6				/* posix_vdisable */
};

/* size of an encoded 'entry' object */
static int dirres_entry_size;

/* Global stuff and statistics */
static struct nfsstats {
		/* A lock for protecting the
		 * linked ist of mounted NFS
		 * and the num_mounted_fs field
		 */
	rtems_id					llock;
		/* A lock for protecting misc
		 * stuff  within the driver.
		 * The lock must only be held
		 * for short periods of time.
		 */
	rtems_id					lock;
		/* Our major number as assigned
		 * by RTEMS
		 */
	rtems_device_major_number	nfs_major;
		/* The number of currently
		 * mounted NFS
		 */
	int							num_mounted_fs;
		/* A list of the currently
		 * mounted NFS
		 */
	struct NfsRec_				*mounted_fs;
		/* A counter for allocating
		 * unique IDs to each mounted
		 * NFS.
		 * Assume we are not going to
		 * do more than 16k mounts
		 * during the system lifetime
		 */
	u_short						fs_ids;
} nfsGlob = {0/* IMPORTANT */};

/*
 * Global variable to tune the 'st_blksize' (stat(2)) value this nfs
 * client should report.
 * size on the server.
 */                   
#ifndef DEFAULT_NFS_ST_BLKSIZE
#define DEFAULT_NFS_ST_BLKSIZE	NFS_MAXDATA
#endif
int nfsStBlksize = DEFAULT_NFS_ST_BLKSIZE;

/* Two pools of RPC transactions;
 * One with small send buffers
 * the other with a big one.
 * The actual size of the small
 * buffer is configurable (see top).
 *
 * Note: The RX buffers are always
 * big
 */
static RpcUdpXactPool smallPool = 0;
static RpcUdpXactPool bigPool   = 0;


/*****************************************
	Implementation
 *****************************************/

/* Create a Nfs object. This is
 * per-mounted NFS information.
 *
 * ARGS:	The Nfs server handle.
 *
 * RETURNS:	Nfs on success,
 * 			NULL on failure with
 * 			errno set
 *
 * NOTE:	The submitted server
 * 			object is 'owned' by
 * 			this Nfs and will be
 * 			destroyed by nfsDestroy()
 */
static Nfs
nfsCreate(RpcUdpServer server)
{
Nfs rval = calloc(1,sizeof(*rval));

	if (rval) {
		rval->server     = server;
		LOCK(nfsGlob.llock);
			rval->next 		   = nfsGlob.mounted_fs;
			nfsGlob.mounted_fs = rval;
		UNLOCK(nfsGlob.llock);
	} else {
		errno = ENOMEM;
	}
		return rval;
}

/* Destroy an Nfs object and
 * its associated server
 */
static void
nfsDestroy(Nfs nfs)
{
register Nfs prev;
	if (!nfs)
		return;

	LOCK(nfsGlob.llock);
		if (nfs == nfsGlob.mounted_fs)
			nfsGlob.mounted_fs = nfs->next;
		else {
			for (prev = nfsGlob.mounted_fs;
				 prev && prev->next != nfs;
				 prev = prev->next)
					/* nothing else to do */;
			assert( prev );
			prev->next = nfs->next;
		}
	UNLOCK(nfsGlob.llock);

	nfs->next = 0; /* paranoia */
	rpcUdpServerDestroy(nfs->server);
	free(nfs);
}

/*
 * Create a Node. The node will
 * be associated with a particular
 * mounted NFS identified by 'nfs'
 * Optionally, a NFS file handle
 * may be copied into this node.
 *
 * ARGS:	nfs of the NFS this node
 * 			belongs to.
 * 			NFS file handle identifying
 * 			this node.
 * RETURNS:	node on success,
 * 			NULL on failure with errno
 * 			set.
 *
 * NOTE:	The caller of this routine
 * 			is responsible for copying
 * 			a NFS file handle if she
 * 			choses to pass a NULL fh.
 *
 * 			The driver code assumes the
 * 			a node always has a valid
 * 			NFS filehandle and file
 * 			attributes (unless the latter
 * 			are aged).
 */
static NfsNode
nfsNodeCreate(Nfs nfs, fhandle *fh)
{
NfsNode	rval = malloc(sizeof(*rval));
unsigned long flags;

#if DEBUG & DEBUG_TRACK_NODES
	fprintf(stderr,"NFS: creating a node\n");
#endif

	if (rval) {
		if (fh)
			memcpy( &SERP_FILE(rval), fh, sizeof(*fh) );
		rtems_interrupt_disable(flags);
			nfs->nodesInUse++;
		rtems_interrupt_enable(flags);
		rval->nfs       = nfs;
		rval->str		= 0;
	} else {
		errno = ENOMEM;
	}

	return rval;
}

/* destroy a node */
static void
nfsNodeDestroy(NfsNode node)
{
unsigned long flags;

#if DEBUG & DEBUG_TRACK_NODES
	fprintf(stderr,"NFS: destroying a node\n");
#endif
#if 0
	if (!node)
		return;
	/* this probably does nothing... */
  	xdr_free(xdr_serporid, &node->serporid);
#endif

	rtems_interrupt_disable(flags);
		node->nfs->nodesInUse--;
#if DEBUG & DEBUG_COUNT_NODES
		if (node->str)
			node->nfs->stringsInUse--;
#endif
	rtems_interrupt_enable(flags);

	if (node->str)
		free(node->str);

	free(node);
}

/* Clone a given node (AKA copy constructor),
 * i.e. create an exact copy.
 *
 * ARGS:	node to clone
 * RETURNS:	new node on success
 * 			NULL on failure with errno set.
 *
 * NOTE:	a string attached to 'str'
 * 			is cloned as well. Outdated
 * 			attributes (of the new copy
 * 			only) will be refreshed
 * 			(if unsuccessful, this could
 * 			be a reason for failure to
 * 			clone a node).
 */
static NfsNode
nfsNodeClone(NfsNode node)
{
NfsNode rval = nfsNodeCreate(node->nfs, 0);

	if (rval) {
		*rval = *node;

		/* must clone the string also */
		if (node->str) {
			rval->args.name = rval->str = strdup(node->str);
			if (!rval->str) {
				nfsNodeDestroy(rval);
				return 0;
			}
#if DEBUG & DEBUG_COUNT_NODES
			{ unsigned long flags;
			rtems_interrupt_disable(flags);
				node->nfs->stringsInUse++;
			rtems_interrupt_enable(flags);
			}
#endif
		}

		/* possibly update the stats */
		if (updateAttr(rval, 0 /* only if necessary */)) {
			nfsNodeDestroy(rval);
			return 0;
		}
	}
	return rval;
}

/* Initialize the driver.
 *
 * ARGS:	depth of the small and big
 * 			transaction pools, i.e. how
 * 			many transactions (buffers)
 * 			should always be kept around.
 *
 * 			(If more transactions are needed,
 * 			they are created and destroyed
 * 			on the fly).
 */
void
nfsInit(int smallPoolDepth, int bigPoolDepth)
{  
entry	dummy;

	fprintf(stderr,"This is RTEMS-NFS Release $Name$\n");
	fprintf(stderr,"($Id$)\n\n");
	fprintf(stderr,"Till Straumann, Stanford/SLAC/SSRL 2002\n");
	fprintf(stderr,"See LICENSE file for licensing info\n");

	/* Get a major number */

	if (RTEMS_SUCCESSFUL != rtems_io_register_driver(0, &drvNfs, &nfsGlob.nfs_major)) {
		fprintf(stderr,"Registering NFS driver failed - %s\n", strerror(errno));
		return;
	}

	if (0==smallPoolDepth)
		smallPoolDepth = 20;
	if (0==bigPoolDepth)
		bigPoolDepth   = 10;

	/* it's crucial to zero out the 'next' pointer
	 * because it terminates the xdr_entry recursion
	 *
	 * we also must make the filename some non-zero
	 * char pointer!
	 */

	memset(&dummy, 0, sizeof(dummy));

	dummy.nextentry   = 0;
	dummy.name        = "somename"; /* guess average length of a filename */
	dirres_entry_size = xdr_sizeof((xdrproc_t)xdr_entry, &dummy);

	assert( smallPool = rpcUdpXactPoolCreate(
							NFS_PROGRAM,
							NFS_VERSION_2,
							CONFIG_NFS_SMALL_XACT_SIZE,
							smallPoolDepth) );

	assert( bigPool   = rpcUdpXactPoolCreate(
							NFS_PROGRAM,
							NFS_VERSION_2,
							CONFIG_NFS_BIG_XACT_SIZE,
							bigPoolDepth) );

	assert( RTEMS_SUCCESSFUL == rtems_semaphore_create(
							rtems_build_name('N','F','S','l'),
							1,
							MUTEX_ATTRIBUTES,
							0,
							&nfsGlob.llock) );

	assert( RTEMS_SUCCESSFUL == rtems_semaphore_create(
							rtems_build_name('N','F','S','m'),
							1,
							MUTEX_ATTRIBUTES,
							0,
							&nfsGlob.lock) );

	if (sizeof(ino_t) < sizeof(u_int)) {
		fprintf(stderr,
				"WARNING: Using 'short st_ino' hits performance and may fail to access/find correct files\n");
		fprintf(stderr,
				"you should fix newlib's sys/stat.h - for now I'll enable a hack...\n");

	}
}

/* Driver cleanup code
 */
int
nfsCleanup(void)
{
rtems_id	l;
int			refuse;

	if (!nfsGlob.llock) {
		/* registering the driver failed - let them still cleanup */
		return 0;
	}

	LOCK(nfsGlob.llock);
	if ( (refuse = nfsGlob.num_mounted_fs) ) {
		fprintf(stderr,"Refuse to unload NFS; %i filesystems still mounted.\n",
						refuse);
		nfsMountsShow(stderr);
		/* yes, printing is slow - but since you try to unload the driver,
		 * you assume nobody is using NFS, so what if they have to wait?
		 */
		UNLOCK(nfsGlob.llock);
		return -1;
	}

	rtems_semaphore_delete(nfsGlob.lock);
	nfsGlob.lock = 0;

	/* hold the lock while cleaning up... */

	rpcUdpXactPoolDestroy(smallPool);
	rpcUdpXactPoolDestroy(bigPool);
	l = nfsGlob.llock;
	rtems_io_unregister_driver(nfsGlob.nfs_major);

	rtems_semaphore_delete(l);
	nfsGlob.llock = 0;
	return 0;
}

/* NFS RPC wrapper.
 *
 * ARGS:	srvr	the NFS server we want to call
 * 			proc	the NFSPROC_xx we want to invoke
 * 			xargs   xdr routine to wrap the arguments
 * 			pargs   pointer to the argument object
 * 			xres	xdr routine to unwrap the results
 * 			pres	pointer to the result object
 *
 * RETURNS:	0 on success, -1 on error with errno set.
 *
 * NOTE:	the caller assumes that errno is set to
 *			a nonzero value if this routine returns
 *			an error (nonzero return value).
 *
 *			This routine prints RPC error messages to
 *			stderr.
 */
STATIC int
nfscall(
	RpcUdpServer	srvr,
	int				proc,
	xdrproc_t		xargs,
	void *			pargs,
	xdrproc_t		xres,
	void *			pres)
{
RpcUdpXact		xact;
enum clnt_stat	stat;
RpcUdpXactPool	pool;
int				rval = -1;


	switch (proc) {
		case NFSPROC_SYMLINK:
		case NFSPROC_WRITE:
					pool = bigPool;		break;
		default:	pool = smallPool;	break;
	}

	xact = rpcUdpXactPoolGet(pool, XactGetCreate);

	if ( !xact ) {
		errno = ENOMEM;
		return -1;
	}

	if ( RPC_SUCCESS != (stat=rpcUdpSend(
								xact,
								srvr,
								NFSCALL_TIMEOUT,
								proc,
								xres,
								pres,
								xargs,
								pargs,
								0)) ||
	     RPC_SUCCESS != (stat=rpcUdpRcv(xact)) ) {

		fprintf(stderr,
				"NFS (proc %i) - %s\n",
				proc,
				clnt_sperrno(stat));

		switch (stat) {
			/* TODO: this is probably not complete and/or fully accurate */
			case RPC_CANTENCODEARGS : errno = EINVAL;	break;
			case RPC_AUTHERROR  	: errno = EPERM;	break;

			case RPC_CANTSEND		:
			case RPC_CANTRECV		: /* hope they have errno set */
			case RPC_SYSTEMERROR	: break;

			default             	: errno = EIO;		break;
		}
	} else {
		rval = 0;
	}

	/* release the transaction back into the pool */
	rpcUdpXactPoolPut(xact);

	if (rval && !errno)
		errno = EIO;

	return rval;
}

/* Check the 'age' of a node's stats
 * and read the attributes from the server
 * if necessary.
 * 
 * ARGS:	node	node to update
 * 			force	enforce updating ignoring
 * 					the timestamp/age
 *
 * RETURNS:	0 on success,
 * 			-1 on failure with errno set
 */

static int
updateAttr(NfsNode node, int force)
{

	if (force
#ifdef CONFIG_ATTR_LIFETIME
		|| (nowSeconds() - node->age > CONFIG_ATTR_LIFETIME)
#endif
		) {
		if ( nfscall(node->nfs->server,
					  NFSPROC_GETATTR,
					  (xdrproc_t)xdr_nfs_fh,	&SERP_FILE(node),
					  (xdrproc_t)xdr_attrstat, &node->serporid) )
		return -1;

		if ( NFS_OK != node->serporid.status ) {
			errno = node->serporid.status;
			return -1;
		}

		node->age = nowSeconds();
	}
	
	return 0;
}

/*
 * IP address helper. Note that we avoid
 * gethostbyname() since it's not reentrant.
 *
 * initialize a sockaddr_in from a
 * [<uid>'.'<gid>'@']<host>':'<path>" string and let
 * pPath point to the <path> part; retrieve the optional
 * uid/gids
 *
 * ARGS:	see description above
 *
 * RETURNS:	0 on success,
 * 			-1 on failure with errno set
 */
static int
buildIpAddr(u_long *puid, u_long *pgid,
			char **pHost, struct sockaddr_in *psa,
			char **pPath)
{
char	host[30];
char	*chpt = *pPath;
char	*path;
int		len;

	if ( !chpt ) {
		errno = EINVAL;
		return -1;
	}

	/* look for the optional uid/gid */
	if ( (chpt = strchr(chpt, UIDSEP)) ) {
		if ( 2 != sscanf(*pPath,"%li.%li",puid,pgid) ) {
			errno = EINVAL;
			return -1;
		}
		chpt++;
	} else {
		*puid = RPCIOD_DEFAULT_ID;
		*pgid = RPCIOD_DEFAULT_ID;
		chpt  = *pPath;
	}
	if ( pHost )
		*pHost = chpt;

	/* split the device name which is in the form
	 *
	 * <host_ip> ':' <path>
	 *
	 * into its components using a local buffer
	 */

	if ( !(path = strchr(chpt, HOSTDELIM)) ||
	      (len  = path - chpt) >= sizeof(host) - 1 ) {
		errno = EINVAL;
		return -1;
	}
	/* point to path beyond ':' */
	path++;

	strncpy(host, chpt, len);
	host[len]=0;

	if ( ! inet_pton(AF_INET, host, &psa->sin_addr) ) {
		errno = ENXIO;
		return -1;
	}

	psa->sin_family = AF_INET;
	psa->sin_port   = 0;
	*pPath          = path;
	return 0;
}

/* wrapper similar to nfscall.
 * However, since it is not used
 * very often, the simpler and less
 * efficient rpcUdpCallRp API is used.
 *
 * ARGS:	see 'nfscall()' above
 *
 * RETURNS:	RPC status
 */
static enum clnt_stat
mntcall(
	struct sockaddr_in	*psrvr,
	int					proc,
	xdrproc_t			xargs,
	void *				pargs,
	xdrproc_t			xres,
	void *				pres,
	u_long				uid,
	u_long				gid)
{
#ifdef MOUNT_V1_PORT
int					retry;
#endif
enum clnt_stat		stat = RPC_FAILED;

#ifdef MOUNT_V1_PORT
	/* if the portmapper fails, retry a fixed port */
	for (retry = 1, psrvr->sin_port = 0, stat = RPC_FAILED;
		 retry >= 0 && stat;
		 stat && (psrvr->sin_port = htons(MOUNT_V1_PORT)), retry-- )
#endif
		stat  = rpcUdpCallRp(
						psrvr,
						MOUNTPROG,
						MOUNTVERS,
						proc,
						xargs,
						pargs,
						xres,
						pres,
						uid,
						gid,
						MNTCALL_TIMEOUT
				);
	return stat;
}

/*****************************************
	RTEMS File System Operations for NFS
 *****************************************/

#if 0 /* for reference */

struct rtems_filesystem_location_info_tt
{
   void                                 *node_access;
   rtems_filesystem_file_handlers_r     *handlers;
   rtems_filesystem_operations_table    *ops;
   rtems_filesystem_mount_table_entry_t *mt_entry;
};

#endif

/*
 * Evaluate a path letting 'pathloc' travel along.
 *
 * The important semantics of this operation are:
 *
 * If this routine returns -1, the caller assumes
 * pathloc to be _invalid_ and hence it will not
 * invoke rtems_filesystem_freenode() on it.
 *
 * OTOH, if evalpath returns 0,
 * rtems_filesystem_freenode() will eventually be
 * called which results in our freeing the associated
 * NfsNode attached to node_access.
 *
 * Therefore, this routine will _always_ allocate
 * a NfsNode and pass it out to *pathloc (provided
 * that the evaluation succeeds).
 *
 * However, if the evaluation finds that it has to
 * step across FS boundaries (mount point or a symlink
 * pointing outside), the NfsNode is destroyed
 * before passing control to the new FS' evalpath_h()
 *
 */
	
STATIC int nfs_do_evalpath(
	const char                        *pathname,      /* IN     */
	void                              *arg,
	rtems_filesystem_location_info_t  *pathloc,       /* IN/OUT */
	int								  forMake
)
{
char			*del = 0, *part;
int				e = 0;
NfsNode			node   = pathloc->node_access;
char			*p     = malloc(MAXPATHLEN+1);
Nfs				nfs    = (Nfs)pathloc->mt_entry->fs_info;
RpcUdpServer	server = nfs->server;
unsigned long	flags;
#if DEBUG & DEBUG_COUNT_NODES
unsigned long	niu,siu;
#endif

	if ( !p ) {
		e = ENOMEM;
		goto cleanup;
	}
	strcpy(p, pathname);

	LOCK(nfsGlob.lock);
	node = nfsNodeClone(node);
	UNLOCK(nfsGlob.lock);

	/* from here on, the NFS is protected from being unmounted
	 * since the node refcount is > 1
	 */
	
	/* clone the node */
	if ( !node ) {
		/* nodeClone sets errno */
		goto cleanup;
	}

	pathloc->node_access = node;

	/* Special case: the RTEMS filesystem code
	 * may emit '..' on a regular file node to
	 * find the parent directory :-(.
	 * (eval.c: rtems_filesystem_evaluate_parent())
	 * Try to catch this case here:
	 */
	if ( NFDIR != SERP_ATTR(node).type && '.'==*p && '.'==*(p+1) ) {
		for ( part = p+2; '/'==*part; part++ )
			/* skip trailing '/' */;
		if ( !*part ) {
			/* this is it; back out dir and let them look up the dir itself... */
			memcpy( &SERP_FILE(node),
					&node->args.dir,
					sizeof(node->args.dir));
			*(p+1)=0;
		}
	}

	for (part=p; part && *part; part=del) {

		if ( NFLNK == SERP_ATTR(node).type ) {
			/* follow midpath link */
			char *b = malloc(NFS_MAXPATHLEN+1);
			int	  l;

			if (!b) {
				e = ENOMEM;
				goto cleanup;
			}
			if (nfs_readlink(pathloc, b, NFS_MAXPATHLEN+1)) {
				free(b);
				e = errno;
				goto cleanup;
			}

			/* prepend the link value to the rest of the path */
			if ( (l=strlen(b)) + strlen(part) + 1 > NFS_MAXPATHLEN ) {
				free(b);
				e = EINVAL;
				goto cleanup;
			}
			/* swap string buffers and reset delimiter */
			b[l++] = DELIM;
			strcpy(b+l,part);
			part = b;
			b    = p;
			p    = del = part;

			free(b);

			/* back up the directory filehandle (only necessary
			 * if we don't back out to the root
		 	 */
			if (! (DELIM == *part) ) {
				memcpy( &SERP_FILE(node),
						&node->args.dir,
						sizeof(node->args.dir));

				if (updateAttr(node, 1 /* force */)) {
					e = errno;
					goto cleanup;
				}
			}
		}

		/* find delimiter and eat /// sequences
		 * (only if we don't restart at the root)
		 */
		if ( DELIM != *part && (del = strchr(part, DELIM))) {
			do {
				*del++=0;
			} while (DELIM==*del);
		}

		/* refuse to backup over the root */
		if ( 0==strcmp(part,UPDIR)
			 && locAreEqual(pathloc, &rtems_filesystem_root) ) {
			part++;
		}

		/* cross mountpoint upwards */
		if ( (0==strcmp(part,UPDIR) && locIsRoot(pathloc)) /* cross mountpoint up */
			|| DELIM == *part                              /* link starts at root */
			) {
			int									rval;

#if DEBUG & DEBUG_EVALPATH
			fprintf(stderr,
					"Crossing mountpoint upwards\n");
#endif

			if (DELIM == *part) {
				*pathloc = rtems_filesystem_root;
			} else {
				*pathloc = pathloc->mt_entry->mt_point_node;
				/* re-append the rest of the path */
				if (del)
					while ( 0 == *--del )
						*del = DELIM;
			}

			nfsNodeDestroy(node);

#if DEBUG & DEBUG_EVALPATH
			fprintf(stderr,
					"Re-evaluating '%s'\n",
					part);
#endif

			if (forMake)
				rval = pathloc->ops->evalformake_h(part, pathloc, (const char**)arg);
			else
				rval = pathloc->ops->evalpath_h(part, (int)arg, pathloc);

			free(p);
			return rval;
		}

		/* lookup one element */
		SERP_ARGS(node).diroparg.name = part;

		/* remember args / directory fh */
		memcpy( &node->args, &SERP_FILE(node), sizeof(node->args));

		/* don't lookup the item we want to create */
		if ( forMake && (!del || !*del) )
				break;

#if DEBUG & DEBUG_EVALPATH
		fprintf(stderr,"Looking up '%s'\n",part);
#endif

		if ( nfscall(server,
						 NFSPROC_LOOKUP,
						 (xdrproc_t)xdr_diropargs, &SERP_FILE(node),
						 (xdrproc_t)xdr_serporid,  &node->serporid) ||
			NFS_OK != (errno=node->serporid.status) ) {
			e = errno;
			goto cleanup;
		}
		node->age = nowSeconds();

#if DEBUG & DEBUG_EVALPATH
		if (NFLNK == SERP_ATTR(node).type && del) {
			fprintf(stderr,
					"Following midpath link '%s'\n",
					part);
		}
#endif

	}

	if (forMake) {
		/* remember the name - do this _before_ copying
		 * the name to local storage; the caller expects a
		 * pointer into pathloc
		 */
		assert( node->args.name );

		*(const char**)arg = pathname + (node->args.name - p);

#if 0
		/* restore the directory node */

		memcpy( &SERP_FILE(node),
				&node->args.dir,
				sizeof(node->args.dir));

		if ( (nfscall(nfs->server,
						NFSPROC_GETATTR,
						(xdrproc_t)xdr_nfs_fh,   &SERP_FILE(node),
						(xdrproc_t)xdr_attrstat, &node->serporid) && !errno && (errno = EIO)) ||
		     (NFS_OK != (errno=node->serporid.status) ) ) {
			goto cleanup;
		}
#endif
	}

	if (locIsRoot(pathloc)) {

		/* stupid filesystem code has no 'op' for comparing nodes
		 * but just compares the 'node_access' pointers.
		 * Luckily, this is only done for comparing the root nodes.
		 * Hence, we never give them a copy of the root but always
		 * the root itself.
		 */
		pathloc->node_access = pathloc->mt_entry->mt_fs_root.node_access;
		/* increment the 'in use' counter since we return one more
		 * reference to the root node
		 */
		rtems_interrupt_disable(flags);
			nfs->nodesInUse++;
		rtems_interrupt_enable(flags);
		nfsNodeDestroy(node);


	} else {
		switch (SERP_ATTR(node).type) {
			case NFDIR:	pathloc->handlers = &nfs_dir_file_handlers;  break;
			case NFREG:	pathloc->handlers = &nfs_file_file_handlers; break;
			case NFLNK: pathloc->handlers = &nfs_link_file_handlers; break;
			default: 	pathloc->handlers = &rtems_filesystem_null_handlers; break;
		}
		pathloc->node_access = node;

		/* remember the name of this directory entry */

		if (node->args.name) {
			if (node->str) {
#if DEBUG & DEBUG_COUNT_NODES
				rtems_interrupt_disable(flags);
					nfs->stringsInUse--;
				rtems_interrupt_enable(flags);
#endif
				free(node->str);
			}
			node->args.name = node->str = strdup(node->args.name);
			if (!node->str) {
				e = ENOMEM;
				goto cleanup;
			}

#if DEBUG & DEBUG_COUNT_NODES
			rtems_interrupt_disable(flags);
				nfs->stringsInUse++;
			rtems_interrupt_enable(flags);
#endif
		}

	}
	node = 0;

	e = 0;

cleanup:
	free(p);
#if DEBUG & DEBUG_COUNT_NODES
	/* cache counters; nfs may be unmounted by other thread after the last
	 * node is destroyed
	 */
	niu = nfs->nodesInUse;
	siu = nfs->stringsInUse;
#endif
	if (node) {
		nfsNodeDestroy(node);
		pathloc->node_access = 0;
	}
#if DEBUG & DEBUG_COUNT_NODES
	fprintf(stderr,
			"leaving evalpath, in use count is %i nodes, %i strings\n",
			niu,siu);
#endif
	if (e) {
#if DEBUG & DEBUG_EVALPATH
		perror("Evalpath");
#endif
		rtems_set_errno_and_return_minus_one(e);
	} else {
		return 0;
	}
}

/* MANDATORY; may set errno=ENOSYS and return -1 */
static int nfs_evalformake(
	const char                       *path,       /* IN */
	rtems_filesystem_location_info_t *pathloc,    /* IN/OUT */
	const char                      **pname       /* OUT    */
)
{
	return nfs_do_evalpath(path, (void*)pname, pathloc, 1 /*forMake*/);
}

static int nfs_evalpath(
	const char						 *path,		  /* IN */
	int								 flags,		  /* IN */
	rtems_filesystem_location_info_t *pathloc    /* IN/OUT */
)
{
	return nfs_do_evalpath(path, (void*)flags, pathloc, 0 /*not forMake*/);
}


/* create a hard link */

static int nfs_link(
	rtems_filesystem_location_info_t  *to_loc,      /* IN */
	rtems_filesystem_location_info_t  *parent_loc,  /* IN */
	const char                        *name         /* IN */
)
{
NfsNode pNode;
nfsstat status;
NfsNode tNode = to_loc->node_access;

#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,"Creating link '%s'\n",name);
#endif

	if ( !locIsNfs(parent_loc) ) {
		errno = EXDEV;
		return -1;
	}

	pNode = parent_loc->node_access;
	if ( tNode->nfs != pNode->nfs ) {
		errno = EXDEV;
		return -1;
	}
	memcpy(&SERP_ARGS(tNode).linkarg.to.dir,
		   &SERP_FILE(pNode),
		   sizeof(SERP_FILE(pNode)));

	SERP_ARGS(tNode).linkarg.to.name = (filename)name;

	if ( nfscall(tNode->nfs->server,
					  NFSPROC_LINK,
					  (xdrproc_t)xdr_linkargs,	&SERP_FILE(tNode),
					  (xdrproc_t)xdr_nfsstat,	&status)
	     || (NFS_OK != (errno = status))
	   ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_link");
#endif
		return -1;
	}

	return 0;

}

static int nfs_do_unlink(
	rtems_filesystem_location_info_t  *loc,       /* IN */
	int								  proc
)
{
nfsstat			status;
NfsNode			node  = loc->node_access;
Nfs				nfs   = node->nfs;
#if DEBUG & DEBUG_SYSCALLS
char			*name = NFSPROC_REMOVE == proc ?
							"nfs_unlink" : "nfs_rmdir";
#endif

	/* The FS generics have determined that pathloc is _not_
	 * a directory. Hence we may assume that the parent
	 * is in our NFS.
	 */

#if DEBUG & DEBUG_SYSCALLS
	assert( node->args.name == node->str && node->str );

	fprintf(stderr,"%s '%s'\n", name, node->args.name);
#endif

	if ( nfscall(nfs->server,
				 proc,
				 (xdrproc_t)xdr_diropargs,	&node->args,
				 (xdrproc_t)xdr_nfsstat,	&status)
	     || (NFS_OK != (errno = status))
	    ) {
#if DEBUG & DEBUG_SYSCALLS
		perror(name);
#endif
		return -1;
	}

	return 0;
}

static int nfs_unlink(
	rtems_filesystem_location_info_t  *loc       /* IN */
)
{
	return nfs_do_unlink(loc, NFSPROC_REMOVE);
}

static int nfs_chown(
	rtems_filesystem_location_info_t  *pathloc,       /* IN */
	uid_t                              owner,         /* IN */
	gid_t                              group          /* IN */
)
{
sattr	arg;

	arg.uid = owner;
	arg.gid = group;

	return nfs_sattr(pathloc->node_access, &arg, SATTR_UID | SATTR_GID);
	
}

/* Cleanup the FS private info attached to pathloc->node_access */
static int nfs_freenode(
	rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
Nfs	nfs    = ((NfsNode)pathloc->node_access)->nfs;

#if DEBUG & DEBUG_COUNT_NODES
	/* print counts at entry where they are > 0 so 'nfs' is safe from being destroyed 
	 * and there's no race condition
	 */
	fprintf(stderr,
			"entering freenode, in use count is %i nodes, %i strings\n",
			nfs->nodesInUse,
			nfs->stringsInUse);
#endif

	/* never destroy the root node; it is released by the unmount
	 * code
	 */
	if (locIsRoot(pathloc)) {
		unsigned long flags;
		/* just adjust the references to the root node but
		 * don't really release it
		 */
		rtems_interrupt_disable(flags);
			nfs->nodesInUse--;
		rtems_interrupt_enable(flags);
	} else {
		nfsNodeDestroy(pathloc->node_access);
		pathloc->node_access = 0;
	}
	return 0;
}

/* NOTE/TODO: mounting on top of NFS is not currently supported
 *
 * Challenge: stateless protocol. It would be possible to
 * delete mount points on the server. We would need some sort
 * of a 'garbage collector' looking for dead/unreachable
 * mount points and unmounting them.
 * Also, the path evaluation routine would have to check
 * for crossing mount points. Crossing over from one NFS
 * into another NFS could probably handled iteratively
 * rather than by recursion.
 */

#ifdef DECLARE_BODY
/* This routine is called when they try to mount something
 * on top of THIS filesystem, i.e. if one of our directories
 * is used as a mount point
 */
static int nfs_mount(
	rtems_filesystem_mount_table_entry_t *mt_entry     /* in */
)DECLARE_BODY
#else
#define nfs_mount 0
#endif

#ifdef DECLARE_BODY
/* This op is called when they try to unmount a FS
 * from a mountpoint managed by THIS FS.
 */
static int nfs_unmount(
	rtems_filesystem_mount_table_entry_t *mt_entry     /* in */
)DECLARE_BODY
#else
#define nfs_unmount 0
#endif

#if 0

/* for reference (libio.h) */

struct rtems_filesystem_mount_table_entry_tt {
  Chain_Node                             Node;
  rtems_filesystem_location_info_t       mt_point_node;
  rtems_filesystem_location_info_t       mt_fs_root;
  int                                    options;
  void                                  *fs_info;

  rtems_filesystem_limits_and_options_t  pathconf_limits_and_options;

  /*
   *  When someone adds a mounted filesystem on a real device,
   *  this will need to be used.
   *
   *  The best option long term for this is probably an open file descriptor.
   */
  char                                  *dev;
};
#endif


/* This op is called as the last step of mounting this FS */
STATIC int nfs_fsmount_me(
	rtems_filesystem_mount_table_entry_t *mt_entry
)
{
char				*host;
struct sockaddr_in	saddr;
enum clnt_stat		stat;
fhstatus			fhstat;
u_long				uid,gid;
#ifdef NFS_V2_PORT
int					retry;
#endif
Nfs					nfs       = 0;
NfsNode				rootNode  = 0;
RpcUdpServer		nfsServer = 0;
int					e         = -1;
char				*path     = mt_entry->dev;


	if ( buildIpAddr(&uid, &gid, &host, &saddr, &path) )
		return -1;


#ifdef NFS_V2_PORT
	/* if the portmapper fails, retry a fixed port */
	for (retry = 1, saddr.sin_port = 0, stat = RPC_FAILED;
		 retry >= 0 && stat;
		 stat && (saddr.sin_port = htons(NFS_V2_PORT)), retry-- )
#endif
		stat = rpcUdpServerCreate(
					&saddr,
					NFS_PROGRAM,
					NFS_VERSION_2,
					uid,
					gid,
					&nfsServer
					);

	if ( RPC_SUCCESS != stat ) {
		fprintf(stderr,
				"Unable to contact NFS server - invalid port? (%s)\n",
				clnt_sperrno(stat));
		e = EPROTONOSUPPORT;
		goto cleanup;
	}


	/* first, try to ping the NFS server by
	 * calling the NULL proc.
	 */
	if ( nfscall(nfsServer,
					 NFSPROC_NULL,
					 (xdrproc_t)xdr_void, 0,
					 (xdrproc_t)xdr_void, 0) ) {

		fputs("NFS Ping ",stderr);
		fwrite(host, 1, path-host-1, stderr);
		fprintf(stderr," failed: %s\n", strerror(errno));

		e = errno ? errno : EIO;
		goto cleanup;
	}

	/* that seemed to work - we now try the
	 * actual mount
	 */

	/* reuse server address but let the mntcall()
	 * search for the mountd's port
	 */
	saddr.sin_port = 0;

	stat = mntcall( &saddr,
					MOUNTPROC_MNT,
					(xdrproc_t)xdr_dirpath,
					&path,
					(xdrproc_t)xdr_fhstatus,
					&fhstat,
				 	uid,
				 	gid );

	if (stat) {
		fprintf(stderr,"MOUNT -- %s\n",clnt_sperrno(stat));
		if ( e<=0 )
			e = EIO;
		goto cleanup;
	} else if (NFS_OK != (e=fhstat.fhs_status)) {
		fprintf(stderr,"MOUNT: %s\n",strerror(e));
		goto cleanup;
	}

	assert( nfs = nfsCreate(nfsServer) );
	nfsServer = 0;

	nfs->uid  = uid;
	nfs->gid  = gid;

	/* that seemed to work - we now create the root node
	 * and we also must obtain the root node attributes
	 */
	assert( rootNode = nfsNodeCreate(nfs, &fhstat.fhstatus_u.fhs_fhandle) );

	if ( updateAttr(rootNode, 1 /* force */) ) {
		e = errno;
		goto cleanup;
	}

	/* looks good so far */

	mt_entry->mt_fs_root.node_access = rootNode;

	rootNode = 0;

	mt_entry->mt_fs_root.ops		 = &nfs_fs_ops;
	mt_entry->mt_fs_root.handlers	 = &nfs_dir_file_handlers;
	mt_entry->pathconf_limits_and_options = nfs_limits_and_options;

	LOCK(nfsGlob.llock);
		nfsGlob.num_mounted_fs++;
		/* allocate a new ID for this FS */
		nfs->id = nfsGlob.fs_ids++;
	UNLOCK(nfsGlob.llock);

	mt_entry->fs_info				 = nfs;
	nfs->mt_entry					 = mt_entry;
	nfs = 0;

	e = 0;

cleanup:
	if (nfs)
		nfsDestroy(nfs);
	if (nfsServer)
		rpcUdpServerDestroy(nfsServer);
	if (rootNode)
		nfsNodeDestroy(rootNode);
	if (e)
		rtems_set_errno_and_return_minus_one(e);
	else
		return 0;
}

/* This op is called when they try to unmount THIS fs */
STATIC int nfs_fsunmount_me(
	rtems_filesystem_mount_table_entry_t *mt_entry    /* in */
)
{
enum clnt_stat		stat;
struct sockaddr_in	saddr;
char				*path = mt_entry->dev;
int					nodesInUse;
u_long				uid,gid;

LOCK(nfsGlob.llock);
	nodesInUse = ((Nfs)mt_entry->fs_info)->nodesInUse;

	if (nodesInUse > 1 /* one ref to the root node used by us */) {
		UNLOCK(nfsGlob.llock);
		fprintf(stderr,
				"Refuse to unmount; there are still %i nodes in use (1 used by us)\n",
				nodesInUse);
		rtems_set_errno_and_return_minus_one(EBUSY);
	}

	assert( 0 == buildIpAddr(&uid, &gid, 0, &saddr, &path) );
	
	stat = mntcall( &saddr,
					MOUNTPROC_UMNT,
					(xdrproc_t)xdr_dirpath, &path,
					(xdrproc_t)xdr_void,	 0,
				    uid,
				    gid
				  );

	if (stat) {
		UNLOCK(nfsGlob.llock);
		fprintf(stderr,"NFS UMOUNT -- %s\n", clnt_sperrno(stat));
		errno = EIO;
		return -1;
	}

	nfsNodeDestroy(mt_entry->mt_fs_root.node_access);
	mt_entry->mt_fs_root.node_access = 0;
	
	nfsDestroy(mt_entry->fs_info);
	mt_entry->fs_info = 0;

	nfsGlob.num_mounted_fs--;
UNLOCK(nfsGlob.llock);

	return 0;
}

/* OPTIONAL; may be NULL - BUT: CAUTION; mount() doesn't check
 * for this handler to be present - a fs bug
 * //NOTE: (10/25/2002) patch submitted and probably applied
 */
static rtems_filesystem_node_types_t nfs_node_type(
	rtems_filesystem_location_info_t    *pathloc      /* in */
)
{
NfsNode node = pathloc->node_access;

	if (updateAttr(node, 0 /* only if old */))
		return -1;

	switch( SERP_ATTR(node).type ) {
		default:
			/* rtems has no value for 'unknown';
			 */
		case NFNON:
		case NFSOCK:
		case NFBAD:
		case NFFIFO:
				break;


		case NFREG: return RTEMS_FILESYSTEM_MEMORY_FILE;
		case NFDIR:	return RTEMS_FILESYSTEM_DIRECTORY;

		case NFBLK:
		case NFCHR:	return RTEMS_FILESYSTEM_DEVICE;

		case NFLNK: return RTEMS_FILESYSTEM_SYM_LINK;
	}
	return -1;
}

static int nfs_mknod(
	const char                        *path,       /* IN */
	mode_t                             mode,       /* IN */
	dev_t                              dev,        /* IN */
	rtems_filesystem_location_info_t  *pathloc     /* IN/OUT */
)
{
rtems_clock_time_value	now;
diropres				res;
NfsNode					node = pathloc->node_access;
mode_t					type = S_IFMT & mode;

	if (type != S_IFDIR && type != S_IFREG)
		rtems_set_errno_and_return_minus_one(ENOTSUP);

#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,"nfs_mknod: creating %s\n", path);
#endif

	rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE, &now);

	SERP_ARGS(node).createarg.name       		= (filename)path;
	SERP_ARGS(node).createarg.attributes.mode	= mode;
	/* TODO: either use our uid or use the Nfs credentials */
	SERP_ARGS(node).createarg.attributes.uid	= 0;
	SERP_ARGS(node).createarg.attributes.gid	= 0;
	SERP_ARGS(node).createarg.attributes.size	= 0;
	SERP_ARGS(node).createarg.attributes.atime.seconds	= now.seconds;
	SERP_ARGS(node).createarg.attributes.atime.useconds	= now.microseconds;
	SERP_ARGS(node).createarg.attributes.mtime.seconds	= now.seconds;
	SERP_ARGS(node).createarg.attributes.mtime.useconds	= now.microseconds;

	if ( nfscall( node->nfs->server,
						NFSPROC_CREATE,
						(xdrproc_t)xdr_createargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_diropres,	&res)
		|| (NFS_OK != (errno = res.status)) ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_mknod");
#endif
		return -1;
	}

	return 0;
}

static int nfs_utime(
	rtems_filesystem_location_info_t  *pathloc,       /* IN */
	time_t                             actime,        /* IN */
	time_t                             modtime        /* IN */
)
{
sattr	arg;

	/* TODO: add rtems EPOCH - UNIX EPOCH seconds */
	arg.atime.seconds  = actime;
	arg.atime.useconds = 0;
	arg.mtime.seconds  = modtime;
	arg.mtime.useconds = 0;

	return nfs_sattr(pathloc->node_access, &arg, SATTR_ATIME | SATTR_MTIME);
}

static int nfs_symlink(
	rtems_filesystem_location_info_t  *loc,         /* IN */
	const char                        *link_name,   /* IN */
	const char                        *node_name
)
{
rtems_clock_time_value	now;
nfsstat					status;
NfsNode					node = loc->node_access;


#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,"nfs_symlink: creating %s -> %s\n", link_name, node_name);
#endif

	rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE, &now);

	SERP_ARGS(node).symlinkarg.name       		= (filename)link_name;
	SERP_ARGS(node).symlinkarg.to				= (nfspath) node_name;

	SERP_ARGS(node).symlinkarg.attributes.mode	= S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO;
	/* TODO */
	SERP_ARGS(node).symlinkarg.attributes.uid	= 0;
	SERP_ARGS(node).symlinkarg.attributes.gid	= 0;
	SERP_ARGS(node).symlinkarg.attributes.size	= 0;
	SERP_ARGS(node).symlinkarg.attributes.atime.seconds	 = now.seconds;
	SERP_ARGS(node).symlinkarg.attributes.atime.useconds = now.microseconds;
	SERP_ARGS(node).symlinkarg.attributes.mtime.seconds	 = now.seconds;
	SERP_ARGS(node).symlinkarg.attributes.mtime.useconds = now.microseconds;

	if ( nfscall( node->nfs->server,
						NFSPROC_SYMLINK,
						(xdrproc_t)xdr_symlinkargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_nfsstat,		&status)
		|| (NFS_OK != (errno = status)) ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_symlink");
#endif
		return -1;
	}

	return 0;
}

static int nfs_do_readlink(
	rtems_filesystem_location_info_t  *loc,     	/* IN  */       
	strbuf							  *psbuf		/* IN/OUT */
)
{
NfsNode				node = loc->node_access;
Nfs					nfs  = node->nfs;
readlinkres_strbuf	rr;
int					wasAlloced;
int					rval;

	rr.strbuf  = *psbuf;

	wasAlloced = (0 == psbuf->buf);

	if ( (rval = nfscall(nfs->server,
							NFSPROC_READLINK,
							(xdrproc_t)xdr_nfs_fh,      		&SERP_FILE(node),
							(xdrproc_t)xdr_readlinkres_strbuf, &rr)) ) {
		if (wasAlloced)
			xdr_free( (xdrproc_t)xdr_strbuf, (caddr_t)&rr.strbuf );
	}


	if (NFS_OK != rr.status) {
		if (wasAlloced)
			xdr_free( (xdrproc_t)xdr_strbuf, (caddr_t)&rr.strbuf );
		rtems_set_errno_and_return_minus_one(rr.status);
	}

	*psbuf = rr.strbuf;

	return 0;
}

static int nfs_readlink(
	rtems_filesystem_location_info_t  *loc,     	/* IN  */       
	char							  *buf,			/* OUT */
	size_t							  len
)
{
strbuf sbuf;
	sbuf.buf = buf;
	sbuf.max = len;

	return nfs_do_readlink(loc, &sbuf);
}

/* The semantics of this routine are:
 *
 * The caller submits a valid pathloc, i.e. it has
 * an NfsNode attached to node_access.
 * On return, pathloc points to the target node which
 * may or may not be an NFS node.
 * Hence, the original NFS node is released in either
 * case:
 *   - link evaluation fails; pathloc points to no valid node
 *   - link evaluation success; pathloc points to a new valid
 *     node. If it's an NFS node, a new NfsNode will be attached
 *     to node_access...
 */

#define LINKVAL_BUFLEN				(MAXPATHLEN+1)
#define RVAL_ERR_BUT_DONT_FREENODE	(-1)
#define RVAL_ERR_AND_DO_FREENODE	( 1)
#define RVAL_OK						( 0)

static int nfs_eval_link(
	rtems_filesystem_location_info_t *pathloc,     /* IN/OUT */
	int                              flags         /* IN     */
)
{
rtems_filesystem_node_types_t	type;
char							*buf = malloc(LINKVAL_BUFLEN);
int	 							rval = RVAL_ERR_AND_DO_FREENODE;

	if (!buf) {
		errno = ENOMEM;
		goto cleanup;
	}

	/* in this loop, we must not use NFS specific ops as we might
	 * step out of our FS during the process...
	 * This algorithm should actually be performed by the
	 * generic's evaluate_path routine :-(
	 *
	 * Unfortunately, there is no way of finding the
	 * directory node who contains 'pathloc', however :-(
	 */
	do {
		/* assume the generics have verified 'pathloc' to be
		 * a link...
		 */
		if ( !pathloc->ops->readlink_h ) {
			errno = ENOTSUP;
			goto cleanup;
		}

		if ( pathloc->ops->readlink_h(pathloc, buf, LINKVAL_BUFLEN) ) {
			goto cleanup;
		}

#if DEBUG & DEBUG_EVALPATH
		fprintf(stderr, "link value is '%s'\n", buf);
#endif

		/* is the link value an absolute path ? */
		if ( DELIM != *buf ) {
			/* NO; a relative path */

			/* we must backup to the link's directory - we
			 * know only how to do that for NFS, however.
			 * In this special case, we can avoid recursion.
			 * Otherwise (i.e. if the link is on another FS),
			 * we must step into its eval_link_h().
			 */
			if (locIsNfs(pathloc)) {
				NfsNode	node = pathloc->node_access;
				int		err;

				memcpy( &SERP_FILE(node),
						&node->args.dir,
						sizeof(node->args.dir) );

				if (updateAttr(node, 1 /* force */))
					goto cleanup;

				if (SERP_ATTR(node).type != NFDIR) {
					errno = ENOTDIR;
					goto cleanup;
				}

				pathloc->handlers = &nfs_dir_file_handlers;

				err = nfs_evalpath(buf, flags, pathloc);

				/* according to its semantics,
				 * nfs_evalpath cloned the node attached
				 * to pathloc. Hence we have to
				 * release the old one (referring to
				 * the link; the new clone has been
				 * updated and refers to the link _value_).
				 */
				nfsNodeDestroy(node);

				if (err) {
					/* nfs_evalpath has set errno;
					 * pathloc->node_access has no
					 * valid node attached in this case
					 */
					rval = RVAL_ERR_BUT_DONT_FREENODE;
					goto cleanup;
				}

			} else {
				if ( ! pathloc->ops->eval_link_h ) {
					errno = ENOTSUP;
					goto cleanup;
				}
				if (!pathloc->ops->eval_link_h(pathloc, flags)) {
					/* FS is responsible for freeing its pathloc->node_access
					 * if necessary
					 */
					rval = RVAL_ERR_BUT_DONT_FREENODE;
					goto cleanup;
				}
			}
		} else {
			/* link points to an absolute path '/xxx' */
				
			/* release this node; filesystem_evaluate_path() will
			 * lookup a new one.
			 */
			rtems_filesystem_freenode(pathloc);

			if (rtems_filesystem_evaluate_path(buf, flags, pathloc, 1)) {
				goto cleanup;
			}
		}

		if ( !pathloc->ops->node_type_h ) {
			errno = ENOTSUP;
			goto cleanup;
		}

		type = pathloc->ops->node_type_h(pathloc);


		/* I dont know what to do about hard links */
	} while ( RTEMS_FILESYSTEM_SYM_LINK == type );

	rval = RVAL_OK;

cleanup:

	free(buf);

	if (RVAL_ERR_AND_DO_FREENODE == rval) {
		rtems_filesystem_freenode(pathloc);
		return -1;
	}

	return rval;
}


struct _rtems_filesystem_operations_table nfs_fs_ops = {
		nfs_evalpath,		/* MANDATORY */
		nfs_evalformake,	/* MANDATORY; may set errno=ENOSYS and return -1 */
		nfs_link,			/* OPTIONAL; may be NULL */
		nfs_unlink,			/* OPTIONAL; may be NULL */
		nfs_node_type,		/* OPTIONAL; may be NULL; BUG in mount - no test!! */
		nfs_mknod,			/* OPTIONAL; may be NULL */
		nfs_chown,			/* OPTIONAL; may be NULL */
		nfs_freenode,		/* OPTIONAL; may be NULL; (release node_access) */
		nfs_mount,			/* OPTIONAL; may be NULL */
		nfs_fsmount_me,		/* OPTIONAL; may be NULL -- but this makes NO SENSE */
		nfs_unmount,		/* OPTIONAL; may be NULL */
		nfs_fsunmount_me,	/* OPTIONAL; may be NULL */
		nfs_utime,			/* OPTIONAL; may be NULL */
		nfs_eval_link,		/* OPTIONAL; may be NULL */
		nfs_symlink,		/* OPTIONAL; may be NULL */
		nfs_readlink,		/* OPTIONAL; may be NULL */
};

/*****************************************
	File Handlers

	NOTE: the FS generics expect a FS'
	      evalpath_h() to switch the
		  pathloc->handlers according
		  to the pathloc/node's file
		  type.
		  We currently have 'file' and
		  'directory' handlers and very
		  few 'symlink' handlers.

		  The handlers for each type are
		  implemented or #defined ZERO
		  in a 'nfs_file_xxx',
		  'nfs_dir_xxx', 'nfs_link_xxx'
		  sequence below this point.

		  In some cases, a common handler,
		  can be used for all file types.
		  It is then simply called
		  'nfs_xxx'.
 *****************************************/


#if 0
/* from rtems/libio.h for convenience */
struct rtems_libio_tt {
		rtems_driver_name_t              *driver;
		off_t                             size;      /* size of file */
		off_t                             offset;    /* current offset into file */
		uint32_t                          flags;
		rtems_filesystem_location_info_t  pathinfo;
		Objects_Id                        sem;
		uint32_t                          data0;     /* private to "driver" */
		void                             *data1;     /* ... */
		void                             *file_info; /* used by file handlers */
		rtems_filesystem_file_handlers_r *handlers;  /* type specific handlers */
};
#endif

/* stateless NFS protocol makes this trivial */
static int nfs_file_open(
	rtems_libio_t *iop,
	const char    *pathname,
	uint32_t      flag,
	uint32_t      mode
)
{
	iop->file_info = 0;
	return 0;
}

/* reading directories is not stateless; we must
 * remember the last 'read' position, i.e.
 * the server 'cookie'. We do manage this information
 * attached to the iop->file_info.
 */
static int nfs_dir_open(
	rtems_libio_t *iop,
	const char    *pathname,
	uint32_t      flag,
	uint32_t      mode
)
{
NfsNode		node = iop->pathinfo.node_access;
DirInfo		di;

	/* create a readdirargs object and copy the file handle;
	 * attach to the file_info.
	 */

	di = (DirInfo) malloc(sizeof(*di));
	iop->file_info = di;

	if ( !di  ) {
		errno = ENOMEM;
		return -1;
	}

	memcpy( &di->readdirargs.dir,
			&SERP_FILE(node),
			sizeof(di->readdirargs.dir) );

	/* rewind cookie */
	memset( &di->readdirargs.cookie,
	        0,
	        sizeof(di->readdirargs.cookie) );

	di->eofreached = FALSE;

	return 0;
}

#define nfs_link_open 0

static int nfs_file_close(
	rtems_libio_t *iop
)
{
	return 0;
}

static int nfs_dir_close(
	rtems_libio_t *iop
)
{
	free(iop->file_info);
	iop->file_info = 0;
	return 0;
}

#define nfs_link_close 0

static ssize_t nfs_file_read(
	rtems_libio_t *iop,
	void          *buffer,
	size_t        count
)
{
readres	rr;
NfsNode node = iop->pathinfo.node_access;
Nfs		nfs  = node->nfs;

	if (count > NFS_MAXDATA)
		count = NFS_MAXDATA;

	SERP_ARGS(node).readarg.offset		= iop->offset;
	SERP_ARGS(node).readarg.count	  	= count;
	SERP_ARGS(node).readarg.totalcount	= 0xdeadbeef;

	rr.readres_u.reply.data.data_val	= buffer;

	if ( nfscall(	nfs->server,
						NFSPROC_READ,
						(xdrproc_t)xdr_readargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_readres,	&rr) ) {
		return -1;
	}


	if (NFS_OK != rr.status) {
		rtems_set_errno_and_return_minus_one(rr.status);
	}

#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,
			"Read %i (asked for %i) bytes from offset %i to 0x%08x\n",
			rr.readres_u.reply.data.data_len,
			count,
			iop->offset,
			rr.readres_u.reply.data.data_val);
#endif


	return rr.readres_u.reply.data.data_len;
}

/* this is called by readdir() / getdents() */
static ssize_t nfs_dir_read(
	rtems_libio_t *iop,
	void          *buffer,
	size_t        count
)
{
DirInfo			di     = iop->file_info;
RpcUdpServer	server = ((Nfs)iop->pathinfo.mt_entry->fs_info)->server;

	if ( di->eofreached )
		return 0;

	di->ptr = di->buf = buffer;

	/* align + round down the buffer */
	count &= ~ (DIRENT_HEADER_SIZE - 1);
	di->len = count;

#if 0
	/* now estimate the number of entries we should ask for */
	count /= DIRENT_HEADER_SIZE + CONFIG_AVG_NAMLEN;

	/* estimate the encoded size that might take up */
	count *= dirres_entry_size + CONFIG_AVG_NAMLEN;
#else
	/* integer arithmetics are better done the other way round */
	count *= dirres_entry_size + CONFIG_AVG_NAMLEN;
	count /= DIRENT_HEADER_SIZE + CONFIG_AVG_NAMLEN;
#endif

	if (count > NFS_MAXDATA)
		count = NFS_MAXDATA;

	di->readdirargs.count = count;

#if DEBUG & DEBUG_READDIR
	fprintf(stderr,
			"Readdir: asking for %i XDR bytes, buffer is %i\n",
			count, di->len);
#endif

	if ( nfscall(
					server,
					NFSPROC_READDIR,
					(xdrproc_t)xdr_readdirargs, &di->readdirargs,
					(xdrproc_t)xdr_dir_info,    di) ) {
		return -1;
	}


	if (NFS_OK != di->status) {
		rtems_set_errno_and_return_minus_one(di->status);
	}

	return (char*)di->ptr - (char*)buffer;
}

#define nfs_link_read 0

static ssize_t nfs_file_write(
	rtems_libio_t *iop,
	const void    *buffer,
	size_t        count
)
{
NfsNode 	node = iop->pathinfo.node_access;
Nfs			nfs  = node->nfs;
int			e;

	if (count > NFS_MAXDATA)
		count = NFS_MAXDATA;


	SERP_ARGS(node).writearg.beginoffset   = 0xdeadbeef;
	if ( LIBIO_FLAGS_APPEND & iop->flags ) {
		if ( updateAttr(node, 0) ) {
			return -1;
		}
		SERP_ARGS(node).writearg.offset	  	   = SERP_ATTR(node).size;
	} else {
		SERP_ARGS(node).writearg.offset	  	   = iop->offset;
	}
	SERP_ARGS(node).writearg.totalcount	   = 0xdeadbeef;
	SERP_ARGS(node).writearg.data.data_len = count;
	SERP_ARGS(node).writearg.data.data_val = (void*)buffer;

	/* write XDR buffer size will be chosen by nfscall based
	 * on the PROC specifier
	 */

	if ( nfscall(	nfs->server,
						NFSPROC_WRITE,
						(xdrproc_t)xdr_writeargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_attrstat,	&node->serporid) ) {
		return -1;
	}


	if (NFS_OK != (e=node->serporid.status) ) {
		/* try at least to recover the current attributes */
		updateAttr(node, 1 /* force */);
		rtems_set_errno_and_return_minus_one(e);
	}

	node->age = nowSeconds();

	return count;
}

#define nfs_dir_write  0
#define nfs_link_write 0

/* IOCTL is unneeded/unsupported */
#ifdef DECLARE_BODY
static int nfs_file_ioctl(
	rtems_libio_t *iop,
	uint32_t      command,
	void          *buffer
)DECLARE_BODY
#else
#define nfs_file_ioctl 0
#define nfs_dir_ioctl 0
#define nfs_link_ioctl 0
#endif

static int nfs_file_lseek(
	rtems_libio_t *iop,
	off_t          length,
	int            whence
)
{
#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,
			"lseek to %i (length %i, whence %i)\n",
			iop->offset,
			length,
			whence);
#endif
	if ( SEEK_END == whence ) {
		/* rtems (4.6.2) libcsupport code 'lseek' uses iop->size to
		 * compute the offset. We don't want to track the file size
	 	 * by updating 'iop->size' constantly.
		 * Since lseek is the only place using iop->size, we work
		 * around this by tweaking the offset here...
		 */
		NfsNode	node = iop->pathinfo.node_access;
		fattr	*fa  = &SERP_ATTR(node);

		if (updateAttr(node, 0 /* only if old */)) {
			return -1;
		}
		iop->offset = fa->size;
	}

	/* this is particularly easy :-) */
	return iop->offset;
}

static int nfs_dir_lseek(
	rtems_libio_t *iop,
	off_t          length,
	int            whence
)
{
DirInfo di = iop->file_info;

	/* we don't support anything other than
	 * rewinding
	 */
	if (SEEK_SET != whence || 0 != length) {
		errno = ENOTSUP;
		return -1;
	}

	/* rewind cookie */
	memset( &di->readdirargs.cookie,
	        0,
	        sizeof(di->readdirargs.cookie) );

	di->eofreached = FALSE;

	return iop->offset;
}

#define nfs_link_lseek 0

#if 0	/* structure types for reference */
struct fattr {
		ftype type;
		u_int mode;
		u_int nlink;
		u_int uid;
		u_int gid;
		u_int size;
		u_int blocksize;
		u_int rdev;
		u_int blocks;
		u_int fsid;
		u_int fileid;
		nfstime atime;
		nfstime mtime;
		nfstime ctime;
};

struct  stat
{
		dev_t     st_dev;
		ino_t     st_ino;
		mode_t    st_mode;
		nlink_t   st_nlink;
		uid_t     st_uid;
		gid_t     st_gid;
		dev_t     st_rdev;
		off_t     st_size;
		/* SysV/sco doesn't have the rest... But Solaris, eabi does.  */
#if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
		time_t    st_atime;
		time_t    st_mtime;
		time_t    st_ctime;
#else
		time_t    st_atime;
		long      st_spare1;
		time_t    st_mtime;
		long      st_spare2;
		time_t    st_ctime;
		long      st_spare3;
		long      st_blksize;
		long      st_blocks;
		long  st_spare4[2];
#endif
};
#endif

/* common for file/dir/link */
static int nfs_fstat(
	rtems_filesystem_location_info_t *loc,
	struct stat                      *buf
)
{
NfsNode	node = loc->node_access;
fattr	*fa  = &SERP_ATTR(node);

	if (updateAttr(node, 0 /* only if old */)) {
		return -1;
	}

/* done by caller 
	memset(buf, 0, sizeof(*buf));
 */

	/* translate */

	/* one of the branches hopefully is optimized away */
	if (sizeof(ino_t) < sizeof(u_int)) {
	buf->st_dev		= NFS_MAKE_DEV_T_INO_HACK((NfsNode)loc->node_access);
	} else {
	buf->st_dev		= NFS_MAKE_DEV_T((NfsNode)loc->node_access);
	}
	buf->st_mode	= fa->mode;
	buf->st_nlink	= fa->nlink;
	buf->st_uid		= fa->uid;
	buf->st_gid		= fa->gid;
	buf->st_size	= fa->size;
	/* Set to "preferred size" of this NFS client implementation */
	buf->st_blksize	= nfsStBlksize ? nfsStBlksize : fa->blocksize;
	buf->st_rdev	= fa->rdev;
	buf->st_blocks	= fa->blocks;
	buf->st_ino     = fa->fileid;
	buf->st_atime	= fa->atime.seconds;
	buf->st_mtime	= fa->mtime.seconds;
	buf->st_ctime	= fa->ctime.seconds;

#if 0 /* NFS should return the modes */
	switch(fa->type) {
		default:
		case NFNON:
		case NFBAD:
				break;

		case NFSOCK: buf->st_mode |= S_IFSOCK; break;
		case NFFIFO: buf->st_mode |= S_IFIFO;  break;
		case NFREG : buf->st_mode |= S_IFREG;  break;
		case NFDIR : buf->st_mode |= S_IFDIR;  break;
		case NFBLK : buf->st_mode |= S_IFBLK;  break;
		case NFCHR : buf->st_mode |= S_IFCHR;  break;
		case NFLNK : buf->st_mode |= S_IFLNK;  break;
	}
#endif

	return 0;
}

/* a helper which does the real work for
 * a couple of handlers (such as chmod,
 * ftruncate or utime)
 */
static int
nfs_sattr(NfsNode node, sattr *arg, u_long mask)
{

rtems_clock_time_value	now;
nfstime					nfsnow, t;
int						e;
u_int					mode;

	if (updateAttr(node, 0 /* only if old */))
		return -1;

	rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE, &now);

	/* TODO: add rtems EPOCH - UNIX EPOCH seconds */
	nfsnow.seconds  = now.seconds;
	nfsnow.useconds = now.microseconds;

	/* merge permission bits into existing type bits */
	mode = SERP_ATTR(node).mode;
	if (mask & SATTR_MODE) {
		mode &= S_IFMT;
		mode |= arg->mode & ~S_IFMT;
	} else {
		mode = -1;
	}
	SERP_ARGS(node).sattrarg.attributes.mode  = mode;

	SERP_ARGS(node).sattrarg.attributes.uid	  =
		(mask & SATTR_UID)  ? arg->uid : -1;

	SERP_ARGS(node).sattrarg.attributes.gid	  =
		(mask & SATTR_GID)  ? arg->gid : -1;

	SERP_ARGS(node).sattrarg.attributes.size  =
		(mask & SATTR_SIZE) ? arg->size : -1;

	if (mask & SATTR_ATIME)
		t = arg->atime;
	else if (mask & SATTR_TOUCHA)
		t = nfsnow;
	else
		t.seconds = t.useconds = -1;
	SERP_ARGS(node).sattrarg.attributes.atime = t;

	if (mask & SATTR_ATIME)
		t = arg->mtime;
	else if (mask & SATTR_TOUCHA)
		t = nfsnow;
	else
		t.seconds = t.useconds = -1;
	SERP_ARGS(node).sattrarg.attributes.mtime = t;

	node->serporid.status = NFS_OK;

	if ( nfscall( node->nfs->server,
						NFSPROC_SETATTR,
						(xdrproc_t)xdr_sattrargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_attrstat,	&node->serporid) ) {
#if DEBUG & DEBUG_SYSCALLS
		fprintf(stderr,
				"nfs_sattr (mask 0x%08x): %s",
				mask,
				strerror(errno));
#endif
		return -1;
	}

	if (NFS_OK != (e=node->serporid.status) ) {
#if DEBUG & DEBUG_SYSCALLS
		fprintf(stderr,"nfs_sattr: %s\n",strerror(e));
#endif
		/* try at least to recover the current attributes */
		updateAttr(node, 1 /* force */);
		rtems_set_errno_and_return_minus_one(e);
	}

	node->age = nowSeconds();

	return 0;
}


/* common for file/dir/link */
static int nfs_fchmod(
	rtems_filesystem_location_info_t *loc,
	mode_t                            mode
)
{
sattr	arg;

	arg.mode = mode;
	return nfs_sattr(loc->node_access, &arg, SATTR_MODE);
	
}

/* just set the size attribute to 'length'
 * the server will take care of the rest :-)
 */
static int nfs_file_ftruncate(
	rtems_libio_t *iop,
	off_t          length
)
{
sattr					arg;

	arg.size = length;
	/* must not modify any other attribute; if we are not the owner
	 * of the file or directory but only have write access changing
	 * any attribute besides 'size' will fail...
	 */
	return nfs_sattr(iop->pathinfo.node_access,
					 &arg,
					 SATTR_SIZE);
}

#define nfs_dir_ftruncate 0
#define nfs_link_ftruncate 0

/* not implemented */
#ifdef DECLARE_BODY
static int nfs_file_fpathconf(
	rtems_libio_t *iop,
	int name
)DECLARE_BODY
#else
#define nfs_file_fpathconf 0
#define nfs_dir_fpathconf 0
#define nfs_link_fpathconf 0
#endif

/* unused */
#ifdef DECLARE_BODY
static int nfs_file_fsync(
	rtems_libio_t *iop
)DECLARE_BODY
#else
#define nfs_file_fsync 0
#define nfs_dir_fsync 0
#define nfs_link_fsync 0
#endif

/* unused */
#ifdef DECLARE_BODY
static int nfs_file_fdatasync(
	rtems_libio_t *iop
)DECLARE_BODY
#else
#define nfs_file_fdatasync 0
#define nfs_dir_fdatasync 0
#define nfs_link_fdatasync 0
#endif

/* unused */
#ifdef DECLARE_BODY
static int nfs_file_fcntl(
	int            cmd,
	rtems_libio_t *iop
)DECLARE_BODY
#else
#define nfs_file_fcntl 0
#define nfs_dir_fcntl 0
#define nfs_link_fcntl 0
#endif

/* files and symlinks are removed
 * by the common nfs_unlink() routine.
 * NFS has a different NFSPROC_RMDIR
 * call, though...
 */
static int nfs_dir_rmnod(
	rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
	return nfs_do_unlink(pathloc, NFSPROC_RMDIR);
}

/* the file handlers table */
static
struct _rtems_filesystem_file_handlers_r nfs_file_file_handlers = {
		nfs_file_open,			/* OPTIONAL; may be NULL */
		nfs_file_close,			/* OPTIONAL; may be NULL */
		nfs_file_read,			/* OPTIONAL; may be NULL */
		nfs_file_write,			/* OPTIONAL; may be NULL */
		nfs_file_ioctl,			/* OPTIONAL; may be NULL */
		nfs_file_lseek,			/* OPTIONAL; may be NULL */
		nfs_fstat,				/* OPTIONAL; may be NULL */
		nfs_fchmod,				/* OPTIONAL; may be NULL */
		nfs_file_ftruncate,		/* OPTIONAL; may be NULL */
		nfs_file_fpathconf,		/* OPTIONAL; may be NULL - UNUSED */
		nfs_file_fsync,			/* OPTIONAL; may be NULL */
		nfs_file_fdatasync,		/* OPTIONAL; may be NULL */
		nfs_file_fcntl,			/* OPTIONAL; may be NULL */
		nfs_unlink,				/* OPTIONAL; may be NULL */
};

/* the directory handlers table */
static
struct _rtems_filesystem_file_handlers_r nfs_dir_file_handlers = {
		nfs_dir_open,			/* OPTIONAL; may be NULL */
		nfs_dir_close,			/* OPTIONAL; may be NULL */
		nfs_dir_read,			/* OPTIONAL; may be NULL */
		nfs_dir_write,			/* OPTIONAL; may be NULL */
		nfs_dir_ioctl,			/* OPTIONAL; may be NULL */
		nfs_dir_lseek,			/* OPTIONAL; may be NULL */
		nfs_fstat,				/* OPTIONAL; may be NULL */
		nfs_fchmod,				/* OPTIONAL; may be NULL */
		nfs_dir_ftruncate,		/* OPTIONAL; may be NULL */
		nfs_dir_fpathconf,		/* OPTIONAL; may be NULL - UNUSED */
		nfs_dir_fsync,			/* OPTIONAL; may be NULL */
		nfs_dir_fdatasync,		/* OPTIONAL; may be NULL */
		nfs_dir_fcntl,			/* OPTIONAL; may be NULL */
		nfs_dir_rmnod,				/* OPTIONAL; may be NULL */
};

/* the link handlers table */
static
struct _rtems_filesystem_file_handlers_r nfs_link_file_handlers = {
		nfs_link_open,			/* OPTIONAL; may be NULL */
		nfs_link_close,			/* OPTIONAL; may be NULL */
		nfs_link_read,			/* OPTIONAL; may be NULL */
		nfs_link_write,			/* OPTIONAL; may be NULL */
		nfs_link_ioctl,			/* OPTIONAL; may be NULL */
		nfs_link_lseek,			/* OPTIONAL; may be NULL */
		nfs_fstat,				/* OPTIONAL; may be NULL */
		nfs_fchmod,				/* OPTIONAL; may be NULL */
		nfs_link_ftruncate,		/* OPTIONAL; may be NULL */
		nfs_link_fpathconf,		/* OPTIONAL; may be NULL - UNUSED */
		nfs_link_fsync,			/* OPTIONAL; may be NULL */
		nfs_link_fdatasync,		/* OPTIONAL; may be NULL */
		nfs_link_fcntl,			/* OPTIONAL; may be NULL */
		nfs_unlink,				/* OPTIONAL; may be NULL */
};

/* we need a dummy driver entry table to get a
 * major number from the system
 */
static
rtems_device_driver nfs_initialize(
		rtems_device_major_number	major,
		rtems_device_minor_number	minor,
		void						*arg
)
{
	/* we don't really use this routine because
     * we cannot supply an argument (contrary
     * to what the 'arg' parameter suggests - it
     * is always set to 0 by the generics :-()
     * and because we don't want the user to
     * have to deal with the major number (which
     * OTOH is something WE are interested in. The
     * only reason for using this API was getting
     * a major number, after all).
     *
	 * Something must be present, however, to 
	 * reserve a slot in the driver table.
	 */
	return RTEMS_SUCCESSFUL;
}

static rtems_driver_address_table	drvNfs = {
		nfs_initialize,
		0,					/* open    */
		0,					/* close   */
		0,					/* read    */
		0,					/* write   */
		0					/* control */
};

/* Dump a list of the currently mounted NFS to a  file */
int
nfsMountsShow(FILE *f)
{
char	*mntpt = 0;
Nfs		nfs;

	if (!f)
		f = stdout;

	if ( !(mntpt=malloc(MAXPATHLEN)) ) {
		fprintf(stderr,"nfsMountsShow(): no memory\n");
		return -1;
	}
	
	fprintf(f,"Currently Mounted NFS:\n");

	LOCK(nfsGlob.llock);

	for (nfs = nfsGlob.mounted_fs; nfs; nfs=nfs->next) {
		fprintf(f,"%s on ", nfs->mt_entry->dev);
		if (rtems_filesystem_resolve_location(mntpt, MAXPATHLEN, &nfs->mt_entry->mt_fs_root))
			fprintf(f,"<UNABLE TO LOOKUP MOUNTPOINT>\n");
		else
			fprintf(f,"%s\n",mntpt);
	}

	UNLOCK(nfsGlob.llock);

	free(mntpt);
	return 0;
}

/* convenience wrapper
 *
 * NOTE: this routine calls NON-REENTRANT
 *       gethostbyname() if the host is
 *       not in 'dot' notation.
 */
int
nfsMount(char *uidhost, char *path, char *mntpoint)
{
rtems_filesystem_mount_table_entry_t	*mtab;
struct stat								st;
int										devl;
char									*host;
int										rval = -1;
char									*dev =  0;

	if (!uidhost || !path || !mntpoint) {
		fprintf(stderr,"usage: nfsMount(""[uid.gid@]host"",""path"",""mountpoint"")\n");
		nfsMountsShow(stderr);
		return -1;
	}

	if ( !(dev = malloc((devl=strlen(uidhost) + 20 + strlen(path)+1))) ) {
		fprintf(stderr,"nfsMount: out of memory\n");
		return -1;
	}

	/* Try to create the mount point if nonexistent */
	if (stat(mntpoint, &st)) {
		if (ENOENT != errno) {
			perror("nfsMount trying to create mount point - stat failed");
			goto cleanup;
		} else if (mkdir(mntpoint,0777)) {
			perror("nfsMount trying to create mount point");
			goto cleanup;
		}
	}

	if ( !(host=strchr(uidhost,UIDSEP)) ) {
		host = uidhost;
	} else {
		host++;
	}

	if (isdigit(*host)) {
		/* avoid using gethostbyname */
		sprintf(dev,"%s:%s",uidhost,path);
	} else {
		struct hostent *h;

		/* copy the uid part (hostname will be
		 * overwritten)
		 */
		strcpy(dev, uidhost);

		/* NOTE NOTE NOTE: gethostbyname is NOT
		 * thread safe. This is UGLY
		 */

/* BEGIN OF NON-THREAD SAFE REGION */

		h = gethostbyname(host);

		if ( !h ||
			 !inet_ntop( AF_INET,
					     (struct in_addr*)h->h_addr_list[0],
						 dev  + (host - uidhost),
						 devl - (host - uidhost) )
			) {
			fprintf(stderr,"nfsMount: host '%s' not found\n",host);
			goto cleanup;
		}

/* END OF NON-THREAD SAFE REGION */

		/* append ':<path>' */
		strcat(dev,":");
		strcat(dev,path);
	}

	printf("Trying to mount %s on %s\n",dev,mntpoint);

	if (mount(&mtab,
			  &nfs_fs_ops,
			  RTEMS_FILESYSTEM_READ_WRITE,
			  dev,
			  mntpoint)) {
		perror("nfsMount - mount");
		goto cleanup;
	}

	rval = 0;

cleanup:
	free(dev);
	return rval;
}

/* HERE COMES A REALLY UGLY HACK */

/* This is stupid; it is _very_ hard to find the path
 * leading to a rtems_filesystem_location_info_t node :-(
 * The only easy way is making the location the current
 * directory and issue a getcwd().
 * However, since we don't want to tamper with the
 * current directory, we must create a separate
 * task to do the job for us - sigh.
 */

typedef struct ResolvePathArgRec_ {
	rtems_filesystem_location_info_t	*loc;	/* IN: location to resolve	*/
	char								*buf;	/* IN/OUT: buffer where to put the path */
	int									len;	/* IN: buffer length		*/
	rtems_id							sync;	/* IN: synchronization		*/
	rtems_status_code					status; /* OUT: result				*/
} ResolvePathArgRec, *ResolvePathArg;

static void
resolve_path(rtems_task_argument arg)
{
ResolvePathArg						rpa = (ResolvePathArg)arg;
rtems_filesystem_location_info_t	old;

	/* IMPORTANT: let the helper task have its own libio environment (i.e. cwd) */
	if (RTEMS_SUCCESSFUL == (rpa->status = rtems_libio_set_private_env())) {

		old = rtems_filesystem_current;

		rtems_filesystem_current = *(rpa->loc);

		if ( !getcwd(rpa->buf, rpa->len) )
			rpa->status = RTEMS_UNSATISFIED;

		/* must restore the cwd because 'freenode' will be called on it */
		rtems_filesystem_current = old;
	}
	rtems_semaphore_release(rpa->sync);
	rtems_task_delete(RTEMS_SELF);
}


/* a utility routine to find the path leading to a
 * rtems_filesystem_location_info_t node
 *
 * INPUT: 'loc' and a buffer 'buf' (length 'len') to hold the
 *        path.
 * OUTPUT: path copied into 'buf'
 *
 * RETURNS: 0 on success, RTEMS error code on error.
 */
rtems_status_code
rtems_filesystem_resolve_location(char *buf, int len, rtems_filesystem_location_info_t *loc)
{
ResolvePathArgRec	arg;
rtems_id			tid = 0;
rtems_task_priority	pri;
rtems_status_code	status;

	arg.loc  = loc;
	arg.buf  = buf;
	arg.len  = len;
	arg.sync = 0;

	status = rtems_semaphore_create(
					rtems_build_name('r','e','s','s'),
					0,
					RTEMS_SIMPLE_BINARY_SEMAPHORE,
					0,
					&arg.sync);

	if (RTEMS_SUCCESSFUL != status)
		goto cleanup;

	rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &pri);

	status = rtems_task_create(
					rtems_build_name('r','e','s','s'),
					pri,
					RTEMS_MINIMUM_STACK_SIZE + 50000,
					RTEMS_DEFAULT_MODES,
					RTEMS_DEFAULT_ATTRIBUTES,
					&tid);

	if (RTEMS_SUCCESSFUL != status)
		goto cleanup;

	status = rtems_task_start(tid, resolve_path, (rtems_task_argument)&arg);

	if (RTEMS_SUCCESSFUL != status) {
		rtems_task_delete(tid);
		goto cleanup;
	}


	/* synchronize with the helper task */
	rtems_semaphore_obtain(arg.sync, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

	status = arg.status;

cleanup:
	if (arg.sync)
		rtems_semaphore_delete(arg.sync);

	return status;
}

int
nfsSetTimeout(uint32_t timeout_ms)
{
rtems_interrupt_level k;
uint32_t	          s,us;

	if ( timeout_ms > 100000 ) {
		/* out of range */
		return -1;
	}

	s  = timeout_ms/1000;
	us = (timeout_ms % 1000) * 1000;

	rtems_interrupt_disable(k);
	_nfscalltimeout.tv_sec  = s;
	_nfscalltimeout.tv_usec = us;
	rtems_interrupt_enable(k);

	return 0;
}

uint32_t
nfsGetTimeout()
{
rtems_interrupt_level k;
uint32_t              s,us;
	rtems_interrupt_disable(k);
	s  = _nfscalltimeout.tv_sec;
	us = _nfscalltimeout.tv_usec;
	rtems_interrupt_enable(k);
	return s*1000 + us/1000;
}
