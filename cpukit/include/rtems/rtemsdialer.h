
#ifndef DIALER_H
#define DIALER_H

/* define constant mode values */
#define DIALER_INIT         0
#define DIALER_CONNECT      1
#define DIALER_WELCOME      2
#define DIALER_DISCONNECT   3

/* define constant return values */
#define DIALER_SUCCESS      0
#define DIALER_INVALIDARG   1
#define DIALER_UNEXPECTED   2
#define DIALER_TIMEOUT      3
#define DIALER_CMDFAILED    4

/* define typedef for dialer function prototype */
typedef int  (*dialerfp)(int tty, int mode, char *pScript);

/* declare default chat program dialer */
extern int chatmain(int tty, int mode, char *pScript);

#endif
