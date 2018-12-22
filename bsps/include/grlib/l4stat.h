/*
 * L4STAT APB-Register Driver.
 * 
 * COPYRIGHT (c) 2017.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __L4STAT_H__
#define __L4STAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define L4STAT_ERR_OK 0
#define L4STAT_ERR_EINVAL -1
#define L4STAT_ERR_ERROR -2
#define L4STAT_ERR_TOOMANY -3
#define L4STAT_ERR_IMPLEMENTED -4

/* Register L4STAT driver to Driver Manager */
void l4stat_register_drv (void);

extern int l4stat_counter_print(unsigned int counter);

/*
 * L4STAT CCTRL register fields
 */
#define CCTRL_NCPU (0xf << CCTRL_NCPU_BIT)
#define CCTRL_NCNT_L3STAT (0x1ff << CCTRL_NCNT_BIT)
#define CCTRL_NCNT (0x1f << CCTRL_NCNT_BIT)
#define CCTRL_MC (0x1 << CCTRL_MC_BIT)
#define CCTRL_IA (0x1 << CCTRL_IA_BIT)
#define CCTRL_DS (0x1 << CCTRL_DS_BIT)
#define CCTRL_EE (0x1 << CCTRL_EE_BIT)
#define CCTRL_AE (0x1 << CCTRL_AE_BIT)
#define CCTRL_EL (0x1 << CCTRL_EL_BIT)
#define CCTRL_CD (0x1 << CCTRL_CD_BIT)
#define CCTRL_SU (0x3 << CCTRL_SU_BIT)
#define CCTRL_CL (0x1 << CCTRL_CL_BIT)
#define CCTRL_EN (0x1 << CCTRL_EN_BIT)
#define CCTRL_EVENTID (0xff << CCTRL_EVENTID_BIT)
#define CCTRL_CPUAHBM (0xf << CCTRL_CPUAHBM_BIT)

#define CCTRL_NCPU_BIT 28
#define CCTRL_NCNT_BIT 23
#define CCTRL_MC_BIT 22
#define CCTRL_IA_BIT 21
#define CCTRL_DS_BIT 20
#define CCTRL_EE_BIT 19
#define CCTRL_AE_BIT 18
#define CCTRL_EL_BIT 17
#define CCTRL_CD_BIT 16
#define CCTRL_SU_BIT 14
#define CCTRL_CL_BIT 13
#define CCTRL_EN_BIT 12
#define CCTRL_EVENTID_BIT 4
#define CCTRL_CPUAHBM_BIT 0

#define L4STAT_OPTIONS_EVENT_LEVEL_ENABLE CCTRL_EL 
#define L4STAT_OPTIONS_EVENT_LEVEL_DISABLE 0
#define L4STAT_OPTIONS_MAXIMUM_DURATION CCTRL_CD
#define L4STAT_OPTIONS_SUPERVISOR_MODE_FILTER (0x1 << CCTRL_SU_BIT)
#define L4STAT_OPTIONS_USER_MODE_FILTER (0x2 << CCTRL_SU_BIT)
#define L4STAT_OPTIONS_NO_FILTER 0
#define L4STAT_OPTIONS_CLEAR_ON_READ CCTRL_CL

extern int l4stat_counter_enable(unsigned int counter, int event, int cpu, int options);
extern int l4stat_counter_disable(unsigned int counter);
extern int l4stat_counter_set(unsigned int counter, uint32_t val);
extern int l4stat_counter_get(unsigned int counter, uint32_t *val);

static inline int l4stat_counter_clear(unsigned int counter)
{
	return l4stat_counter_set(counter, 0);
}

extern int l4stat_counter_max_set(unsigned int counter, uint32_t val);
extern int l4stat_counter_max_get(unsigned int counter, uint32_t *val);
extern int l4stat_tstamp_set(uint32_t val);
extern int l4stat_tstamp_get(uint32_t *val);

#ifdef __cplusplus
}
#endif

#endif
