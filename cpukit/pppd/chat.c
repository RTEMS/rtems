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
 *
 */

#ifndef lint
/* static char rcsid[] = ""; */
#endif

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#undef	TERMIOS
#define TERMIOS


#include <termios.h>

#define	STR_LEN	1024
char temp2[STR_LEN];

#ifndef SIGTYPE
#define SIGTYPE void
#endif

#undef __P
#undef __V

#ifdef __STDC__
#include <stdarg.h>
#define __V(x)	x
#define __P(x)	x
#else
#include <varargs.h>
#define __V(x)	(va_alist) va_dcl
#define __P(x)	()
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

char *program_name;

#define	MAX_ABORTS		5
#define	MAX_REPORTS		5
#define	DEFAULT_CHAT_TIMEOUT	45
#define fcntl(a, b,c ) 0
#define MAX_TIMEOUTS 10

int echo          = 0;
int verbose       = 0;
int to_log        = 1;
int to_stderr     = 0;
int Verbose       = 0;
int quiet         = 0;
int report        = 0;
int exit_code     = 0;
char *report_file = (char *) 0;
char *chat_file   = (char *) 0;
char *phone_num   = (char *) 0;
char *phone_num2  = (char *) 0;
int chat_timeout       = DEFAULT_CHAT_TIMEOUT;
static int timeout       = DEFAULT_CHAT_TIMEOUT;
int have_tty_parameters = 0;

#ifdef TERMIOS
#define term_parms struct termios
#define get_term_param(param) tcgetattr(ttyfd, param)
#define set_term_param(param) tcsetattr(ttyfd, TCSANOW, param)
struct termios saved_tty_parameters;
#endif

char *abort_string[MAX_ABORTS]={"BUSY","NO DIALTONE","NO CARRIER","NO ASWER","RINGING\r\n\r\nRINGING"};
char *fail_reason = (char *)0,
	fail_buffer[50];
int n_aborts = MAX_ABORTS, abort_next = 0, timeout_next = 0, echo_next = 0;
int clear_abort_next = 0;

char *report_string[MAX_REPORTS] ;
char  report_buffer[50] ;
int n_reports = 0, report_next = 0, report_gathering = 0 ; 
int clear_report_next = 0;

int say_next = 0, hup_next = 0;

void *dup_mem __P((void *b, size_t c));
void *copy_of __P((char *s));
/*
SIGTYPE sigalrm __P((int signo));
SIGTYPE sigint __P((int signo));
SIGTYPE sigterm __P((int signo));
SIGTYPE sighup __P((int signo));
*/
void unalarm __P((void));
void init __P((void));
void set_tty_parameters __P((void));
void echo_stderr __P((int));
void break_sequence __P((void));
void terminate __P((int status));
void do_file __P((char *chat_file));
int  get_string __P((register char *string));
int  put_string __P((register char *s));
int  write_char __P((int c));
int  put_char __P((int c));
int  get_char __P((void));
void chat_send __P((register char *s));
char *character __P((int c));
void chat_expect __P((register char *s));
char *clean __P((register char *s, int sending));
void break_sequence __P((void));
void terminate __P((int status));
void pack_array __P((char **array, int end));
char *expect_strtok __P((char *, char *));
int vfmtmsg __P((char *, int, const char *, va_list));	/* vsprintf++ */

#if 0
int usleep( long usec );				  /* returns 0 if ok, else -1 */
#endif
    
extern int input_fd,output_fd;

int main __P((int, char *[]));

void *dup_mem(b, c)
void *b;
size_t c;
{
    void *ans = malloc (c);
    if (!ans)
		return NULL;

    memcpy (ans, b, c);
    return ans;
}

void *copy_of (s)
char *s;
{
    return dup_mem (s, strlen (s) + 1);
}

/*
 * chat [ -v ] [-T number] [-U number] [ -t timeout ] [ -f chat-file ] \
 * [ -r report-file ] \
 *		[...[[expect[-say[-expect...]] say expect[-say[-expect]] ...]]]
 *
 *	Perform a UUCP-dialer-like chat script on stdin and stdout.
 */
