mqueue
======
This program should print out the default attribute settings for a 
POSIX message queue.

```c
#include <mqueue.h>
#include <stdio.h>

main()
{
  mqd_t mqfd;
  struct mq_attr mqstat;
  int status;

  /* this should create it */
  mqfd = mq_open("myipc",O_WRONLY|O_CREAT,NULL);
  if ( (int)mqfd == -1 ) {
    perror( "Unable to open message queue" );
    exit( 1 );
  }

  status = mq_getattr(mqfd, &mqstat);
  if ( !status ) {
    printf( "mq_maxmsg: %d\n", mqstat.mq_maxmsg );
    printf( "mq_msgsize: %d\n", mqstat.mq_msgsize );
    printf( "mq_curmsgs: %d\n", mqstat.mq_curmsgs );
  } else {
    perror( "Unable to get attributes on message queue" );
    exit( 1 );
  }

  /* this should delete it */
  (void) mq_close( mqfd );
  exit( 0 );
}
```
