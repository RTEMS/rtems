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

#include "sgIP_sockets.h"
#include "sgIP_TCP.h"
#include "sgIP_UDP.h"
#include "sgIP_ICMP.h"
#include "sgIP_DNS.h"


sgIP_socket_data socketlist[SGIP_SOCKET_MAXSOCKETS];
extern unsigned long sgIP_timems;


void sgIP_sockets_Init() {
	int i;
	for(i=0;i<SGIP_SOCKET_MAXSOCKETS;i++) {
		socketlist[i].conn_ptr=0;
		socketlist[i].flags = 0;
	}
}

 // spawn/kill socket for internal use ONLY.
int spawn_socket(int flags) {
   int s;
   SGIP_INTR_PROTECT();
   for(s=0;s<SGIP_SOCKET_MAXSOCKETS;s++) if(!(socketlist[s].flags&SGIP_SOCKET_FLAG_ACTIVE)) break;
   if(s==SGIP_SOCKET_MAXSOCKETS) {
      SGIP_INTR_UNPROTECT();
      return SGIP_ERROR(ENOMEM);
   }
   socketlist[s].flags=SGIP_SOCKET_FLAG_ACTIVE | flags;
   socketlist[s].conn_ptr=0;
   SGIP_INTR_UNPROTECT();
   return s+1;
}
int kill_socket(int s) {
   if(s<1 || s>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EINVAL);
   SGIP_INTR_PROTECT();
   s--;
   socketlist[s].conn_ptr=0;
   socketlist[s].flags=0;
   SGIP_INTR_UNPROTECT();
   return 0;
}

int socket(int domain, int type, int protocol) {
	int s;
	if(domain!=AF_INET) return SGIP_ERROR(EINVAL);
	if(protocol!=0) return SGIP_ERROR(EINVAL);
	if(type!=SOCK_DGRAM && type!=SOCK_STREAM) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	for(s=0;s<SGIP_SOCKET_MAXSOCKETS;s++) if(!(socketlist[s].flags&SGIP_SOCKET_FLAG_ACTIVE)) break;
   if(s==SGIP_SOCKET_MAXSOCKETS) {
      SGIP_INTR_UNPROTECT();
      return SGIP_ERROR(ENOMEM);
   }
	if(type==SOCK_STREAM) {
		socketlist[s].flags=SGIP_SOCKET_FLAG_ACTIVE | SGIP_SOCKET_FLAG_TYPE_TCP;
		socketlist[s].conn_ptr=sgIP_TCP_AllocRecord();
	} else if(type==SOCK_DGRAM) {
		socketlist[s].flags=SGIP_SOCKET_FLAG_ACTIVE | SGIP_SOCKET_FLAG_TYPE_UDP;
		socketlist[s].conn_ptr=sgIP_UDP_AllocRecord();
	} else {
		SGIP_INTR_UNPROTECT();
		return SGIP_ERROR(EINVAL);
	}
#ifdef SGIP_SOCKET_DEFAULT_NONBLOCK
	socketlist[s].flags|=SGIP_SOCKET_FLAG_NONBLOCKING;
#endif
	SGIP_INTR_UNPROTECT();
	return s+1;
}


int closesocket(int socket) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	socket--;
	if(!(socketlist[socket].flags&SGIP_SOCKET_FLAG_ACTIVE)) { SGIP_INTR_UNPROTECT(); return 0; }
	if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
		sgIP_TCP_FreeRecord((sgIP_Record_TCP *)socketlist[socket].conn_ptr);
	} else if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
		sgIP_UDP_FreeRecord((sgIP_Record_UDP *)socketlist[socket].conn_ptr);
	}
	socketlist[socket].conn_ptr=0;
	socketlist[socket].flags=0;
	SGIP_INTR_UNPROTECT();
	return 0;
}

