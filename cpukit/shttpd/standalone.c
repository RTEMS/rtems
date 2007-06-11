/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include "defs.h"

static const char	*config_file = CONFIG;
static int		exit_flag;

static void
signal_handler(int sig_num)
{
	switch (sig_num) {
#ifndef _WIN32
	case SIGCHLD:
		while (waitpid(-1, &sig_num, WNOHANG) > 0) ;
		break;
#endif /* !_WIN32 */
	default:
		exit_flag = sig_num;
		break;
	}
}

int
main(int argc, char *argv[])
{
	struct shttpd_ctx	*ctx;

	current_time = time(NULL);
	if (argc > 1 && argv[argc - 2][0] != '-' && argv[argc - 1][0] != '-')
		config_file = argv[argc - 1];

#if !defined(NO_AUTH)
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'A') {
		if (argc != 6)
			usage(argv[0]);
		exit(edit_passwords(argv[2],argv[3],argv[4],argv[5]));
	}
#endif /* NO_AUTH */

	ctx = init_from_argc_argv(config_file, argc, argv);

	if (ctx->inetd_mode)
		(void) freopen("/dev/null", "a", stderr);

	if (ctx->inetd_mode)
		shttpd_add_socket(ctx, fileno(stdin));
	else if (shttpd_listen(ctx, ctx->port) == -1)
		elog(E_FATAL, NULL, "Cannot open socket on port %d", ctx->port);

#ifndef _WIN32
	/* Switch to alternate UID, it is safe now, after shttpd_listen() */
	if (ctx->uid != NULL) {
		struct passwd	*pw;

		if ((pw = getpwnam(ctx->uid)) == NULL)
			elog(E_FATAL, 0, "main: unknown user [%s]", ctx->uid);
		else if (setgid(pw->pw_gid) == -1)
			elog(E_FATAL, NULL, "main: setgid(%s): %s",
			    ctx->uid, strerror(errno));
		else if (setuid(pw->pw_uid) == -1)
			elog(E_FATAL, NULL, "main: setuid(%s): %s",
			    ctx->uid, strerror(errno));
	}
	(void) signal(SIGCHLD, signal_handler);
	(void) signal(SIGPIPE, SIG_IGN);
#endif /* _WIN32 */

	(void) signal(SIGTERM, signal_handler);
	(void) signal(SIGINT, signal_handler);

	elog(E_LOG, NULL, "shttpd %s started on port %d, serving %s",
	    VERSION, ctx->port, ctx->document_root);

	while (exit_flag == 0)
		shttpd_poll(ctx, 5000);

	elog(E_LOG, NULL, "%d requests %.2lf Mb in %.2lf Mb out. "
	    "Exit on signal %d", ctx->nrequests, (double) (ctx->in / 1048576),
	    (double) ctx->out / 1048576, exit_flag);

	shttpd_fini(ctx);

	return (EXIT_SUCCESS);
}
