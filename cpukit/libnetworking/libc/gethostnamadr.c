/*-
 * Copyright (c) 1994, Garrett Wollman
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
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
#include <ctype.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <arpa/nameser.h>		/* XXX hack for _res */
#include <resolv.h>			/* XXX hack for _res */

#define _PATH_HOSTCONF	"/etc/host.conf"

enum service_type {
  SERVICE_NONE = 0,
  SERVICE_BIND,
  SERVICE_HOSTS,
  SERVICE_NIS };
#define SERVICE_MAX	SERVICE_NIS

static struct {
  const char *name;
  enum service_type type;
} service_names[] = {
  { "hosts", SERVICE_HOSTS },
  { "/etc/hosts", SERVICE_HOSTS },
  { "hosttable", SERVICE_HOSTS },
  { "htable", SERVICE_HOSTS },
  { "bind", SERVICE_BIND },
  { "dns", SERVICE_BIND },
  { "domain", SERVICE_BIND },
  { "yp", SERVICE_NIS },
  { "yellowpages", SERVICE_NIS },
  { "nis", SERVICE_NIS },
  { 0, SERVICE_NONE }
};

static enum service_type service_order[SERVICE_MAX + 1];
static int service_done = 0;

static enum service_type
get_service_name(const char *name) {
	int i;
	for(i = 0; service_names[i].type != SERVICE_NONE; i++) {
		if(!strcasecmp(name, service_names[i].name)) {
			return service_names[i].type;
		}
	}
	return SERVICE_NONE;
}

static void
init_services(void)
{
	char *cp, *p, buf[BUFSIZ];
	register int cc = 0;
	FILE *fd;

	if ((fd = (FILE *)fopen(_PATH_HOSTCONF, "r")) == NULL) {
				/* make some assumptions */
		service_order[0] = SERVICE_BIND;
		service_order[1] = SERVICE_HOSTS;
		service_order[2] = SERVICE_NONE;
	} else {
		while (fgets(buf, BUFSIZ, fd) != NULL && cc < SERVICE_MAX) {
			if(buf[0] == '#')
				continue;

			p = buf;
			while ((cp = strsep(&p, "\n \t,:;")) != NULL && *cp == '\0')
				;
			if (cp == NULL)
				continue;
			do {
				if (isalpha((unsigned char)cp[0])) {
					service_order[cc] = get_service_name(cp);
					if(service_order[cc] != SERVICE_NONE)
						cc++;
				}
				while ((cp = strsep(&p, "\n \t,:;")) != NULL && *cp == '\0')
					;
			} while(cp != NULL && cc < SERVICE_MAX);
		}
		service_order[cc] = SERVICE_NONE;
		fclose(fd);
	}
	service_done = 1;
}

struct hostent *
gethostbyname(const char *name)
{
	struct hostent *hp;

	if (_res.options & RES_USE_INET6) {		/* XXX */
		hp = gethostbyname2(name, AF_INET6);	/* XXX */
		if (hp)					/* XXX */
			return (hp);			/* XXX */
	}						/* XXX */
	return (gethostbyname2(name, AF_INET));
}

struct hostent *
gethostbyname2(const char *name, int type)
{
	struct hostent *hp = 0;
	int nserv = 0;

	if (!service_done)
		init_services();

	while (!hp) {
		switch (service_order[nserv]) {
		      case SERVICE_NONE:
			return NULL;
		      case SERVICE_HOSTS:
			hp = _gethostbyhtname(name, type);
			break;
		      case SERVICE_BIND:
			hp = _gethostbydnsname(name, type);
			break;
		      case SERVICE_NIS:
			hp = _gethostbynisname(name, type);
			break;
		}
		nserv++;
	}
	return hp;
}

int gethostbyaddr_r(const void *addr, socklen_t len, int type,
               struct hostent *ret, char *buf, size_t buflen,
               struct hostent **result, int *h_errnop)
{
  #warning "implement a proper gethostbyaddr_r"
 
  *result = gethostbyaddr( addr, len, type );
  if ( *result ) 
    return 0;
  return -1;
}

