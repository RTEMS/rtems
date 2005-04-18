/*
 * Copyright (c) 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mount.h	8.21 (Berkeley) 5/20/95
 * $FreeBSD: src/sys/sys/mount.h,v 1.195 2005/03/24 07:29:23 jeff Exp $
 */

/*
 * $Id$
 */
 
#ifndef _SYS_MOUNT_H_
#define _SYS_MOUNT_H_

#ifndef _KERNEL
#include <sys/ucred.h>
#endif
#include <sys/queue.h>

typedef struct fsid { int32_t val[2]; } fsid_t;	/* filesystem id type */

/*
 * File identifier.
 * These are unique per filesystem on a single machine.
 */
#define	MAXFIDSZ	16

struct fid {
	u_short		fid_len;		/* length of data in bytes */
	u_short		fid_reserved;		/* force longword alignment */
	char		fid_data[MAXFIDSZ];	/* data (variable length) */
};

/*
 * filesystem statistics
 */

#define MNAMELEN 90	/* length of buffer for returned name */

struct statfs {
	long	f_spare2;		/* placeholder */
	long	f_bsize;		/* fundamental filesystem block size */
	long	f_iosize;		/* optimal transfer block size */
	long	f_blocks;		/* total data blocks in filesystem */
	long	f_bfree;		/* free blocks in fs */
	long	f_bavail;		/* free blocks avail to non-superuser */
	long	f_files;		/* total file nodes in filesystem */
	long	f_ffree;		/* free file nodes in fs */
	fsid_t	f_fsid;			/* filesystem id */
	uid_t	f_owner;		/* user that mounted the filesystem */
	int	f_type;			/* type of filesystem (see below) */
	int	f_flags;		/* copy of mount flags */
	long	f_spare[6];		/* spare for later */
	char	f_mntonname[MNAMELEN];	/* directory on which mounted */
	char	f_mntfromname[MNAMELEN];/* mounted filesystem */
};

/*
 * Structure per mounted filesystem.  Each mounted filesystem has an
 * array of operations and an instance record.  The filesystems are
 * put on a doubly linked list.
 *
 */
LIST_HEAD(vnodelst, vnode);

struct mount {
	CIRCLEQ_ENTRY(mount) mnt_list;		/* mount list */
	struct vfsops	*mnt_op;		/* operations on fs */
	struct vfsconf	*mnt_vfc;		/* configuration info */
	struct vnode	*mnt_vnodecovered;	/* vnode we mounted on */
	struct vnodelst	mnt_vnodelist;		/* list of vnodes this mount */
	int		mnt_flag;		/* flags */
	int		mnt_maxsymlinklen;	/* max size of short symlink */
	struct statfs	mnt_stat;		/* cache of filesystem stats */
	void *		mnt_data;		/* private data */
	time_t		mnt_time;		/* last time written*/
};

/*
 * Mount flags.
 *
 * Unmount uses MNT_FORCE flag.
 */
#define	MNT_RDONLY	0x00000001	/* read only filesystem */
#define	MNT_SYNCHRONOUS	0x00000002	/* filesystem written synchronously */
#define	MNT_NOEXEC	0x00000004	/* can't exec from filesystem */
#define	MNT_NOSUID	0x00000008	/* don't honor setuid bits on fs */
#define	MNT_NODEV	0x00000010	/* don't interpret special files */
#define	MNT_UNION	0x00000020	/* union with underlying filesystem */
#define	MNT_ASYNC	0x00000040	/* filesystem written asynchronously */
#define	MNT_NOATIME	0x10000000	/* Disable update of file access times */

/*
 * exported mount flags.
 */
#define	MNT_EXRDONLY	0x00000080	/* exported read only */
#define	MNT_EXPORTED	0x00000100	/* filesystem is exported */
#define	MNT_DEFEXPORTED	0x00000200	/* exported to the world */
#define	MNT_EXPORTANON	0x00000400	/* use anon uid mapping for everyone */
#define	MNT_EXKERB	0x00000800	/* exported with Kerberos uid mapping */
#define	MNT_EXPUBLIC	0x20000000	/* public export (WebNFS) */

/*
 * Flags set by internal operations.
 */
#define	MNT_LOCAL	0x00001000	/* filesystem is stored locally */
#define	MNT_QUOTA	0x00002000	/* quotas are enabled on filesystem */
#define	MNT_ROOTFS	0x00004000	/* identifies the root filesystem */
#define	MNT_USER	0x00008000	/* mounted by a user */
#define	MNT_IGNORE	0x00800000	/* do not show entry in df */

