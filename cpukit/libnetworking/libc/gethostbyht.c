/*-
 * Copyright (c) 1985, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h> /* realloc, malloc, free */
#include <ctype.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <arpa/nameser.h>	/* XXX */
#include <resolv.h>		/* XXX */
#include <sys/fcntl.h>

#define	MAXALIASES	35

static struct hostent host;
static char *host_aliases[MAXALIASES];
static char hostbuf[BUFSIZ+1];
static FILE *hostf = NULL;
static u_char host_addr[16];	/* IPv4 or IPv6 */
static char *h_addr_ptrs[2];
static int stayopen = 0;

#ifdef _THREAD_SAFE
static char* hostmap = NULL;
static unsigned int hostlen = 0; 
static char *cur;
#endif

void
_sethosthtent(int f)
{
	if (!hostf)
		hostf = fopen(_PATH_HOSTS, "r" );
	else
		rewind(hostf);
	stayopen = f;
}

void
_endhosthtent(void)
{
	if (hostf && !stayopen) {
		(void) fclose(hostf);
		hostf = NULL;
	}
}

struct hostent *
gethostent(void)
{
	char *p;
	register char *cp, **q;
	int af, len;

	if (!hostf && !(hostf = fopen(_PATH_HOSTS, "r" ))) {
		h_errno = NETDB_INTERNAL;
		return (NULL);
	}
 again:
	if (!(p = fgets(hostbuf, sizeof hostbuf, hostf))) {
		h_errno = HOST_NOT_FOUND;
		return (NULL);
	}
	if (*p == '#')
		goto again;
	if (!(cp = strpbrk(p, "#\n")))
		goto again;
	*cp = '\0';
	if (!(cp = strpbrk(p, " \t")))
		goto again;
	*cp++ = '\0';
	if (inet_pton(AF_INET6, p, host_addr) > 0) {
		af = AF_INET6;
		len = IN6ADDRSZ;
	} else if (inet_pton(AF_INET, p, host_addr) > 0) {
		if (_res.options & RES_USE_INET6) {
			_map_v4v6_address((char*)host_addr, (char*)host_addr);
			af = AF_INET6;
			len = IN6ADDRSZ;
		} else {
			af = AF_INET;
			len = INADDRSZ;
		}
	} else {
		goto again;
	}
	h_addr_ptrs[0] = (char *)host_addr;
	h_addr_ptrs[1] = NULL;
	host.h_addr_list = h_addr_ptrs;
	host.h_length = len;
	host.h_addrtype = af;
	while (*cp == ' ' || *cp == '\t')
		cp++;
	host.h_name = cp;
	q = host.h_aliases = host_aliases;
	if ((cp = strpbrk(cp, " \t")) != NULL)
		*cp++ = '\0';
	while (cp && *cp) {
		if (*cp == ' ' || *cp == '\t') {
			cp++;
			continue;
		}
		if (q < &host_aliases[MAXALIASES - 1])
			*q++ = cp;
		if ((cp = strpbrk(cp, " \t")) != NULL)
			*cp++ = '\0';
	}
	*q = NULL;
	h_errno = NETDB_SUCCESS;
	return (&host);
}

struct hostent *
_gethostbyhtname(
	const char *name,
	int af)
{
	register struct hostent *p;
	register char **cp;
	
	sethostent(0);
	while ((p = gethostent()) != NULL) {
		if (p->h_addrtype != af)
			continue;
		if (strcasecmp(p->h_name, name) == 0)
			break;
		for (cp = p->h_aliases; *cp != 0; cp++)
			if (strcasecmp(*cp, name) == 0)
				goto found;
	}
found:
	endhostent();
	return (p);
}

struct hostent *
_gethostbyhtaddr(
	const char *addr,
	int len, 
	int af)
{
	register struct hostent *p;

	sethostent(0);
	while ((p = gethostent()) != NULL)
		if (p->h_addrtype == af && !bcmp(p->h_addr, addr, len))
			break;
	endhostent();
	return (p);
}


#ifdef _THREAD_SAFE
struct hostent* gethostent_r(char* buf, int len) 
{
  char  *dest;
  struct hostent* pe=(struct hostent*)buf;
  char*  last;
  char*  max=buf+len;
  int    aliasidx;
  int    curlen;
  
   
  if (!hostf) return 0;
  fseek(hostf,0,SEEK_END);
  curlen=ftell(hostf);
  fseek(hostf,0,SEEK_SET);
  
  if (curlen > hostlen) {
    if (hostmap) {
      hostmap = realloc(hostmap,curlen);
    }
    else {
      hostmap = malloc(curlen);
    }
  }
  hostlen = curlen;
  
  if (hostmap) {
    if (fread(hostmap,hostlen,1,hostf) != hostlen) {
	hostmap=0; goto error; 
    }
    cur=hostmap;
  }
  last=hostmap+hostlen;
again:
  if ((size_t)len<sizeof(struct hostent)+11*sizeof(char*)) goto nospace;
  dest=buf+sizeof(struct hostent);
  pe->h_name=0;
  pe->h_aliases=(char**)dest; pe->h_aliases[0]=0; dest+=10*sizeof(char*);
  pe->h_addr_list=(char**)dest; dest+=2*sizeof(char**);
  if (cur>=last) return 0;
  if (*cur=='#' || *cur=='\n') goto parseerror;
  /* first, the ip number */
  pe->h_name=cur;
  while (cur<last && !isspace((unsigned char)*cur)) cur++;
  if (cur>=last) return 0;
  if (*cur=='\n') goto parseerror;
  {
    char save=*cur;
    *cur=0;
    pe->h_addr_list[0]=dest;
    pe->h_addr_list[1]=0;
    if (max-dest<16) goto nospace;
    if (inet_pton(AF_INET6,pe->h_name,dest)>0) {
      pe->h_addrtype=AF_INET6;
      pe->h_length=16;
      dest+=16;
    } else if (inet_pton(AF_INET,pe->h_name,dest)>0) {
      pe->h_addrtype=AF_INET;
      pe->h_length=4;
      dest+=4;
    } else {
      *cur=save;
      goto parseerror;
    }
    *cur=save;
  }
  ++cur;
  /* now the aliases */
  for (aliasidx=0;aliasidx<9;++aliasidx) {
    while (cur<last && isblank((unsigned char)*cur)) ++cur;
    pe->h_aliases[aliasidx]=cur;
    while (cur<last && !isspace((unsigned char)*cur)) ++cur;
    {
      char *from=pe->h_aliases[aliasidx];
      int len=cur-from;
      if (max-dest<len+2) goto nospace;
      pe->h_aliases[aliasidx]=dest;
      memmove(dest,from,(size_t)(cur-from));
      dest+=len;
      *dest=0; ++dest;
    }
    if (*cur=='\n') { ++cur; ++aliasidx; break; }
    if (cur>=last || !isblank((unsigned char)*cur)) break;
    cur++;
  }
  pe->h_aliases[aliasidx]=0;
  pe->h_name=pe->h_aliases[0];
  pe->h_aliases++;
  return pe;
parseerror:
  while (cur<last && *cur!='\n') cur++;
  cur++;
  goto again;
nospace:
  errno=ERANGE;
  goto __error;
error:
  errno=ENOMEM;
__error:
  if (hostmap!=NULL) free(hostmap);
  hostmap=NULL;
  return 0;
}
#endif