int bind(int socket, const struct sockaddr * addr, int addr_len) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EINVAL);
   if(addr_len!=sizeof(struct sockaddr_in)) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	int retval=SGIP_ERROR(EINVAL);
	socket--;
	if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
      retval=sgIP_TCP_Bind((sgIP_Record_TCP *)socketlist[socket].conn_ptr,((struct sockaddr_in *)addr)->sin_port,((struct sockaddr_in *)addr)->sin_addr.s_addr);
	} else if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
		retval=sgIP_UDP_Bind((sgIP_Record_UDP *)socketlist[socket].conn_ptr,((struct sockaddr_in *)addr)->sin_port,((struct sockaddr_in *)addr)->sin_addr.s_addr);
	}
	SGIP_INTR_UNPROTECT();
	return retval;
}
int connect(int socket, const struct sockaddr * addr, int addr_len) {
   if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EINVAL);
   if(addr_len!=sizeof(struct sockaddr_in)) return SGIP_ERROR(EINVAL);
   SGIP_INTR_PROTECT();
   int i;
   int retval=SGIP_ERROR(EINVAL);
   socket--;
   if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
      retval=sgIP_TCP_Connect((sgIP_Record_TCP *)socketlist[socket].conn_ptr,((struct sockaddr_in *)addr)->sin_addr.s_addr,((struct sockaddr_in *)addr)->sin_port);
	  if(retval==0) {
		do {
			i=((sgIP_Record_TCP *)socketlist[socket].conn_ptr)->tcpstate;
			if(i==SGIP_TCP_STATE_ESTABLISHED || i==SGIP_TCP_STATE_CLOSE_WAIT) {retval=0; break;}
			if(i==SGIP_TCP_STATE_CLOSED || i==SGIP_TCP_STATE_UNUSED || i==SGIP_TCP_STATE_LISTEN || i==SGIP_TCP_STATE_NODATA)
			{	retval=SGIP_ERROR(((sgIP_Record_TCP *)socketlist[socket].conn_ptr)->errorcode); break; }
			if(socketlist[socket].flags&SGIP_SOCKET_FLAG_NONBLOCKING) break;
			SGIP_INTR_UNPROTECT();
			SGIP_WAITEVENT();
			SGIP_INTR_REPROTECT();
		} while(1);
	  }
   }
   SGIP_INTR_UNPROTECT();
   return retval;
}
int send(int socket, const void * data, int sendlength, int flags) {
   if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return -1;
   SGIP_INTR_PROTECT();
   int retval=SGIP_ERROR(EINVAL);
   socket--;
   if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
       do {
           retval=sgIP_TCP_Send((sgIP_Record_TCP *)socketlist[socket].conn_ptr,data,sendlength,flags);
           if(retval!=-1) break;
           if(errno!=EWOULDBLOCK) break;
           if(socketlist[socket].flags&SGIP_SOCKET_FLAG_NONBLOCKING) break;
           SGIP_INTR_UNPROTECT();
           SGIP_WAITEVENT();
           SGIP_INTR_REPROTECT();
       } while(1);
   }
   SGIP_INTR_UNPROTECT();
   return retval;
}
int recv(int socket, void * data, int recvlength, int flags) {
   if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return -1;
   SGIP_INTR_PROTECT();
   int retval=SGIP_ERROR(EINVAL);
   socket--;
   if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
      do {
         retval=sgIP_TCP_Recv((sgIP_Record_TCP *)socketlist[socket].conn_ptr,data,recvlength,flags);
         if(retval!=-1) break;
         if(errno!=EWOULDBLOCK) break;
         if(socketlist[socket].flags&SGIP_SOCKET_FLAG_NONBLOCKING) break;
         SGIP_INTR_UNPROTECT();
         SGIP_WAITEVENT();
         SGIP_INTR_REPROTECT();
      } while(1);
   }
   SGIP_INTR_UNPROTECT();
   return retval;
}
int sendto(int socket, const void * data, int sendlength, int flags, const struct sockaddr * addr, int addr_len) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return -1;
	if(!addr) return -1;
	SGIP_INTR_PROTECT();
	int retval=SGIP_ERROR(EINVAL);
	socket--;
	if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
	} else if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
		retval=sgIP_UDP_SendTo((sgIP_Record_UDP *)socketlist[socket].conn_ptr,data,sendlength,flags,((struct sockaddr_in *)addr)->sin_addr.s_addr,((struct sockaddr_in *)addr)->sin_port);
	}
	SGIP_INTR_UNPROTECT();
	return retval;
}
int recvfrom(int socket, void * data, int recvlength, int flags, struct sockaddr * addr, int * addr_len) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return -1;
	if(!addr) return -1;
	SGIP_INTR_PROTECT();
	int retval=SGIP_ERROR(EINVAL);
	socket--;
	if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
	} else if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
      do {
         retval=sgIP_UDP_RecvFrom((sgIP_Record_UDP *)socketlist[socket].conn_ptr,data,recvlength,flags,&(((struct sockaddr_in *)addr)->sin_addr.s_addr),&(((struct sockaddr_in *)addr)->sin_port));
         if(retval!=-1) break;
         if(errno!=EWOULDBLOCK) break;
         if(socketlist[socket].flags&SGIP_SOCKET_FLAG_NONBLOCKING) break;
         SGIP_INTR_UNPROTECT(); // give interrupts a chance to occur.
         SGIP_WAITEVENT(); // don't just try again immediately
         SGIP_INTR_REPROTECT();
      } while(1);
		*addr_len = sizeof(struct sockaddr_in);
	}
	SGIP_INTR_UNPROTECT();
	return retval;
}
int listen(int socket, int max_connections) {
   if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EINVAL);
   SGIP_INTR_PROTECT();
   int retval=SGIP_ERROR(EINVAL);
   socket--;
   if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
      retval=sgIP_TCP_Listen((sgIP_Record_TCP *)socketlist[socket].conn_ptr,max_connections);
   }
   SGIP_INTR_UNPROTECT();
   return retval;
}
int accept(int socket, struct sockaddr * addr, int * addr_len) {
   if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS || !addr || !addr_len) return SGIP_ERROR(EINVAL);
   SGIP_INTR_PROTECT();
   sgIP_Record_TCP * ret;
   int retval,s;
   retval=SGIP_ERROR0(EINVAL);
   ret=0;
   socket--;
   if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
      s=spawn_socket((socketlist[socket].flags&SGIP_SOCKET_FLAG_NONBLOCKING) | SGIP_SOCKET_FLAG_TYPE_TCP);
      if(s>0) {
         do {
            ret=sgIP_TCP_Accept((sgIP_Record_TCP *)socketlist[socket].conn_ptr);
            if(ret!=0) break;
            if(errno!=EWOULDBLOCK) break;
            if(socketlist[socket].flags&SGIP_SOCKET_FLAG_NONBLOCKING) break;
            SGIP_INTR_UNPROTECT(); // give interrupts a chance to occur.
            SGIP_WAITEVENT(); // don't just try again immediately
            SGIP_INTR_REPROTECT();
         } while(1);
      }
      if(ret==0) {
         kill_socket(s);
         retval=-1;
      } else {
		 *addr_len=sizeof(struct sockaddr_in);
		 ((struct sockaddr_in *)addr)->sin_family=AF_INET;
		 ((struct sockaddr_in *)addr)->sin_port=ret->destport;
		 ((struct sockaddr_in *)addr)->sin_addr.s_addr=ret->destip;
         socketlist[s-1].conn_ptr=ret;
         retval=s;
      }
   }
   SGIP_INTR_UNPROTECT();
   return retval;
}
int shutdown(int socket, int shutdown_type) {
   if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EINVAL);
   SGIP_INTR_PROTECT();
   int retval=SGIP_ERROR(EINVAL);
   socket--;
   if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
	   retval=sgIP_TCP_Close((sgIP_Record_TCP *)socketlist[socket].conn_ptr);
   }
   SGIP_INTR_UNPROTECT();
   return retval;
}

