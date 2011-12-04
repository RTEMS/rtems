/*	$NetBSD: mknod.c,v 1.39 2009/02/13 01:37:23 lukem Exp $	*/

/*-
 * Copyright (c) 1998, 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#if 0
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1998\
 The NetBSD Foundation, Inc.  All rights reserved.");
__RCSID("$NetBSD: mknod.c,v 1.39 2009/02/13 01:37:23 lukem Exp $");
#endif /* not lint */
#endif

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#define __need_getopt_newlib
#include <getopt.h>

#include <sys/cdefs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#if !HAVE_NBTOOL_CONFIG_H && defined(KERN_DRIVERS)
#include <sys/sysctl.h>
#endif

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <ctype.h>

#include "mknod-pack_dev.h"

#include <setjmp.h>

typedef struct {
  int exit_code;
  jmp_buf exit_jmp;
} rtems_shell_mknod_globals;

#define exit_jump &(globals->exit_jmp)

#define exit(ec) rtems_shell_mknod_exit(globals, ec)

static int gid_name(const char *, gid_t *);
static portdev_t callPack(rtems_shell_mknod_globals* globals,
                          pack_t *, int, u_long *);

static int	main_mknod(rtems_shell_mknod_globals*, int, char *[]);
static	void	usage(rtems_shell_mknod_globals* );

static void
rtems_shell_mknod_exit (rtems_shell_mknod_globals* globals, int code)
{
  globals->exit_code = code;
  longjmp (globals->exit_jmp, 1);
}

#include "mknod-pack_dev.c"

static int
rtems_shell_main_mknod(int argc, char *argv[])
{
  rtems_shell_mknod_globals  mknod_globals;
  rtems_shell_mknod_globals* globals = &mknod_globals;
  memset (globals, 0, sizeof (mknod_globals));
  mknod_globals.exit_code = 1;
  if (setjmp (mknod_globals.exit_jmp) == 0)
    return main_mknod (globals, argc, argv);
  return mknod_globals.exit_code;
}

#define getprogname() "mknod"

#ifdef KERN_DRIVERS
#error invalid for RTEMS
static struct kinfo_drivers *kern_drivers;
static int num_drivers;

static void get_device_info(void);
static void print_device_info(char **);
static int major_from_name(const char *, mode_t);
#endif

#define	MAXARGS	3		/* 3 for bsdos, 2 for rest */

