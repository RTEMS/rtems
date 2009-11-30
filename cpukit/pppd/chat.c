/*
 *	Chat -- a program for automatic session establishment (i.e. dial
 *		the phone and log in).
 *
 * Standard termination codes:
 *  0 - successful completion of the script
 *  1 - invalid argument, expect string too large, etc.
 *  2 - error on an I/O operation or fatal error condition.
 *  3 - timeout waiting for a simple string.
 *  4 - the first string declared as "ABORT"
 *  5 - the second string declared as "ABORT"
 *  6 - ... and so on for successive ABORT strings.
 *
 *	This software is in the public domain.
 *
 * -----------------
 *	22-May-99 added environment substitutuion, enabled with -E switch.
 *	Andreas Arens <andras@cityweb.de>.
 *
 *	12-May-99 added a feature to read data to be sent from a file,
 *	if the send string starts with @.  Idea from gpk <gpk@onramp.net>.
 *
 *	added -T and -U option and \T and \U substitution to pass a phone
 *	number into chat script. Two are needed for some ISDN TA applications.
 *	Keith Dart <kdart@cisco.com>
 *	
 *
 *	Added SAY keyword to send output to stderr.
 *      This allows to turn ECHO OFF and to output specific, user selected,
 *      text to give progress messages. This best works when stderr
 *      exists (i.e.: pppd in nodetach mode).
 *
 * 	Added HANGUP directives to allow for us to be called
 *      back. When HANGUP is set to NO, chat will not hangup at HUP signal.
 *      We rely on timeouts in that case.
 *
 *      Added CLR_ABORT to clear previously set ABORT string. This has been
 *      dictated by the HANGUP above as "NO CARRIER" (for example) must be
 *      an ABORT condition until we know the other host is going to close
 *      the connection for call back. As soon as we have completed the
 *      first stage of the call back sequence, "NO CARRIER" is a valid, non
 *      fatal string. As soon as we got called back (probably get "CONNECT"),
 *      we should re-arm the ABORT "NO CARRIER". Hence the CLR_ABORT command.
 *      Note that CLR_ABORT packs the abort_strings[] array so that we do not
 *      have unused entries not being reclaimed.
 *
 *      In the same vein as above, added CLR_REPORT keyword.
 *
 *      Allow for comments. Line starting with '#' are comments and are
 *      ignored. If a '#' is to be expected as the first character, the
 *      expect string must be quoted.
 *
 *
 *		Francis Demierre <Francis@SwissMail.Com>
 * 		Thu May 15 17:15:40 MET DST 1997
 *
 *
 *      Added -r "report file" switch & REPORT keyword.
 *              Robert Geer <bgeer@xmission.com>
 *
 *      Added -s "use stderr" and -S "don't use syslog" switches.
 *              June 18, 1997
 *              Karl O. Pinc <kop@meme.com>
 *
 *
 *	Added -e "echo" switch & ECHO keyword
 *		Dick Streefland <dicks@tasking.nl>
 *
 *
 *	Considerable updates and modifications by
 *		Al Longyear <longyear@pobox.com>
 *		Paul Mackerras <paulus@cs.anu.edu.au>
 *
 *
 *	The original author is:
 *
 *		Karl Fox <karl@MorningStar.Com>
 *		Morning Star Technologies, Inc.
 *		1760 Zollinger Road
 *		Columbus, OH  43221
 *		(614)451-1883
 *
 */

/*	$Id$ */

/*
	Fixes and some Changes by Wilfried Busalski Lancier-Monitoring GmbH Germany
	wilfried.busalski@muenster.de
	
	Fixes:	put_string()		Free memory allocated by clean()
			get_string()		Free memory allocated by clean()
			chat_main()			Will Distroy's no more the chat-script
			getnextcommand()	sepatator changed to '@'
*/

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <termios.h>
#include "pppd.h"

#undef	TERMIOS
#define TERMIOS


#define	STR_LEN	1024
char temp2[STR_LEN];

#ifndef SIGTYPE
#define SIGTYPE void
#endif

#undef __V

