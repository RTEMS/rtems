/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   T_check_posix_keys().
 */

/*
 * Copyright (C) 2018 embedded brains GmbH & Co. KG
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

#undef __STRICT_ANSI__

#include "t-test-rtems.h"

#include <rtems/test.h>

#include <rtems/posix/keyimpl.h>

static Objects_Maximum T_posix_key_count;

static ssize_t T_posix_key_value_count;

static POSIX_Keys_Control *
T_get_next_posix_key(Objects_Id *id)
{
	return (POSIX_Keys_Control *)
	    _Objects_Get_next(*id, &_POSIX_Keys_Information, id);
}

static ssize_t
T_get_active_posix_key_value_pairs(void)
{
	ssize_t count;
	Objects_Id id;
	POSIX_Keys_Control *the_key;

	count = 0;
	id = OBJECTS_ID_INITIAL_INDEX;

	while ((the_key = T_get_next_posix_key(&id)) != NULL ) {
		count += (ssize_t)
		    _Chain_Node_count_unprotected(&the_key->Key_value_pairs);
		_Objects_Allocator_unlock();
	}

	return count;
}

static void
T_posix_keys_run_initialize(void)
{
	T_posix_key_count = T_objects_count(OBJECTS_POSIX_API,
	    OBJECTS_POSIX_KEYS);
	T_posix_key_value_count = T_get_active_posix_key_value_pairs();
}

static void
T_posix_keys_case_end(void)
{
	ssize_t count;
	ssize_t delta;

	T_objects_check(OBJECTS_POSIX_API, OBJECTS_POSIX_KEYS,
	    &T_posix_key_count, "POSIX key");

	count = T_get_active_posix_key_value_pairs();
	delta = count - T_posix_key_value_count;

	if (delta != 0) {
		T_posix_key_value_count = count;
		T_check(&T_special, false, "POSIX key value pair leak (%zi)",
		    delta);
	}
}

void
T_check_posix_keys(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_posix_keys_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_posix_keys_case_end();
		break;
	default:
		break;
	};
}
