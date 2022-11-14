/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2007.
 *  Cobham Gaisler.
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
 *
 */

/**
 * @file
 * @ingroup RTEMSBSPsSharedGRLIB
 */

#ifndef __DEBUG_DEFS_H__
#define __DEBUG_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef DEBUG

 #ifndef DEBUG_FLAGS
  #define DEBUG_FLAGS 0
 #endif

 #define DBG(fmt, args...)    do { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); } while(0)
 #define DBG2(fmt)            do { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__); } while(0)
 #define DBGC(c,fmt, args...) do { if (DEBUG_FLAGS & c) { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args);  }} while(0)

#else

 #define DBG(fmt, args...)
 #define DBG2(fmt, args...)
 #define DBGC(c, fmt, args...)

#endif

#ifdef DEBUGFUNCS
  #define FUNCDBG()         do { printk("%s\n\r",__FUNCTION__); } while(0)
#else
  #define FUNCDBG()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_DEFS_H__ */