/*
 * Mask of flags that are visible to statfs()
 */
#define	MNT_VISFLAGMASK	(MNT_RDONLY|MNT_SYNCHRONOUS|MNT_NOEXEC|MNT_NOSUID| \
			 MNT_NODEV|MNT_UNION|MNT_ASYNC|MNT_EXRDONLY|MNT_EXPORTED| \
			 MNT_DEFEXPORTED|MNT_EXPORTANON|MNT_EXKERB|MNT_LOCAL| \
			 MNT_QUOTA|MNT_ROOTFS|MNT_USER|MNT_NOATIME)

/*
 * filesystem control flags.
 *
 * MNT_MLOCK lock the mount entry so that name lookup cannot proceed
 * past the mount point.  This keeps the subtree stable during mounts
 * and unmounts.
 */
#define	MNT_UPDATE	0x00010000	/* not a real mount, just an update */
#define	MNT_DELEXPORT	0x00020000	/* delete export host lists */
#define	MNT_RELOAD	0x00040000	/* reload filesystem data */
#define	MNT_FORCE	0x00080000	/* force unmount or readonly change */
#define	MNT_MLOCK	0x00100000	/* lock so that subtree is stable */
#define	MNT_MWAIT	0x00200000	/* someone is waiting for lock */
#define MNT_MPBUSY	0x00400000	/* scan of mount point in progress */
#define MNT_MPWANT	0x00800000	/* waiting for mount point */
#define MNT_UNMOUNT	0x01000000	/* unmount in progress */
#define MNT_WANTRDWR	0x02000000	/* want upgrade to read/write */

/*
 * used to get configured filesystems information
 */
#define VFS_MAXNAMELEN 32
struct vfsconf {
	void *vfc_vfsops;
	char vfc_name[VFS_MAXNAMELEN];
	int vfc_index;
	int vfc_refcount;
	int vfc_flags;
};

/*
 * Operations supported on mounted filesystem.
 */
#ifdef _KERNEL

extern	int	doforce;	/* Flag to permit forcible unmounting. */
extern struct vfsconf void_vfsconf;
extern struct vfsconf *vfsconf[];

#ifdef __STDC__
struct nameidata;
struct mbuf;
#endif

struct vfsops {
	int	(*vfs_mount)	__P((struct mount *mp, char *path, caddr_t data,
				    struct nameidata *ndp, struct proc *p));
	int	(*vfs_start)	__P((struct mount *mp, int flags,
				    struct proc *p));
	int	(*vfs_unmount)	__P((struct mount *mp, int mntflags,
				    struct proc *p));
	int	(*vfs_root)	__P((struct mount *mp, struct vnode **vpp));
	int	(*vfs_quotactl)	__P((struct mount *mp, int cmds, uid_t uid,
				    caddr_t arg, struct proc *p));
	int	(*vfs_statfs)	__P((struct mount *mp, struct statfs *sbp,
				    struct proc *p));
	int	(*vfs_sync)	__P((struct mount *mp, int waitfor,
				    struct ucred *cred, struct proc *p));
	int	(*vfs_vget)	__P((struct mount *mp, ino_t ino,
				    struct vnode **vpp));
	int	(*vfs_fhtovp)	__P((struct mount *mp, struct fid *fhp,
				    struct mbuf *nam, struct vnode **vpp,
				    int *exflagsp, struct ucred **credanonp));
	int	(*vfs_vptofh)	__P((struct vnode *vp, struct fid *fhp));
	int	(*vfs_init)	__P((void));
};

#define VFS_MOUNT(MP, PATH, DATA, NDP, P) \
	(*(MP)->mnt_op->vfs_mount)(MP, PATH, DATA, NDP, P)
#define VFS_START(MP, FLAGS, P)	  (*(MP)->mnt_op->vfs_start)(MP, FLAGS, P)
#define VFS_UNMOUNT(MP, FORCE, P) (*(MP)->mnt_op->vfs_unmount)(MP, FORCE, P)
#define VFS_ROOT(MP, VPP)	  (*(MP)->mnt_op->vfs_root)(MP, VPP)
#define VFS_QUOTACTL(MP,C,U,A,P)  (*(MP)->mnt_op->vfs_quotactl)(MP, C, U, A, P)
#define VFS_STATFS(MP, SBP, P)	  (*(MP)->mnt_op->vfs_statfs)(MP, SBP, P)
#define VFS_SYNC(MP, WAIT, C, P)  (*(MP)->mnt_op->vfs_sync)(MP, WAIT, C, P)
#define VFS_VGET(MP, INO, VPP)	  (*(MP)->mnt_op->vfs_vget)(MP, INO, VPP)
#define VFS_FHTOVP(MP, FIDP, NAM, VPP, EXFLG, CRED) \
	(*(MP)->mnt_op->vfs_fhtovp)(MP, FIDP, NAM, VPP, EXFLG, CRED)