#ifdef __STDC__
#include <stdarg.h>
#define __V(x)	x
#else
#include <varargs.h>
#define __V(x)	(va_alist) va_dcl
#define const
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK	O_NDELAY
#endif


/*************** Micro getopt() *********************************************/
#define	OPTION(c,v)	(_O&2&&**v?*(*v)++:!c||_O&4?0:(!(_O&1)&& \
				(--c,++v),_O=4,c&&**v=='-'&&v[0][1]?*++*v=='-'\
				&&!v[0][1]?(--c,++v,0):(_O=2,*(*v)++):0))
#define	OPTARG(c,v)	(_O&2?**v||(++v,--c)?(_O=1,--c,*v++): \
				(_O=4,(char*)0):(char*)0)
#define	OPTONLYARG(c,v)	(_O&2&&**v?(_O=1,--c,*v++):(char*)0)
#define	ARG(c,v)	(c?(--c,*v++):(char*)0)

#if 0
static int _O = 0;		/* Internal state */
#endif
/*************** Micro getopt() *********************************************/

#define	MAX_ABORTS		16
#define	MAX_REPORTS		16
#define	DEFAULT_CHAT_TIMEOUT	45
#define MAX_TIMEOUTS            10

static int echo             = 0;
static int quiet            = 0;
static int use_env          = 0;
static int exit_code        = 0;
static char *phone_num      = (char *) 0;
static char *phone_num2     = (char *) 0;
static int ttyfd;
static int timeout   = DEFAULT_CHAT_TIMEOUT;

#ifdef TERMIOS
#define term_parms struct termios
#define get_term_param(param) tcgetattr(0, param)
#define set_term_param(param) tcsetattr(0, TCSANOW, param)
struct termios saved_tty_parameters;
#endif

char *fail_reason = (char *)0;
char  fail_buffer[50];
char *abort_string[MAX_ABORTS]={"BUSY","NO DIALTONE","NO CARRIER","NO ANSWER","RING\r\nRING"};
int n_aborts = 5;
int abort_next = 0, timeout_next = 0, echo_next = 0;
int clear_abort_next = 0;

char *report_string[MAX_REPORTS] ;
char  report_buffer[50] ;
int n_reports = 0, report_next = 0, report_gathering = 0 ;
int clear_report_next = 0;

int say_next = 0, hup_next = 0;

void *dup_mem(void *b, size_t c);
void *copy_of(char *s);
void break_sequence(void);
static int  get_string(register char *string);
static int  put_string(register char *s);
static int  write_char(int c);
static int  put_char(int c);
static int  get_char(void);
void chat_send(register char *s);
/* static char *character(int c); */
void chat_expect(register char *s);
static char *clean(register char *s, int sending);
char *expect_strtok(char *, char *);
int chatmain(int, int, char *);


void *dup_mem(
  void *b,
  size_t c)
{
    void *ans = malloc (c);
    if (!ans)
	return NULL;

    memcpy(ans, b, c);
    return ans;
}

void *copy_of(
  char *s)
{
    return dup_mem(s, strlen (s) + 1);
}

char *getnextcommand(char *string,char *buff)
{
	char *token;
	int len;
	
	token=strchr(string,'@');
	if (token==NULL){
		return NULL;
	}
	len=token-string;
	if(len > 78 ){
		len=78;
	}
	memcpy(buff,string,len);
	buff[len]=0;
	return(token+1);
}

int chatmain(int fd, int mode, char *pScript)
{
  char    arg[80];
  char 	  *script;

  /* initialize exit code */
  exit_code = 0;
  ttyfd     = fd;

  script=pScript;

  if ( debug ) {
    dbglog("chat_main: %s\n", script);
  }

  /* get first expect string */
  script = getnextcommand(script,arg);
  while (( script != NULL ) && ( exit_code == 0 )) {
    /* process the expect string */
    chat_expect(arg);
    if ( exit_code == 0 ) {
      /* get the next send string */
      script = getnextcommand(script,arg);
      if ( script != NULL ) {
        /* process the send string */
        chat_send(arg);

        /* get the next expect string */
        script = getnextcommand(script,arg);
      }
    }
  }
  ttyfd = (int)-1;

  return ( exit_code );
}

