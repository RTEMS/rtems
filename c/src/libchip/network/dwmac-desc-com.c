/**
 * @file
 *
 * @brief  DWMAC 10/100/1000 Common Descriptor Handling
 *
 * DWMAC 10/100/1000 on-chip Ethernet controllers.
 * Functions which are common to normal and enhanced DMA descriptors.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dwmac-desc-com.h"

#undef DWMAC_DESC_COM_DEBUG
#ifdef DWMAC_DESC_COM_DEBUG
#define DWMAC_DESC_COM_PRINT_DBG( fmt, args ... )  printk( fmt, ## args )
#else
#define DWMAC_DESC_COM_PRINT_DBG( fmt, args ... )  do { } while ( 0 )
#endif

struct mbuf *dwmac_desc_com_new_mbuf( dwmac_common_context *self ) {
  struct  ifnet *ifp = &self->arpcom.ac_if;
  struct  mbuf  *m   = NULL;


  MGETHDR( m, M_DONTWAIT, MT_DATA );

  if ( m != NULL ) {
    MCLGET( m, M_DONTWAIT );

    if ( m->m_ext.ext_buf != NULL ) {
      if ( ( m->m_flags & M_EXT ) != 0 ) {
        /* Set receive interface */
        m->m_pkthdr.rcvif = ifp;

        /* Make sure packet data will be aligned */
        m->m_data = mtod( m, char * ) + ETHER_ALIGN;
        return m;
      } else {
        m_free( m );
      }
    } else {
      m_free( m );
    }
  }

  return NULL;
}
