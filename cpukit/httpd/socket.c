/*
 * socket.c -- Socket support module for UNIX
 *
 * Copyright (c) Go Ahead, 1995-1999
 */

/******************************** Description *********************************/

/*
 *	SCO Unix Socket Module.  This supports non-blocking buffered socket I/O.
 */

/********************************** Includes **********************************/

#include	<errno.h>
#include	<fcntl.h>
#include	<string.h>
#include	<stdlib.h>
#include	<unistd.h>

#include	"uemf.h"

/*********************************** Defines **********************************/

typedef struct {
	char			host[64];				/* Host name */
	ringq_t			inBuf;					/* Input ring queue */
	ringq_t			outBuf;					/* Output ring queue */
	ringq_t			lineBuf;				/* Line ring queue */
	socketAccept_t	accept;					/* Accept handler */
	socketHandler_t	handler;				/* User I/O handler */
	int				handler_data;			/* User handler data */
	int				sid;					/* Index into socket[] */
	int				port;					/* Port to listen on */
	int				flags;					/* Current state flags */
	int				readyMask;				/* Events now ready */
	int				interestMask;			/* Events interest */
	int				error;					/* Last error */
	int				sock;					/* Actual socket handle */
} socket_t;

/************************************ Locals **********************************/

static socket_t**	socketList;				/* List of open sockets */
static int			socketMax;				/* Maximum size of socket */
static int			socketHighestFd = -1;	/* Highest socket fd opened */

/***************************** Forward Declarations ***************************/

static int 	socketAlloc(char* host, int port, socketAccept_t accept, int flags);
static void socketFree(int sid);
static void socketAccept(socket_t* sp);
static int 	socketGetInput(int sid, char* buf, int toRead, int* errCode);
static int 	socketDoOutput(socket_t* sp, char* buf, int toWrite, int* errCode);
static int 	socketDoEvent(socket_t *sp);
static int	socketGetError();
static int 	socketWaitForEvent(socket_t* sp, int events, int* errCode);
static int	socketNonBlock(socket_t *sp);
static socket_t* socketPtr(int sid);

/*********************************** Code *************************************/
/*
 *	Open socket module
 */

int socketOpen()
{
	return 0;
}

/******************************************************************************/
/*
 *	Close the socket module, by closing all open connections
 */

void socketClose()
{
	int		i;

	for (i = socketMax; i >= 0; i--) {
		if (socketList && socketList[i]) {
			socketCloseConnection(i);
		}
	}
}

/******************************************************************************/
/*
 *	Open a client or server socket. Host is NULL if we want server capability.
 */

int socketOpenConnection(char* host, int port, socketAccept_t accept, int flags)
{
	socket_t			*sp;
	struct sockaddr_in 	sockaddr;
	struct hostent 		*hostent;		/* Host database entry */
	int					sid, rc;

/*
 *	Allocate a socket structure
 */
	if ((sid = socketAlloc(host, port, accept, flags)) < 0) {
		return -1;
	}
	sp = socketList[sid];
	a_assert(sp);

/*
 *	Create the socket address structure
 */
	memset((char *) &sockaddr, '\0', sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons((short) (port & 0xFFFF));

	if (host == NULL) {
		sockaddr.sin_addr.s_addr = INADDR_ANY;
	} else {
		sockaddr.sin_addr.s_addr = inet_addr(host);
		if (sockaddr.sin_addr.s_addr == INADDR_NONE) {
			hostent = gethostbyname(host);
			if (hostent != NULL) {
				memcpy((char *) &sockaddr.sin_addr, 
					(char *) hostent->h_addr_list[0],
					(size_t) hostent->h_length);
			} else {
				errno = ENXIO;
				socketFree(sid);
				return -1;
			}
		}
	}

/*
 *	Create the socket. Set the close on exec flag so children don't 
 *	inherit the socket.
 */
	sp->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sp->sock < 0) {
		socketFree(sid);
		return -1;
	}
	fcntl(sp->sock, F_SETFD, FD_CLOEXEC);
	socketHighestFd = max(socketHighestFd, sp->sock);

/*
 *	Host is set if we are the client
 */
	if (host) {
/*
 *		Connect to the remote server
 */
		if (connect(sp->sock, (struct sockaddr *) &sockaddr,
				sizeof(sockaddr)) < 0) {
			socketFree(sid);
			return -1;
		}
		socketNonBlock(sp);

	} else {
/*
 *		Bind to the socket endpoint with resule and the call listen()  
 **		to start listening
 */
		rc = 1;
		setsockopt(sp->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&rc, sizeof(rc));
		if (bind(sp->sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) 
				< 0) {
			socketFree(sid);
			return -1;
		}
		sp->flags |= SOCKET_LISTENING;

		if (listen(sp->sock, SOMAXCONN) < 0) {
			socketFree(sid);
			return -1;
		}
		sp->interestMask = SOCKET_READABLE;
	}
	return sid;
}

