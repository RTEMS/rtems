/*
 * utils.c - various utility functions used in pppd.
 *
 * Copyright (c) 1999 The Australian National University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the Australian National University.  The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#ifndef HAVE_STRLCAT
/*
 * strlcat - like strcat/strncat, doesn't overflow destination buffer,
 * always leaves destination null-terminated (for len > 0).
 */
size_t
strlcat(
    char *dest,
    const char *src,
    size_t len )
{
    size_t dlen = strlen(dest);

    return dlen + strlcpy(dest + dlen, src, (len > dlen? len - dlen: 0));
}
#endif