int ioctl(int socket, long cmd, void * arg) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EBADF);
	socket--;
	int retval,i;
	retval=0;
	SGIP_INTR_PROTECT();
	switch(cmd) {
	case FIONBIO:
		if(!arg){
			retval=SGIP_ERROR(EINVAL);
		} else {
			socketlist[socket].flags &= ~SGIP_SOCKET_FLAG_NONBLOCKING;
			if(*((unsigned long *)arg)) socketlist[socket].flags |= SGIP_SOCKET_FLAG_NONBLOCKING;
		}
		break;
	case FIONREAD:
		if(!arg) {
			retval=SGIP_ERROR(EINVAL);
		} else {
			if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
				i=((sgIP_Record_TCP *)socketlist[socket].conn_ptr)->buf_rx_out-((sgIP_Record_TCP *)socketlist[socket].conn_ptr)->buf_rx_in;
				if(i<0) i+=SGIP_TCP_RECEIVEBUFFERLENGTH;
				*((int *)arg)=i;
			} else {
				retval=SGIP_ERROR(EINVAL);
			}
		}
	}
	SGIP_INTR_UNPROTECT();
	return retval;
}

int setsockopt(int socket, int level, int option_name, const void * data, int data_len) {
   return 0;
}
int getsockopt(int socket, int level, int option_name, void * data, int * data_len) {
   return 0;
}

