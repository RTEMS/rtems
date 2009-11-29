/* $NetBSD: utils.c,v 1.29 2005/10/15 18:22:18 christos Exp $ */

/*-
 * Copyright (c) 1991, 1993, 1994
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
 * 3. Neither the name of the University nor the names of its contributors
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/cdefs.h>
#if 0
#ifndef lint
#if 0
static char sccsid[] = "@(#)utils.c	8.3 (Berkeley) 4/1/94";
#else
__RCSID("$NetBSD: utils.c,v 1.29 2005/10/15 18:22:18 christos Exp $");
#endif
#endif /* not lint */
#endif

#if 0
#include <sys/mman.h>
#endif
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "extern-cp.h"

#define lchmod  chmod
#define lchown  chown

#define cp_pct(x, y)    ((y == 0) ? 0 : (int)(100.0 * (x) / (y)))

int
set_utimes(const char *file, struct stat *fs)
{
  struct utimbuf tv;

  tv.actime = fs->st_atime;
  tv.modtime = fs->st_mtime;

  if (utime(file, &tv)) {
    warn("lutimes: %s", file);
    return (1);
  }
  return (0);
}

int
copy_file(rtems_shell_cp_globals* cp_globals __attribute__((unused)), FTSENT *entp, int dne)
{
#define MAX_READ max_read
  int max_read;
	char* buf;
	struct stat *fs;
	ssize_t wcount;
	size_t wresid;
	off_t wtotal;
	int ch, checkch, from_fd = 0, rcount, rval, to_fd = 0;
	char *bufp;
#ifdef VM_AND_BUFFER_CACHE_SYNCHRONIZED
	char *p;
#endif

	fs = entp->fts_statp;

  max_read = fs->st_blksize;
  if (max_read < (8 * 1024))
    max_read = 8 * 1024;
  buf = malloc (max_read);
  if (!buf)
  {
		warn("no memory");
    return (1);
  }

	if ((from_fd = open(entp->fts_path, O_RDONLY, 0)) == -1) {
		warn("%s", entp->fts_path);
    (void)free(buf);
		return (1);
	}

	/*
	 * If the file exists and we're interactive, verify with the user.
	 * If the file DNE, set the mode to be the from file, minus setuid
	 * bits, modified by the umask; arguably wrong, but it makes copying
	 * executables work right and it's been that way forever.  (The
	 * other choice is 666 or'ed with the execute bits on the from file
	 * modified by the umask.)
	 */
	if (!dne) {
#define YESNO "(y/n [n]) "
		if (nflag) {
			if (vflag)
				printf("%s not overwritten\n", to.p_path);
			(void)close(from_fd);
      (void)free(buf);
			return (0);
		} else if (iflag) {
			(void)fprintf(stderr, "overwrite %s? %s",
					to.p_path, YESNO);
			checkch = ch = getchar();
			while (ch != '\n' && ch != EOF)
				ch = getchar();
			if (checkch != 'y' && checkch != 'Y') {
				(void)close(from_fd);
        (void)free(buf);
				(void)fprintf(stderr, "not overwritten\n");
				return (1);
			}
		}

		if (fflag) {
		    /* remove existing destination file name,
		     * create a new file  */
		    (void)unlink(to.p_path);
				if (!lflag)
		    	to_fd = open(to.p_path, O_WRONLY | O_TRUNC | O_CREAT,
				  fs->st_mode & ~(S_ISUID | S_ISGID));
		} else {
				if (!lflag)
		    	/* overwrite existing destination file name */
		    	to_fd = open(to.p_path, O_WRONLY | O_TRUNC, 0);
		}
	} else {
		if (!lflag)
			to_fd = open(to.p_path, O_WRONLY | O_TRUNC | O_CREAT,
		  fs->st_mode & ~(S_ISUID | S_ISGID));
	}

	if (to_fd == -1) {
		warn("%s", to.p_path);
		(void)close(from_fd);
    (void)free(buf);
		return (1);
	}

	rval = 0;

	if (!lflag) {
		/*
		 * Mmap and write if less than 8M (the limit is so we don't totally
		 * trash memory on big files.  This is really a minor hack, but it
		 * wins some CPU back.
		 */
#ifdef CCJ_REMOVED_VM_AND_BUFFER_CACHE_SYNCHRONIZED
		if (S_ISREG(fs->st_mode) && fs->st_size > 0 &&
	    	fs->st_size <= 8 * 1048576) {
			if ((p = mmap(NULL, (size_t)fs->st_size, PROT_READ,
		    	MAP_SHARED, from_fd, (off_t)0)) == MAP_FAILED) {
				warn("%s", entp->fts_path);
				rval = 1;
			} else {
				wtotal = 0;
				for (bufp = p, wresid = fs->st_size; ;
			    	bufp += wcount, wresid -= (size_t)wcount) {
					wcount = write(to_fd, bufp, wresid);
					if (wcount <= 0)
						break;
					wtotal += wcount;
					if (info) {
						info = 0;
						(void)fprintf(stderr,
						    "%s -> %s %3d%%\n",
						    entp->fts_path, to.p_path,
						    cp_pct(wtotal, fs->st_size));
					}
					if (wcount >= (ssize_t)wresid)
						break;
				}
				if (wcount != (ssize_t)wresid) {
					warn("%s", to.p_path);
					rval = 1;
				}
				/* Some systems don't unmap on close(2). */
				if (munmap(p, fs->st_size) < 0) {
					warn("%s", entp->fts_path);
					rval = 1;
				}
			}
		} else
#endif
		{
			wtotal = 0;
			while ((rcount = read(from_fd, buf, MAX_READ)) > 0) {
				for (bufp = buf, wresid = rcount; ;
			    	bufp += wcount, wresid -= wcount) {
					wcount = write(to_fd, bufp, wresid);
					if (wcount <= 0)
						break;
					wtotal += wcount;
					if (info) {
						info = 0;
						(void)fprintf(stderr,
						    "%s -> %s %3d%%\n",
						    entp->fts_path, to.p_path,
						    cp_pct(wtotal, fs->st_size));
					}
					if (wcount >= (ssize_t)wresid)
						break;
				}
				if (wcount != (ssize_t)wresid) {
					warn("%s", to.p_path);
					rval = 1;
					break;
				}
			}
			if (rcount < 0) {
				warn("%s", entp->fts_path);
				rval = 1;
			}
		}
	} else {
		if (link(entp->fts_path, to.p_path)) {
			warn("%s", to.p_path);
			rval = 1;
		}
	}
	(void)close(from_fd);

	/*
	 * Don't remove the target even after an error.  The target might
	 * not be a regular file, or its attributes might be important,
	 * or its contents might be irreplaceable.  It would only be safe
	 * to remove it if we created it and its length is 0.
	 */

	if (!lflag) {
		if (pflag && setfile(cp_globals, fs, to_fd))
			rval = 1;
		if (pflag && preserve_fd_acls(from_fd, to_fd) != 0)
			rval = 1;
		(void)close(from_fd);
		if (close(to_fd)) {
			warn("%s", to.p_path);
			rval = 1;
		}
	}
  (void)free(buf);
	return (rval);
}