/******************************************************************************/
/*
 *	Close a socket
 */

void socketCloseConnection(int sid)
{
	socket_t*	sp;

	if ((sp = socketPtr(sid)) == NULL) {
		return;
	}

/* 
 *	We always flush all output before closing. Unlink from the emf event
 *	mechanism and then free (and close) the connection
 */
	socketFlush(sid, 1);
	socketFree(sid);
}

/******************************************************************************/
/*
 *	Accept a connection. Called by socketDoEvent
 */

static void socketAccept(socket_t* sp)
{
	struct sockaddr_in	addr;
	socket_t 			*nsp;
	int				len;
	int 				newSock, nid;

	a_assert(sp);

/*
 *	Accept the connection and prevent inheriting by children (F_SETFD)
 */
	len = sizeof(struct sockaddr_in);
	if ((newSock = accept(sp->sock, (struct sockaddr *) &addr, &len)) < 0) {
		return;
	}
	fcntl(newSock, F_SETFD, FD_CLOEXEC);
	socketHighestFd = max(socketHighestFd, newSock);

/*
 *	Create a socket structure and insert into the socket list
 */
	nid = socketAlloc(sp->host, sp->port, sp->accept, 0);
	nsp = socketList[nid];
	a_assert(nsp);
	nsp->sock = newSock;

	if (nsp == NULL) {
		return;
	}
/*
 *	Call the user accept callback, the user must call socketCreateHandler
 *	to register for further events of interest.
 */
	if (sp->accept != NULL) {
		if ((sp->accept)(nid, inet_ntoa(addr.sin_addr), 
				ntohs(addr.sin_port)) < 0) {
			socketFree(nid);
			return;
		}
	}
	socketNonBlock(nsp);
}

/******************************************************************************/
/*
 *	Write to a socket. This may block if the underlying socket cannot
 *	absorb the data. Returns -1 on error, otherwise the number of bytes 
 *	written.
 */

int	socketWrite(int sid, char* buf, int bufsize)
{
	socket_t*	sp;
	ringq_t*	rq;
	int			len, bytesWritten, room;

	a_assert(buf);
	a_assert(bufsize >= 0);

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}
	
/*
 *	Loop adding as much data to the output ringq as we can absorb
 *	Flush when the ringq is too full and continue.
 */
	rq = &sp->outBuf;
	for (bytesWritten = 0; bufsize > 0; ) {
		if ((room = ringqPutBlkMax(rq)) == 0) {
			if (socketFlush(sid, 0) < 0) {
				return -1;
			}
			if ((room = ringqPutBlkMax(rq)) == 0) {
				break;
			}
			continue;
		}
		len = min(room, bufsize);
		ringqPutBlk(rq, (unsigned char*) buf, len);
		bytesWritten += len;
		bufsize -= len;
		buf += len;
	}
	return bytesWritten;
}

/******************************************************************************/
/*
 *	Read from a socket. Return the number of bytes read if successful. This
 *	may be less than the requested "bufsize" and may be zero. Return -1 for
 *	errors. Return 0 for EOF. Otherwise return the number of bytes read. Since
 *	this may be zero, callers should use socketEof() to distinguish between
 *	this and EOF. Note: this ignores the line buffer, so a previous socketGets 
 *	which read a partial line may cause a subsequent socketRead to miss 
 *	some data. 
 */