char *getnextcommand(char **string)
{
	char *buf=*string,*res;
	res=strchr(buf,'@');
	if (res==NULL)
		return NULL;
	*res='\0';
	*string=res+1;
	return buf;
}

 
extern int ttyfd;
int chatmain(argv)
char *argv;
{
  char    *arg;

  /* initialize exit code */
  exit_code = 0;

printf("chat_main: %s\n", argv);

  /* get first expect string */
  arg = getnextcommand(&argv);
  while ( arg != NULL ) {
    /* process the expect string */
    chat_expect(arg);

    /* get the next send string */
    arg = getnextcommand(&argv);
    if ( arg != NULL ) {
      /* process the send string */
      chat_send(arg);

      /* get the next expect string */
      arg = getnextcommand(&argv);
    }
  }

  return 0;
}



/*
 *	Print an error message and terminate.
 */

void init()
{
    set_tty_parameters();
}

void set_tty_parameters()
{
    term_parms t;

	if (get_term_param (&t) < 0)
		syslog(LOG_NOTICE,"Can't get terminal parameters:")
		;
	
    saved_tty_parameters = t;
    have_tty_parameters  = 1;
    t.c_iflag     |= IGNBRK | ISTRIP | IGNPAR;
    t.c_oflag      = 0;
    t.c_lflag      = 0;
    t.c_cc[VERASE] =
    t.c_cc[VKILL]  = 0;
    t.c_cc[VMIN]   = 0;
    t.c_cc[VTIME]  = 1;
    if (set_term_param (&t) < 0)
		syslog(LOG_NOTICE,"Can't set terminal parameters:")
		;
}

void break_sequence()
{

/*    tcsendbreak (0, 0);*/
}

/*void terminate(status)
int status;
{
    echo_stderr(-1);

    if (have_tty_parameters) {
	if (set_term_param (&saved_tty_parameters) < 0)
	    fatal(2, "Can't restore terminal parameters: %m");
    }
}
*/
/*
 *	'Clean up' this string.
 */
