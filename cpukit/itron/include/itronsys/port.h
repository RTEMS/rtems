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

#ifndef __ITRON_PORT_h_
#define __ITRON_PORT_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Port (cre_por) Structure
 */

typedef struct t_cpor {
  VP    exinf;     /* extended information */
  ATR   poratr;    /* port attributes */
  INT   maxcmsz;   /* maximum call message size */
  INT   maxrmsz;   /* maximum reply message size */
  /* additional information may be included depending on the implementation */
} T_CPOR;

/*
 *  poratr
 */

#define TA_NULL   0   /* specifies no particular attributes */

/*
 *  TA_NULL should be used in place of zeroes to turn off all
 *          attribute features.
 */

/*
 *  Reference Port (ref_por) Structure
 */

typedef struct t_rpor {
  VP        exinf;   /* extended information */
  BOOL_ID   wtsk;    /* indicates whether or not there is a task */
                     /* waiting to call a rendezvous */
  BOOL_ID   atsk;    /* indicates whether or not there is a task */
                     /* waiting to accept a rendezvous */
  /* additional information may be included depending on the implementation */
} T_RPOR;

/*
 *  Port Functions
 */

/*
 *  cre_por - Create Port for Rendezvous
 */

ER cre_por(
  ID porid,
  T_CPOR *pk_cpor
);

/*
 *  del_por - Delete Port for Rendezvous
 */

ER del_por(
  ID porid
);

/*
 *  cal_por - Call Port for Rendezvous Poll 
 */

ER cal_por(
  VP msg,
  INT *p_rmsgsz,
  ID porid,
  UINT calptn,
  INT cmsgsz
);

/*
 *  pcal_por - Poll and Call Port for Rendezvous
 */

ER pcal_por(
  VP msg,
  INT *p_rmsgsz,
  ID porid,
  UINT calptn,
  INT cmsgsz
);

/*
 *  tcal_por - Call Port for Rendezvous with Timeout
 */

ER tcal_por(
  VP msg,
  INT *p_rmsgsz,
  ID porid,
  UINT calptn,
  INT cmsgsz,
  TMO tmout
);

/*
 *  acp_por - Accept Port for Rendezvous Poll
 */

ER acp_por(
  RNO *p_rdvno,
  VP msg,
  INT *p_cmsgsz,
  ID porid,
  UINT acpptn
);

/*
 *  pacp_por - Poll and Accept Port for Rendezvous
 */

ER pacp_por(
  RNO *p_rdvno,
  VP msg,
  INT *p_cmsgsz,
  ID porid,
  UINT acpptn
);

/*
 *  tacp_por - Accept Port for Rendezvous with Timeout
 */

ER tacp_por(
  RNO *p_rdvno,
  VP msg,
  INT *p_cmsgsz,
  ID porid,
  UINT acpptn,
  TMO tmout
);

/*
 *  fwd_por - Forward Rendezvous to Other Port
 */

ER fwd_por(
  ID porid,
  UINT calptn,
  RNO rdvno,
  VP msg,
  INT cmsgsz
);

/*
 *  rpl_rdv - Reply Rendezvous
 */

ER rpl_rdv(
  RNO rdvno,
  VP msg,
  INT rmsgsz
);

/*
 *  ref_por - Reference Port Status
 */

ER ref_por(
  T_RPOR *pk_rpor,
  ID porid
);




#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

