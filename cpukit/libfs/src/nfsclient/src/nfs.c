/* NFS client implementation for RTEMS; hooks into the RTEMS filesystem */

/* Author: Till Straumann <strauman@slac.stanford.edu> 2002 */

/*
 * Hacked on by others.
 *
 * Modifications to support reference counting in the file system are
 * Copyright (c) 2012 embedded brains GmbH.
 */

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
#include "librtemsNfs.h"

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
			(((rtems_device_minor_number)((node)->nfs->id))<<16) | (((rtems_device_minor_number)SERP_ATTR((node)).fileid) >> 16) )

/* use our 'nfs id' and the server's fsid for the minor device number
 * this should be fairly unique
 */
#define	NFS_MAKE_DEV_T(node) \
		rtems_filesystem_make_dev_t( NFS_MAJOR, \
			(((rtems_device_minor_number)((node)->nfs->id))<<16) | (SERP_ATTR((node)).fsid & (((rtems_device_minor_number)1<<16)-1)) )

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

static inline char *
nfs_dupname(const char *name, size_t namelen)
{
	char *dupname = malloc(namelen + 1);

	if (dupname != NULL) {
		memcpy(dupname, name, namelen);
		dupname [namelen] = '\0';
	} else {
		errno = ENOMEM;
	}

	return dupname;
}

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
			uint32_t	offset;
			uint32_t	count;
			uint32_t	totalcount;
		}					readarg;
		struct {
			uint32_t	beginoffset;
			uint32_t	offset;
			uint32_t	totalcount;
			struct {
				uint32_t data_len;
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
			uint32_t	count;
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
  rtems_interval rval;
  rtems_clock_get_seconds_since_epoch( &rval );
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

static ssize_t nfs_readlink_with_node(
	NfsNode node,
	char *buf,
	size_t len
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

extern const struct _rtems_filesystem_operations_table nfs_fs_ops;
static const struct _rtems_filesystem_file_handlers_r nfs_file_file_handlers;
static const struct _rtems_filesystem_file_handlers_r nfs_dir_file_handlers;
static const struct _rtems_filesystem_file_handlers_r nfs_link_file_handlers;
static		   rtems_driver_address_table		 drvNfs;

int
nfsMountsShow(FILE*);

rtems_status_code
rtems_filesystem_resolve_location(char *buf, int len, rtems_filesystem_location_info_t *loc);

/*****************************************
	Global Variables
 *****************************************/

/* These are (except for MAXNAMLEN/MAXPATHLEN) copied from IMFS */

static const rtems_filesystem_limits_and_options_t
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
} nfsGlob = {0, 0,  0, 0, 0, 0};

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
				errno = ENOMEM;
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
static int initialised = 0;
entry	dummy;
rtems_status_code status;

	if (initialised)
		return;

	initialised = 1;

	fprintf(stderr,
          "RTEMS-NFS $Release$, "                       \
          "Till Straumann, Stanford/SLAC/SSRL 2002, " \
          "See LICENSE file for licensing info.\n");

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

	smallPool = rpcUdpXactPoolCreate(
		NFS_PROGRAM,
		NFS_VERSION_2,
		CONFIG_NFS_SMALL_XACT_SIZE,
		smallPoolDepth);
	assert( smallPool );

	bigPool = rpcUdpXactPoolCreate(
		NFS_PROGRAM,
		NFS_VERSION_2,
		CONFIG_NFS_BIG_XACT_SIZE,
		bigPoolDepth);
	assert( bigPool );

	status = rtems_semaphore_create(
		rtems_build_name('N','F','S','l'),
		1,
		MUTEX_ATTRIBUTES,
		0,
		&nfsGlob.llock);
	assert( status == RTEMS_SUCCESSFUL );
	status = rtems_semaphore_create(
		rtems_build_name('N','F','S','m'),
		1,
		MUTEX_ATTRIBUTES,
		0,
		&nfsGlob.lock);
	assert( status == RTEMS_SUCCESSFUL );

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
 * IP address helper.
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
struct hostent *h;
char	host[64];
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
		*puid = geteuid();
		*pgid = getegid();
		chpt  = *pPath;
	}
	if ( pHost )
		*pHost = chpt;

	/* split the device name which is in the form
	 *
	 * <host> ':' <path>
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

  /* BEGIN OF NON-THREAD SAFE REGION */

	h = gethostbyname(host);

	if ( !h ) {
		errno = EINVAL;
		return -1;
	}

	memcpy(&psa->sin_addr, h->h_addr, sizeof (struct in_addr));
  
  /* END OF NON-THREAD SAFE REGION */

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

static bool nfs_is_directory(
	rtems_filesystem_eval_path_context_t *ctx,
	void *arg
)
{
	bool is_dir = false;
	rtems_filesystem_location_info_t *currentloc =
		rtems_filesystem_eval_path_get_currentloc(ctx);
	NfsNode node = currentloc->node_access;
	int force_update = 0;

	if (updateAttr(node, force_update) == 0) {
		is_dir = SERP_ATTR(node).type == NFDIR;
	}

	return is_dir;
}

static int nfs_search_in_directory(
	Nfs nfs,
	const NfsNode dir,
	char *part,
	NfsNode entry
)
{
	int rv;

	entry->nfs = nfs;

	/* lookup one element */
	SERP_ATTR(entry) = SERP_ATTR(dir);
	SERP_FILE(entry) = SERP_FILE(dir);
	SERP_ARGS(entry).diroparg.name = part;

	/* remember args / directory fh */
	memcpy(&entry->args, &SERP_FILE(dir), sizeof(dir->args));

#if DEBUG & DEBUG_EVALPATH
	fprintf(stderr,"Looking up '%s'\n",part);
#endif

	rv = nfscall(
		nfs->server,
		NFSPROC_LOOKUP,
		(xdrproc_t) xdr_diropargs, &SERP_FILE(entry),
		(xdrproc_t) xdr_serporid,  &entry->serporid
	);

	if (rv == 0 && entry->serporid.status == NFS_OK) {
		int force_update = 1;

		rv = updateAttr(entry, force_update);
	} else {
		rv = -1;
	}

	return rv;
}

static void nfs_eval_follow_link(
	rtems_filesystem_eval_path_context_t *ctx,
	NfsNode link
)
{
	const size_t len = NFS_MAXPATHLEN + 1;
	char *buf = malloc(len);

	if (buf != NULL) {
		ssize_t rv = nfs_readlink_with_node(link, buf, len);

		if (rv >= 0) {
			rtems_filesystem_eval_path_recursive(ctx, buf, (size_t) rv);
		} else {
			rtems_filesystem_eval_path_error(ctx, 0);
		}

		free(buf);
	} else {
		rtems_filesystem_eval_path_error(ctx, ENOMEM);
	}
}

static void nfs_eval_set_handlers(
	rtems_filesystem_eval_path_context_t *ctx,
	ftype type
)
{
	rtems_filesystem_location_info_t *currentloc =
		rtems_filesystem_eval_path_get_currentloc(ctx);

	switch (type) {
		case NFDIR:
			currentloc->handlers = &nfs_dir_file_handlers;
			break;
		case NFREG:
			currentloc->handlers = &nfs_file_file_handlers;
			break;
		case NFLNK:
			currentloc->handlers = &nfs_link_file_handlers;
			break;
		default:
			currentloc->handlers = &rtems_filesystem_handlers_default;
			break;
	}
}

static int nfs_move_node(NfsNode dst, const NfsNode src)
{
	int rv = 0;

	if (dst->str != NULL) {
#if DEBUG & DEBUG_COUNT_NODES
		rtems_interrupt_level flags;
		rtems_interrupt_disable(flags);
			dst->nfs->stringsInUse--;
		rtems_interrupt_enable(flags);
#endif
		free(dst->str);
	}

	*dst = *src;
	dst->str = NULL;

	if (src->args.name != NULL) {
		dst->str = dst->args.name = strdup(src->args.name);
		if (dst->str != NULL) {
#if DEBUG & DEBUG_COUNT_NODES
			rtems_interrupt_level flags;
			rtems_interrupt_disable(flags);
				dst->nfs->stringsInUse++;
			rtems_interrupt_enable(flags);
#endif
		} else {
			rv = -1;
		}
	}

	return rv;
}

static rtems_filesystem_eval_path_generic_status nfs_eval_part(
	rtems_filesystem_eval_path_context_t *ctx,
	char *part
)
{
	rtems_filesystem_eval_path_generic_status status =
		RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
	rtems_filesystem_location_info_t *currentloc =
		rtems_filesystem_eval_path_get_currentloc(ctx);
	Nfs nfs = currentloc->mt_entry->fs_info;
	NfsNode dir = currentloc->node_access;
	NfsNodeRec entry;
	int rv = nfs_search_in_directory(nfs, dir, part, &entry);

	if (rv == 0) {
		bool terminal = !rtems_filesystem_eval_path_has_path(ctx);
		int eval_flags = rtems_filesystem_eval_path_get_flags(ctx);
		bool follow_sym_link = (eval_flags & RTEMS_FS_FOLLOW_SYM_LINK) != 0;
		ftype type = SERP_ATTR(&entry).type;

		rtems_filesystem_eval_path_clear_token(ctx);

		if (type == NFLNK && (follow_sym_link || !terminal)) {
			nfs_eval_follow_link(ctx, &entry);
		} else {
			rv = nfs_move_node(dir, &entry);
			if (rv == 0) {
				nfs_eval_set_handlers(ctx, type);
				if (!terminal) {
					status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
				}
			} else {
				rtems_filesystem_eval_path_error(ctx, ENOMEM);
			}
		}
	} else {
		status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY;
	}

	return status;
}

static rtems_filesystem_eval_path_generic_status nfs_eval_token(
	rtems_filesystem_eval_path_context_t *ctx,
	void *arg,
	const char *token,
	size_t tokenlen
)
{
	rtems_filesystem_eval_path_generic_status status =
		RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;

	if (rtems_filesystem_is_current_directory(token, tokenlen)) {
		rtems_filesystem_eval_path_clear_token(ctx);
		if (rtems_filesystem_eval_path_has_path(ctx)) {
			status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
		}
	} else {
		char *part = nfs_dupname(token, tokenlen);

		if (part != NULL) {
			status = nfs_eval_part(ctx, part);
			free(part);
		} else {
			rtems_filesystem_eval_path_error(ctx, ENOMEM);
		}
	}

	return status;
}

static const rtems_filesystem_eval_path_generic_config nfs_eval_config = {
	.is_directory = nfs_is_directory,
	.eval_token = nfs_eval_token
};

static void nfs_eval_path(rtems_filesystem_eval_path_context_t *ctx)
{
	rtems_filesystem_eval_path_generic(ctx, NULL, &nfs_eval_config);
}

/* create a hard link */

static int nfs_link(
	const rtems_filesystem_location_info_t *parentloc,
	const rtems_filesystem_location_info_t *targetloc,
	const char *name,
	size_t namelen
)
{
int rv = 0;
NfsNode pNode = parentloc->node_access;
nfsstat status;
NfsNode tNode = targetloc->node_access;
char *dupname;

	dupname = nfs_dupname(name, namelen);
	if (dupname == NULL)
		return -1;

#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,"Creating link '%s'\n",dupname);
#endif

	memcpy(&SERP_ARGS(tNode).linkarg.to.dir,
		   &SERP_FILE(pNode),
		   sizeof(SERP_FILE(pNode)));

	SERP_ARGS(tNode).linkarg.to.name = dupname;

	if ( nfscall(tNode->nfs->server,
					  NFSPROC_LINK,
					  (xdrproc_t)xdr_linkargs,	&SERP_FILE(tNode),
					  (xdrproc_t)xdr_nfsstat,	&status)
	     || (NFS_OK != (errno = status))
	   ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_link");
#endif
		rv = -1;
	}

	free(dupname);

	return rv;

}

static int nfs_do_unlink(
	const rtems_filesystem_location_info_t *parentloc,
	const rtems_filesystem_location_info_t *loc,
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

static int nfs_chown(
	const rtems_filesystem_location_info_t  *pathloc,       /* IN */
	uid_t                                    owner,         /* IN */
	gid_t                                    group          /* IN */
)
{
sattr	arg;

	arg.uid = owner;
	arg.gid = group;

	return nfs_sattr(pathloc->node_access, &arg, SATTR_UID | SATTR_GID);

}

static int nfs_clonenode(rtems_filesystem_location_info_t *loc)
{
	NfsNode	node = loc->node_access;

	LOCK(nfsGlob.lock);
	node = nfsNodeClone(node);
	UNLOCK(nfsGlob.lock);

	loc->node_access = node;

	return node != NULL ? 0 : -1;
}

/* Cleanup the FS private info attached to pathloc->node_access */
static void nfs_freenode(
	const rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
#if DEBUG & DEBUG_COUNT_NODES
Nfs	nfs    = ((NfsNode)pathloc->node_access)->nfs;

	/* print counts at entry where they are > 0 so 'nfs' is safe from being destroyed
	 * and there's no race condition
	 */
	fprintf(stderr,
			"entering freenode, in use count is %i nodes, %i strings\n",
			nfs->nodesInUse,
			nfs->stringsInUse);
#endif

	nfsNodeDestroy(pathloc->node_access);
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

int rtems_nfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
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

  if (rpcUdpInit () < 0) {
    fprintf (stderr, "error: initialising RPC\n");
    return -1;
  }
  
	nfsInit(0, 0);

#if 0
	printf("Trying to mount %s on %s\n",path,mntpoint);
#endif
  
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

	nfs = nfsCreate(nfsServer);
	assert( nfs );
	nfsServer = 0;

	nfs->uid  = uid;
	nfs->gid  = gid;

	/* that seemed to work - we now create the root node
	 * and we also must obtain the root node attributes
	 */
	rootNode = nfsNodeCreate(nfs, &fhstat.fhstatus_u.fhs_fhandle);
	assert( rootNode );

	if ( updateAttr(rootNode, 1 /* force */) ) {
		e = errno;
		goto cleanup;
	}

	/* looks good so far */

	mt_entry->mt_fs_root->location.node_access = rootNode;

	rootNode = 0;

	mt_entry->ops = &nfs_fs_ops;
	mt_entry->mt_fs_root->location.handlers	 = &nfs_dir_file_handlers;
	mt_entry->pathconf_limits_and_options = &nfs_limits_and_options;

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
STATIC void nfs_fsunmount_me(
	rtems_filesystem_mount_table_entry_t *mt_entry    /* in */
)
{
enum clnt_stat		stat;
struct sockaddr_in	saddr;
char			*path = mt_entry->dev;
int			nodesInUse;
u_long			uid,gid;
int			status;

LOCK(nfsGlob.llock);
	nodesInUse = ((Nfs)mt_entry->fs_info)->nodesInUse;

	if (nodesInUse > 1 /* one ref to the root node used by us */) {
		UNLOCK(nfsGlob.llock);
		fprintf(stderr,
				"Refuse to unmount; there are still %i nodes in use (1 used by us)\n",
				nodesInUse);
                rtems_fatal_error_occurred(0xdeadbeef);
                return;
	}

	status = buildIpAddr(&uid, &gid, 0, &saddr, &path);
	assert( !status );

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
		return;
	}

	nfsNodeDestroy(mt_entry->mt_fs_root->location.node_access);
	mt_entry->mt_fs_root->location.node_access = 0;

	nfsDestroy(mt_entry->fs_info);
	mt_entry->fs_info = 0;

	nfsGlob.num_mounted_fs--;
UNLOCK(nfsGlob.llock);
}

/* OPTIONAL; may be NULL - BUT: CAUTION; mount() doesn't check
 * for this handler to be present - a fs bug
 * //NOTE: (10/25/2002) patch submitted and probably applied
 */
static rtems_filesystem_node_types_t nfs_node_type(
  const rtems_filesystem_location_info_t *loc
)
{
NfsNode node = loc->node_access;

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
	const rtems_filesystem_location_info_t *parentloc,
	const char *name,
	size_t namelen,
	mode_t mode,
	dev_t dev
)
{

int					rv = 0;
struct timeval				now;
diropres				res;
NfsNode					node = parentloc->node_access;
Nfs					nfs  = node->nfs;
mode_t					type = S_IFMT & mode;
char					*dupname;

	if (type != S_IFDIR && type != S_IFREG)
		rtems_set_errno_and_return_minus_one(ENOTSUP);

	dupname = nfs_dupname(name, namelen);
	if (dupname == NULL)
		return -1;

#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,"nfs_mknod: creating %s\n", dupname);
#endif

        rtems_clock_get_tod_timeval(&now);

	SERP_ARGS(node).createarg.name       		= dupname;
	SERP_ARGS(node).createarg.attributes.mode	= mode;
	SERP_ARGS(node).createarg.attributes.uid	= nfs->uid;
	SERP_ARGS(node).createarg.attributes.gid	= nfs->gid;
	SERP_ARGS(node).createarg.attributes.size	= 0;
	SERP_ARGS(node).createarg.attributes.atime.seconds	= now.tv_sec;
	SERP_ARGS(node).createarg.attributes.atime.useconds	= now.tv_usec;
	SERP_ARGS(node).createarg.attributes.mtime.seconds	= now.tv_sec;
	SERP_ARGS(node).createarg.attributes.mtime.useconds	= now.tv_usec;

	if ( nfscall( nfs->server,
						(type == S_IFDIR) ? NFSPROC_MKDIR : NFSPROC_CREATE,
						(xdrproc_t)xdr_createargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_diropres,	&res)
		|| (NFS_OK != (errno = res.status)) ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_mknod");
#endif
                rv = -1;
	}

	free(dupname);

	return rv;
}

static int nfs_rmnod(
	const rtems_filesystem_location_info_t *parentloc,
	const rtems_filesystem_location_info_t *loc
)
{
	int rv = 0;
	NfsNode	node  = loc->node_access;
	int force_update = 0;

	if (updateAttr(node, force_update) == 0) {
		int proc = SERP_ATTR(node).type == NFDIR
			? NFSPROC_RMDIR
				: NFSPROC_REMOVE;

		rv = nfs_do_unlink(parentloc, loc, proc);
	} else {
		rv = -1;
	}

	return rv;
}

static int nfs_utime(
	const rtems_filesystem_location_info_t  *pathloc, /* IN */
	time_t                                   actime,  /* IN */
	time_t                                   modtime  /* IN */
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
	const rtems_filesystem_location_info_t *parentloc,
	const char *name,
	size_t namelen,
	const char *target
)
{
int					rv = 0;
struct timeval				now;
nfsstat					status;
NfsNode					node = parentloc->node_access;
Nfs					nfs  = node->nfs;
char					*dupname;

	dupname = nfs_dupname(name, namelen);
	if (dupname == NULL)
		return -1;

#if DEBUG & DEBUG_SYSCALLS
	fprintf(stderr,"nfs_symlink: creating %s -> %s\n", dupname, target);
#endif

	rtems_clock_get_tod_timeval(&now);

	SERP_ARGS(node).symlinkarg.name       		= dupname;
	SERP_ARGS(node).symlinkarg.to				= (nfspath) target;

	SERP_ARGS(node).symlinkarg.attributes.mode	= S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO;
	SERP_ARGS(node).symlinkarg.attributes.uid	= nfs->uid;
	SERP_ARGS(node).symlinkarg.attributes.gid	= nfs->gid;
	SERP_ARGS(node).symlinkarg.attributes.size	= 0;
	SERP_ARGS(node).symlinkarg.attributes.atime.seconds  = now.tv_sec;
	SERP_ARGS(node).symlinkarg.attributes.atime.useconds = now.tv_usec;
	SERP_ARGS(node).symlinkarg.attributes.mtime.seconds  = now.tv_sec;
	SERP_ARGS(node).symlinkarg.attributes.mtime.useconds = now.tv_usec;

	if ( nfscall( nfs->server,
						NFSPROC_SYMLINK,
						(xdrproc_t)xdr_symlinkargs,	&SERP_FILE(node),
						(xdrproc_t)xdr_nfsstat,		&status)
		|| (NFS_OK != (errno = status)) ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_symlink");
#endif
                rv = -1;
	}

	free(dupname);

	return rv;
}

static ssize_t nfs_readlink_with_node(
	NfsNode node,
	char *buf,
	size_t len
)
{
	Nfs nfs = node->nfs;
	readlinkres_strbuf rr;

	rr.strbuf.buf = buf;
	rr.strbuf.max = len - 1;

	if ( nfscall(nfs->server,
							NFSPROC_READLINK,
							(xdrproc_t)xdr_nfs_fh,      		&SERP_FILE(node),
							(xdrproc_t)xdr_readlinkres_strbuf, &rr)
		|| (NFS_OK != (errno = rr.status)) ) {
#if DEBUG & DEBUG_SYSCALLS
		perror("nfs_readlink_with_node");
#endif
		return -1;
	}

	return (ssize_t) strlen(rr.strbuf.buf);
}

static ssize_t nfs_readlink(
	const rtems_filesystem_location_info_t *loc,
	char *buf,
	size_t len
)
{
	NfsNode	node = loc->node_access;

	return nfs_readlink_with_node(node, buf, len);
}

static int nfs_rename(
	const rtems_filesystem_location_info_t *oldparentloc,
	const rtems_filesystem_location_info_t *oldloc,
	const rtems_filesystem_location_info_t *newparentloc,
	const char *name,
	size_t namelen
)
{
	int rv = 0;
	char *dupname = nfs_dupname(name, namelen);

	if (dupname != NULL) {
		NfsNode oldParentNode = oldparentloc->node_access;
		NfsNode oldNode = oldloc->node_access;
		NfsNode newParentNode = newparentloc->node_access;
		Nfs nfs = oldParentNode->nfs;
		const nfs_fh *toDirSrc = &SERP_FILE(newParentNode);
		nfs_fh *toDirDst = &SERP_ARGS(oldParentNode).renamearg.to.dir;
		nfsstat	status;

		SERP_ARGS(oldParentNode).renamearg.name = oldNode->str;
		SERP_ARGS(oldParentNode).renamearg.to.name = dupname;
		memcpy(toDirDst, toDirSrc, sizeof(*toDirDst));

		rv = nfscall(
			nfs->server,
			NFSPROC_RENAME,
			(xdrproc_t) xdr_renameargs,
			&SERP_FILE(oldParentNode),
			(xdrproc_t) xdr_nfsstat,
			&status
		);
		if (rv == 0 && (errno = status) != NFS_OK) {
			rv = -1;
		}

		free(dupname);
	} else {
		rv = -1;
	}

	return rv;
}

static void nfs_lock(const rtems_filesystem_mount_table_entry_t *mt_entry)
{
}

static void nfs_unlock(const rtems_filesystem_mount_table_entry_t *mt_entry)
{
}

static bool nfs_are_nodes_equal(
	const rtems_filesystem_location_info_t *a,
	const rtems_filesystem_location_info_t *b
)
{
	bool equal = false;
	NfsNode na = a->node_access;

	if (updateAttr(na, 0) == 0) {
		NfsNode nb = b->node_access;

		if (updateAttr(nb, 0) == 0) {
			equal = SERP_ATTR(na).fileid == SERP_ATTR(nb).fileid
				&& SERP_ATTR(na).fsid == SERP_ATTR(nb).fsid;
		}
	}

	return equal;
}

static int nfs_fchmod(
	const rtems_filesystem_location_info_t *loc,
	mode_t mode
)
{
sattr	arg;

	arg.mode = mode;
	return nfs_sattr(loc->node_access, &arg, SATTR_MODE);

}

const struct _rtems_filesystem_operations_table nfs_fs_ops = {
	.lock_h         = nfs_lock,
	.unlock_h       = nfs_unlock,
	.eval_path_h    = nfs_eval_path,
	.link_h         = nfs_link,
	.are_nodes_equal_h = nfs_are_nodes_equal,
	.node_type_h    = nfs_node_type,
	.mknod_h        = nfs_mknod,
	.rmnod_h        = nfs_rmnod,
	.fchmod_h       = nfs_fchmod,
	.chown_h        = nfs_chown,
	.clonenod_h     = nfs_clonenode,
	.freenod_h      = nfs_freenode,
	.mount_h        = rtems_filesystem_default_mount,
	.fsmount_me_h   = rtems_nfs_initialize,
	.unmount_h      = rtems_filesystem_default_unmount,
	.fsunmount_me_h = nfs_fsunmount_me,
	.utime_h        = nfs_utime,
	.symlink_h      = nfs_symlink,
	.readlink_h     = nfs_readlink,
	.rename_h       = nfs_rename,
	.statvfs_h      = rtems_filesystem_default_statvfs
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

/* stateless NFS protocol makes this trivial */
static int nfs_file_open(
	rtems_libio_t *iop,
	const char    *pathname,
	int           oflag,
	mode_t        mode
)
{
	return 0;
}

/* reading directories is not stateless; we must
 * remember the last 'read' position, i.e.
 * the server 'cookie'. We do manage this information
 * attached to the pathinfo.node_access_2.
 */
static int nfs_dir_open(
	rtems_libio_t *iop,
	const char    *pathname,
	int           oflag,
	mode_t        mode
)
{
NfsNode		node = iop->pathinfo.node_access;
DirInfo		di;

	/* create a readdirargs object and copy the file handle;
	 * attach to the pathinfo.node_access_2
	 */

	di = (DirInfo) malloc(sizeof(*di));
	iop->pathinfo.node_access_2 = di;

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
	free(iop->pathinfo.node_access_2);
	iop->pathinfo.node_access_2 = 0;
	return 0;
}

static ssize_t nfs_file_read_chunk(
	NfsNode node,
	uint32_t offset,
	void *buffer,
	size_t count
)
{
readres	rr;
Nfs		nfs  = node->nfs;

	SERP_ARGS(node).readarg.offset		= offset;
	SERP_ARGS(node).readarg.count	  	= count;
	SERP_ARGS(node).readarg.totalcount	= UINT32_C(0xdeadbeef);

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

static ssize_t nfs_file_read(
	rtems_libio_t *iop,
	void *buffer,
	size_t count
)
{
	ssize_t rv = 0;
	NfsNode node = iop->pathinfo.node_access;
	uint32_t offset = iop->offset;
	char *in = buffer;

	do {
		size_t chunk = count <= NFS_MAXDATA ? count : NFS_MAXDATA;
		ssize_t done = nfs_file_read_chunk(node, offset, in, chunk);

		if (done > 0) {
			offset += (uint32_t) done;
			in += done;
			count -= (size_t) done;
			rv += done;
		} else {
			count = 0;
			if (done < 0) {
				rv = -1;
			}
		}
	} while (count > 0);

	if (rv > 0) {
		iop->offset = offset;
	}

	return rv;
}

/* this is called by readdir() / getdents() */
static ssize_t nfs_dir_read(
	rtems_libio_t *iop,
	void          *buffer,
	size_t        count
)
{
DirInfo			di     = iop->pathinfo.node_access_2;
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


	SERP_ARGS(node).writearg.beginoffset   = UINT32_C(0xdeadbeef);
	if ( LIBIO_FLAGS_APPEND & iop->flags ) {
		if ( updateAttr(node, 0) ) {
			return -1;
		}
		SERP_ARGS(node).writearg.offset	  	   = SERP_ATTR(node).size;
	} else {
		SERP_ARGS(node).writearg.offset	  	   = iop->offset;
	}
	SERP_ARGS(node).writearg.totalcount	   = UINT32_C(0xdeadbeef);
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

	iop->offset += count;

	return count;
}

static off_t nfs_dir_lseek(
	rtems_libio_t *iop,
	off_t          length,
	int            whence
)
{
	off_t rv = rtems_filesystem_default_lseek_directory(iop, length, whence);

	if (rv == 0) {
		DirInfo di = iop->pathinfo.node_access_2;
		nfscookie *cookie = &di->readdirargs.cookie;

		di->eofreached = FALSE;

		/* rewind cookie */
		memset(cookie, 0, sizeof(*cookie));
	}

	return rv;
}

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
		dev_t         st_dev;
		ino_t         st_ino;
		mode_t        st_mode;
		nlink_t       st_nlink;
		uid_t         st_uid;
		gid_t         st_gid;
		dev_t         st_rdev;
		off_t         st_size;
		/* SysV/sco doesn't have the rest... But Solaris, eabi does.  */
#if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
		time_t        st_atime;
		time_t        st_mtime;
		time_t        st_ctime;
#else
		time_t        st_atime;
		long          st_spare1;
		time_t        st_mtime;
		long          st_spare2;
		time_t        st_ctime;
		long          st_spare3;
		long          st_blksize;
		long          st_blocks;
		long      st_spare4[2];
#endif
};
#endif

/* common for file/dir/link */
static int nfs_fstat(
	const rtems_filesystem_location_info_t *loc,
	struct stat *buf
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

struct timeval				now;
nfstime					nfsnow, t;
int						e;
u_int					mode;

	if (updateAttr(node, 0 /* only if old */))
		return -1;

        rtems_clock_get_tod_timeval(&now);

	/* TODO: add rtems EPOCH - UNIX EPOCH seconds */
	nfsnow.seconds  = now.tv_sec;
	nfsnow.useconds = now.tv_usec;

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

/* the file handlers table */
static const
struct _rtems_filesystem_file_handlers_r nfs_file_file_handlers = {
	.open_h      = nfs_file_open,
	.close_h     = nfs_file_close,
	.read_h      = nfs_file_read,
	.write_h     = nfs_file_write,
	.ioctl_h     = rtems_filesystem_default_ioctl,
	.lseek_h     = rtems_filesystem_default_lseek_file,
	.fstat_h     = nfs_fstat,
	.ftruncate_h = nfs_file_ftruncate,
	.fsync_h     = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h     = rtems_filesystem_default_fcntl
};

/* the directory handlers table */
static const
struct _rtems_filesystem_file_handlers_r nfs_dir_file_handlers = {
	.open_h      = nfs_dir_open,
	.close_h     = nfs_dir_close,
	.read_h      = nfs_dir_read,
	.write_h     = rtems_filesystem_default_write,
	.ioctl_h     = rtems_filesystem_default_ioctl,
	.lseek_h     = nfs_dir_lseek,
	.fstat_h     = nfs_fstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate_directory,
	.fsync_h     = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h     = rtems_filesystem_default_fcntl
};

/* the link handlers table */
static const
struct _rtems_filesystem_file_handlers_r nfs_link_file_handlers = {
	.open_h      = rtems_filesystem_default_open,
	.close_h     = rtems_filesystem_default_close,
	.read_h      = rtems_filesystem_default_read,
	.write_h     = rtems_filesystem_default_write,
	.ioctl_h     = rtems_filesystem_default_ioctl,
	.lseek_h     = rtems_filesystem_default_lseek,
	.fstat_h     = nfs_fstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate,
	.fsync_h     = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h     = rtems_filesystem_default_fcntl
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
		if (rtems_filesystem_resolve_location(mntpt, MAXPATHLEN, &nfs->mt_entry->mt_fs_root->location))
			fprintf(f,"<UNABLE TO LOOKUP MOUNTPOINT>\n");
		else
			fprintf(f,"%s\n",mntpt);
	}

	UNLOCK(nfsGlob.llock);

	free(mntpt);
	return 0;
}

#if 0
CCJ_REMOVE_MOUNT
/* convenience wrapper
 *
 * NOTE: this routine calls NON-REENTRANT
 *       gethostbyname() if the host is
 *       not in 'dot' notation.
 */
int
nfsMount(char *uidhost, char *path, char *mntpoint)
{
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

	if (isdigit((unsigned char)*host)) {
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

	if (mount(dev,
			  mntpoint,
			  "nfs",
 			  RTEMS_FILESYSTEM_READ_WRITE,
 			  NULL)) {
		perror("nfsMount - mount");
		goto cleanup;
	}

	rval = 0;

cleanup:
	free(dev);
	return rval;
}
#endif

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

		old = rtems_filesystem_current->location;

		rtems_filesystem_current->location = *(rpa->loc);

		if ( !getcwd(rpa->buf, rpa->len) )
			rpa->status = RTEMS_UNSATISFIED;

		/* must restore the cwd because 'freenode' will be called on it */
		rtems_filesystem_current->location = old;
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
nfsGetTimeout( void )
{
rtems_interrupt_level k;
uint32_t              s,us;
	rtems_interrupt_disable(k);
	s  = _nfscalltimeout.tv_sec;
	us = _nfscalltimeout.tv_usec;
	rtems_interrupt_enable(k);
	return s*1000 + us/1000;
}
