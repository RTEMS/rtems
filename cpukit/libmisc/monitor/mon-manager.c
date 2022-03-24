/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Monitor "manager" support.
 *
 * Used to traverse object (chain) lists and print them out.
 */

/*
 * COPYRIGHT (c) 1989-2022. On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>

#include <stdio.h>

/*
 * "next" routine for all objects that are RTEMS manager objects
 */

const void *
rtems_monitor_manager_next(
    void      *table_void,
    void      *canonical,
    rtems_id  *next_id
)
{
    Objects_Information     *table = table_void;
    rtems_monitor_generic_t *copy;
    Objects_Control         *object = 0;

    /*
     * When we are called, it must be local
     */

#if defined(RTEMS_MULTIPROCESSING)
    if ( ! _Objects_Is_local_id(*next_id) )
        goto done;
#endif

    object = _Objects_Get_next(*next_id, table, next_id);

    if (object)
    {
        copy = (rtems_monitor_generic_t *) canonical;
        copy->id = object->id;
        copy->name = object->name.name_u32;
    }

#if defined(RTEMS_MULTIPROCESSING)
done:
#endif
    return object;
}
