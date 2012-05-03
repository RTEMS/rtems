/**
 * @file rtems/rtems_bsdnet.h
 */


#ifndef _RTEMS_BSDNET_SERVERS_H
#define _RTEMS_BSDNET_SERVERS_H

extern struct in_addr *rtems_bsdnet_ntpserver;
extern int rtems_bsdnet_ntpserver_count;

/*
 * Network configuration
 */
extern struct in_addr *rtems_bsdnet_nameserver;
extern int rtems_bsdnet_nameserver_count;

#endif /* _RTEMS_BSDNET_SERVERS_H */