int
copy_link(rtems_shell_cp_globals* cp_globals, FTSENT *p, int exists)
{
	ssize_t len;
	char llink[PATH_MAX];

	if ((len = readlink(p->fts_path, llink, sizeof(llink) - 1)) == -1) {
		warn("readlink: %s", p->fts_path);
		return (1);
	}
	llink[len] = '\0';
	if (exists && unlink(to.p_path)) {
		warn("unlink: %s", to.p_path);
		return (1);
	}
	if (symlink(llink, to.p_path)) {
		warn("symlink: %s", llink);
		return (1);
	}
	return (pflag ? setfile(cp_globals, p->fts_statp, -1) : 0);
}

int
copy_fifo(rtems_shell_cp_globals* cp_globals, struct stat *from_stat, int exists)
{
	if (exists && unlink(to.p_path)) {
		warn("unlink: %s", to.p_path);
		return (1);
	}
	if (mkfifo(to.p_path, from_stat->st_mode)) {
		warn("mkfifo: %s", to.p_path);
		return (1);
	}
	return (pflag ? setfile(cp_globals, from_stat, -1) : 0);
}

int
copy_special(rtems_shell_cp_globals* cp_globals, struct stat *from_stat, int exists)
{
	if (exists && unlink(to.p_path)) {
		warn("unlink: %s", to.p_path);
		return (1);
	}
	if (mknod(to.p_path, from_stat->st_mode, from_stat->st_rdev)) {
		warn("mknod: %s", to.p_path);
		return (1);
	}
	return (pflag ? setfile(cp_globals, from_stat, -1) : 0);
}

#define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = *(ts);                                           \
        (tv)->tv_usec = 0;                                              \
}

#define st_atimespec st_atime
#define st_mtimespec st_mtime
#define lutimes utimes

