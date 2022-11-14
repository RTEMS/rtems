/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplApplConfig
 *
 * @brief This header file evaluates configuration options related to the PCI
 *   library configuration.
 */

/*
 * Copyright (C) 2011 Cobham Gaisler AB
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_CONFDEFS_LIBPCI_H
#define _RTEMS_CONFDEFS_LIBPCI_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 *  Select PCI Configuration Library
 */
#ifdef RTEMS_PCI_CONFIG_LIB
  #ifdef CONFIGURE_INIT
    #define PCI_LIB_NONE 0
    #define PCI_LIB_AUTO 1
    #define PCI_LIB_STATIC 2
    #define PCI_LIB_READ 3
    #define PCI_LIB_PERIPHERAL 4
    #if CONFIGURE_PCI_LIB == PCI_LIB_AUTO
      #define PCI_CFG_AUTO_LIB
      #include <pci/cfg.h>
      struct pci_bus pci_hb;
      #define PCI_LIB_INIT pci_config_auto
      #define PCI_LIB_CONFIG pci_config_auto_register
    #elif CONFIGURE_PCI_LIB == PCI_LIB_STATIC
      #define PCI_CFG_STATIC_LIB
      #include <pci/cfg.h>
      #define PCI_LIB_INIT pci_config_static
      #define PCI_LIB_CONFIG NULL
      /* Let user define PCI configuration (struct pci_bus pci_hb) */
    #elif CONFIGURE_PCI_LIB == PCI_LIB_READ
      #define PCI_CFG_READ_LIB
      #include <pci/cfg.h>
      #define PCI_LIB_INIT pci_config_read
      #define PCI_LIB_CONFIG NULL
      struct pci_bus pci_hb;
    #elif CONFIGURE_PCI_LIB == PCI_LIB_PERIPHERAL
      #define PCI_LIB_INIT pci_config_peripheral
      #define PCI_LIB_CONFIG NULL
      /* Let user define PCI configuration (struct pci_bus pci_hb) */
    #elif CONFIGURE_PCI_LIB == PCI_LIB_NONE
      #define PCI_LIB_INIT NULL
      #define PCI_LIB_CONFIG NULL
      /* No PCI Configuration at all, user can use/debug access routines */
    #else
      #error NO PCI LIBRARY DEFINED
    #endif

    const int pci_config_lib_type = CONFIGURE_PCI_LIB;
    int (*pci_config_lib_init)(void) = PCI_LIB_INIT;
    void (*pci_config_lib_register)(void *config) = PCI_LIB_CONFIG;
  #endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_CONFDEFS_LIBPCI_H */