int	socketRead(int sid, char* buf, int bufsize)
{
	socket_t*	sp;
	ringq_t*	rq;
	int			len, room, errCode, bytesRead;

	a_assert(buf);
	a_assert(bufsize > 0);

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}

	if (sp->flags & SOCKET_EOF) {
		return 0;
	}

	rq = &sp->inBuf;
	for (bytesRead = 0; bufsize > 0; ) {
		len = min(ringqLen(rq), bufsize);
		if (len <= 0) {
			room = ringqPutBlkMax(rq);
			len = socketGetInput(sid, (char*) rq->endp, room, &errCode);
			if (len < 0) {
				if (errCode == EWOULDBLOCK) {
					if (bytesRead >= 0) {
						return bytesRead;
					}
				}
				return -1;

			} else if (len == 0) {
/*
 *				This is EOF, but we may have already read some data so pass that
 *				back first before notifying EOF. The next read will return 0
 *				to indicate EOF.
 */
				return bytesRead;
			}
			ringqPutBlkAdj(rq, len);
			len = min(len, bufsize);
		}
		memcpy(&buf[bytesRead], rq->servp, len);
		ringqGetBlkAdj(rq, len);
		bufsize -= len;
		bytesRead += len;
	}
	return bytesRead;
}

/******************************************************************************/
/*
 *	Get a string from a socket. This returns data in *buf in a malloced string 
 *	after trimming the '\n'. If there is zero bytes returned, *buf will be set
 *	to NULL. It returns -1 for error, EOF or when no complete line yet read. 
 *	Otherwise the length of the line is returned. If a partial line is read
 * 	socketInputBuffered or socketEof can be used to distinguish between EOF 
 *	and partial line still buffered. This routine eats and ignores carriage
 *  returns.
 */

int	socketGets(int sid, char** buf)
{
	socket_t*	sp;
	ringq_t*	lq;
	char		c;
	int			rc, len;

	a_assert(buf);

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}
	lq = &sp->lineBuf;

	while (1) {

		if ((rc = socketRead(sid, &c, 1)) < 0) {
			return rc;

		} else if (rc == 0) {
/*
 *			If there is a partial line and we are at EOF, pretend we saw a '\n'
 */
			if (ringqLen(lq) > 0 && (sp->flags & SOCKET_EOF)) {
				c = '\n';
			} else {
				return -1;
			}
		}
/*
 *		If a newline is seen, return the data excluding the new line to the
 *		caller. If carriage return is seen, just eat it.
 */
		if (c == '\n') {
			len = ringqLen(lq);
			if (len > 0) {
				if ((*buf = balloc(B_L, len + 1)) == NULL) {
					return -1;
				}
				memset(*buf, 0, len + 1);
				ringqGetBlk(lq, (unsigned char*) *buf, len);
			} else {
				*buf = NULL;
			}
			return len;

		} else if (c == '\r') {
			continue;
		}
		ringqPutc(lq, c);
	}
}

/******************************************************************************/
/*
 *	Flush a socket. Do not wait, just initiate output and return.
 *	This will return -1 on errors and 0 if successful.
 */

int socketFlush(int sid, int block)
{
	socket_t*	sp;
	ringq_t*	rq;
	int			len, bytesWritten, errCode;

	a_assert(block == 0 || block == 1);

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}
	rq = &sp->outBuf;

/*
 *	Set the background flushing flag which socketDoEvent will check to
 *	continue the flush.
 */
	if (!block) {
		sp->flags |= SOCKET_FLUSHING;
	}

/*
 *	Break from loop if not blocking after initiating output. If we are blocking
 *	we wait for a write event.
 */
	while (ringqLen(rq) > 0) {
		len = ringqGetBlkMax(&sp->outBuf);
		bytesWritten = socketDoOutput(sp, (char*) rq->servp, len, &errCode);
		if (bytesWritten < 0) {
			if (errCode == EINTR) {
				continue;
			} else if (errCode == EWOULDBLOCK || errCode == EAGAIN) {
				if (! block) {
					return 0;
				}
				if (socketWaitForEvent(sp, SOCKET_WRITABLE | SOCKET_EXCEPTION, 
						&errCode)) {
					continue;
				}
			}
			return -1;
		}
		ringqGetBlkAdj(rq, bytesWritten);
		if (! block) {
			break;
		}
	}
	return 0;
}

