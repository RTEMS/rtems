/*
 *  i960ha
 *
 *  $Id$
 */

#ifndef __i960HA_h
#define __i960HA_h



/* i960HA control structures */

/* Intel i960HA Control Table */

typedef struct {
                            /* Control Group 0 */
  unsigned int ipb0;              /* IP breakpoint 0 */
  unsigned int ipb1;              /* IP breakpoint 1 */
  unsigned int dab0;              /* data address breakpoint 0 */
  unsigned int dab1;              /* data address breakpoint 1 */
                            /* Control Group 1 */
  unsigned int imap0;             /* interrupt map 0 */
  unsigned int imap1;             /* interrupt map 1 */
  unsigned int imap2;             /* interrupt map 2 */
  unsigned int icon;              /* interrupt control */
                            /* Control Group 2 */
  unsigned int mcon0;             /* memory region 0 configuration */
  unsigned int mcon1;             /* memory region 1 configuration */
  unsigned int mcon2;             /* memory region 2 configuration */
  unsigned int mcon3;             /* memory region 3 configuration */
                            /* Control Group 3 */
  unsigned int mcon4;             /* memory region 4 configuration */
  unsigned int mcon5;             /* memory region 5 configuration */
  unsigned int mcon6;             /* memory region 6 configuration */
  unsigned int mcon7;             /* memory region 7 configuration */
                            /* Control Group 4 */
  unsigned int mcon8;             /* memory region 8 configuration */
  unsigned int mcon9;             /* memory region 9 configuration */
  unsigned int mcon10;            /* memory region 10 configuration */
  unsigned int mcon11;            /* memory region 11 configuration */
                            /* Control Group 5 */
  unsigned int mcon12;            /* memory region 12 configuration */
  unsigned int mcon13;            /* memory region 13 configuration */
  unsigned int mcon14;            /* memory region 14 configuration */
  unsigned int mcon15;            /* memory region 15 configuration */
                            /* Control Group 6 */
  unsigned int reserved;          /* reserved */
  unsigned int bpcon;             /* breakpoint control */
  unsigned int tc;                /* trace control */
  unsigned int bcon;              /* bus configuration control */
}   i960ha_control_table;

/* Intel i960HA Processor Control Block */

typedef struct {
  unsigned int    *fault_tbl;     /* fault table base address */
  i960ha_control_table
                  *control_tbl;   /* control table base address */
  unsigned int     initial_ac;    /* AC register initial value */
  unsigned int     fault_config;  /* fault configuration word */
  void           **intr_tbl;      /* interrupt table base address */
  void            *sys_proc_tbl;  /* system procedure table
                                     base address */
  unsigned int     reserved;      /* reserved */
  unsigned int    *intr_stack;    /* interrupt stack pointer */
  unsigned int     ins_cache_cfg; /* instruction cache
                                     configuration word */
  unsigned int     reg_cache_cfg; /* register cache configuration word */
}   i960ha_PRCB;

typedef i960ha_control_table i960_control_table;
typedef i960ha_PRCB i960_PRCB;

#define I960_SOFT_RESET_COMMAND 0x30000

#define i960_soft_reset( prcb ) \
 { register i960_PRCB    *_prcb = (prcb); \
   register unsigned int *_next=0; \
   register unsigned int  _cmd  = I960_SOFT_RESET_COMMAND; \
   asm volatile( "lda    next,%1; \
                  sysctl %0,%1,%2; \
            next: mov    g0,g0" \
                  : "=d" (_cmd), "=d" (_next), "=d" (_prcb) \
                  : "0"  (_cmd), "1"  (_next), "2"  (_prcb) ); \
 }



#endif
/* end of include file */