int getpeername(int socket, struct sockaddr *addr, int * addr_len) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EBADF);
	if(!addr || !addr_len) return SGIP_ERROR(EFAULT);
	if(*addr_len<sizeof(struct sockaddr_in)) return SGIP_ERROR(EFAULT);
	socket--;
	SGIP_INTR_PROTECT();
	if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
		{
			struct sockaddr_in * sain = (struct sockaddr_in *)addr;
			sgIP_Record_TCP * rec = (sgIP_Record_TCP *)socketlist[socket].conn_ptr;
			if(rec->tcpstate!=SGIP_TCP_STATE_ESTABLISHED) {
				SGIP_INTR_UNPROTECT();
				return SGIP_ERROR(ENOTCONN);
			} else {
				sain->sin_addr.s_addr=rec->destip;
				sain->sin_family=AF_INET;
				sain->sin_port=rec->destport;
				*addr_len=sizeof(struct sockaddr_in);
			}
		}
	} else {
		SGIP_INTR_UNPROTECT();
		return SGIP_ERROR(EOPNOTSUPP);
	}
	SGIP_INTR_UNPROTECT();
	return 0;
}
int getsockname(int socket, struct sockaddr *addr, int * addr_len) {
	if(socket<1 || socket>SGIP_SOCKET_MAXSOCKETS) return SGIP_ERROR(EBADF);
	if(!addr || !addr_len) return SGIP_ERROR(EFAULT);
	if(*addr_len<sizeof(struct sockaddr_in)) return SGIP_ERROR(EFAULT);
	socket--;
	SGIP_INTR_PROTECT();
	if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
		{
			struct sockaddr_in * sain = (struct sockaddr_in *)addr;
			sgIP_Record_TCP * rec = (sgIP_Record_TCP *)socketlist[socket].conn_ptr;
			if(rec->tcpstate==SGIP_TCP_STATE_UNUSED || rec->tcpstate==SGIP_TCP_STATE_CLOSED) {
				SGIP_INTR_UNPROTECT();
				return SGIP_ERROR(EINVAL);
			} else {
				sain->sin_addr.s_addr=rec->srcip;
				sain->sin_family=AF_INET;
				sain->sin_port=rec->srcport;
				*addr_len=sizeof(struct sockaddr_in);
			}
		}
	} else if((socketlist[socket].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
        {
            struct sockaddr_in * sain = (struct sockaddr_in *)addr;
            sgIP_Record_UDP * rec = (sgIP_Record_UDP *)socketlist[socket].conn_ptr;
            if(rec->state==SGIP_UDP_STATE_UNUSED) {
                SGIP_INTR_UNPROTECT();
                return SGIP_ERROR(EINVAL);
            } else {
                sain->sin_addr.s_addr=rec->srcip;
                sain->sin_family=AF_INET;
                sain->sin_port=rec->srcport;
                *addr_len=sizeof(struct sockaddr_in);
            }
        }
	} else {
		SGIP_INTR_UNPROTECT();
		return SGIP_ERROR(EOPNOTSUPP);
	}
	SGIP_INTR_UNPROTECT();
	return 0;
}


struct hostent * gethostbyname(const char * name) {
   return (struct hostent *)sgIP_DNS_gethostbyname(name);
};


