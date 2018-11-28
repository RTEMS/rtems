/**
 *  @file
 *  @brief mq_notify() API Conformance Test
 */

 /*
  *  COPYRIGHT (c) 2018.
  *  Himanshu Sekhar Nayak
  *
  *  Permission to use, copy, modify, and/or distribute this software
  *  for any purpose with or without fee is hereby granted.
  *
  *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
  *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
  *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
  *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
  *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
  *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
  *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  */

  #ifdef HAVE_CONFIG_H
  #include "config.h"
  #endif

  #include <fcntl.h>
  #include <mqueue.h>

  int test( void );

  #define MQ_MAXMSG     1
  #define MQ_MSGSIZE    sizeof(int)

  int test( void )
  {
    mqd_t mqdes;
    struct mq_attr mqstat;
    struct sigevent sevp;
    const char *q_name;
    int result;

    mqstat.mq_maxmsg  = MQ_MAXMSG;
    mqstat.mq_msgsize = MQ_MSGSIZE;
    sevp.sigev_notify = SIGEV_SIGNAL;
    sevp.sigev_signo  = SIGUSR1;
    q_name = "queue";

    mqdes = mq_open( q_name, O_CREAT | O_RDWR, 0x777, &mqstat );
    result = mq_notify( mqdes, &sevp );

    return result;
  }
