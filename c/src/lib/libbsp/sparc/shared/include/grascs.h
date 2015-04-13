/*
 * Header file for GRASCS RTEMS driver
 *
 * COPYRIGHT (c) 2008.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRASCS_H__
#define __GRASCS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Minimum and maximum system frequency */
#define GRASCS_MIN_SFREQ 10000
#define GRASCS_MAX_SFREQ 255000

/* Default, minimum and maximum ETR pulse frequency */
#define GRASCS_DEFAULT_ETRFREQ 10
#define GRASCS_MIN_ETRFREQ 1
#define GRASCS_MAX_ETRFREQ 100

/* Maximum number of external time markers */
#define GRASCS_MAX_TMS 6

/* Error codes */
#define GRASCS_ERROR_STARTSTOP 1 /* Serial/synch interface is running/stopped */
#define GRASCS_ERROR_TRANSACTIVE 2 /* Busy with transaction */
#define GRASCS_ERROR_CAPFAULT 3 /* Core capabilities prohibit request */

/* Command register */
#define GRASCS_CMD_RESET (1 << 0)
#define GRASCS_CMD_STARTSTOP (1 << 1)
#define GRASCS_CMD_ESTARTSTOP (1 << 2)
#define GRASCS_CMD_SENDTM (1 << 3)
#define GRASCS_CMD_ETRCTRL (7 << 4)
#define GRASCS_CMD_ETRCTRL_BITS 4
#define GRASCS_CMD_SLAVESEL (15 << 8)
#define GRASCS_CMD_SLAVESEL_BITS 8
#define GRASCS_CMD_TCDONE (1 << 12)
#define GRASCS_CMD_TMDONE (1 << 13)
#define GRASCS_CMD_US1 (255 << 16)
#define GRASCS_CMD_US1_BITS 16
#define GRASCS_CMD_US1C (1 << 24)

/* Clock scale register */
#define GRASCS_CLK_ETRFREQ_BITS 12

/* Status register */
#define GRASCS_STS_RUNNING (1 << 0)
#define GRASCS_STS_ERUNNING (1 << 1)
#define GRASCS_STS_TCDONE (1 << 4)
#define GRASCS_STS_TMDONE (1 << 5)
#define GRASCS_STS_DBITS_BITS 8
#define GRASCS_STS_NSLAVES_BITS 13
#define GRASCS_STS_USCONF_BITS 18
#define GRASCS_STS_TMCONF_BITS 19

extern int ASCS_init(void);

extern int ASCS_input_select(int slave);

extern int ASCS_etr_select(int etr, int freq);

extern void ASCS_start(void);

extern void ASCS_stop(void);

extern int ASCS_iface_status(void);

extern int ASCS_TC_send(int *word);

extern int ASCS_TC_send_block(int *block, int ntrans);

extern void ASCS_TC_sync_start(void);

extern void ASCS_TC_sync_stop(void);

extern int ASCS_TM_recv(int *word);

extern int ASCS_TM_recv_block(int *block, int ntrans);

#ifdef __cplusplus
}
#endif

#endif
