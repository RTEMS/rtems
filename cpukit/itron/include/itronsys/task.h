/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __ITRON_TASK_h_
#define __ITRON_TASK_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Task (cre_tsk) Structure
 */

typedef struct t_ctsk {
  VP    exinf;     /* extended information */
  ATR   tskatr;    /* task attributes */
  FP    task;      /* task start address */
  PRI   itskpri;   /* initial task priority */
  INT   stksz;     /* stack size */
  /* additional information may be included depending on the implementation */
} T_CTSK;

/*
 *  Values for the tskatr field
 */

#define TA_ASM    0x00     /* program written in assembly language */
#define TA_HLNG   0x01     /* program written in high-level language */
#define TA_COP0   0x8000   /* uses coprocessor having ID = 0 */
#define TA_COP1   0x4000   /* uses coprocessor having ID = 1 */
#define TA_COP2   0x2000   /* uses coprocessor having ID = 2 */
#define TA_COP3   0x1000   /* uses coprocessor having ID = 3 */
#define TA_COP4   0x0800   /* uses coprocessor having ID = 4 */
#define TA_COP5   0x0400   /* uses coprocessor having ID = 5 */
#define TA_COP6   0x0200   /* uses coprocessor having ID = 6 */
#define TA_COP7   0x0100   /* uses coprocessor having ID = 7 */

/*
 *  Values for the tskid field
 */

#define TSK_SELF   0   /* task specifies itself */

/* XXX is this a mistake in ITRON?  FALSE was here and in the types list */
#if 0

#define FALSE      0   /* indicates a task-independent portion (return */
                       /*   parameters only) */
#endif

/*
 *  Values for the tskpri field
 */

#define TPRI_INI   0   /* specifies the initial priority on */
                       /*   task startup (chg_pri) */
#define TPRI_RUN   0   /* specifies the highest priority during */
                       /*   execution (rot_rdq) */


/*
 *  Reference Task (ref_tsk) Structure
 */

typedef struct t_rtsk {
  VP     exinf;     /* extended information */
  PRI    tskpri;    /* current priority */
  UINT   tskstat;   /* task state */
  /*
   *  The following are represent extended features of support
   *  [level X] (implementation-dependent).
   */
  UINT   tskwait;   /* cause of wait */
  ID     wid;       /* ID of object being waited for */
  INT    wupcnt;    /* wakeup request count */
  INT    suscnt;    /* SUSPEND request count */
  ATR    tskatr;    /* task attributes */
  FP     task;      /* task start address */
  PRI    itskpri;   /* initial task priority */
  INT    stksz;     /* stack size */
} T_RTSK;

/*
 *  Values for the tskstat field
 */


#define TTS_RUN   0x01   /* RUN */
#define TTS_RDY   0x02   /* READY */
#define TTS_WAI   0x04   /* WAIT */
#define TTS_SUS   0x08   /* SUSPEND */
#define TTS_WAS   0x0C   /* WAIT-SUSPEND */
#define TTS_DMT   0x10   /* DORMANT */

/*
 *  Values for the tskwait field
 */

#define TTW_SLP   0x0001   /* wait due to slp_tsk or tslp_tsk */
#define TTW_DLY   0x0002   /* wait due to dly_tsk */
#define TTW_NOD   0x0008   /* connection function response wait */
#define TTW_FLG   0x0010   /* wait due to wai_flg or twai_flg */
#define TTW_SEM   0x0020   /* wait due to wai_sem or twai_sem */
#define TTW_MBX   0x0040   /* wait due to rcv_msg or trcv_msg */
#define TTW_SMBF  0x0080   /* wait due to snd_mbf or tsnd_mbf */
#define TTW_MBF   0x0100   /* wait due to rcv_mbf or trcv_mbf */
#define TTW_CAL   0x0200   /* wait for rendezvous call */
#define TTW_ACP   0x0400   /* wait for rendezvous accept */
#define TTW_RDV   0x0800   /* wait for rendezvous completion */
#define TTW_MPL   0x1000   /* wait due to get_blk or tget_blk */
#define TTW_MPF   0x2000   /* wait due to get_blf or tget_blf */

/*
 *  Since the task states given by tskstat and tskwait are expressed
 *  by bit correspondences, they are convenient when looking for OR
 *  conditions (such as whether a task is in WAIT or READY state).
 */

/*
 *  Task Management Functions
 */

/*
 *  cre_tsk - Create Task
 */

ER cre_tsk(
  ID tskid,
  T_CTSK *pk_ctsk
);

/*
 *  del_tsk - Delete Task
 */

ER del_tsk(
  ID tskid
);

/*
 *  sta_tsk - Start Task
 */

ER sta_tsk(
  ID tskid,
  INT stacd
);

/*
 *  ext_tsk - Exit Issuing Task
 */

void ext_tsk( void );

/*
 *  exd_tsk - Exit and Delete Task
 */

void exd_tsk( void );

/*
 *  ter_tsk - Terminate Other Task
 */

ER ter_tsk(
  ID tskid
);

/*
 *  dis_dsp - Disable Dispatch
 */

ER dis_dsp( void );

/*
 *  ena_dsp - Enable Dispatch
 */

ER ena_dsp( void );

/*
 *  chg_pri - Change Task Priority
 */

ER chg_pri(
  ID tskid,
  PRI tskpri
);

/*
 *  rot_rdq - Rotate Tasks on the Ready Queue
 */

ER rot_rdq(
  PRI tskpri
);

/*
 *  rel_wai - Release Wait of Other Task
 */

ER rel_wai(
  ID tskid
);

/*
 *  get_tid - Get Task Identifier
 */

ER get_tid(
  ID *p_tskid
);

/*
 *  ref_tsk - Reference Task Status
 */

ER ref_tsk(
  T_RTSK *pk_rtsk,
  ID tskid
);


/*
 *  Task-Dependent Synchronization Functions
 */

/*
 *  sus_tsk - Suspend Other Task
 */

ER sus_tsk(
  ID tskid
);

/*
 *  rsm_tsk - Resume Suspended Task
 */

ER rsm_tsk(
  ID tskid
);

/*
 *  frsm_tsk - Forcibly Resume Suspended Task
 */

ER frsm_tsk(
  ID tskid
);

/*
 *  slp_tsk - Sleep Task Sleep Task with Timeout
 */

ER slp_tsk( void );

/*
 *  tslp_tsk - Sleep Task with Timeout
 */

ER tslp_tsk(
  TMO tmout
);

/*
 *  wup_tsk - Wakeup Other Task
 */

ER wup_tsk(
  ID tskid
);

/*
 *  can_wup - Cancel Wakeup Request
 */

ER can_wup(
  INT *p_wupcnt,
  ID tskid
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