#define	VFS_VPTOFH(VP, FIDP)	  (*(VP)->v_mount->mnt_op->vfs_vptofh)(VP, FIDP)

#endif /* _KERNEL */

/*
 * Flags for various system call interfaces.
 *
 * waitfor flags to vfs_sync() and getfsstat()
 */
#define MNT_WAIT	1
#define MNT_NOWAIT	2

/*
 * Generic file handle
 */
struct fhandle {
	fsid_t	fh_fsid;	/* Filesystem id of mount point */
	struct	fid fh_fid;	/* Filesys specific id */
};
typedef struct fhandle	fhandle_t;

#ifdef _KERNEL
#include <net/radix.h>
#include <sys/socket.h>		/* XXX for AF_MAX */

/*
 * Network address lookup element
 */
struct netcred {
	struct	radix_node netc_rnodes[2];
	int	netc_exflags;
	struct	ucred netc_anon;
};

/*
 * Network export information
 */
struct netexport {
	struct	netcred ne_defexported;		      /* Default export */
	struct	radix_node_head *ne_rtable[AF_MAX+1]; /* Individual exports */
};
#endif /* _KERNEL */

/*
 * Export arguments for local filesystem mount calls.
 */
struct export_args {
	int	ex_flags;		/* export related flags */
	uid_t	ex_root;		/* mapping for root uid */
	struct	ucred ex_anon;		/* mapping for anonymous user */
	struct	sockaddr *ex_addr;	/* net address to which exported */
	int	ex_addrlen;		/* and the net address length */
	struct	sockaddr *ex_mask;	/* mask of valid bits in saddr */
	int	ex_masklen;		/* and the smask length */
};

#ifdef _KERNEL
extern	int (*mountroot) __P((void *));
extern	struct vfsops	*mountrootvfsops;

/*
 * exported vnode operations
 */
int	dounmount __P((struct mount *, int, struct proc *));
struct	mount *getvfs __P((fsid_t *));      /* return vfs given fsid */
void	getnewfsid __P((struct mount *, int));
int	vflush __P((struct mount *, struct vnode *, int));
int	vfs_export			    /* process mount export info */
	  __P((struct mount *, struct netexport *, struct export_args *));
struct	netcred *vfs_export_lookup	    /* lookup host in fs export list */
	  __P((struct mount *, struct netexport *, struct mbuf *));
int	vfs_lock __P((struct mount *));         /* lock a vfs */
int	vfs_mountedon __P((struct vnode *));    /* is a vfs mounted on vp */
int	vfs_mountroot __P((void *));		/* XXX goes away? */
void	vfs_msync __P((struct mount *, int));
void	vfs_unlock __P((struct mount *));       /* unlock a vfs */
void	vfs_unmountall __P((void));
int	vfs_busy __P((struct mount *));         /* mark a vfs  busy */
void	vfs_unbusy __P((struct mount *));       /* mark a vfs not busy */
extern	CIRCLEQ_HEAD(mntlist, mount) mountlist;	/* mounted filesystem list */
extern	struct vfsops *vfssw[];			/* filesystem type table */

#else /* _KERNEL */

#include <sys/cdefs.h>

__BEGIN_DECLS
int	fstatfs __P((int, struct statfs *));
int	getfh __P((const char *, fhandle_t *));
int	getfsstat __P((struct statfs *, long, int));
int	getmntinfo __P((struct statfs **, int));
int	mount __P((int, const char *, int, void *));
int	statfs __P((const char *, struct statfs *));
int	unmount __P((const char *, int));

/* C library stuff */
struct	vfsconf *getvfsbyname __P((const char *));
struct	vfsconf *getvfsbytype __P((int));
struct	vfsconf *getvfsent __P((void));
void	setvfsent __P((int));
void	endvfsent __P((void));
int	vfsisloadable __P((const char *));
int	vfsload __P((const char *));
__END_DECLS

#endif /* _KERNEL */

#endif /* !_SYS_MOUNT_H_ */
