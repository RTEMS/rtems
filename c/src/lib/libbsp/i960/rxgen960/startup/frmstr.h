  
  /* frmstr.h
   * Interface to procedures used to implement
   * the printf group.
   */

#ifndef _FRMSTR_H_
#define _FRMSTR_H_

#ifdef __i960

typedef int ArgType;
typedef unsigned int ArgType_U;

#else

typedef long ArgType;
typedef unsigned long ArgType_U;

#endif

  /* Format a string.
   */
extern int format_string(const char * frmt, ArgType * args, char * buffer);
  /* Get arguments from formatted string.
   */
extern int unformat_string(const char * fmt, int ** argp, const char * buffer);

  /* Definitions for control characters.
   */
#define BELL            ('G' & 0x1f)
#define BACKSPACE       '\b'
#define DELETE          0x7f
#define KILL            ('U' - 'A' + 1)
#define XON             ('Q' & 0x1f)
#define XOFF            ('S' & 0x1f)
#define CNTRL_C		0x3

#endif

  /* End of File
   */

