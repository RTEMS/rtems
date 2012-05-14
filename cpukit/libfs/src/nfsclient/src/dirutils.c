/* very crude and basic fs utilities for testing the NFS */

/* Till Straumann, <strauman@slac.stanford.edu>, 10/2002 */

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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __vxworks
#include <vxWorks.h>
#endif
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h> /* PATH_MAX */

#include <inttypes.h> /* PRI* */

#if SIZEOF_MODE_T == 8
#define PRIomode_t PRIo64
#elif SIZEOF_MODE_T == 4
#define PRIomode_t PRIo32
#else
#error "unsupport size of mode_t"
#endif

#if SIZEOF_OFF_T == 8
#define PRIdoff_t PRIo64
#elif SIZEOF_OFF_T == 4
#define PRIdoff_t PRIo32
#else
#error "unsupported size of off_t"
#endif

#ifdef HAVE_CEXP
#include <cexpHelp.h>
#endif

#ifndef __vxworks
int
pwd(void)
{
char buf[PATH_MAX];

	if ( !getcwd(buf,PATH_MAX)) {
		perror("getcwd");
		return -1;
	} else {
		printf("%s\n",buf);
	}
	return 0;
}

static int
ls_r(char *path, char *chpt, char *name, struct stat *buf)
{
char *t;
	sprintf(chpt, "/%s", name);
	if (lstat(path,buf)) {
		fprintf(stderr,"stat(%s): %s\n", path, strerror(errno));
		return -1;
	}
	switch ( buf->st_mode & S_IFMT ) {
		case S_IFSOCK:
		case S_IFIFO:	t = "|"; break;

		default:
		case S_IFREG:
		case S_IFBLK:
		case S_IFCHR:
						t = "";  break;
		case S_IFDIR:
						t = "/"; break;
		case S_IFLNK:
						t = "@"; break;
	}

	printf("%10li, %10" PRIdoff_t "b, %5i.%-5i 0%04" PRIomode_t " %s%s\n",
				buf->st_ino,
				buf->st_size,
				buf->st_uid,
				buf->st_gid,
				buf->st_mode & ~S_IFMT,
				name,
				t);
	*chpt = 0;
	return 0;
}

int
ls(char *dir, char *opts)
{
struct dirent	*de;
char			path[PATH_MAX+1];
char			*chpt;
DIR				*dp  = 0;
int				rval = -1;
struct stat		buf;

	if ( !dir )
		dir = ".";

	strncpy(path, dir, PATH_MAX);
	path[PATH_MAX] = 0;
	chpt = path+strlen(path);

	if ( !(dp=opendir(dir)) ) {
		perror("opendir");
		goto cleanup;
	}

	while ( (de = readdir(dp)) ) {
		ls_r(path, chpt, de->d_name, &buf);
	}

	rval = 0;

cleanup:
	if (dp)
		closedir(dp);
	return rval;
}
#endif

#if 0
		fprintf(stderr, "usage: cp(""from"",[""to""[,""-f""]]\n");
		fprintf(stderr, "          ""to""==NULL -> stdout\n");
		fprintf(stderr, "          ""-f""       -> overwrite existing file\n");
#endif

