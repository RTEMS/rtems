/*
 *  $Id$
 */

#ifndef NET_EXTERNS_H
#define NET_EXTERNS_H

/*
 * External entry points
 */

extern int uti596_attach(struct rtems_bsdnet_ifconfig *);
extern int uti596dump(char * );
extern void uti596reset(void);
extern void uti596Diagnose(int);
extern void uti596_request_reset(void);

#endif