/******************************************************************************/
/*
 *	Return the count of input characters buffered. We look at both the line
 *	buffer and the input (raw) buffer. Return -1 on error or EOF.
 */

int socketInputBuffered(int sid)
{
	socket_t*	sp;

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}
	if (socketEof(sid)) {
		return -1;
	}
	return ringqLen(&sp->lineBuf) + ringqLen(&sp->inBuf);
}

/******************************************************************************/
/*
 *	Return true if EOF
 */

int socketEof(int sid)
{
	socket_t*	sp;

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}
	return sp->flags & SOCKET_EOF;
}

/******************************************************************************/
/*
 *	Create a user handler for this socket. The handler called whenever there
 *	is an event of interest as defined by interestMask (SOCKET_READABLE, ...)
 */

void socketCreateHandler(int sid, int interestMask, socketHandler_t handler, 
	int data)
{
	socket_t*	sp;

	if ((sp = socketPtr(sid)) == NULL) {
		return;
	}
	sp->handler = handler;
	sp->handler_data = data;
	sp->interestMask = interestMask;
}

/******************************************************************************/
/*
 *	Delete a handler
 */

void socketDeleteHandler(int sid)
{
	socket_t*	sp;

	if ((sp = socketPtr(sid)) == NULL) {
		return;
	}
	sp->handler = NULL;
	sp->interestMask = 0;
}

/******************************************************************************/
/*
 *	Get more input from the socket and return in buf.
 *	Returns 0 for EOF, -1 for errors and otherwise the number of bytes read.
 */

static int socketGetInput(int sid, char* buf, int toRead, int* errCode)
{
	struct sockaddr_in 	server;
	socket_t*			sp;
	int 				len, bytesRead;
	
	a_assert(buf);
	a_assert(errCode);

	*errCode = 0;

	if ((sp = socketPtr(sid)) == NULL) {
		return -1;
	}

/*
 *	If we have previously seen an EOF condition, then just return
 */
	if (sp->flags & SOCKET_EOF) {
		return 0;
	}

/*
 *	Read the data
 */
	if (sp->flags & SOCKET_BROADCAST) {
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_BROADCAST;
		server.sin_port = htons((short)(sp->port & 0xFFFF));
		len = sizeof(server);
		bytesRead = recvfrom(sp->sock, buf, toRead, 0, 
			(struct sockaddr*) &server, &len);
	} else {
		bytesRead = recv(sp->sock, buf, toRead, 0);
	}

	if (bytesRead < 0) {
		if (errno == ECONNRESET) {
			return 0;
		}
		*errCode = socketGetError();
		return -1;

	} else if (bytesRead == 0) {
		sp->flags |= SOCKET_EOF;
	} 
	return bytesRead;
}

/******************************************************************************/
/*
 *	Socket output procedure. Return -1 on errors otherwise return the number 
 *	of bytes written.
 */

static int socketDoOutput(socket_t* sp, char* buf, int toWrite, int* errCode)
{
	struct sockaddr_in	server;
	int 				bytes;

	a_assert(sp);
	a_assert(buf);
	a_assert(toWrite > 0);
	a_assert(errCode);

	*errCode = 0;

/*
 *	Write the data
 */
	if (sp->flags & SOCKET_BROADCAST) {
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_BROADCAST;
		server.sin_port = htons((short)(sp->port & 0xFFFF));
		bytes = sendto(sp->sock, buf, toWrite, 0, 
			(struct sockaddr*) &server, sizeof(server));
	} else {
		bytes = send(sp->sock, buf, toWrite, 0);
	}

	if (bytes == 0 && bytes != toWrite) {
		*errCode = EWOULDBLOCK;
		return -1;
	}

	if (bytes < 0) {
		*errCode = socketGetError();
	}
	return bytes;
}

/******************************************************************************/
/*
 *	Return TRUE if there is a socket with an event ready to process,
 */

int socketReady()
{
	socket_t 	*sp;
	int			i;
	
	for (i = 0; i < socketMax; i++) {
		if ((sp = socketList[i]) == NULL) {
			continue;
		} 
		if (sp->readyMask & sp->interestMask) {
			return 1;
		}
	}
	return 0;
}