struct hostent *
gethostbyaddr(const void *addr, socklen_t len, int type)
{
	struct hostent *hp = 0;
	int nserv = 0;

	if (!service_done)
		init_services();

	while (!hp) {
		switch (service_order[nserv]) {
		      case SERVICE_NONE:
			return 0;
		      case SERVICE_HOSTS:
			hp = _gethostbyhtaddr(addr, len, type);
			break;
		      case SERVICE_BIND:
			hp = _gethostbydnsaddr(addr, len, type);
			break;
		      case SERVICE_NIS:
			hp = _gethostbynisaddr(addr, len, type);
			break;
		}
		nserv++;
	}
	return hp;
}

void
sethostent(int stayopen)
{
	_sethosthtent(stayopen);
	_sethostdnsent(stayopen);
}

void
endhostent(void)
{
	_endhosthtent();
	_endhostdnsent();
}

#ifdef _THREAD_SAFE

/* return length of decoded data or -1 */
static int __dns_decodename(unsigned char *packet,unsigned int offset,unsigned char *dest,
         unsigned int maxlen,unsigned char* behindpacket) {
  unsigned char *tmp;
  unsigned char *max=dest+maxlen;
  unsigned char *after=packet+offset;
  int ok=0;
  for (tmp=after; maxlen>0&&*tmp; ) {
    if (tmp>=behindpacket) return -1;
    if ((*tmp>>6)==3) {   /* goofy DNS decompression */
      unsigned int ofs=((unsigned int)(*tmp&0x3f)<<8)|*(tmp+1);
      if (ofs>=(unsigned int)offset) return -1; /* RFC1035: "pointer to a _prior_ occurrance" */
      if (after<tmp+2) after=tmp+2;
      tmp=packet+ofs;
      ok=0;
    } else {
      unsigned int duh;
      if (dest+*tmp+1>max) return -1;
      if (tmp+*tmp+1>=behindpacket) return -1;
      for (duh=*tmp; duh>0; --duh)
  *dest++=*++tmp;
      *dest++='.'; ok=1;
      ++tmp;
      if (tmp>after) { after=tmp; if (!*tmp) ++after; }
    }
  }
  if (ok) --dest;
  *dest=0;
  return after-packet;
}

static int __dns_gethostbyx_r(
    const char* name, 
    struct hostent* result,
    char *buf, size_t buflen,
    struct hostent **RESULT, 
    int *h_errnop, 
    int lookfor) 
{

  int names,ips;
  unsigned char *cur;
  unsigned char *max;
  unsigned char inpkg[1500];
  char* tmp;
  int size;

  if (lookfor==1) {
    result->h_addrtype=AF_INET;
    result->h_length=4;
  } else {
    result->h_addrtype=AF_INET6;
    result->h_length=16;
  }
  result->h_aliases=(char**)(buf+8*sizeof(char*));
  result->h_addr_list=(char**)buf;
  result->h_aliases[0]=0;

  cur=(unsigned char*)buf+16*sizeof(char*);
  max=(unsigned char*)buf+buflen;
  names=ips=0;

  if ((size=res_query(name,C_IN,lookfor,inpkg,512))<0) {
invalidpacket:
    *h_errnop=HOST_NOT_FOUND;
    return -1;
  }
  {
    tmp=(char*)inpkg+12;
    {
      char Name[257];
      unsigned short q=((unsigned short)inpkg[4]<<8)+inpkg[5];
      while (q>0) {
  if (tmp>(char*)inpkg+size) goto invalidpacket;
  while (*tmp) { tmp+=*tmp+1; if (tmp>(char*)inpkg+size) goto invalidpacket; }
  tmp+=5;
  --q;
      }
      if (tmp>(char*)inpkg+size) goto invalidpacket;
      q=((unsigned short)inpkg[6]<<8)+inpkg[7];
      if (q<1) goto nodata;
      while (q>0) {
        int decofs=__dns_decodename(inpkg,(size_t)(tmp-(char*)inpkg),(unsigned char*)Name,256,inpkg+size);
  if (decofs<0) break;
  tmp=(char*)inpkg+decofs;
  --q;
  if (tmp[0]!=0 || tmp[1]!=lookfor || /* TYPE != A */
      tmp[2]!=0 || tmp[3]!=1) {   /* CLASS != IN */
    if (tmp[1]==5) {  /* CNAME */
      tmp+=10;
      decofs=__dns_decodename(inpkg,(size_t)(tmp-(char*)inpkg),(unsigned char*)Name,256,inpkg+size);
      if (decofs<0) break;
      tmp=(char*)inpkg+decofs;
    } else
      break;
    continue;
  }
  tmp+=10;  /* skip type, class, TTL and length */
  {
    int slen;
    if (lookfor==1 || lookfor==28) /* A or AAAA*/ {
      slen=strlen(Name);
      if (cur+slen+8+(lookfor==28?12:0)>=max) { *h_errnop=NO_RECOVERY; return -1; }
    } else if (lookfor==12) /* PTR */ {
      decofs=__dns_decodename(inpkg,(size_t)(tmp-(char*)inpkg),(unsigned char*)Name,256,inpkg+size);
      if (decofs<0) break;
      tmp=(char*)inpkg+decofs;
      slen=strlen(Name);
    } else
      slen=strlen(Name);
    strcpy((char*)cur,Name);
    if (names==0)
      result->h_name=(char*)cur;
    else
      result->h_aliases[names-1]=(char*)cur;
    result->h_aliases[names]=0;
    if (names<8) ++names;
/*    cur+=slen+1; */
    cur+=(slen|3)+1;
    result->h_addr_list[ips++] = (char*)cur;
    if (lookfor==1) /* A */ {
      *(int*)cur=*(int*)tmp;
      cur+=4;
      result->h_addr_list[ips]=0;
    } else if (lookfor==28) /* AAAA */ {
      {
        int k;
        for (k=0; k<16; ++k) cur[k]=tmp[k];
      }
      cur+=16;
      result->h_addr_list[ips]=0;
    }
  }
/*        puts(Name); */
      }
    }
  }
  if (!names) {
nodata:
    *h_errnop=NO_DATA;
    return -1;
  }
  *h_errnop=0;
  *RESULT=result;
  return 0;
}




