/*
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef _MC68681_P_H_
#define _MC68681_P_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _mc68681_context
{
        unsigned8       ucModemCtrl;
} mc68681_context;

/*
 * Driver functions
 */
static boolean mc68681_probe(int minor);

static void mc68681_init(int minor);

static int mc68681_open(
	int	major,
	int	minor,
	void	* arg
);

static int mc68681_close(
	int	major,
	int	minor,
	void	* arg
);

static void mc68681_write_polled(
	int   minor, 
	char  cChar
);

static int mc68681_assert_RTS(
	int minor
);

static int mc68681_negate_RTS(
	int minor
);

static int mc68681_assert_DTR(
	int minor
);

static int mc68681_negate_DTR(
	int minor
);

static void mc68681_initialize_interrupts(int minor);

static int mc68681_flush(int major, int minor, void *arg);

static int mc68681_write_support_int(
	int   minor,
	const char *buf,
	int   len
);

static int mc68681_write_support_polled(
	int   minor,
	const char *buf,
	int   len
	);

static int mc68681_inbyte_nonblocking_polled(
	int minor
);

#ifdef __cplusplus
}
#endif

#endif /* _MC68681_P_H_ */
