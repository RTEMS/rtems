/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH
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

#include <rtems/config.h>
#include <rtems/extensionimpl.h>
#include <rtems/rtems/barrierimpl.h>
#include <rtems/rtems/dpmemimpl.h>
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/partimpl.h>
#include <rtems/rtems/ratemonimpl.h>
#include <rtems/rtems/regionimpl.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/objectimpl.h>

static uint32_t get_config_max( const Objects_Information *info )
{
	if ( _Objects_Is_auto_extend( info ) ) {
		return info->objects_per_block | RTEMS_UNLIMITED_OBJECTS;
	}

	return _Objects_Get_maximum_index( info );
}

uint32_t rtems_configuration_get_maximum_barriers( void )
{
	return get_config_max( &_Barrier_Information );
}

uint32_t rtems_configuration_get_maximum_extensions( void )
{
	return get_config_max( &_Extension_Information );
}

uint32_t rtems_configuration_get_maximum_message_queues( void )
{
	return get_config_max( &_Message_queue_Information );
}

uint32_t rtems_configuration_get_maximum_partitions( void )
{
	return get_config_max( &_Partition_Information );
}

uint32_t rtems_configuration_get_maximum_periods( void )
{
	return get_config_max( &_Rate_monotonic_Information );
}

uint32_t rtems_configuration_get_maximum_ports( void )
{
	return get_config_max( &_Dual_ported_memory_Information );
}

uint32_t rtems_configuration_get_maximum_regions( void )
{
	return get_config_max( &_Region_Information );
}

uint32_t rtems_configuration_get_maximum_semaphores( void )
{
	return get_config_max( &_Semaphore_Information );
}

uint32_t rtems_configuration_get_maximum_timers( void )
{
	return get_config_max( &_Timer_Information );
}

uint32_t rtems_configuration_get_maximum_tasks( void )
{
	return get_config_max( &_RTEMS_tasks_Information.Objects );
}
