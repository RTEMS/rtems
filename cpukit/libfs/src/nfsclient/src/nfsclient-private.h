struct mbuf;
struct sockaddr;

ssize_t sendto_nocpy(int s, const void *buf, size_t buflen, int flags,
    const struct sockaddr *toaddr, int tolen, void *closure, void
    (*freeproc)(caddr_t, u_int), void (*refproc)(caddr_t, u_int));

ssize_t recv_mbuf_from(int s, struct mbuf **ppm, long len,
    struct sockaddr *fromaddr, int *fromlen);

struct __rpc_xdr;
enum xdr_op;

void xdrmbuf_create(struct __rpc_xdr *, struct mbuf *, enum xdr_op);