/******************************************************************************/
/*
 * 	Wait for a handle to become readable or writable and return a number of 
 *	noticed events.
 */

int socketSelect()
{
	socket_t	*sp;
	fd_mask 	*readFds, *writeFds, *exceptFds;
	int 		sid, len, nwords, index, bit, nEvents;

/*
 *	Allocate and zero the select masks
 */
	nwords = (socketHighestFd + NFDBITS - 1) / NFDBITS;
	len = nwords * sizeof(int);

	readFds = balloc(B_L, len);
	memset(readFds, 0, len);
	writeFds = balloc(B_L, len);
	memset(writeFds, 0, len);
	exceptFds = balloc(B_L, len);
	memset(exceptFds, 0, len);

/*
 *	Set the select event masks for events to watch
 */
	for (sid = 0; sid < socketMax; sid++) {
		if ((sp = socketList[sid]) == NULL) {
			continue;
		}
		a_assert(sp);

/*
 * 		Initialize the ready masks and compute the mask offsets.
 */
		index = sp->sock / (NBBY * sizeof(fd_mask));
		bit = 1 << (sp->sock % (NBBY * sizeof(fd_mask)));
		
/*
 * 		Set the appropriate bit in the ready masks for the sp->sock.
 */
		if (sp->interestMask & SOCKET_READABLE) {
			readFds[index] |= bit;
		}
		if (sp->interestMask & SOCKET_WRITABLE) {
			writeFds[index] |= bit;
		}
		if (sp->interestMask & SOCKET_EXCEPTION) {
			exceptFds[index] |= bit;
		}
	}

/*
 * 	Wait for the event or a timeout.
 */
	nEvents = select(socketHighestFd + 1, (fd_set *) readFds,
		(fd_set *) writeFds, (fd_set *) exceptFds, NULL);
	if (nEvents > 0) {
		for (sid = 0; sid < socketMax; sid++) {
			if ((sp = socketList[sid]) == NULL) {
				continue;
			}

			index = sp->sock / (NBBY * sizeof(fd_mask));
			bit = 1 << (sp->sock % (NBBY * sizeof(fd_mask)));
		
			if (readFds[index] & bit) {
				sp->readyMask |= SOCKET_READABLE;
			}
			if (writeFds[index] & bit) {
				sp->readyMask |= SOCKET_WRITABLE;
			}
			if (exceptFds[index] & bit) {
				sp->readyMask |= SOCKET_EXCEPTION;
			}
		}
	}

	bfree(B_L, readFds);
	bfree(B_L, writeFds);
	bfree(B_L, exceptFds);

	return nEvents;
}

/******************************************************************************/
/*
 *	Process socket events
 */

void socketProcess()
{
	socket_t	*sp;
	int			sid;

/*
 * 	Process each socket
 */
	for (sid = 0; sid < socketMax; sid++) {
		if ((sp = socketList[sid]) == NULL) {
			continue;
		}
		if ((sp->readyMask & sp->interestMask) ||
				((sp->interestMask & SOCKET_READABLE) && 
				socketInputBuffered(sid))) {
			socketDoEvent(sp);
		}
	}
}

/******************************************************************************/
/*
 *	Process and event on the event queue
 */

static int socketDoEvent(socket_t *sp)
{
	ringq_t*		rq;
	int 			sid;

	a_assert(sp);

    sid = sp->sid;
	if (sp->readyMask & SOCKET_READABLE) {
		if (sp->flags & SOCKET_LISTENING) {
			socketAccept(sp);
			sp->readyMask = 0;
			return 1;
		}
	} else {
/*
 *		If there is still read data in the buffers, trigger the read handler
 *		NOTE: this may busy spin if the read handler doesn't read the data
 */
		if (sp->interestMask & SOCKET_READABLE && socketInputBuffered(sid)) {
			sp->readyMask |= SOCKET_READABLE;
		}
	}


/*
 *	If now writable and flushing in the background, continue flushing
 */
	if (sp->readyMask & SOCKET_WRITABLE) {
		if (sp->flags & SOCKET_FLUSHING) {
			rq = &sp->outBuf;
			if (ringqLen(rq) > 0) {
				socketFlush(sp->sid, 0);
			} else {
				sp->flags &= ~SOCKET_FLUSHING;
			}
		}
	}

/*
 *	Now invoke the users socket handler. NOTE: the handler may delete the
 *	socket, so we must be very careful after calling the handler.
 */
	if (sp->handler && (sp->interestMask & sp->readyMask)) {
		(sp->handler)(sid, sp->interestMask & sp->readyMask, 
			sp->handler_data);
/*
 *		Make sure socket pointer is still valid, then set the readyMask
 *		to 0.
 */ 
		if (socketPtr(sid)) {
			sp->readyMask = 0;
		}
	}
	return 1;
}