void break_sequence(void)
{
  tcsendbreak(ttyfd, 0);
}

/*
 *	'Clean up' this string.
 */
static char *clean(
  char *s,
  int sending )  /* set to 1 when sending (putting) this string. */
{
    char temp[STR_LEN], env_str[STR_LEN], cur_chr;
    register char *s1, *phchar;
    int add_return = sending;
#define isoctal(chr)	(((chr) >= '0') && ((chr) <= '7'))
#define isalnumx(chr)	((((chr) >= '0') && ((chr) <= '9')) \
			 || (((chr) >= 'a') && ((chr) <= 'z')) \
			 || (((chr) >= 'A') && ((chr) <= 'Z')) \
			 || (chr) == '_')

    s1 = temp;
    while (*s) {
	cur_chr = *s++;
	if (cur_chr == '^') {
	    cur_chr = *s++;
	    if (cur_chr == '\0') {
		*s1++ = '^';
		break;
	    }
	    cur_chr &= 0x1F;
	    if (cur_chr != 0) {
		*s1++ = cur_chr;
	    }
	    continue;
	}
	
	if (use_env && cur_chr == '$') {		/* ARI */
	    phchar = env_str;
	    while (isalnumx(*s))
		*phchar++ = *s++;
	    *phchar = '\0';
	    phchar = getenv(env_str);
	    if (phchar)
		while (*phchar)
		    *s1++ = *phchar++;
	    continue;
	}

	if (cur_chr != '\\') {
	    *s1++ = cur_chr;
	    continue;
	}

	cur_chr = *s++;
	if (cur_chr == '\0') {
	    if (sending) {
		*s1++ = '\\';
		*s1++ = '\\';
	    }
	    break;
	}

	switch (cur_chr) {
	case 'b':
	    *s1++ = '\b';
	    break;

	case 'c':
	    if (sending && *s == '\0')
		add_return = 0;
	    else
		*s1++ = cur_chr;
	    break;

	case '\\':
	case 'K':
	case 'p':
	case 'd':
	    if (sending)
		*s1++ = '\\';
	    *s1++ = cur_chr;
	    break;

	case 'T':
	    if (sending && phone_num) {
		for (phchar = phone_num; *phchar != '\0'; phchar++)
		    *s1++ = *phchar;
	    }
	    else {
		*s1++ = '\\';
		*s1++ = 'T';
	    }
	    break;

	case 'U':
	    if (sending && phone_num2) {
		for (phchar = phone_num2; *phchar != '\0'; phchar++)
		    *s1++ = *phchar;
	    }
	    else {
		*s1++ = '\\';
		*s1++ = 'U';
	    }
	    break;

	case 'q':
	    quiet = 1;
	    break;

	case 'r':
	    *s1++ = '\r';
	    break;

	case 'n':
	    *s1++ = '\n';
	    break;

	case 's':
	    *s1++ = ' ';
	    break;

	case 't':
	    *s1++ = '\t';
	    break;

	case 'N':
	    if (sending) {
		*s1++ = '\\';
		*s1++ = '\0';
	    }
	    else
		*s1++ = 'N';
	    break;
	
	case '$':			/* ARI */
	    if (use_env) {
		*s1++ = cur_chr;
		break;
	    }
	    /* FALL THROUGH */

	default:
	    if (isoctal (cur_chr)) {
		cur_chr &= 0x07;
		if (isoctal (*s)) {
		    cur_chr <<= 3;
		    cur_chr |= *s++ - '0';
		    if (isoctal (*s)) {
			cur_chr <<= 3;
			cur_chr |= *s++ - '0';
		    }
		}

		if (cur_chr != 0 || sending) {
		    if (sending && (cur_chr == '\\' || cur_chr == 0))
			*s1++ = '\\';
		    *s1++ = cur_chr;
		}
		break;
	    }

	    if (sending)
		*s1++ = '\\';
	    *s1++ = cur_chr;
	    break;
	}
    }

    if (add_return)
	*s1++ = '\r';

    *s1++ = '\0'; /* guarantee closure */
    *s1++ = '\0'; /* terminate the string */
    return dup_mem (temp, (size_t) (s1 - temp)); /* may have embedded nuls */
}

