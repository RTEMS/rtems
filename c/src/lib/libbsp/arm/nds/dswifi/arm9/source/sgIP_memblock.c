// DSWifi Project - sgIP Internet Protocol Stack Implementation
// Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org - http://www.akkit.org
/******************************************************************************
DSWifi Lib and test materials are licenced under the MIT open source licence:
Copyright (c) 2005-2006 Stephen Stair

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "sgIP_memblock.h"

#include <stdlib.h>
#include <string.h>

#ifndef SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

#ifndef SGIP_USEDYNAMICMEMORY
sgIP_memblock memblock_pool[SGIP_MEMBLOCK_BASENUM];
#else
sgIP_memblock * memblock_pool;
#endif

sgIP_memblock * memblock_poolfree;
int numused, numfree;
void * pool_link;


sgIP_memblock * sgIP_memblock_getunused() {
	int i;
	sgIP_memblock * mb;
	SGIP_INTR_PROTECT();
	if(memblock_poolfree) { // we still have free memblocks!
		mb=memblock_poolfree;
		memblock_poolfree=mb->next;
		numfree--;
		numused++;
	} else { // oh noes, we have no more free memblocks.
		mb = 0; // eventually alloc new blocks, but for now just stop.
	}

	SGIP_INTR_UNPROTECT();
	return mb;
}
#endif  //SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

void sgIP_memblock_Init() {
#ifndef SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL
	int i;
#ifdef SGIP_USEDYNAMICMEMORY
	pool_link = sgIP_malloc(sizeof(sgIP_memblock)*SGIP_MEMBLOCK_BASENUM+4);
	((long *)pool_link)[0]=0;
	memblock_pool = (sgIP_memblock *) (((char *)pool_link)+4);
#endif
	numused=numfree=0;
	memblock_poolfree=0;
	for(i=0;i<SGIP_MEMBLOCK_BASENUM;i++) {
		memblock_pool[i].totallength=0;
		memblock_pool[i].next=memblock_poolfree;
		memblock_poolfree=memblock_pool+i;
		numfree++;
	}
#endif //SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL
}

#ifdef SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

sgIP_memblock * sgIP_memblock_allocHW(int headersize, int packetsize) {
   sgIP_memblock * mb;
   mb = (sgIP_memblock *) sgIP_malloc(SGIP_MEMBLOCK_HEADERSIZE+SGIP_MAXHWHEADER+packetsize);
   if(!mb) return 0;
   mb->totallength=headersize+packetsize;
   mb->thislength=mb->totallength;
   mb->datastart=mb->reserved+SGIP_MAXHWHEADER-headersize;
   mb->next=0;
   return mb;
}

#else //SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

sgIP_memblock * sgIP_memblock_allocHW(int headersize, int packetsize) {
	sgIP_memblock * mb, * tmb, *t;
	int totlen;
	mb = sgIP_memblock_getunused();
	if(!mb) return 0;
	mb->totallength=headersize+packetsize;
	mb->datastart=mb->reserved+SGIP_MAXHWHEADER-headersize;
	mb->next=0;
	mb->thislength=headersize+SGIP_MEMBLOCK_FIRSTINTERNALSIZE;
	if(mb->thislength>=mb->totallength) {
		mb->thislength = mb->totallength;
//		SGIP_DEBUG_MESSAGE(("memblock_alloc: %i free, %i used",numfree,numused));
		return mb;
	} else { // need more blocks
		totlen=mb->thislength;
		tmb=mb;
		while(totlen<mb->totallength) {
			t=sgIP_memblock_getunused();
			if(!t) { // we're skrewed.
				sgIP_memblock_free(mb);
				return 0;
			}
			tmb->next=t;
			t->totallength=mb->totallength;
			t->datastart=mb->reserved; // no header on blocks after the first.
			t->next=0;
			t->thislength=SGIP_MEMBLOCK_INTERNALSIZE;
			if(t->thislength+totlen>=mb->totallength) {
				t->thislength=mb->totallength-totlen;
//				SGIP_DEBUG_MESSAGE(("memblock_alloc: %i free, %i used",numfree,numused));
				return mb;
			} else { // need YET more blocks.
				totlen+=t->thislength;
				tmb=t;
			} // the cycle contiues.
		}
		sgIP_memblock_free(mb); // should never get here.
	}
	return 0;
}
#endif //SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

sgIP_memblock * sgIP_memblock_alloc(int packetsize) {
	return sgIP_memblock_allocHW(0,packetsize);
}

#ifdef SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

void sgIP_memblock_free(sgIP_memblock * mb) {
   sgIP_memblock * f;

   SGIP_INTR_PROTECT();
   while(mb) {
      mb->totallength=0;
      mb->thislength=0;
      f=mb;
      mb = mb->next;

      sgIP_free(f);
   }

   SGIP_INTR_UNPROTECT();
}

#else //SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

void sgIP_memblock_free(sgIP_memblock * mb) {
	sgIP_memblock * f;

	SGIP_INTR_PROTECT();
	while(mb) {
		mb->totallength=0;
		mb->thislength=0;
		f=mb;
		mb = mb->next;

		numfree++; // reinstate memblock into the pool!
		numused--;
		f->next=memblock_poolfree;
		memblock_poolfree=f;
	}
//	SGIP_DEBUG_MESSAGE(("memblock_free: %i free, %i used",numfree,numused));

	SGIP_INTR_UNPROTECT();

}

#endif //SGIP_MEMBLOCK_DYNAMIC_MALLOC_ALL

// positive to expose, negative to hide.
void sgIP_memblock_exposeheader(sgIP_memblock * mb, int change) {
	if(mb) {
		mb->thislength+=change;
		mb->totallength+=change;
		mb->datastart-=change;
		while(mb->next) {
			mb->next->totallength=mb->totallength;
			mb=mb->next;
		}
	}
}
void sgIP_memblock_trimsize(sgIP_memblock * mb, int newsize) {
	int lentot;
	if(mb) {
		mb->totallength=newsize;
		lentot=0;
		while(mb) {
			lentot+=mb->thislength;
			if(lentot>newsize) {
				mb->thislength-=(lentot-newsize);
				if(mb->next) sgIP_memblock_free(mb->next);
				mb->next=0;
				return;
			} else {
				mb=mb->next;
			}
		}
	}
}


int sgIP_memblock_IPChecksum(sgIP_memblock * mb, int startbyte, int chksum_length) {
	int chksum_temp,offset;
	// check checksum
	chksum_temp=0;
	offset=0;
	while(mb && startbyte>mb->thislength) { startbyte-=mb->thislength; mb=mb->next; }
	if(!mb) return 0;
	while(chksum_length) {
		while(startbyte+offset+1<mb->thislength && chksum_length>1) {
			chksum_temp+= ((unsigned char *)mb->datastart)[startbyte+offset] + (((unsigned char *)mb->datastart)[startbyte+offset+1]<<8);
			offset+=2;
			chksum_length-=2;
		}
      chksum_temp= (chksum_temp&0xFFFF) +(chksum_temp>>16);
		if(startbyte+offset<mb->thislength && chksum_length>0) {
			chksum_temp+= ((unsigned char *)mb->datastart)[startbyte+offset];
			if(chksum_length==1) break;
			chksum_length--;
			offset=0;
			startbyte=0;
			mb=mb->next;
			if(!mb) break;
			if(mb->thislength==0) break;
			chksum_temp+= ((unsigned char *)mb->datastart)[startbyte+offset]<<8;
			if(chksum_length==1) break;
			offset++;
			chksum_length--;
		}
	}
   chksum_temp= (chksum_temp&0xFFFF) +(chksum_temp>>16);
   chksum_temp= (chksum_temp&0xFFFF) +(chksum_temp>>16);
	return chksum_temp;
}
int sgIP_memblock_CopyToLinear(sgIP_memblock * mb, void * dest_buf, int startbyte, int copy_length) {
	int copylen,ofs_src, tot_copy;
	ofs_src=startbyte;
	while(mb && ofs_src>=mb->thislength) { ofs_src-=mb->thislength; mb=mb->next; }
	if(!mb) return 0;
	if(startbyte+copy_length>mb->totallength) copy_length=mb->totallength-startbyte;
	if(copy_length<0) copy_length=0;
	tot_copy=0;
	while(copy_length>0) {
		copylen=copy_length;
		if(copylen>mb->thislength-ofs_src) copylen=mb->thislength-ofs_src;
		memcpy(((char *)dest_buf)+tot_copy,mb->datastart+ofs_src,copylen);
		copy_length-=copylen;
		tot_copy+=copylen;
		ofs_src=0;
		mb=mb->next;
		if(!mb) break;
	}
	return tot_copy;
}
int sgIP_memblock_CopyFromLinear(sgIP_memblock * mb, void * src_buf, int startbyte, int copy_length) {
	int copylen,ofs_src, tot_copy;
	ofs_src=startbyte;
	while(mb && ofs_src>=mb->thislength) { ofs_src-=mb->thislength; mb=mb->next; }
	if(!mb) return 0;
	if(startbyte+copy_length>mb->totallength) copy_length=mb->totallength-startbyte;
	if(copy_length<0) copy_length=0;
	tot_copy=0;
	while(copy_length>0) {
		copylen=copy_length;
		if(copylen>mb->thislength-ofs_src) copylen=mb->thislength-ofs_src;
		memcpy(mb->datastart+ofs_src,((char *)src_buf)+tot_copy,copylen);
		copy_length-=copylen;
		tot_copy+=copylen;
		ofs_src=0;
		mb=mb->next;
		if(!mb) break;
	}
	return tot_copy;

}
int sgIP_memblock_CopyBlock(sgIP_memblock * mb_src, sgIP_memblock * mb_dest, int start_src, int start_dest, int copy_length) {

	return 0;
}