int
main_mknod(rtems_shell_mknod_globals* globals, int argc, char **argv)
{
	char	*name, *p;
	mode_t	 mode;
	portdev_t	 dev;
	pack_t	*pack;
	u_long	 numbers[MAXARGS];
	int	 n, ch, fifo, hasformat;
	int	 r_flag = 0;		/* force: delete existing entry */
#ifdef KERN_DRIVERS
	int	 l_flag = 0;		/* list device names and numbers */
	int	 major;
#endif
#if RTEMS_REMOVED
	void	*modes = 0;
#endif
	uid_t	 uid = -1;
	gid_t	 gid = -1;
	int	 rval;

	struct getopt_data getopt_reent;
	memset(&getopt_reent, 0, sizeof(getopt_data));

	dev = 0;
	fifo = hasformat = 0;
	pack = pack_native;

#ifdef KERN_DRIVERS
	while ((ch = getopt(argc, argv, "lrRF:g:m:u:")) != -1) {
#else
	while ((ch = getopt_r(argc, argv, "rRF:g:m:u:", &getopt_reent)) != -1) {
#endif
		switch (ch) {

#ifdef KERN_DRIVERS
		case 'l':
			l_flag = 1;
			break;
#endif

		case 'r':
			r_flag = 1;
			break;

		case 'R':
			r_flag = 2;
			break;

		case 'F':
			pack = pack_find(getopt_reent.optarg);
			if (pack == NULL)
				errx(exit_jump, 1, "invalid format: %s", getopt_reent.optarg);
			hasformat++;
			break;

		case 'g':
			if (getopt_reent.optarg[0] == '#') {
				gid = strtol(getopt_reent.optarg + 1, &p, 10);
				if (*p == 0)
					break;
			}
			if (gid_name(getopt_reent.optarg, &gid) == 0)
				break;
			gid = strtol(getopt_reent.optarg, &p, 10);
			if (*p == 0)
				break;
			errx(exit_jump, 1, "%s: invalid group name", getopt_reent.optarg);

		case 'm':
#if RTEMS_REMOVED
			modes = setmode(getopt_reent.optarg);
			if (modes == NULL)
#endif
				err(exit_jump, 1, "Cannot set file mode `%s'", getopt_reent.optarg);
			break;

		case 'u':
			if (getopt_reent.optarg[0] == '#') {
				uid = strtol(getopt_reent.optarg + 1, &p, 10);
				if (*p == 0)
					break;
			}
#if RTEMS_REMOVED
			if (uid_from_user(getopt_reent.optarg, &uid) == 0)
				break;
#endif
			uid = strtol(getopt_reent.optarg, &p, 10);
			if (*p == 0)
				break;
			errx(exit_jump, 1, "%s: invalid user name", getopt_reent.optarg);

		default:
		case '?':
			usage(globals);
		}
	}
	argc -= getopt_reent.optind;
	argv += getopt_reent.optind;

#ifdef KERN_DRIVERS
	if (l_flag) {
		print_device_info(argv);
		return 0;
	}
#endif

	if (argc < 2 || argc > 10)
		usage(globals);

	name = *argv;
	argc--;
	argv++;

	umask(mode = umask(0));
	mode = (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) & ~mode;

	if (argv[0][1] != '\0')
		goto badtype;
	switch (*argv[0]) {
	case 'c':
		mode |= S_IFCHR;
		break;

	case 'b':
		mode |= S_IFBLK;
		break;

	case 'p':
		if (hasformat)
			errx(exit_jump, 1, "format is meaningless for fifos");
		mode |= S_IFIFO;
		fifo = 1;
		break;

	default:
 badtype:
		errx(exit_jump, 1, "node type must be 'b', 'c' or 'p'.");
	}
	argc--;
	argv++;

	if (fifo) {
		if (argc != 0)
			usage(globals);
	} else {
		if (argc < 1 || argc > MAXARGS)
			usage(globals);
	}

	for (n = 0; n < argc; n++) {
		errno = 0;
		numbers[n] = strtoul(argv[n], &p, 0);
		if (*p == 0 && errno == 0)
			continue;
#ifdef KERN_DRIVERS
		if (n == 0) {
			major = major_from_name(argv[0], mode);
			if (major != -1) {
				numbers[0] = major;
				continue;
			}
			if (!isdigit(*(unsigned char *)argv[0]))
				errx(1, "unknown driver: %s", argv[0]);
		}
#endif
		errx(exit_jump, 1, "invalid number: %s", argv[n]);
	}

	switch (argc) {
	case 0:
		dev = 0;
		break;

	case 1:
		dev = numbers[0];
		break;

	default:
		dev = callPack(globals, pack, argc, numbers);
		break;
	}

#if RTEMS_REMOVED
	if (modes != NULL)
		mode = getmode(modes, mode);
#endif
	umask(0);
	rval = fifo ? mkfifo(name, mode) : mknod(name, mode, dev);
	if (rval < 0 && errno == EEXIST && r_flag) {
		struct stat sb;
		if (lstat(name, &sb) != 0 || (!fifo && sb.st_rdev != dev))
			sb.st_mode = 0;

		if ((sb.st_mode & S_IFMT) == (mode & S_IFMT)) {
			if (r_flag == 1)
				/* Ignore permissions and user/group */
				return 0;
			if (sb.st_mode != mode)
				rval = chmod(name, mode);
			else
				rval = 0;
		} else {
			unlink(name);
			rval = fifo ? mkfifo(name, mode)
				    : mknod(name, mode, dev);
		}
	}
	if (rval < 0)
		err(exit_jump, 1, "%s", name);
	if ((uid != (uid_t)-1 || gid != (uid_t)-1) && chown(name, uid, gid) == -1)
		/* XXX Should we unlink the files here? */
		warn("%s: uid/gid not changed", name);

	return 0;
}

static void
usage(rtems_shell_mknod_globals* globals)
{
	const char *progname = getprogname();

	(void)fprintf(stderr,
	    "usage: %s [-rR] [-F format] [-m mode] [-u user] [-g group]\n",
	    progname);
	(void)fprintf(stderr,
#ifdef KERN_DRIVERS
	    "                   [ name [b | c] [major | driver] minor\n"
#else
	    "                   [ name [b | c] major minor\n"
#endif
	    "                   | name [b | c] major unit subunit\n"
	    "                   | name [b | c] number\n"
	    "                   | name p ]\n");
#ifdef KERN_DRIVERS
	(void)fprintf(stderr, "       %s -l [driver] ...\n", progname);
#endif
	exit(1);
}

static int
gid_name(const char *name, gid_t *gid)
{
	struct group *g;

	g = getgrnam(name);
	if (!g)
		return -1;
	*gid = g->gr_gid;
	return 0;
}

static portdev_t
callPack(rtems_shell_mknod_globals* globals, pack_t *f, int n, u_long *numbers)
{
	portdev_t d;
	const char *error = NULL;

	d = (*f)(n, numbers, &error);
	if (error != NULL)
		errx(exit_jump, 1, "%s", error);
	return d;
}

#ifdef KERN_DRIVERS
static void
get_device_info(void)
{
	static int mib[2] = {CTL_KERN, KERN_DRIVERS};
	size_t len;

	if (sysctl(mib, 2, NULL, &len, NULL, 0) != 0)
		err(1, "kern.drivers" );
	kern_drivers = malloc(len);
	if (kern_drivers == NULL)
		err(1, "malloc");
	if (sysctl(mib, 2, kern_drivers, &len, NULL, 0) != 0)
		err(1, "kern.drivers" );

	num_drivers = len / sizeof *kern_drivers;
}

static void
print_device_info(char **names)
{
	int i;
	struct kinfo_drivers *kd;

	if (kern_drivers == NULL)
		get_device_info();

	do {
		kd = kern_drivers;
		for (i = 0; i < num_drivers; kd++, i++) {
			if (*names && strcmp(*names, kd->d_name))
				continue;
			printf("%s", kd->d_name);
			if (kd->d_cmajor != -1)
				printf(" character major %d", kd->d_cmajor);
			if (kd->d_bmajor != -1)
				printf(" block major %d", kd->d_bmajor);
			printf("\n");
		}
	} while (*names && *++names);
}

static int
major_from_name(const char *name, mode_t mode)
{
	int i;
	struct kinfo_drivers *kd;

	if (kern_drivers == NULL)
		get_device_info();

	kd = kern_drivers;
	for (i = 0; i < num_drivers; kd++, i++) {
		if (strcmp(name, kd->d_name))
			continue;
		if (S_ISCHR(mode))
			return kd->d_cmajor;
		return kd->d_bmajor;
	}
	return -1;
}
#endif

rtems_shell_cmd_t rtems_shell_MKNOD_Command = {
  "mknod",                                                   /* name */
  "mknod mknod [-rR] [-F fmt] [-m mode] name [c | b] minor", /* usage */
  "files",                                                   /* topic */
  rtems_shell_main_mknod,                                    /* command */
  NULL,                                                      /* alias */
  NULL                                                       /* next */
};