char *clean(s, sending)
register char *s;
int sending;  /* set to 1 when sending (putting) this string. */
{
    char temp[STR_LEN], cur_chr;
    register char *s1, *phchar;
    int add_return = sending;
#define isoctal(chr) (((chr) >= '0') && ((chr) <= '7'))

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
		for ( phchar = phone_num; *phchar != '\0'; phchar++) 
		    *s1++ = *phchar;
	    }
	    else {
		*s1++ = '\\';
		*s1++ = 'T';
	    }
	    break;

	case 'U':
	    if (sending && phone_num2) {
		for ( phchar = phone_num2; *phchar != '\0'; phchar++) 
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

char *expect_strtok (s, term)
     char *s, *term;
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

void   chat_expect (s)
char *s;
{
    char *expect;
    char *reply;

    if (strcmp(s, "HANGUP") == 0) {
	++hup_next;
        return ;
    }
 
    if (strcmp(s, "ABORT") == 0) {
	++abort_next;
	return ;
    }

    if (strcmp(s, "CLR_ABORT") == 0) {
	++clear_abort_next;
	return ;
    }

    if (strcmp(s, "REPORT") == 0) {
	++report_next;
	return ;
    }

    if (strcmp(s, "CLR_REPORT") == 0) {
	++clear_report_next;
	return ;
    }

    if (strcmp(s, "TIMEOUT") == 0) {
	++timeout_next;
	return ;
    }

    if (strcmp(s, "ECHO") == 0) {
	++echo_next;
	return ;
    }

    if (strcmp(s, "SAY") == 0) {
	++say_next;
	return ;
    }

/*
 * Fetch the expect and reply string.
 */
    for (;;) {
	expect = expect_strtok (s, "-");
	s      = (char *)0 ;

	if (expect == (char *) 0)
	    return ;

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

/*
 * The expectation did not occur. This is terminal.
 */
    return ;
}

/*
 *  process the reply string
 */
void chat_send (s)
register char *s;
{
    if (say_next) {
	say_next = 0;
	s = clean(s,0);
	write(ttyfd, s, strlen(s));
        free(s);
	return;
    }

    if (hup_next) {
        hup_next = 0;
    }

    if (echo_next) {
	echo_next = 0;
	echo = (strcmp(s, "ON") == 0);
	return;
    }

    if (abort_next) {
	/* char *s1; */
	
	
	   ;
	

	return;
    }

    if (timeout_next) {
	timeout=atoi(s);
	timeout_next = 0;
	chat_timeout = atoi(s);
	
	if (chat_timeout <= 0)
	    chat_timeout = DEFAULT_CHAT_TIMEOUT;


	return;
    }
    if (strcmp(s, "EOT") == 0)
	s = "^D\\c";
    else if (strcmp(s, "BREAK") == 0)
	s = "\\K\\c";

    if (!put_string(s))
	    {
	      exit_code=1;
	      return;
	    }
}

int get_char()
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

int put_char(c)
int c;
{
    int status;
    char ch = c;

	/* inter-character typing delay (?) */

    status = write(ttyfd, &ch, 1);

    switch (status) {
    case 1:
	return (0);
	
    default:
	
	
    }
  return 0;
}

int write_char (c)
int c;
{
    if (put_char(c) < 0) {
	return (0);
    }
    return (1);
}

int put_string (s)
register char *s;
{
    quiet = 0;
    s = clean(s, 1);
    while (*s) {
	register char c = *s++;

	if (c != '\\') {
	    if (!write_char (c))
		return 0;
	    continue;
	}

	c = *s++;
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
	    if (!write_char (c))
		return 0;
	    break;
	}
    }

   /* alarm(0);*/
    return (1);
}

/*
 *	Echo a character to stderr.
 *	When called with -1, a '\n' character is generated when
 *	the cursor is not at the beginning of a line.
 */
void echo_stderr(n)
int n;
{
/*    static int need_lf;
    char *s;

    switch (n) {
    case '\r':		 
	break;
    case -1:
	if (need_lf == 0)
	    break;
 
    case '\n':
	write(2, "\n", 1);
	need_lf = 0;
	break;
    default:
	s = character(n);
	write(2, s, strlen(s));
	need_lf = 1;
	break;
    }*/
}

/*
 *	'Wait for' this string to appear on this file descriptor.
 */

int get_string(string)
register char *string;
{
    int c, len, minlen;
    register char *s = temp2, *end = s + STR_LEN;
    char *logged = temp2;
    struct termios tios;

    tcgetattr(ttyfd, &tios);
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = timeout*10/MAX_TIMEOUTS;
    tcsetattr(ttyfd, TCSANOW, &tios);
		
    string = clean(string, 0);
    len = strlen(string);
    minlen = (len > sizeof(fail_buffer)? len: sizeof(fail_buffer)) - 1;
	
    if (len > STR_LEN) {
	exit_code = 1;
	return 0;
    }

    if (len == 0) {
			return (1);
    }


   while ( (c = get_char()) >= 0) {
		int n, abort_len;

	*s++ = c;
	*s=0;
	
	if (s - temp2 >= len &&
	    c == string[len - 1] &&
	    strncmp(s - len, string, len) == 0) {
	    return (1);
	}

	for (n = 0; n < n_aborts; ++n) {
	    if (s - temp2 >= (abort_len = strlen(abort_string[n])) &&
		strncmp(s - abort_len, abort_string[n], abort_len) == 0) {

		exit_code = n + 4;
		strcpy(fail_reason = fail_buffer, abort_string[n]);
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
    return (0);
}

/*
 * Gross kludge to handle Solaris versions >= 2.6 having usleep.
 */

/*
  usleep -- support routine for 4.2BSD system call emulations
  last edit:  29-Oct-1984     D A Gwyn
  */


#if 0
int
usleep( usec )				  /* returns 0 if ok, else -1 */
    long		usec;		/* delay in microseconds */
{
  rtems_status_code status;
  rtems_interval    ticks_per_second;
  rtems_interval    ticks;
  status = rtems_clock_get(
    RTEMS_CLOCK_GET_TICKS_PER_SECOND,
    &ticks_per_second);
    ticks = (usec * (ticks_per_second/1000))/1000;
    status = rtems_task_wake_after( ticks );
  return 0;
}
#endif

void pack_array (array, end)
    char **array; /* The address of the array of string pointers */
    int    end;   /* The index of the next free entry before CLR_ */
{
    int i, j;

    for (i = 0; i < end; i++) {
	if (array[i] == NULL) {
	    for (j = i+1; j < end; ++j)
		if (array[j] != NULL)
		    array[i++] = array[j];
	    for (; i < end; ++i)
		array[i] = NULL;
	    break;
	}
    }
}

/*
 * vfmtmsg - format a message into a buffer.  Like vsprintf except we
 * also specify the length of the output buffer, and we handle the
 * %m (error message) format.
 * Doesn't do floating-point formats.
 * Returns the number of chars put into buf.
 */
#define OUTCHAR(c)	(buflen > 0? (--buflen, *buf++ = (c)): 0)
