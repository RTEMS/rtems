/* $Id$ */

/* Read a password, encrypt it and compare to the encrypted 
 * password in the TELNETD_PASSWD environment variable.
 * No password is required if TELNETD_PASSWD is unset
 */

/* 
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2003-2007
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
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

#if !defined(INSIDE_TELNETD) && !defined(__rtems__)
#include <crypt.h>
#endif
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "passwd.h"

/* rtems has global filedescriptors but per-thread stdio streams... */
#define STDI_FD fileno(stdin)
#define MAXPASSRETRY	3

extern char *__des_crypt_r(char *, char*, char*, int);

#if !defined(INSIDE_TELNETD)
#define sockpeername(s,b,sz) (-1)
#endif

#if defined(INSIDE_TELNETD)
static
#endif
int check_passwd(char *peername)
{
  char			*pw;
  int			rval = -1, tmp, retries;
  struct termios	t,told;
  int			restore_flags = 0;
  char			buf[30], cryptbuf[21];
  char			salt[3];

  if ( !(pw=getenv("TELNETD_PASSWD")) || 0 == strlen(pw) )
#ifdef TELNETD_DEFAULT_PASSWD
    pw = TELNETD_DEFAULT_PASSWD;
#else
    return 0;
#endif

  if ( tcgetattr(STDI_FD, &t) ) {
    perror("check_passwd(): tcgetattr");
    goto done;	
  }
  told = t;
  t.c_lflag &= ~ECHO;
  t.c_lflag &= ~ICANON;
  t.c_cc[VTIME] = 255;
  t.c_cc[VMIN]  = 0;

  strncpy(salt,pw,2);
  salt[2]=0;

  if ( tcsetattr(STDI_FD, TCSANOW, &t) ) {
    perror("check_passwd(): tcsetattr");
    goto done;	
  }
  restore_flags = 1;

  /* Here we ask for the password... */
  for ( retries = MAXPASSRETRY; retries > 0; retries-- ) {
    fflush(stdin);
    fprintf(stderr,"Password:");
    fflush(stderr);
    if ( 0 == fgets(buf,sizeof(buf),stdin) ) {
      /* Here comes an ugly hack:
       * The termios driver's 'read()' handler
       * returns 0 to the c library's fgets if
       * it times out. 'fgets' interprets this
       * (correctly) as EOF, a condition we want
       * to undo since it's not really true since
       * we really have a read error (termios bug??)
       *
       * As a workaround we push something back and
       * read it again. This should simply reset the
       * EOF condition.
       */
      if (ungetc('?',stdin) >= 0)
        fgetc(stdin);
      goto done;
    }
    fputc('\n',stderr);
    tmp = strlen(buf);
    while ( tmp > 0 && ('\n' == buf[tmp-1] || '\r' == buf[tmp-1]) ) {
      buf[--tmp]=0;
    }
    if ( !strcmp(__des_crypt_r(buf, salt, cryptbuf, sizeof(cryptbuf)), pw) ) {
      rval = 0;
      break;
    }
    fprintf(stderr,"Incorrect Password.\n");
    sleep(2);
  }

  if ( 0 == retries ) {
    syslog( LOG_AUTHPRIV | LOG_WARNING,
      "telnetd: %i wrong passwords entered from %s",
      MAXPASSRETRY,
      peername ? peername : "<UNKNOWN>");
  }

done:
  /* what to do if restoring the flags fails?? */
  if (restore_flags)
    tcsetattr(STDI_FD, TCSANOW, &told);
  
  if (rval) {
    sleep(2);
  }
  return rval;
}

#if !defined(INSIDE_TELNETD) && !defined(__rtems__)
int
main(int argc, char **argv)
{
char *str, *enc=0;
int   ch;

while ( (ch=getopt(argc, argv, "g:")) > 0 ) {
  switch (ch) {
    default:
      fprintf(stderr,"Unknown option\n");
    return(1);

    case 'g':
      printf("Generated encrypted password: '%s'\n", (enc=crypt(optarg,"td")));
    break;
      
  }
}
if (argc>optind && !enc) {
  enc=argv[optind];
}
if (enc) {
  str = malloc(strlen(enc) + 30);
  sprintf(str,"TELNETD_PASSWD=%s",enc);
  putenv(str);
}
if (check_passwd(-1)) {
  fprintf(stderr,"check_passwd() failed\n");
}
return 0;
}

#endif
