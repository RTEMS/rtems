/* Subroutines from cpukit/libnetworking/nfs/bootp_subr.c */

#if !defined (__RTEMS_BOOTP_H__)
#define __RTEMS_BOOTP_H__

#include <stdbool.h>

#if __cplusplus
extern "C"
{
#endif

struct bootp_packet;
struct proc;
struct ifreq;
struct socket;
struct sockaddr_in;

bool bootpc_init(bool, bool);

int bootpc_call(
     struct bootp_packet *call,
     struct bootp_packet *reply,
     struct proc *procp);
int bootpc_fakeup_interface(struct ifreq *ireq,
                                    struct socket *so,
                                    struct proc *procp);
int bootpc_adjust_interface(struct ifreq *ireq,
                                    struct socket *so,
                                    struct sockaddr_in *myaddr,
                                    struct sockaddr_in *netmask,
                                    struct sockaddr_in *gw,
                                    struct proc *procp);

void *bootp_strdup_realloc(char *dst, const char *src);

#if __cplusplus
}
#endif

#endif
