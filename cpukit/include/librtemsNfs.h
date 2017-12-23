/**
 * @file
 *
 * @brief Public Interface to the NFS Client Library for RTEMS
 *
 * @ingroup rtems-nfsclient
 */

/*
 * Author: Till Straumann <strauman@slac.stanford.edu> 2002-2003
 *
 * Authorship
 * ----------
 * This software (NFS-2 client implementation for RTEMS) was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The NFS-2 client implementation for RTEMS was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#ifndef LIB_RTEMS_NFS_CLIENT_H
#define LIB_RTEMS_NFS_CLIENT_H

/**
 * @defgroup rtems-nfsclient NFS Client Library
 *
 * @ingroup nfsclient
 * @{
 */

#ifdef	HAVE_CONFIG_H
#include <config.h>
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <netdb.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/** RPCIO driver interface.
 * If you need RPCIO for other purposes than NFS
 * you may want to include <rpcio.h>
#include "rpcio.h"
 */

/** Priority of daemon; may be setup prior to calling rpcUdpInit();
 * otherwise the network task priority from the rtems_bsdnet_config
 * is used...
 */
extern rtems_task_priority rpciodPriority;

#ifdef RTEMS_SMP
/** CPU affinity of daemon; may be setup prior to calling rpcUdpInit();
 * otherwise the network task CPU affinity from the rtems_bsdnet_config
 * is used...
 */
extern const cpu_set_t *rpciodCpuset;
extern size_t rpciodCpusetSize;
#endif

/**
 * @brief Sets the XIDs of the RPC transaction hash table.
 *
 * The active RPC transactions are stored in a hash table.  Each table entry
 * contains the XID of its corresponding transaction.  The XID consists of two
 * parts.  The lower part is determined by the hash table index.  The upper
 * part is incremented in each send operation.
 *
 * This function sets the upper part of the XID in all hash table entries.
 * This can be used to ensure that the XIDs are not reused in a short interval
 * for example during a boot process or after resets.
 *
 * @param[in] xid The upper part is used to set the upper XID part of the hash
 * table entries.
 */
void
rpcSetXIDs(uint32_t xid);

/** Initialize the driver.
 *
 * Note, called in nfsfs initialise when mount is called.
 *
 * @retval 0 on success, -1 on failure
 */
int
rpcUdpInit(void);

/**
 * @brief RPC cleanup and stop.
 *
 * @retval 0 on success, nonzero if still in use
 */
int
rpcUdpCleanup(void);

/** NFS driver interface */

/**
 * @brief Initialize the NFS driver.
 *
 * The RPCIO driver must have been initialized prior to calling this.
 *
 * Note, called in nfsfs initialise when mount is called with defaults.
 *
 * ARGS:	depth of the small and big
 * 			transaction pools, i.e. how
 * 			many transactions (buffers)
 * 			should always be kept around.
 *
 * 			(If more transactions are needed,
 * 			they are created and destroyed
 * 			on the fly).
 *
 * 			Supply zero values to have the
 * 			driver chose reasonable defaults.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The errno is set to indicate the error.
 */
int
nfsInit(int smallPoolDepth, int bigPoolDepth);

/**
 * @brief Driver cleanup code.
 *
 * @retval 0 on success, nonzero if still in use
 */
int
nfsCleanup(void);

/**
 * @brief Dump a list of the currently mounted NFS to a file.
 *
 * Dump a list of the currently mounted NFS to a file
 * (stdout is used in case f==NULL)
 */
int
nfsMountsShow(FILE *f);

/**
 * @brief Filesystem mount table mount handler.
 *
 * Filesystem mount table mount handler. Do not call, use the mount call.
 */
int
rtems_nfs_initialize(rtems_filesystem_mount_table_entry_t *mt_entry,
                     const void                           *data);

/**
 * @brief A utility routine to find the path leading to a
 * rtems_filesystem_location_info_t node.
 *
 * This should really be present in libcsupport...
 *
 * @param[in] 'loc' and a buffer 'buf' (length 'len') to hold the path.
 *
 * @param[out] path copied into 'buf'
 *
 * @retval 0 on success, RTEMS error code on error.
 */
rtems_status_code
rtems_filesystem_resolve_location(char *buf, int len, rtems_filesystem_location_info_t *loc);

/**
 * @brief Set the timeout (initial default: 10s) for NFS and mount calls.
 *
 * Set the timeout (initial default: 10s) for NFS and mount calls.
 *
 * @retval 0 on success, nonzero if the requested timeout is less than
 * a clock tick or if the system clock rate cannot be determined.
 */

int
nfsSetTimeout(uint32_t timeout_ms);

/** Read current timeout (in milliseconds) */
uint32_t
nfsGetTimeout(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