/*
 * A modified version of 'strtok'. This version skips \ sequences.
 */
char *expect_strtok (
     char *s, char *term)
{
    static  char *str   = "";
    int	    escape_flag = 0;
    char   *result;

/*
 * If a string was specified then do initial processing.
 */
    if (s)
	str = s;

/*
 * If this is the escape flag then reset it and ignore the character.
 */
    if (*str)
	result = str;
    else
	result = (char *) 0;

    while (*str) {
	if (escape_flag) {
	    escape_flag = 0;
	    ++str;
	    continue;
	}

	if (*str == '\\') {
	    ++str;
	    escape_flag = 1;
	    continue;
	}

/*
 * If this is not in the termination string, continue.
 */
	if (strchr (term, *str) == (char *) 0) {
	    ++str;
	    continue;
	}

/*
 * This is the terminator. Mark the end of the string and stop.
 */
	*str++ = '\0';
	break;
    }
    return (result);
}

/*
 * Process the expect string
 */
void chat_expect (
  char *s)
{
    char *expect;
    char *reply;

    if (strcmp(s, "HANGUP") == 0) {
		++hup_next;
        return;
    }

    if (strcmp(s, "ABORT") == 0) {
		++abort_next;
		return;
    }

    if (strcmp(s, "CLR_ABORT") == 0) {
		++clear_abort_next;
		return;
    }

    if (strcmp(s, "REPORT") == 0) {
		++report_next;
		return;
    }

    if (strcmp(s, "CLR_REPORT") == 0) {
		++clear_report_next;
		return;
    }

    if (strcmp(s, "TIMEOUT") == 0) {
		++timeout_next;
		return;
    }

    if (strcmp(s, "ECHO") == 0) {
		++echo_next;
		return;
    }

    if (strcmp(s, "SAY") == 0) {
		++say_next;
		return;
    }

/*
 * Fetch the expect and reply string.
 */
    for (;;) {
	expect = expect_strtok (s, "-");
	s      = (char *) 0;

	if (expect == (char *) 0)
	    return;

	reply = expect_strtok (s, "-");

/*
 * Handle the expect string. If successful then exit.
 */
	if (get_string (expect))
	    return;

/*
 * If there is a sub-reply string then send it. Otherwise any condition
 * is terminal.
 */
	if (reply == (char *) 0 || exit_code != 3)
	    break;

	chat_send (reply);
    }
}

#if 0
/*
 * Translate the input character to the appropriate string for printing
 * the data.
 */

static char *character(
  int c)
{
    static char string[10];
    char *meta;

    meta = (c & 0x80) ? "M-" : "";
    c &= 0x7F;

    if (c < 32)
	sprintf(string, "%s^%c", meta, (int)c + '@');
    else if (c == 127)
	sprintf(string, "%s^?", meta);
    else
	sprintf(string, "%s%c", meta, c);

    return (string);
}
#endif

/*
 *  process the reply string
 */
void chat_send (
  char *s)
{
/*  char file_data[STR_LEN];  */

    if (say_next) {
		say_next = 0;
		s = clean(s, 1);
		write(2, s, strlen(s));
        free(s);
		return;
    }

    if (hup_next) {
        hup_next = 0;
        return;
    }

    if (echo_next) {
	echo_next = 0;
	echo = (strcmp(s, "ON") == 0);
	return;
    }

    if (abort_next) {
		abort_next = 0;
		if ( n_aborts < MAX_ABORTS ) {
			char *s1;
			s1 = clean(s, 0);
			if (( strlen(s1) <= strlen(s) ) && ( strlen(s1) <  sizeof(fail_buffer)))
				abort_string[n_aborts++] = s1;
			else
				free(s1);
        	}
		return;
    }

    if (clear_abort_next) {
		clear_abort_next = 0;
		return;
    }

    if (report_next) {
		report_next = 0;
		return;
    }

    if (clear_report_next) {
		clear_report_next = 0;
		return;
    }

    if (timeout_next) {
		timeout_next = 0;
		timeout = atoi(s);
	
		if (timeout <= 0){
	    	timeout = DEFAULT_CHAT_TIMEOUT;
	    }
		return;
    }

    if (strcmp(s, "EOT") == 0){
		s = "^D\\c";
	}
    else{
    	if (strcmp(s, "BREAK") == 0){
			s = "\\K\\c";
		}

    	if (!put_string(s)) {
      		exit_code = 2;
    	}
	}
}

