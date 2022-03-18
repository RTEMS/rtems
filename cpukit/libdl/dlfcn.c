/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtl
 *
 * @brief RTEMS POSIX Dynamic Module Loading Interface.
 *
 * This is the POSIX interface to run-time loading of code into RTEMS.
 */

/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
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

#include <stdint.h>
#include <dlfcn.h>
#include <rtems/rtl/rtl.h>

static rtems_rtl_obj*
dl_get_obj_from_handle (void* handle)
{
  rtems_rtl_obj* obj;

  /*
   * Handle the special cases provided in NetBSD and Sun documentation.
   *   http://download.oracle.com/docs/cd/E19253-01/816-5168/dlsym-3c/index.html
   * We currently do not manage the loading dependences in the module mappings
   * so we cannot handle the searching based on loading order where overriding
   * can occur.
   */

  if ((handle == RTLD_DEFAULT) || (handle == RTLD_SELF))
    obj = rtems_rtl_baseimage ();
  else
    obj = rtems_rtl_check_handle (handle);

  return obj;
}

void*
dlopen (const char* name, int mode)
{
  rtems_rtl_obj* obj = NULL;

  if (!rtems_rtl_lock ())
    return NULL;

  _rtld_debug.r_state = RT_ADD;
  _rtld_debug_state ();

  if (name)
    obj = rtems_rtl_load (name, mode);
  else
    obj = rtems_rtl_baseimage ();

  _rtld_debug.r_state = RT_CONSISTENT;
  _rtld_debug_state();

  rtems_rtl_unlock ();

  return obj;
}

int
dlclose (void* handle)
{
  rtems_rtl_obj* obj;
  int            r;

  if (!rtems_rtl_lock ())
    return -1;

  obj = rtems_rtl_check_handle (handle);
  if (!obj)
  {
    rtems_rtl_unlock ();
    return -1;
  }

  _rtld_debug.r_state = RT_DELETE;
  _rtld_debug_state ();

  r = rtems_rtl_unload (obj) ? 0 : -1;

  _rtld_debug.r_state = RT_CONSISTENT;
  _rtld_debug_state ();

  rtems_rtl_unlock ();

  return r;
}

void*
dlsym (void* handle, const char *symbol)
{
  rtems_rtl_obj*     obj;
  rtems_rtl_obj_sym* sym = NULL;
  void*              symval = NULL;

  if (!rtems_rtl_lock ())
    return NULL;

  /*
   * If the handle is "default" search the global symbol table.
   */
  if (handle == RTLD_DEFAULT)
  {
    sym = rtems_rtl_symbol_global_find (symbol);
  }
  else
  {
    obj = dl_get_obj_from_handle (handle);
    if (obj)
      sym = rtems_rtl_symbol_obj_find (obj, symbol);
  }

  if (sym)
    symval = sym->value;

  rtems_rtl_unlock ();

  return symval;
}

const char*
dlerror (void)
{
  static char msg[64];
  int         eno;
  eno = rtems_rtl_get_error (msg, sizeof (msg));
  if (eno == 0)
    return NULL;
  return msg;
}

int
dlinfo (void* handle, int request, void* p)
{
  rtems_rtl_obj* obj;
  int            rc = -1;

  if (!rtems_rtl_lock () || !p)
    return -1;

  obj = dl_get_obj_from_handle (handle);
  if (obj)
  {
    switch (request)
    {
      case RTLD_DI_UNRESOLVED:
        *((int*) p) = rtems_rtl_obj_unresolved (obj) ? 1 : 0;
        rc = 0;
        break;
      default:
        break;
    }
  }

  rtems_rtl_unlock ();

  return rc;
}
