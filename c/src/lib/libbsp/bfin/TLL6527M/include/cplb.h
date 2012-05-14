/*  cplb.h
 *  
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef _CPLB_H
#define _CPLB_H

/* CPLB configurations */
#define CPLB_DEF_CACHE_WT	CPLB_L1_CHBL | CPLB_WT
#define CPLB_DEF_CACHE_WB	CPLB_L1_CHBL 
#define CPLB_CACHE_ENABLED	CPLB_L1_CHBL | CPLB_DIRTY

#define CPLB_DEF_CACHE		CPLB_L1_CHBL | CPLB_WT
#define CPLB_ALL_ACCESS	CPLB_SUPV_WR | CPLB_USER_RD | CPLB_USER_WR

#define CPLB_I_PAGE_MGMT	CPLB_LOCK | CPLB_VALID
#define CPLB_D_PAGE_MGMT	CPLB_LOCK | CPLB_ALL_ACCESS | CPLB_VALID

#define CPLB_DNOCACHE		CPLB_ALL_ACCESS | CPLB_VALID
#define CPLB_DDOCACHE		CPLB_DNOCACHE | CPLB_DEF_CACHE
#define CPLB_INOCACHE   	CPLB_USER_RD | CPLB_VALID
#define CPLB_IDOCACHE   	CPLB_INOCACHE | CPLB_L1_CHBL

#define CPLB_DDOCACHE_WT	CPLB_DNOCACHE | CPLB_DEF_CACHE_WT
#define CPLB_DDOCACHE_WB	CPLB_DNOCACHE | CPLB_DEF_CACHE_WB

#endif /* _CPLB_H */