int
setfile(rtems_shell_cp_globals* cp_globals, struct stat *fs, int fd)
{
	static struct timeval tv[2];
	struct stat ts;
	int rval, gotstat, islink, fdval;

	rval = 0;
	fdval = fd != -1;
	islink = !fdval && S_ISLNK(fs->st_mode);
	fs->st_mode &= S_ISUID | S_ISGID | S_ISVTX |
		       S_IRWXU | S_IRWXG | S_IRWXO;

	TIMESPEC_TO_TIMEVAL(&tv[0], &fs->st_atimespec);
	TIMESPEC_TO_TIMEVAL(&tv[1], &fs->st_mtimespec);
#if 0
	if (islink ? lutimes(to.p_path, tv) : utimes(to.p_path, tv)) {
		warn("%sutimes: %s", islink ? "l" : "", to.p_path);
		rval = 1;
	}
#endif
	if (fdval ? fstat(fd, &ts) :
	    (islink ? lstat(to.p_path, &ts) : stat(to.p_path, &ts)))
		gotstat = 0;
	else {
		gotstat = 1;
		ts.st_mode &= S_ISUID | S_ISGID | S_ISVTX |
			      S_IRWXU | S_IRWXG | S_IRWXO;
	}
	/*
	 * Changing the ownership probably won't succeed, unless we're root
	 * or POSIX_CHOWN_RESTRICTED is not set.  Set uid/gid before setting
	 * the mode; current BSD behavior is to remove all setuid bits on
	 * chown.  If chown fails, lose setuid/setgid bits.
	 */
	if (!gotstat || fs->st_uid != ts.st_uid || fs->st_gid != ts.st_gid)
		if (fdval ? fchown(fd, fs->st_uid, fs->st_gid) :
		    (islink ? lchown(to.p_path, fs->st_uid, fs->st_gid) :
		    chown(to.p_path, fs->st_uid, fs->st_gid))) {
			if (errno != EPERM) {
				warn("chown: %s", to.p_path);
				rval = 1;
			}
			fs->st_mode &= ~(S_ISUID | S_ISGID);
		}

	if (!gotstat || fs->st_mode != ts.st_mode)
		if (fdval ? fchmod(fd, fs->st_mode) :
		    (islink ? lchmod(to.p_path, fs->st_mode) :
		    chmod(to.p_path, fs->st_mode))) {
			warn("chmod: %s", to.p_path);
			rval = 1;
		}

#if 0
	if (!gotstat || fs->st_flags != ts.st_flags)
		if (fdval ?
		    fchflags(fd, fs->st_flags) :
		    (islink ? (errno = ENOSYS) :
		    chflags(to.p_path, fs->st_flags))) {
			warn("chflags: %s", to.p_path);
			rval = 1;
		}
#endif

	return (rval);
}

int
preserve_fd_acls(int source_fd __attribute__((unused)), int dest_fd __attribute__((unused)))
{
#if 0
	struct acl *aclp;
	acl_t acl;

	if (fpathconf(source_fd, _PC_ACL_EXTENDED) != 1 ||
	    fpathconf(dest_fd, _PC_ACL_EXTENDED) != 1)
		return (0);
	acl = acl_get_fd(source_fd);
	if (acl == NULL) {
		warn("failed to get acl entries while setting %s", to.p_path);
		return (1);
	}
	aclp = &acl->ats_acl;
	if (aclp->acl_cnt == 3)
		return (0);
	if (acl_set_fd(dest_fd, acl) < 0) {
		warn("failed to set acl entries for %s", to.p_path);
		return (1);
	}
#endif
	return (0);
}

int
preserve_dir_acls(struct stat *fs __attribute__((unused)), char *source_dir __attribute__((unused)), char *dest_dir __attribute__((unused)))
{
#if 0
	acl_t (*aclgetf)(const char *, acl_type_t);
	int (*aclsetf)(const char *, acl_type_t, acl_t);
	struct acl *aclp;
	acl_t acl;

	if (pathconf(source_dir, _PC_ACL_EXTENDED) != 1 ||
	    pathconf(dest_dir, _PC_ACL_EXTENDED) != 1)
		return (0);
	/*
	 * If the file is a link we will not follow it
	 */
	if (S_ISLNK(fs->st_mode)) {
		aclgetf = acl_get_link_np;
		aclsetf = acl_set_link_np;
	} else {
		aclgetf = acl_get_file;
		aclsetf = acl_set_file;
	}
	/*
	 * Even if there is no ACL_TYPE_DEFAULT entry here, a zero
	 * size ACL will be returned. So it is not safe to simply
	 * check the pointer to see if the default ACL is present.
	 */
	acl = aclgetf(source_dir, ACL_TYPE_DEFAULT);
	if (acl == NULL) {
		warn("failed to get default acl entries on %s",
		    source_dir);
		return (1);
	}
	aclp = &acl->ats_acl;
	if (aclp->acl_cnt != 0 && aclsetf(dest_dir,
	    ACL_TYPE_DEFAULT, acl) < 0) {
		warn("failed to set default acl entries on %s",
		    dest_dir);
		return (1);
	}
	acl = aclgetf(source_dir, ACL_TYPE_ACCESS);
	if (acl == NULL) {
		warn("failed to get acl entries on %s", source_dir);
		return (1);
	}
	aclp = &acl->ats_acl;
	if (aclsetf(dest_dir, ACL_TYPE_ACCESS, acl) < 0) {
		warn("failed to set acl entries on %s", dest_dir);
		return (1);
	}
#endif
	return (0);
}

void
usage(rtems_shell_cp_globals* cp_globals)
{
	(void)fprintf(stderr, "%s\n%s\n",
"usage: cp [-R [-H | -L | -P]] [-f | -i | -n] [-alpv] source_file target_file",
"       cp [-R [-H | -L | -P]] [-f | -i | -n] [-alpv] source_file ... "
"target_directory");
  longjmp (cp_globals->exit_jmp, 1);
}