int gethostbyname_r(const char*      name, 
        struct hostent*  result,
        char            *buf, 
        int              buflen,
        struct hostent **RESULT, 
        int             *h_errnop) 
{
  uintptr_t current = (uintptr_t) buf;
  uintptr_t end = current + buflen;
  size_t L=strlen(name);

  *RESULT = NULL;
  *h_errnop = 0;

  result->h_name = (char *) current;
  current += L + 1;
  if (current > end) { *h_errnop = ERANGE; return 1; }
  strcpy(result->h_name, name);

  current += sizeof(char **);
  current -= current & (sizeof(char **) - 1);
  result->h_addr_list = (char **) current;
  current += 2 * sizeof(char **);
  result->h_aliases = (char **) current;
  current += sizeof(char **);
  if (current > end) { *h_errnop = ERANGE; return 1; }
  result->h_addr_list [0]= (char *) current;
  current += 16;
  result->h_addr_list [1] = NULL;
  result->h_aliases [0] = NULL;
  if (current > end) { *h_errnop = ERANGE; return 1; }
  if (inet_pton(AF_INET,name,result->h_addr_list[0])) {
    result->h_addrtype=AF_INET;
    result->h_length=4;
    *RESULT=result;
    return 0;
  } else if (inet_pton(AF_INET6,name,result->h_addr_list[0])) {
    result->h_addrtype=AF_INET6;
    result->h_length=16;
    *RESULT=result;
    return 0;
  }


  {
    struct hostent* r;
    sethostent(0);
    while ((r=gethostent_r(buf,buflen))) {
      int i;
      if (r->h_addrtype==AF_INET && !strcasecmp(r->h_name,name)) {  /* found it! */
found:
  memmove(result,r,sizeof(struct hostent));
  *RESULT=result;
  endhostent();
  return 0;
      }
      for (i=0; i<16; ++i) {
  if (r->h_aliases[i]) {
    if (!strcasecmp(r->h_aliases[i],name)) goto found;
  } else break;
      }
    }
    endhostent();
  }

  return __dns_gethostbyx_r(name,result,buf+L,buflen-L,RESULT,h_errnop,1);
}

#endif

