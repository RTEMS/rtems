/* Test program for evaluating NFS read throughput */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2006 */

/* This test code allows for evaluating NFS read performance
 * under various scenarios:
 *  - synchronous reads with various buffer sizes (select
 *    'num_readers' == 0, see below).
 *  - pseudo 'read-ahead' using multiple threads that issue
 *    NFS reads from the same file (but from different offsets)
 *    in parallel.
 *    Rationale: each NFS read request is synchronous, i.e., the
 *    caller sends a request to the server and waits for the
 *    reply to come back. Performance enhancement can be expected
 *    by requesting multiple blocks in parallel rather than
 *    sequentially.
 *
 * rtems_interval
 * nfsTestRead(char *file_name, int chunk_size, int num_readers);
 *
 * 1) creates 'num_readers' threads, each opening 'file_name' for
 *    reading on a separate file descriptor.
 * 2) creates message queues for communicating with reader threads
 *
 * 3) read file using nfsTestReadBigbuf() until EOF is reached
 *
 * 4) releases resources.
 *
 * RETURNS: Time elapsed during step 3 in ms. This is measured
 *          using the system clock so make sure the test file
 *          is big enough.
 *
 * nfsTestReadBigbuf() synchronously reads a block of
 * 'num_readers * chunk_size' (which may be bigger than
 * the UDP limit of 8k) using 'num_reader' threads to
 * retrieve the various pieces of the big block in parallel.
 * This speeds up things since several RPC calls can
 * be in the works at once.
 *
 * NOTES:
 *  - if 'num_readers' == 0 this corresponds to an 'ordinary'
 *    NFS read. 'num_readers' == 1 schedules a single reader
 *    thread (== ordinary NFS read + message passing overhead).
 *  - no actual processing on the data is done; they are simply
 *    thrown away. A real, performance-critical application could
 *    pipeline 'reader' and 'cruncher' threads.
 *  - read is not completely asynchronous; synchronization is still
 *    performed at 'big block' boundaries (num_readers * chunk_size).
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
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/error.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

unsigned nfsTestReaderPri = 80;

struct nfsTestReq_ {
	unsigned offset;	/* file offset */
	int      size;		/* IN: block size to read (must be < 8192), OUT: bytes actually read */
	void	 *buf;		/* data buffer address */
};

/* Queue for sending requests to parallel reader tasks */
rtems_id nfsTestRQ = 0;
/* Queue to pickup replies from parallel reader tasks  */
rtems_id nfsTestAQ = 0;


/* Reader task; opens its own file descriptor
 * and works on requests:
 *    - obtain request from request queue.
 *    - lseek to the requested file offset
 *    - NFS read into buffer
 *    - queue reply.
 *
 * Note that this implementation is very simple
 *  - no full error checking.
 *  - file is opened/closed by thread
 * it's main purpose is running quick tests.
 */
static rtems_task
nfsTestReader(rtems_task_argument arg)
{
int                fd = open((char*)arg,O_RDONLY);
unsigned long      s;
struct nfsTestReq_ r;
rtems_status_code  sc;

	if ( fd < 0 ) {
		perror("nfsReader: opening file");
		goto cleanup;
	}
	do {
		s  = sizeof(r);
		sc = rtems_message_queue_receive(nfsTestRQ, &r, &s, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc, "(Error) reading from message queue");
			goto cleanup;
		}
		if ( !r.buf ) {
			/* They send a NULL buffer as a shutdown request */
			break;
		}
#ifdef DEBUG
		printf("Reader: reading offset %u, size %i to %p ... ",
			r.offset, r.size, r.buf);
#endif
		/* seek to requested offset */
		lseek(fd, r.offset, SEEK_SET);
		r.size = read(fd, r.buf, r.size);
#ifdef DEBUG
		printf("got %i\n",r.size);
#endif
		rtems_message_queue_send(nfsTestAQ, &r, sizeof(r));
	} while (1) ;

cleanup:
	if ( fd >= 0 )
		close(fd);
	rtems_task_delete(RTEMS_SELF);
}


/* helper to create and start a reader task */
static rtems_id
taskSpawn(char *filenm, int inst)
{
rtems_status_code sc;
rtems_id          tid;

	sc = rtems_task_create(
			rtems_build_name('n','t','t','0'+inst),
			nfsTestReaderPri,
			1400,
			RTEMS_DEFAULT_MODES,
			RTEMS_DEFAULT_ATTRIBUTES,
			&tid);
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_error(sc,"(Error) Creating nfs reader task %i",inst);
		return 0;
	}

	sc = rtems_task_start(tid, nfsTestReader, (rtems_task_argument)filenm);
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_error(sc,"(Error) Staritng nfs reader task %i",inst);
		rtems_task_delete(tid);
		return 0;
	}

	return tid;
}