/******************************************************************************/
/*
 *	Allocate a new socket structure
 */

static int socketAlloc(char* host, int port, socketAccept_t accept, int flags)
{
	socket_t	*sp;
	int			sid;

	if ((sid = hAlloc((void***) &socketList)) < 0) {
		return -1;
	}
	if ((sp = (socket_t*) balloc(B_L, sizeof(socket_t))) == NULL) {
		hFree((void***) &socket, sid);
		return -1;
	}
	memset(sp, 0, sizeof(socket_t));
	socketList[sid] = sp;
	if (sid >= socketMax)
		socketMax = sid + 1;

	sp->sid = sid;
	sp->accept = accept;
	sp->port = port;
	sp->flags = flags;

	if (host) {
		strncpy(sp->host, host, sizeof(sp->host));
	}

	ringqOpen(&sp->inBuf, SOCKET_BUFSIZ, SOCKET_BUFSIZ);
	ringqOpen(&sp->outBuf, SOCKET_BUFSIZ, SOCKET_BUFSIZ);
	ringqOpen(&sp->lineBuf, SOCKET_BUFSIZ, -1);

	return sid;
}

/******************************************************************************/
/*
 *	Free a socket structure
 */

static void socketFree(int sid)
{
	socket_t*	sp;
	int			i;

	if ((sp = socketPtr(sid)) == NULL) {
		return;
	}
	if (sp->sock >= 0) {
		close(sp->sock);
	}

	ringqClose(&sp->inBuf);
	ringqClose(&sp->outBuf);
	ringqClose(&sp->lineBuf);

	bfree(B_L, sp);
	socketMax = hFree((void***) &socketList, sid);

/*
 *	Calculate the new highest socket number
 */
	socketHighestFd = -1;
	for (i = 0; i < socketMax; i++) {
		if ((sp = socketList[i]) == NULL) {
			continue;
		} 
		socketHighestFd = max(socketHighestFd, sp->sock);
	}
}

/******************************************************************************/
/*
 *	Validate a socket handle
 */

static socket_t* socketPtr(int sid)
{
	if (sid < 0 || sid >= socketMax || socketList[sid] == NULL) {
		a_assert(NULL);
		return NULL;
	}

	a_assert(socketList[sid]);
	return socketList[sid];
}

/******************************************************************************/
/*
 *	Get the operating system error code
 */

static int socketGetError()
{
	return errno;
}

/******************************************************************************/
/*
 *	Wait until an event occurs on a socket. Return 1 on success, 0 on failure.
 */

static int socketWaitForEvent(socket_t* sp, int interestMask, int* errCode)
{
	a_assert(sp);

	while (socketSelect()) {
		if (sp->readyMask & interestMask) {
			break;
		}
	}
	if (sp->readyMask & SOCKET_EXCEPTION) {
		return -1;
	} else if (sp->readyMask & SOCKET_WRITABLE) {
		return 0;
	} else {
		*errCode = errno = EWOULDBLOCK;
		return -1;
	}
}

/******************************************************************************/
/*
 *	Put the socket into non-blocking mode
 */

static int socketNonBlock(socket_t *sp)
{
	int		flags;

	flags = fcntl(sp->sock, F_GETFL) | O_NONBLOCK;
	if (fcntl(sp->sock, F_SETFL, flags) < 0) {
		return -1;
    }
	return 0;
}

/******************************************************************************/
/*
 *	Duplicate stdin and stdout
 */

int DuplicateStdFile (int sid)
{
  if (0 != dup2(socketList[sid]->sock, 0)  || 1 != dup2(socketList[sid]->sock, 1))
    return -1;
  
  return 0;

}
