/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

#include <rtems/record.h>

void rtems_record_line( void )
{
  rtems_record_produce(
    RTEMS_RECORD_LINE,
    (rtems_record_data) RTEMS_RETURN_ADDRESS()
  );
}

void rtems_record_line_2(
  rtems_record_event event,
  rtems_record_data  data
)
{
  rtems_record_produce_2(
    RTEMS_RECORD_LINE,
    (rtems_record_data) RTEMS_RETURN_ADDRESS(),
    event,
    data
  );
}

void rtems_record_line_3(
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
)
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = event_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = event_1;
  items[ 2 ].data = data_1;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg( rtems_record_data data )
{
  rtems_record_produce_2(
    RTEMS_RECORD_LINE,
    (rtems_record_data) RTEMS_RETURN_ADDRESS(),
    RTEMS_RECORD_ARG_0,
    data
  );
}

void rtems_record_line_arg_2(
  rtems_record_data data_0,
  rtems_record_data data_1
)
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_3(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2
)
{
  rtems_record_item items[ 4 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_4(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3
)
{
  rtems_record_item items[ 5 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_5(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4
)
{
  rtems_record_item items[ 6 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_6(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5
)
{
  rtems_record_item items[ 7 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_7(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6
)
{
  rtems_record_item items[ 8 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_8(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7
)
{
  rtems_record_item items[ 9 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_ARG_7;
  items[ 8 ].data = data_7;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_9(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7,
  rtems_record_data data_8
)
{
  rtems_record_item items[ 10 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_ARG_7;
  items[ 8 ].data = data_7;
  items[ 9 ].event = RTEMS_RECORD_ARG_8;
  items[ 9 ].data = data_8;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void rtems_record_line_arg_10(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7,
  rtems_record_data data_8,
  rtems_record_data data_9
)
{
  rtems_record_item items[ 11 ];

  items[ 0 ].event = RTEMS_RECORD_LINE;
  items[ 0 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_ARG_7;
  items[ 8 ].data = data_7;
  items[ 9 ].event = RTEMS_RECORD_ARG_8;
  items[ 9 ].data = data_8;
  items[ 10 ].event = RTEMS_RECORD_ARG_9;
  items[ 10 ].data = data_9;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller( void *return_address )
{
  rtems_record_produce_2(
    RTEMS_RECORD_CALLER,
    (rtems_record_data) return_address,
    RTEMS_RECORD_LINE,
    (rtems_record_data) RTEMS_RETURN_ADDRESS()
  );
}

void _Record_Caller_3(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data
)
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = event;
  items[ 2 ].data = data;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_4(
  void               *return_address,
  rtems_record_event  event_0,
  rtems_record_data   data_0,
  rtems_record_event  event_1,
  rtems_record_data   data_1
)
{
  rtems_record_item items[ 4 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = event_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = event_1;
  items[ 3 ].data = data_1;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg( void *return_address, rtems_record_data data )
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_2(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1
)
{
  rtems_record_item items[ 4 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_3(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2
)
{
  rtems_record_item items[ 5 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_4(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3
)
{
  rtems_record_item items[ 6 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_5(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4
)
{
  rtems_record_item items[ 7 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  items[ 6 ].event = RTEMS_RECORD_ARG_4;
  items[ 6 ].data = data_4;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_6(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5
)
{
  rtems_record_item items[ 8 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  items[ 6 ].event = RTEMS_RECORD_ARG_4;
  items[ 6 ].data = data_4;
  items[ 7 ].event = RTEMS_RECORD_ARG_5;
  items[ 7 ].data = data_5;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_7(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6
)
{
  rtems_record_item items[ 9 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  items[ 6 ].event = RTEMS_RECORD_ARG_4;
  items[ 6 ].data = data_4;
  items[ 7 ].event = RTEMS_RECORD_ARG_5;
  items[ 7 ].data = data_5;
  items[ 8 ].event = RTEMS_RECORD_ARG_6;
  items[ 8 ].data = data_6;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_8(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7
)
{
  rtems_record_item items[ 10 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  items[ 6 ].event = RTEMS_RECORD_ARG_4;
  items[ 6 ].data = data_4;
  items[ 7 ].event = RTEMS_RECORD_ARG_5;
  items[ 7 ].data = data_5;
  items[ 8 ].event = RTEMS_RECORD_ARG_6;
  items[ 8 ].data = data_6;
  items[ 9 ].event = RTEMS_RECORD_ARG_7;
  items[ 9 ].data = data_7;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_9(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8
)
{
  rtems_record_item items[ 11 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  items[ 6 ].event = RTEMS_RECORD_ARG_4;
  items[ 6 ].data = data_4;
  items[ 7 ].event = RTEMS_RECORD_ARG_5;
  items[ 7 ].data = data_5;
  items[ 8 ].event = RTEMS_RECORD_ARG_6;
  items[ 8 ].data = data_6;
  items[ 9 ].event = RTEMS_RECORD_ARG_7;
  items[ 9 ].data = data_7;
  items[ 10 ].event = RTEMS_RECORD_ARG_8;
  items[ 10 ].data = data_8;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Caller_arg_10(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8,
  rtems_record_data  data_9
)
{
  rtems_record_item items[ 12 ];

  items[ 0 ].event = RTEMS_RECORD_CALLER;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_LINE;
  items[ 1 ].data = (rtems_record_data) RTEMS_RETURN_ADDRESS();
  items[ 2 ].event = RTEMS_RECORD_ARG_0;
  items[ 2 ].data = data_0;
  items[ 3 ].event = RTEMS_RECORD_ARG_1;
  items[ 3 ].data = data_1;
  items[ 4 ].event = RTEMS_RECORD_ARG_2;
  items[ 4 ].data = data_2;
  items[ 5 ].event = RTEMS_RECORD_ARG_3;
  items[ 5 ].data = data_3;
  items[ 6 ].event = RTEMS_RECORD_ARG_4;
  items[ 6 ].data = data_4;
  items[ 7 ].event = RTEMS_RECORD_ARG_5;
  items[ 7 ].data = data_5;
  items[ 8 ].event = RTEMS_RECORD_ARG_6;
  items[ 8 ].data = data_6;
  items[ 9 ].event = RTEMS_RECORD_ARG_7;
  items[ 9 ].data = data_7;
  items[ 10 ].event = RTEMS_RECORD_ARG_8;
  items[ 10 ].data = data_8;
  items[ 11 ].event = RTEMS_RECORD_ARG_9;
  items[ 11 ].data = data_9;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_2(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1
)
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_3(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2
)
{
  rtems_record_item items[ 4 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_4(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3
)
{
  rtems_record_item items[ 5 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_5(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4
)
{
  rtems_record_item items[ 6 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_6(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5
)
{
  rtems_record_item items[ 7 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_7(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6
)
{
  rtems_record_item items[ 8 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_8(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7
)
{
  rtems_record_item items[ 9 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_ARG_7;
  items[ 8 ].data = data_7;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_9(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8
)
{
  rtems_record_item items[ 10 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_ARG_7;
  items[ 8 ].data = data_7;
  items[ 9 ].event = RTEMS_RECORD_ARG_8;
  items[ 9 ].data = data_8;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Entry_10(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8,
  rtems_record_data   data_9
)
{
  rtems_record_item items[ 11 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_ARG_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_ARG_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_ARG_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_ARG_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_ARG_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_ARG_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_ARG_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_ARG_7;
  items[ 8 ].data = data_7;
  items[ 9 ].event = RTEMS_RECORD_ARG_8;
  items[ 9 ].data = data_8;
  items[ 10 ].event = RTEMS_RECORD_ARG_9;
  items[ 10 ].data = data_9;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_2(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1
)
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_3(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2
)
{
  rtems_record_item items[ 4 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_4(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3
)
{
  rtems_record_item items[ 5 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_5(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4
)
{
  rtems_record_item items[ 6 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_RETURN_4;
  items[ 5 ].data = data_4;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_6(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5
)
{
  rtems_record_item items[ 7 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_RETURN_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_RETURN_5;
  items[ 6 ].data = data_5;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_7(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6
)
{
  rtems_record_item items[ 8 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_RETURN_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_RETURN_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_RETURN_6;
  items[ 7 ].data = data_6;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_8(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7
)
{
  rtems_record_item items[ 9 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_RETURN_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_RETURN_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_RETURN_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_RETURN_7;
  items[ 8 ].data = data_7;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_9(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8
)
{
  rtems_record_item items[ 10 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_RETURN_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_RETURN_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_RETURN_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_RETURN_7;
  items[ 8 ].data = data_7;
  items[ 9 ].event = RTEMS_RECORD_RETURN_8;
  items[ 9 ].data = data_8;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Exit_10(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8,
  rtems_record_data   data_9
)
{
  rtems_record_item items[ 11 ];

  items[ 0 ].event = event;
  items[ 0 ].data = (rtems_record_data) return_address;
  items[ 1 ].event = RTEMS_RECORD_RETURN_0;
  items[ 1 ].data = data_0;
  items[ 2 ].event = RTEMS_RECORD_RETURN_1;
  items[ 2 ].data = data_1;
  items[ 3 ].event = RTEMS_RECORD_RETURN_2;
  items[ 3 ].data = data_2;
  items[ 4 ].event = RTEMS_RECORD_RETURN_3;
  items[ 4 ].data = data_3;
  items[ 5 ].event = RTEMS_RECORD_RETURN_4;
  items[ 5 ].data = data_4;
  items[ 6 ].event = RTEMS_RECORD_RETURN_5;
  items[ 6 ].data = data_5;
  items[ 7 ].event = RTEMS_RECORD_RETURN_6;
  items[ 7 ].data = data_6;
  items[ 8 ].event = RTEMS_RECORD_RETURN_7;
  items[ 8 ].data = data_7;
  items[ 9 ].event = RTEMS_RECORD_RETURN_8;
  items[ 9 ].data = data_8;
  items[ 10 ].event = RTEMS_RECORD_RETURN_9;
  items[ 10 ].data = data_9;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

uint32_t rtems_record_interrupt_disable( void )
{
  uint32_t             level;
  rtems_record_context context;

  _CPU_ISR_Disable( level );
  rtems_record_prepare_critical( &context, _Per_CPU_Get() );
  rtems_record_add(
    &context,
    RTEMS_RECORD_ISR_DISABLE,
    (rtems_record_data) RTEMS_RETURN_ADDRESS()
  );
  rtems_record_commit_critical( &context );

  return level;
}

void rtems_record_interrupt_enable( uint32_t level )
{
  rtems_record_context context;

  rtems_record_prepare_critical( &context, _Per_CPU_Get() );
  rtems_record_add(
    &context,
    RTEMS_RECORD_ISR_ENABLE,
    (rtems_record_data) RTEMS_RETURN_ADDRESS()
  );
  rtems_record_commit_critical( &context );
  _CPU_ISR_Enable( level );
}
