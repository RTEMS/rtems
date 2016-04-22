/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_NETDB_H
#define _RTEMS_RTEMS_NETDB_H

#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* DO NOT USE THESE, THEY ARE SUBJECT TO CHANGE AND ARE NOT PORTABLE!!! */
void	_sethosthtent(int);
void	_endhosthtent(void);
void	_sethostdnsent(int);
void	_endhostdnsent(void);
void	_setnethtent(int);
void	_endnethtent(void);
void	_setnetdnsent(int);
void	_endnetdnsent(void);
struct hostent * _gethostbyhtname(const char *, int);
struct hostent * _gethostbydnsname(const char *, int);
struct hostent * _gethostbynisname(const char *, int);
struct hostent * _gethostbyhtaddr (const char *, int, int);
struct hostent * _gethostbydnsaddr(const char *, int, int);
struct hostent * _gethostbynisaddr(const char *, int, int);
struct netent *  _getnetbyhtname (const char *);
struct netent *  _getnetbydnsname(const char *);
struct netent *  _getnetbynisname(const char *);
struct netent *  _getnetbyhtaddr (unsigned long, int);
struct netent *  _getnetbydnsaddr(unsigned long, int);
struct netent *  _getnetbynisaddr(unsigned long, int);
void _map_v4v6_address(const char *, char *);
void _map_v4v6_hostent(struct hostent *, char **, int *len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RTEMS_NETDB_H */