int
cp(char *from, char *to, char *opts)
{
struct stat	st;
int			rval  = -1;
int			fd    = -1;
FILE		*fst  = 0;
FILE		*tst  = 0;
int			flags = O_CREAT | O_WRONLY | O_TRUNC | O_EXCL;

	if (from) {

	if ((fd=open(from,O_RDONLY,0)) < 0) {
		fprintf(stderr,
				"Opening %s for reading: %s\n",
				from,
				strerror(errno));
		goto cleanup;
	}

	if (fstat(fd, &st)) {
		fprintf(stderr,
				"rstat(%s): %s\n",
				from,
				strerror(errno));
		goto cleanup;
	}


	if (!S_ISREG(st.st_mode)) {
		fprintf(stderr,"Refuse to copy a non-regular file\n");
		errno = EINVAL;
		goto cleanup;
	}
	/* Now create a stream -- I experienced occasional weirdness
	 * when circumventing the streams attached to fildno(stdin)
	 * by reading/writing to the underlying fd's directly ->
	 * for now we always go through buffered I/O...
	 */
	if ( !(fst=fdopen(fd,"r")) ) {
		fprintf(stderr,
				"Opening input stream [fdopen()] failed: %s\n",
				strerror(errno));
		goto cleanup;
	}
	/* at this point, we have a stream and don't need 'fd' anymore */
	fd = -1;

	} else {
		fst			= stdin;
		st.st_mode	= 0644;
	}

	if (opts && strchr(opts,'f'))
		flags &= ~ O_EXCL;

	if (to) {
		if ( (fd=open(to,flags,st.st_mode)) < 0 ) {
			fprintf(stderr,
					"Opening %s for writing: %s\n",
					to,
					strerror(errno));
			goto cleanup;
		}
		if ( !(tst=fdopen(fd, "w")) ) {
			fprintf(stderr,
					"Opening output stream [fdopen()] failed: %s\n",
					strerror(errno));
			goto cleanup;
		}
		/* at this point we have a stream and don't need 'fd' anymore */
		fd = -1;
	} else {
		tst = stdout;
	}

	/* clear old errors */
	clearerr(fst);
	clearerr(tst);

	/* use macro versions on register vars; stdio is already buffered,
	 * there's nothing to be gained by reading/writing blocks into
	 * a secondary buffer...
	 */
	{
	register int ch;
	register FILE *f = fst;
	register FILE *t = tst;
		while ( EOF != (ch = getc(f)) && EOF != putc(ch, t) )
			/* nothing else */;
	}

	if ( ferror(fst) ) {
		fprintf(stderr,"Read error: %s\n",strerror(errno));
		goto cleanup;
	}
	if ( ferror(tst) ) {
		fprintf(stderr,"Write error: %s\n",strerror(errno));
		goto cleanup;
	}

	rval = 0;

cleanup:

	if ( fd >= 0 )
		close(fd);

	if ( fst ) {
		if ( from )
			fclose(fst);
		else
			clearerr(fst);
	}
	if ( tst ) {
		if ( to )
			fclose(tst);
		else {
			/* flush stdout */
			fflush(tst);
			clearerr(tst);
		}
	}

	return rval;
}

int
ln(char *to, char *name, char *opts)
{
	if (!to) {
		fprintf(stderr,"ln: need 'to' argument\n");
		return -1;
	}
	if (!name) {
		if ( !(name = strrchr(to,'/')) ) {
			fprintf(stderr,
					"ln: 'unable to link %s to %s\n",
					to,to);
			return -1;
		}
		name++;
	}
	if (opts || strchr(opts,'s')) {
		if (symlink(name,to)) {
			fprintf(stderr,"symlink: %s\n",strerror(errno));
			return -1;
		}
	} else {
		if (link(name,to)) {
			fprintf(stderr,"hardlink: %s\n",strerror(errno));
			return -1;
		}
	}
	return 0;
}

int
rm(char *path)
{
	return unlink(path);
}

int
cd(char *path)
{
	return chdir(path);
}

#ifdef HAVE_CEXP
static CexpHelpTabRec _cexpHelpTabDirutils[] __attribute__((unused)) = {
	HELP(
"copy a file: cp(""from"",[""to""[,""-f""]])\n\
                 from = NULL <-- stdin\n\
                 to   = NULL --> stdout\n\
                 option -f: overwrite existing file\n",
		int,
		cp, (char *from, char *to, char *options)
		),
	HELP(
"list a directory: ls([""dir""])\n",
		int,
		ls, (char *dir)
		),
	HELP(
"remove a file\n",
		int,
		rm, (char *path)
		),
	HELP(
"change the working directory\n",
		int,
		cd, (char *path)
		),
	HELP(
"create a link: ln(""to"",""name"",""[-s]""\n\
                   -s creates a symlink\n",
		int,
		ln, (char *to, char *name, char *options)
		),
	HELP("",,0,)
};
#endif