/*
 * Read nrd*sz bytes into 'buf' from file offset 'off'
 * using 'nrd' parallel reader tasks to do the job.
 * This helper routine schedules 'nrd' requests to
 * the reader tasks and waits for all requests to
 * finish.
 *
 * RETURNS: number of bytes read or -1 (error).
 *
 * CAVEATS:
 *	- assumes read requests always return 'sz' bytes
 *    unless the end of file is reached.
 *    THIS ASSUMPTION SHOULD NOT BE MADE WHEN WRITING
 *    ANY 'REAL' CODE.
 */
static int
nfsTestReadBigbuf(char *buf, int off, int sz, int nrd)
{
int i,rval=0;
struct nfsTestReq_ r;
	r.buf    = buf;
	r.size   = sz;
	r.offset = off;
	/* send out parallel requests */
	for (i=0; i<nrd; i++) {
		rtems_message_queue_send(nfsTestRQ, &r, sizeof(r));
		r.offset += sz;
		r.buf    += sz;
	}
	/* wait for answers */
	for (i=0; i<nrd; i++) {
		unsigned long s = sizeof(r);
		rtems_message_queue_receive(nfsTestAQ, &r, &s, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		if ( r.size < 0 ) {
			fprintf(stderr,"A reader failed\n");
			rval = -1;
		} else {
			/* FIXME sanity checks:
			 *   - catch case where any-but-last read returns < sz
			 */
			if ( rval >= 0 ) {
				rval += r.size;
			}
		}
	}
	return rval;
}

/* Main test routine
 *
 * Read file 'fname' usint 'nrd' parallel reader tasks,
 * each operating on chunks of 'sz' bytes.
 *
 * RETURNS: time elapsed in milliseconds. This is measured
 *          using the system clock. Hence, for the result
 *          to be meaningful, the file must be big enough.
 *
 */
rtems_interval
nfsTestRead(char *fnam, int sz, int nrd)
{
int               i;
unsigned          off;
rtems_interval    now=-1, then, tickspsec;
rtems_status_code sc;
int		          fd=-1;
char	          *buf=0;

	if ( nrd < 0 )
		nrd = 0;

	if ( sz < 0 || sz > 8192 ) {
		fprintf(stderr,"\n");
		return -1;
	}

	nfsTestRQ = nfsTestAQ = 0;

	/* Allocate buffer */
	if ( ! (buf=malloc(sz*(nrd ? nrd : 1))) ) {
		perror("allocating buffer");
		goto cleanup;
	}

	/* Don't bother proceeding if we can't open the file for reading */
	if ( (fd=open(fnam,O_RDONLY)) < 0 ) {
		perror("opening file");
		goto cleanup;
	}
	if ( nrd ) {
		close(fd); fd = -1;
	}

	/* Create request queue */
	if ( nrd ) {
		sc = rtems_message_queue_create(
				rtems_build_name('n','t','r','q'),
				nrd,
				sizeof(struct nfsTestReq_),
				RTEMS_DEFAULT_ATTRIBUTES,
				& nfsTestRQ );

		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc, "(Error) creating request queue");
			nfsTestRQ = 0;
			goto cleanup;
		}

		/* Spawn reader tasks */
		for ( i=0; i<nrd; i++ ) {
			if ( ! taskSpawn(fnam, i) )
				goto cleanup;
		}

		/* Create reply queue */
		sc = rtems_message_queue_create(
				rtems_build_name('n','t','a','q'),
				nrd,
				sizeof(struct nfsTestReq_),
				RTEMS_DEFAULT_ATTRIBUTES,
				& nfsTestAQ );

		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc, "(Error) creating reply queue");
			nfsTestAQ = 0;
			goto cleanup;
		}
	}

	/* Timed main loop */
        then = rtems_clock_get_ticks_since_boot();

	if ( nrd ) {
		off = 0;
		while  ((i = nfsTestReadBigbuf(buf, off, sz, nrd)) > 0 ) {
#ifdef DEBUG
			printf("bigbuf got %i\n", i);
#endif
			off += i;
		}
	} else {
		while ( (i = read(fd, buf, sz)) > 0 )
			/* nothing else to do */;
		if ( i < 0 ) {
			perror("reading");
			goto cleanup;
		}
	}

        now = rtems_clock_get_ticks_since_boot();
	now = (now-then)*1000;
	ticksspec = rtems_clock_get_ticks_per_second();
	now /= tickspsec; /* time in ms */

cleanup:
	if ( fd >= 0 )
		close(fd);

	if ( nfsTestRQ ) {
		/* request tasks to shutdown by sending NULL buf request  */
		struct nfsTestReq_ r;
		r.buf = 0;
		for ( i=0; i<nrd; i++ ) {
			rtems_message_queue_send( nfsTestRQ, &r, sizeof(r) );
		}
		/* cheat: instead of proper synchronization with shutdown we simply
		 * delay for a second...
		 */
		rtems_task_wake_after( tickspsec );
		rtems_message_queue_delete( nfsTestRQ );
	}
	if ( nfsTestAQ )
		rtems_message_queue_delete( nfsTestAQ );
	free(buf);
	return now;
}