extern int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout) {
	// 31 days = 2678400 seconds
	unsigned long timeout_ms, lasttime, temp;
	sgIP_Record_TCP * rec;
	sgIP_Record_UDP * urec;
	lasttime=sgIP_timems;
	if(!timeout) timeout_ms=2678400000UL;
	else {
		if(timeout->tv_sec>=2678400) {
			timeout_ms=2678400000UL;
		} else {
			timeout_ms=timeout->tv_sec*1000 + (timeout->tv_usec/1000);
		}
	}
	SGIP_INTR_PROTECT();
	nfds=SGIP_SOCKET_MAXSOCKETS;

	int i,j,retval;
	while(timeout_ms>0) { // check all fd sets
		// readfds
		if(readfds) {
			for(i=0;i<nfds;i++) {
				if(FD_ISSET(i+1,readfds)) {
					if((socketlist[i].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
						rec = (sgIP_Record_TCP *)socketlist[i].conn_ptr;
						if(rec->buf_rx_in!=rec->buf_rx_out) { timeout_ms=0; break; }
					} else if((socketlist[i].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
						urec = (sgIP_Record_UDP *)socketlist[i].conn_ptr;
						if(urec->incoming_queue) { timeout_ms=0; break;	}
					}
				}
			}
			if(timeout_ms==0) break;
		}
		if(writefds) {
			// writefds
			for(i=0;i<nfds;i++) {
				if(FD_ISSET(i+1,writefds)) {
					if((socketlist[i].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
						rec = (sgIP_Record_TCP *)socketlist[i].conn_ptr;
						j=rec->buf_tx_in-1;
						if(j<0) j=SGIP_TCP_TRANSMITBUFFERLENGTH-1;
						if(rec->buf_tx_in!=j) { timeout_ms=0; break; }
					}
				}
			}
			if(timeout_ms==0) break;
		}
		// errorfds
		// ignore errorfds for now.

		temp=sgIP_timems-lasttime;
		if(timeout_ms<temp) timeout_ms=0; else timeout_ms -= temp;
		lasttime+=temp;
		SGIP_INTR_UNPROTECT(); // give interrupts a chance to occur.
		SGIP_WAITEVENT(); // don't just try again immediately
		SGIP_INTR_REPROTECT();
	}
	// markup fd sets and return
	// readfds
	retval=0;
	if(readfds) {
		for(i=0;i<nfds;i++) {
			if(FD_ISSET(i+1,readfds)) {
				if((socketlist[i].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
					rec = (sgIP_Record_TCP *)socketlist[i].conn_ptr;
					if(rec->buf_rx_in==rec->buf_rx_out) { FD_CLR(i+1,readfds);} else retval++;
				} else if((socketlist[i].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_UDP) {
					urec = (sgIP_Record_UDP *)socketlist[i].conn_ptr;
					if(!urec->incoming_queue) { FD_CLR(i+1,readfds);} else retval++;
				}
			}
		}
	}

	// writefds
	if(writefds) {
		for(i=0;i<nfds;i++) {
			if(FD_ISSET(i+1,writefds)) {
				if((socketlist[i].flags&SGIP_SOCKET_FLAG_TYPEMASK)==SGIP_SOCKET_FLAG_TYPE_TCP) {
					rec = (sgIP_Record_TCP *)socketlist[i].conn_ptr;
					j=rec->buf_tx_in-1;
					if(j<0) j=SGIP_TCP_TRANSMITBUFFERLENGTH-1;
					if(rec->buf_tx_in==j) { FD_CLR(i+1,writefds); } else retval++;
				}
			}
		}
	}

	if(errorfds) { FD_ZERO(errorfds); }

	SGIP_INTR_UNPROTECT();
	return retval;
}


/*
extern void FD_CLR(int fd, fd_set * fdset) {
	if(fd<1 || fd>FD_SETSIZE || !fdset) return;
	fd--;
	fdset->fdbits[fd>>5] &= ~(1<<(fd&31));
}
extern int FD_ISSET(int fd, fd_set * fdset) {
	if(fd<1 || fd>FD_SETSIZE || !fdset) return 0;
	fd--;
	return (fdset->fdbits[fd>>5] & 1<<(fd&31))?1:0;
}
extern void FD_SET(int fd, fd_set * fdset) {
	if(fd<1 || fd>FD_SETSIZE || !fdset) return;
	fd--;
	fdset->fdbits[fd>>5] |= 1<<(fd&31);
}
extern void FD_ZERO(fd_set * fdset) {
	int i;
	if(!fdset) return;
	for(i=0;i<(FD_SETSIZE+31)/32;i++) {
		fdset->fdbits[i]=0;
	}
}
*/