static int get_char(void)
{
    int status;
    char c;
    int tries=MAX_TIMEOUTS;

	while(tries)
	{
	    status = read(ttyfd, &c, 1);
	    switch (status) {
		   case 1:
				return ((int)c & 0x7F);
		    default:
				tries--;			
	    }
    }
	return -1;						
}

static int put_char(
  int c)
{
  char ch = c;

  return(write(ttyfd, &ch, 1));
}

static int write_char (
  int c)
{
    if (put_char(c) < 1) {
		return (0);
    }
    return (1);
}

static int put_string (
  char *s)
{
	char *out,*free_ptr=0;
	
    quiet = 0;
    out = free_ptr = clean(s, 1);
    while (*out) {
		register char c = *out++;

		if (c != '\\') {
	    	if (!write_char (c)){
	    		free(free_ptr);
				return 0;
			}
	    	continue;
		}

		c = *out++;

		switch (c) {
			case 'd':
	    		sleep(1);
		    break;

			case 'K':
	    		break_sequence();
	    	break;

			case 'p':
#if 0 /* FIXME!!! */
	    		usleep(10000); 	/* 1/100th of a second (arg is microseconds) */
#else
	    		sleep(1);
#endif
	    	break;

			default:
	    		if (!write_char (c)){
	    			free(free_ptr);
					return 0;
				}
	    	break;
		}
    }
    free(free_ptr);

    return (1);
}

/*
 *	'Wait for' this string to appear on this file descriptor.
 */
static int get_string(
    char *in_string)
{
    int c, len, minlen;
    register char *s = temp2, *end = s + STR_LEN;
    char *logged = temp2;
    char *string=0;
    struct termios tios;

    memset(temp2, 0, sizeof(temp2));

    tcgetattr(ttyfd, &tios);
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = timeout*10/MAX_TIMEOUTS;
    tcsetattr(ttyfd, TCSANOW, &tios);
		
    string = clean(in_string, 0);
    len = strlen(string);
    minlen = (len > sizeof(fail_buffer)? len: sizeof(fail_buffer)) - 1;
	
    if (len > STR_LEN) {
		exit_code = 1;
		free(string);
		return 0;
    }

    if (len == 0) {
	free(string);
	return (1);
    }

   while ( (c = get_char()) >= 0) {
		int n, abort_len;

	if(c == '\n' || c == '\r'){
		s = temp2;
		*s=0;
	}
	else{
		*s++ = c;
		*s=0;
	}

	if (s - temp2 >= len &&
	    c == string[len - 1] &&
	    strncmp(s - len, string, len) == 0) {
	    free(string);
	    return (1);
	}

	for (n = 0; n < n_aborts; ++n) {
	    if (s - temp2 >= (abort_len = strlen(abort_string[n])) &&
		strncmp(s - abort_len, abort_string[n], abort_len) == 0) {

		exit_code = n + 4;
		strcpy(fail_reason = fail_buffer, abort_string[n]);
		free(string);
		return (0);
	    }
	}

	if (s >= end) {
	    if (logged < s - minlen) {
		logged = s;
	    }
	    s -= minlen;
	    memmove(temp2, s, minlen);
	    logged = temp2 + (logged - s);
	    s = temp2 + minlen;
	}
    }

    exit_code = 3;
    free(string);
    return (0);
}
