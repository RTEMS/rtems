#include <sys/cdefs.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/priority.h>

#ifndef FALSE
#define	FALSE	0
#endif
#ifndef TRUE
#define	TRUE	1
#endif

#ifndef _BYTEORDER_PROTOTYPED
#define	_BYTEORDER_PROTOTYPED
__BEGIN_DECLS
__uint32_t	 htonl(__uint32_t);
__uint16_t	 htons(__uint16_t);
__uint32_t	 ntohl(__uint32_t);
__uint16_t	 ntohs(__uint16_t);
__END_DECLS
#endif

#ifndef _BYTEORDER_FUNC_DEFINED
#define	_BYTEORDER_FUNC_DEFINED
#define	htonl(x)	__htonl(x)
#define	htons(x)	__htons(x)
#define	ntohl(x)	__ntohl(x)
#define	ntohs(x)	__ntohs(x)
#endif /* !_BYTEORDER_FUNC_DEFINED */
