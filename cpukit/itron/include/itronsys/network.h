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

#ifndef __ITRON_NETWORK_h_
#define __ITRON_NETWORK_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  NODE srcnode, dstnode, nod:
 */

#define TND_SELF   0      /* specifies the local node */
#define TND_OTHR   (-1)   /* specifies default remote node */

/*
 *  Network Functions
 */

/*
 *  nrea_dat - Read Data from another Node
 */

ER nrea_dat(
  INT *p_reasz,
  VP dstadr,
  NODE srcnode,
  VP srcadr,
  INT datsz
);

/*
 *  nwri_dat - Write Data to another Node
 */

ER nwri_dat(
  INT *p_wrisz,
  NODE dstnode,
  VP dstadr,
  VP srcadr,
  INT datsz
);

/*
 *  nget_nod - Get Local Node Number
 */

ER nget_nod(
  NODE *p_node
);

/*
 *  nget_ver - Get Version Information of another Node
 */

ER nget_ver(
  T_VER *pk_ver,
  NODE node
);


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

