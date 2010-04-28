/*
 *  This file contains the RTEMS thread awareness support for GDB stubs.
 *
 *  This file is derived from an RTEMS thread aware i386-stub.c that
 *  had the following copyright announcements:
 *
 *    This software is Copyright (C) 1998 by T.sqware - all rights limited
 *    It is provided in to the public domain "as is", can be freely modified
 *    as far as this copyight notice is kept unchanged, but does not imply
 *    an endorsement by T.sqware of the product in which it is included.
 *
 *
 *     Modifications for RTEMS threads and more
 *
 *     Copyright (C) 2000 Quality Quorum, Inc.
 *
 *     All Rights Reserved
 *
 *     Permission to use, copy, modify, and distribute this software and its
 *     documentation for any purpose and without fee is hereby granted.
 *
 *     QQI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *     ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 *     QQI BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 *     ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *     WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *     ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 *     SOFTWARE.
 */

#include <rtems.h>
#include <string.h>

#include "gdb_if.h"

/* Change it to something meaningful when debugging */
#undef ASSERT
#define ASSERT(x)

extern const char gdb_hexchars[];

/*
 *  Prototypes for CPU dependent routines that are conditional
 *  at the bottom of this file.
 */

void rtems_gdb_stub_get_registers_from_context(
  unsigned int   *registers,
  Thread_Control *th
);

/* Check whether it is OK to enable thread support */
int rtems_gdb_stub_thread_support_ok(void)
{
  if (_System_state_Get() == SYSTEM_STATE_UP) {
    return 1;
  }
  return 0;
}

/*
 *  rtems_gdb_stub_id_to_index
 *
 *  Return the gdb thread id for the specified RTEMS thread id
 */

int rtems_gdb_stub_id_to_index(
  Objects_Id thread_obj_id
)
{
  Objects_Id min_id, max_id;
  int first_posix_id, first_rtems_id;
  Objects_Information *obj_info;

  if (_System_state_Get() != SYSTEM_STATE_UP) {
    /* We have one thread let us use value reserved for idle thread */
    return 1;
  }

  if (_Thread_Executing == _Thread_Idle) {
    return 1;
  }

  /* Let us figure out thread_id for gdb */
  first_rtems_id = 2;

  obj_info = _Objects_Information_table[OBJECTS_CLASSIC_API][1];

  min_id = obj_info->minimum_id;
  max_id = obj_info->maximum_id;

  if (thread_obj_id >= min_id && thread_obj_id < max_id) {
    return first_rtems_id + (thread_obj_id - min_id);
  }

  first_posix_id = first_rtems_id + (max_id - min_id) + 1;

  min_id = _Objects_Information_table[OBJECTS_POSIX_API][1]->minimum_id;

  return first_posix_id + (thread_obj_id - min_id);
}

/* Return the RTEMS thread id from a gdb thread id */
Thread_Control *rtems_gdb_index_to_stub_id(
  int thread
)
{
   Objects_Id thread_obj_id;
   Objects_Id min_id, max_id;
   int first_posix_id, first_rtems_id;
   Objects_Information *obj_info;
   Thread_Control *th;

   ASSERT(registers != NULL);

   if (_System_state_Get() != SYSTEM_STATE_UP || thread <= 0) {
      /* Should not happen */
      return NULL;
   }

   if (thread == 1) {
      th = _Thread_Idle;
      goto found;
   }

   /* Let us get object associtated with current thread */
   first_rtems_id = 2;

   thread_obj_id = _Thread_Executing->Object.id;

   /* Let us figure out thread_id for gdb */
   obj_info = _Objects_Information_table[OBJECTS_CLASSIC_API][1];

   min_id = obj_info->minimum_id;
   max_id = obj_info->maximum_id;

   if (thread <= (first_rtems_id + (max_id - min_id))) {
      th = (Thread_Control *)(obj_info->local_table[thread - first_rtems_id + 1]);

      if (th != NULL) {
         goto found;
      }

      /* Thread does not exist */
      return NULL;
   }

   first_posix_id = first_rtems_id + (max_id - min_id) + 1;

   obj_info = _Objects_Information_table[OBJECTS_POSIX_API][1];

   min_id = obj_info->minimum_id;
   max_id = obj_info->maximum_id;

   th = (Thread_Control *)(obj_info->local_table[thread - first_posix_id + 1]);
   if (th == NULL) {
      /* Thread does not exist */
      return NULL;
   }

  found:
   return th;
}

/* Get id of the thread stopped by exception */
int rtems_gdb_stub_get_current_thread(void)
{
  return rtems_gdb_stub_id_to_index( _Thread_Executing->Object.id );
}

/* Get id of the next thread after athread, if argument <= 0 find the
   first available thread, return thread if found or 0 if not */
int rtems_gdb_stub_get_next_thread(int athread)
{
  Objects_Id id, min_id, max_id;
  int lim, first_posix_id, first_rtems_id;
  Objects_Information *obj_info;
  int start;

  if (_System_state_Get() != SYSTEM_STATE_UP) {
    /* We have one thread let us use value of idle thread */
    return (athread < 1) ? 1 : 0;
  }

  if (athread < 1) {
    return 1;
  }

  first_rtems_id = 2;

  obj_info = _Objects_Information_table[OBJECTS_CLASSIC_API][1];

  min_id = obj_info->minimum_id;
  max_id = obj_info->maximum_id;

  lim = first_rtems_id + max_id - min_id;

  if (athread < lim) {
    if (athread < first_rtems_id) {
      start = first_rtems_id;
    } else {
      start = 1 + athread;
    }

    for (id=start; id<=lim; id++) {
      if (obj_info->local_table[id - first_rtems_id + 1] != NULL) {
        return id;
      }
    }
  }

  first_posix_id = first_rtems_id + (max_id - min_id) + 1;

  obj_info = _Objects_Information_table[OBJECTS_POSIX_API][1];

  min_id = obj_info->minimum_id;
  max_id = obj_info->maximum_id;

  lim = first_posix_id + (max_id - min_id);

  if (athread < lim) {
    if (athread < first_posix_id) {
      start = first_posix_id;
    } else {
      start = 1 + athread;
    }

    for (id=start; id<=lim; id++) {
      if (obj_info->local_table[id - first_posix_id + 1] != NULL) {
        return id;
      }
    }
  }

  /* Not found */
  return 0;
}

/* Get thread registers, return 0 if thread does not
   exist, and 1 otherwise */
int rtems_gdb_stub_get_thread_regs(
  int thread,
  unsigned int *registers
)
{
   Thread_Control *th;

   th= rtems_gdb_index_to_stub_id(thread);

   if( th )
   {
      rtems_gdb_stub_get_registers_from_context( registers, th );
      return 1;
   }
   return 0;
}

/* Set thread registers, return 0 if thread does not
   exist or register values will screw up the threads,
   and 1 otherwise */

int rtems_gdb_stub_set_thread_regs(
  int thread,
  unsigned int *registers
)
{
  /* In current situation there is no point in changing any registers here
     thread status is displayed as being deep inside thread switching
     and we better do not screw up anything there - it may be fixed eventually
     though */
  return 1;
}

/* Get thread information, return 0 if thread does not
   exist and 1 otherwise */
int rtems_gdb_stub_get_thread_info(
  int thread,
  struct rtems_gdb_stub_thread_info *info
)
{
   Objects_Id thread_obj_id;
   Objects_Id min_id, max_id;
   int first_posix_id, first_rtems_id;
   Objects_Information *obj_info;
   Thread_Control *th;
   char tmp_buf[20];

   ASSERT(info != NULL);

   if (thread <= 0) {
      return 0;
   }

   if (_System_state_Get() != SYSTEM_STATE_UP || thread == 1) {
      /* We have one thread let us use value
         which will never happen for real thread */
      strcpy(info->display, "idle thread");
      strcpy(info->name, "IDLE");
      info->more_display[0] = 0; /* Nothing */

      return 1;
   }

   /* Let us get object associtated with current thread */
   thread_obj_id = _Thread_Executing->Object.id;

   /* Let us figure out thread_id for gdb */
   first_rtems_id = 2;

   obj_info = _Objects_Information_table[OBJECTS_CLASSIC_API][1];

   min_id = obj_info->minimum_id;
   max_id = obj_info->maximum_id;

   if (thread <= (first_rtems_id + (max_id - min_id))) {
      th = (Thread_Control *)(obj_info->local_table[thread -
                                                    first_rtems_id + 1]);

      if (th == NULL) {
         /* Thread does not exist */
         return 0;
      }

      strcpy(info->display, "rtems task:   control at 0x");

      tmp_buf[0] = gdb_hexchars[(((int)th) >> 28) & 0xf];
      tmp_buf[1] = gdb_hexchars[(((int)th) >> 24) & 0xf];
      tmp_buf[2] = gdb_hexchars[(((int)th) >> 20) & 0xf];
      tmp_buf[3] = gdb_hexchars[(((int)th) >> 16) & 0xf];
      tmp_buf[4] = gdb_hexchars[(((int)th) >> 12) & 0xf];
      tmp_buf[5] = gdb_hexchars[(((int)th) >> 8) & 0xf];
      tmp_buf[6] = gdb_hexchars[(((int)th) >> 4) & 0xf];
      tmp_buf[7] = gdb_hexchars[((int)th) & 0xf];
      tmp_buf[8] = 0;

      strcat(info->display, tmp_buf);
      rtems_object_get_name( ((Objects_Control*)th)->id, 5, info->name );
      info->more_display[0] = 0; /* Nothing */

      return 1;
   }

   first_posix_id = first_rtems_id + (max_id - min_id) + 1;

   obj_info = _Objects_Information_table[OBJECTS_POSIX_API][1];

   min_id = obj_info->minimum_id;
   max_id = obj_info->maximum_id;

   th = (Thread_Control *)(obj_info->local_table[thread - first_posix_id + 1]);
   if (th == NULL) {
      /* Thread does not exist */
      return 0;
   }

   strcpy(info->display, "posix thread: control at 0x");

   tmp_buf[0] = gdb_hexchars[(((int)th) >> 28) & 0xf];
   tmp_buf[1] = gdb_hexchars[(((int)th) >> 24) & 0xf];
   tmp_buf[2] = gdb_hexchars[(((int)th) >> 20) & 0xf];
   tmp_buf[3] = gdb_hexchars[(((int)th) >> 16) & 0xf];
   tmp_buf[4] = gdb_hexchars[(((int)th) >> 12) & 0xf];
   tmp_buf[5] = gdb_hexchars[(((int)th) >> 8) & 0xf];
   tmp_buf[6] = gdb_hexchars[(((int)th) >> 4) & 0xf];
   tmp_buf[7] = gdb_hexchars[((int)th) & 0xf];
   tmp_buf[8] = 0;

   strcat(info->display, tmp_buf);
   rtems_object_get_name( ((Objects_Control*)th)->id, 5, info->name );
   info->more_display[0] = 0; /* Nothing */

   return 1;
}

/*******************************************************/

/* Format: x<type-1x>,<address-x>,<length-x>, where x is 'z' or 'Z' */
int parse_zbreak(const char *in, int *type, unsigned char **addr, int *len)
{
  int ttmp, atmp, ltmp;

  ASSERT(in != NULL);
  ASSERT(type != NULL);
  ASSERT(addr != NULL);
  ASSERT(len != NULL);

  ASSERT(*in == 'z' || *in == 'Z');

  in++;

  if (!hstr2nibble(in, &ttmp) || *(in+1) != ',')
    {
      return 0;
    }
  in += 2;

  in = vhstr2int(in, &atmp);
  if (in == NULL || *in != ',')
    {
      return 0;
    }
  in++;

  in = vhstr2int(in, &ltmp);
  if (in == NULL || ltmp < 1)
    {
      return 0;
    }

  *type = ttmp;
  *addr = (unsigned char *)atmp;
  *len  = ltmp;

  return 1;
}

/* Format: qP<mask-08x><thread_id-ft> */
static int
parse_qp(const char *in, int *mask, int *thread)
{
  const char *ptr;

  ASSERT(in != NULL);
  ASSERT(*in == 'q');
  ASSERT(*(in+1) == 'P');

  ptr = fhstr2int(in+2, mask);
  if (ptr == NULL)
    {
      return 0;
    }

  ptr = fhstr2thread(ptr, thread);
  if (ptr == NULL)
    {
      return 0;
    }

  return 1;
}

/* Format: qQ<mask-08x><thread_id-ft><tag-08x><length-02x><value>...] */
static void
pack_qq(char *out, int mask, int thread, struct rtems_gdb_stub_thread_info *info)
{
  int len;

  ASSERT(out != NULL);
  ASSERT(info != NULL);

  *out++ = 'q';
  *out++ = 'Q';
  out = int2fhstr(out, mask);
  out = thread2fhstr(out, thread);

  if (mask & 0x1) {
    /* Thread id once again */
    memcpy(out, "00000001", 8);
    out   += 8;
    *out++ = '1';
    *out++ = '0';
    out = thread2fhstr(out, thread);
  }

  if (mask & 0x2) {
    /* Exists */
    memcpy(out, "00000002", 8);
    out   += 8;
    *out++ = '0';
    *out++ = '1';
    *out++ = '1';
  }

  if (mask & 0x4) {
    /* Display */
    memcpy(out, "00000004", 8);
    out += 8;

    info->display[sizeof(info->display)-1] = 0; /* Fot God sake */

    len = strlen(info->display);

    *out++ = gdb_hexchars[len >> 4];
    *out++ = gdb_hexchars[len & 0x0f];

    memcpy(out, info->display, len);

    out += len;
  }

  if (mask & 0x8) {
    /* Name */
    memcpy(out, "00000008", 8);
    out += 8;

    info->name[sizeof(info->name)-1] = 0; /* Fot God sake */

    len = strlen(info->name);

    *out++ = gdb_hexchars[len >> 4];
    *out++ = gdb_hexchars[len & 0x0f];

    memcpy(out, info->name, len);

    out += len;
  }

  if (mask & 0x10) {
    /* More display */
    memcpy(out, "00000010", 8);
    out += 8;

    info->more_display[sizeof(info->more_display)-1] = 0; /* Fot God sake */

    len = strlen(info->more_display);

    *out++ = gdb_hexchars[len >> 4];
    *out++ = gdb_hexchars[len & 0x0f];

    memcpy(out, info->more_display, len);

    out += len;
  }

  *out = 0;

  return;
}

/* Format qL<first-01x><max_count-02x><arg_thread_id-ft> */
static int
parse_ql(const char *in, int *first, int *max_count, int *athread)
{
  const char *ptr;

  ASSERT(in != NULL);
  ASSERT(*in == 'q');
  ASSERT(*(in+1) == 'L');
  ASSERT(first != NULL);
  ASSERT(max_count != NULL);
  ASSERT(athread != NULL);

  ptr = in + 2;

  /*  First */
  if (!hstr2nibble(ptr, first))
    {
      return 0;
    }
  ptr++;

  /* Max count */
  if (!hstr2byte(ptr, max_count))
    {
      return 0;
    }
  ptr += 2;

  /* A thread */
  ptr = fhstr2thread(ptr, athread);
  if (ptr == NULL)
    {
      return 0;
    }

  return 1;
}

/* Format: qM<count-02x><done-01x><arg_thread_id>[<found_thread_id-ft>...] */
static char *
reserve_qm_header(char *out)
{
  ASSERT(out != NULL);

  return out + 21;
}

/* Format: qM<count-02x><done-01x><arg_thread_id>[<found_thread_id-ft>...] */
static char*
pack_qm_thread(char *out, int thread)
{
  ASSERT(out != 0);

  return thread2fhstr(out, thread);
}

/* Format: qM<count-02x><done-01x><arg_thread_id>[<found_thread_id-ft>...] */
static void
pack_qm_header(char *out, int count, int done, int athread)
{
   ASSERT(out != 0);
   ASSERT(count >= 0 && count < 256);

   *out++ = 'q';
   *out++ = 'M';

   *out++ = gdb_hexchars[(count >> 4) & 0x0f];
   *out++ = gdb_hexchars[count & 0x0f];

   if (done) {
      *out++ = '1';
   } else {
      *out++ = '0';
   }

   thread2fhstr(out, athread);
   return;
}

void rtems_gdb_process_query(
  char *inbuffer,
  char *outbuffer,
  int   do_threads,
  int   thread
)
{
  char *optr;

  switch(inbuffer[1]) {
    case 'C':
      /* Current thread query query - return stopped thread */
      if (!do_threads) {
        break;
      }

      optr = outbuffer;

      *optr++ = 'Q';
      *optr++ = 'C';
      optr    = thread2vhstr(optr, thread);
      *optr   = 0;
      break;

    case 'P':
      /* Thread info query */
      if (!do_threads) {
        break;
      }

      {
        int ret, rthread, mask;
        struct rtems_gdb_stub_thread_info info;

        ret = parse_qp(inbuffer, &mask, &rthread);
        if (!ret|| mask & ~0x1f) {
          strcpy(outbuffer, "E01");
          break;
        }

        ret = rtems_gdb_stub_get_thread_info(rthread, &info);
        if (!ret) {
          /* Good implementation would never ask for non-existing thread,
             should we care about bad ones - it does not seem so */
          strcpy(outbuffer, "E02");
          break;
        }

        /* Build response */
        pack_qq(outbuffer, mask, rthread, &info);
      }
      break;

    case 'L':
      /* Thread list query */
      if (!do_threads) {
        break;
      }

      {
        int ret, athread, first, max_cnt, i, done, rthread;

        ret = parse_ql(inbuffer, &first, &max_cnt, &athread);
        if (!ret) {
          strcpy(outbuffer, "E02");
          break;
        }

        if (max_cnt == 0) {
          strcpy(outbuffer, "E02");
          break;
        }

        if (max_cnt > QM_MAX_THREADS) {
          /* Limit max count by buffer size */
          max_cnt = QM_MAX_THREADS;
        }

        /* Reserve place for output header */
        optr = reserve_qm_header(outbuffer);

        if (first) {
          rthread = 0;
        } else {
          rthread = athread;
        }

        done = 0;

        for (i=0; i<max_cnt; i++) {
          rthread = rtems_gdb_stub_get_next_thread(rthread);

          if (rthread <= 0) {
            done = 1; /* Set done flag */
            break;
          }

          optr = pack_qm_thread(optr, rthread);
        }

        *optr = 0;

        ASSERT((optr - outbuffer) < BUFMAX);

        /* Fill header */
        pack_qm_header(outbuffer, i, done, athread);
      }
      break;

    default:
      if (memcmp(inbuffer, "qOffsets", 8) == 0) {
        unsigned char *t, *d, *b;
        char *out;

        if (!rtems_gdb_stub_get_offsets(&t, &d, &b)) {
          break;
        }

        out = outbuffer;

        *out++ = 'T';
        *out++ = 'e';
        *out++ = 'x';
        *out++ = 't';
        *out++ = '=';

        out = int2vhstr(out, (int)t);

        *out++ = ';';
        *out++ = 'D';
        *out++ = 'a';
        *out++ = 't';
        *out++ = 'a';
        *out++ = '=';

        out = int2vhstr(out, (int)d);

        *out++ = ';';
        *out++ = 'B';
        *out++ = 's';
        *out++ = 's';
        *out++ = '=';

        out = int2vhstr(out, (int)b);

        *out++ = ';';
        *out++ = 0;

        break;
      }

      /*  qCRC, qRcmd, qu and qz will be added here */
      break;
    }
}

/* Present thread in the variable length string format */
char*
thread2vhstr(char *buf, int thread)
{
  int i, nibble, shift;

  ASSERT(buf != NULL);

  for(i=0, shift=28; i<8; i++, shift-=4)
    {
      nibble = (thread >> shift) & 0x0f;

      if (nibble != 0)
	{
	  break;
	}
    }

  if (i == 8)
    {
      *buf++ = '0';
      return buf;
    }

  *buf++ = gdb_hexchars[nibble];

  for(i++, shift-=4; i<8; i++, shift-=4, buf++)
    {
      nibble = (thread >> shift) & 0x0f;
      *buf   = gdb_hexchars[nibble];
    }

  return buf;
}

/* Present thread in fixed length string format */
char*
thread2fhstr(char *buf, int thread)
{
  int i, nibble, shift;

  ASSERT(buf != NULL);

  for(i=0; i<8; i++, buf++)
    {
      *buf = '0';
    }

  for(i=0, shift=28; i<8; i++, shift-=4, buf++)
    {
      nibble = (thread >> shift) & 0x0f;
      *buf   = gdb_hexchars[nibble];
    }

  return buf;
}

/* Parse thread presented in fixed length format */
const char*
fhstr2thread(const char *buf, int *thread)
{
  int i, val, nibble;

  ASSERT(buf != NULL);
  ASSERT(thread != NULL);

  for(i=0; i<8; i++, buf++)
    {
      if (*buf != '0')
	{
	  return NULL;
	}
    }

  val = 0;

  for(i=0; i<8; i++, buf++)
    {
      if (!hstr2nibble(buf, &nibble))
	{
	  return NULL;
	}

      ASSERT(nibble >=0 && nibble < 16);

      val = (val << 4) | nibble;
    }

  *thread = val;

  return buf;
}

/* Parse thread presented in variable length format */
const char*
vhstr2thread(const char *buf, int *thread)
{
  int i, val, nibble;
  int found_zero, lim;

  ASSERT(buf != NULL);
  ASSERT(thread != NULL);

  /* If we have leading zeros, skip them */
  found_zero = 0;

  for(i=0; i<16; i++, buf++)
    {
      if (*buf != '0')
	{
	  break;
	}

      found_zero = 1;
    }

  /* Process non-zeros */
  lim = 16 - i;
  val = 0;

  for(i=0; i<lim; i++, buf++)
    {
      if (!hstr2nibble(buf, &nibble))
	{
	  if (i == 0 && !found_zero)
	    {
	      /* Empty value */
	      return NULL;
	    }

	  *thread = val;
	  return buf;
	}

      ASSERT(nibble >= 0 && nibble < 16);

      val = (val << 4) | nibble;
    }

  if (hstr2nibble(buf, &nibble))
    {
      /* Value is too long */
      return NULL;
    }

  *thread = val;
  return buf;
}

/* Present integer in the variable length string format */
char*
int2vhstr(char *buf, int val)
{
  int i, nibble, shift;

  ASSERT(buf != NULL);

  for(i=0, shift=28; i<8; i++, shift-=4)
    {
      nibble = (val >> shift) & 0x0f;

      if (nibble != 0)
	{
	  break;
	}
    }

  if (i == 8)
    {
      *buf++ = '0';
      return buf;
    }

  *buf++ = gdb_hexchars[nibble];

  for(i++, shift-=4; i<8; i++, shift-=4, buf++)
    {
      nibble = (val >> shift) & 0x0f;
      *buf   = gdb_hexchars[nibble];
    }

  return buf;
}

/* Present int in fixed length string format */
char*
int2fhstr(char *buf, int val)
{
  int i, nibble, shift;

  ASSERT(buf != NULL);

  for(i=0, shift=28; i<8; i++, shift-=4, buf++)
    {
      nibble = (val >> shift) & 0x0f;
      *buf   = gdb_hexchars[nibble];
    }

  return buf;
}

/* Parse int presented in fixed length format */
const char*
fhstr2int(const char *buf, int *ival)
{
  int i, val, nibble;

  ASSERT(buf != NULL);
  ASSERT(ival != NULL);

  val = 0;

  for(i=0; i<8; i++, buf++)
    {
      if (!hstr2nibble(buf, &nibble))
	{
	  return NULL;
	}

      ASSERT(nibble >=0 && nibble < 16);

      val = (val << 4) | nibble;
    }

  *ival = val;

  return buf;
}

/* Parse int presented in variable length format */
const char*
vhstr2int(const char *buf, int *ival)
{
  int i, val, nibble;
  int found_zero, lim;

  ASSERT(buf != NULL);
  ASSERT(ival != NULL);

  /* If we have leading zeros, skip them */
  found_zero = 0;

  for(i=0; i<8; i++, buf++)
    {
      if (*buf != '0')
	{
	  break;
	}

      found_zero = 1;
    }

  /* Process non-zeros */
  lim = 8 - i;
  val = 0;

  for(i=0; i<lim; i++, buf++)
    {
      if (!hstr2nibble(buf, &nibble))
	{
	  if (i == 0 && !found_zero)
	    {
	      /* Empty value */
	      return NULL;
	    }

	  *ival = val;
	  return buf;
	}

      ASSERT(nibble >= 0 && nibble < 16);

      val = (val << 4) | nibble;
    }

  if (hstr2nibble(buf, &nibble))
    {
      /* Value is too long */
      return NULL;
    }

  *ival = val;
  return buf;
}

int
hstr2byte(const char *buf, int *bval)
{
  int hnib, lnib;

  ASSERT(buf != NULL);
  ASSERT(bval != NULL);

  if (!hstr2nibble(buf, &hnib) || !hstr2nibble(buf+1, &lnib))
    {
      return 0;
    }

  *bval = (hnib << 4) | lnib;
  return 1;
}

int
hstr2nibble(const char *buf, int *nibble)
{
  int ch;

  ASSERT(buf != NULL);
  ASSERT(nibble != NULL);

  ch = *buf;

  if (ch >= '0' && ch <= '9')
    {
      *nibble = ch  - '0';
      return 1;
    }

  if (ch >= 'a' && ch <= 'f')
    {
      *nibble = ch - 'a' + 10;
      return 1;
    }

  if (ch >= 'A' && ch <= 'F')
    {
      *nibble = ch - 'A' + 10;
      return 1;
    }

  return 0;
}

static volatile char mem_err = 0;
void  set_mem_err(void);
static void (*volatile mem_fault_routine) (void) = NULL;

/* convert count bytes of the memory pointed to by mem into hex string,
   placing result in buf, return pointer to next location in hex strng
   in case of success or NULL otherwise */
char*
mem2hstr(char *buf, const unsigned char *mem, int count)
{
  int i;
  unsigned char ch;

  mem_err = 0;

  mem_fault_routine = set_mem_err;

  for (i = 0; i<count; i++, mem++)
    {
      ch = get_byte (mem);
      if (mem_err)
	{
	  mem_fault_routine = NULL;
	  return NULL;
	}

      *buf++ = gdb_hexchars[ch >> 4];
      *buf++ = gdb_hexchars[ch & 0x0f];
    }

  *buf = 0;

  mem_fault_routine = NULL;

  return buf;
}

/* convert the hex string to into count bytes of binary to be placed in mem
   return 1 in case of success and  0 otherwise */
int
hstr2mem (unsigned char *mem, const char *buf, int count)
{
  int i;
  int bval;

  mem_err = 0;

  mem_fault_routine = set_mem_err;

  for (i = 0; i < count; i++, mem++, buf+=2)
    {
      if (!hstr2byte(buf, &bval))
	{
	  mem_fault_routine = NULL;
	  return 0;
	}

      ASSERT(bval >=0 && bval < 256);

      set_byte (mem, bval);

      if (mem_err)
	{
	  mem_fault_routine = NULL;
	  return 0;
	}
    }

  mem_fault_routine = NULL;
  return 1;
}

void
set_mem_err (void)
{
  mem_err = 1;
}

/* These are separate functions so that they are so short and sweet
   that the compiler won't save any registers (if there is a fault
   to mem_fault, they won't get restored, so there better not be any
   saved).  */

unsigned char
get_byte (const unsigned char *addr)
{
  return *addr;
}

void
set_byte (unsigned char *addr, int val)
{
  *addr = val;
}

/*
 *  From here down, the code is CPU model specific and generally maps
 *  the RTEMS thread context format to gdb's.
 */

#if defined(__i386__)

#include "i386-stub.h"

/* Packing order of registers */
enum i386_stub_regnames {
  I386_STUB_REG_EAX, I386_STUB_REG_ECX, I386_STUB_REG_EDX, I386_STUB_REG_EBX,
  I386_STUB_REG_ESP, I386_STUB_REG_EBP, I386_STUB_REG_ESI, I386_STUB_REG_EDI,
  I386_STUB_REG_PC /* also known as eip */ ,
  I386_STUB_REG_PS /* also known as eflags */ ,
  I386_STUB_REG_CS, I386_STUB_REG_SS, I386_STUB_REG_DS, I386_STUB_REG_ES,
  I386_STUB_REG_FS, I386_STUB_REG_GS
};

void rtems_gdb_stub_get_registers_from_context(
  unsigned int   *registers,
  Thread_Control *th
)
{
  registers[I386_STUB_REG_EAX] = 0;
  registers[I386_STUB_REG_ECX] = 0;
  registers[I386_STUB_REG_EDX] = 0;
  registers[I386_STUB_REG_EBX] = (int)th->Registers.ebx;
  registers[I386_STUB_REG_ESP] = (int)th->Registers.esp;
  registers[I386_STUB_REG_EBP] = (int)th->Registers.ebp;
  registers[I386_STUB_REG_ESI] = (int)th->Registers.esi;
  registers[I386_STUB_REG_EDI] = (int)th->Registers.edi;
  registers[I386_STUB_REG_PC]  = *(int *)th->Registers.esp;
  registers[I386_STUB_REG_PS]  = (int)th->Registers.eflags;

  /* RTEMS never changes base registers (especially once
     threads are running) */

  registers[I386_STUB_REG_CS]  = 0x8; /* We just know these values */
  registers[I386_STUB_REG_SS]  = 0x10;
  registers[I386_STUB_REG_DS]  = 0x10;
  registers[I386_STUB_REG_ES]  = 0x10;
  registers[I386_STUB_REG_FS]  = 0x10;
  registers[I386_STUB_REG_GS]  = 0x10;
}

int rtems_gdb_stub_get_offsets(
  unsigned char **text_addr,
  unsigned char **data_addr,
  unsigned char **bss_addr
)
{
  extern unsigned char _text_start;
  extern unsigned char _data_start;
  extern unsigned char _bss_start;

  *text_addr = &_text_start;
  *data_addr = &_data_start;
  *bss_addr  = &_bss_start;

  return 1;
}

#elif defined(__mips__)

void rtems_gdb_stub_get_registers_from_context(
  unsigned int   *registers,
  Thread_Control *th
)
{
   registers[S0] = (unsigned)th->Registers.s0;
   registers[S1] = (unsigned)th->Registers.s1;
   registers[S2] = (unsigned)th->Registers.s2;
   registers[S3] = (unsigned)th->Registers.s3;
   registers[S4] = (unsigned)th->Registers.s4;
   registers[S5] = (unsigned)th->Registers.s5;
   registers[S6] = (unsigned)th->Registers.s6;
   registers[S7] = (unsigned)th->Registers.s7;

   registers[SP] = (unsigned)th->Registers.sp;
   registers[RA] = (unsigned)th->Registers.ra;

   registers[SR] = (unsigned)th->Registers.c0_sr;
   registers[PC] = (unsigned)th->Registers.c0_epc;
}

int rtems_gdb_stub_get_offsets(
  unsigned char **text_addr,
  unsigned char **data_addr,
  unsigned char **bss_addr
)
{
/*
  extern uint32_t   _ftext;
  extern uint32_t   _fdata;
  extern uint32_t   _bss_start;

  *text_addr = &_ftext;
  *data_addr = &_fdata;
  *bss_addr  = &_bss_start;
*/
  *text_addr = 0;
  *data_addr = 0;
  *bss_addr  = 0;
  return 1;
}

#elif defined(__mc68000__)

void rtems_gdb_stub_get_registers_from_context(
  unsigned int   *registers,
  Thread_Control *th
)
{
  /*
   * how about register D0/D1/A0/A1
   * they are located on thread stack ...
   * -> they are not needed for context switch
   */
  registers[D0] = 0;
  registers[D1] = 0;
  registers[D2] = (uint32_t)th->Registers.d2;
  registers[D3] = (uint32_t)th->Registers.d3;
  registers[D4] = (uint32_t)th->Registers.d4;
  registers[D5] = (uint32_t)th->Registers.d5;
  registers[D6] = (uint32_t)th->Registers.d6;
  registers[D7] = (uint32_t)th->Registers.d7;

  registers[A0] = 0;
  registers[A1] = 0;
  registers[A2] = (uint32_t)th->Registers.a2;
  registers[A3] = (uint32_t)th->Registers.a3;
  registers[A4] = (uint32_t)th->Registers.a4;
  registers[A5] = (uint32_t)th->Registers.a5;
  registers[A6] = (uint32_t)th->Registers.a6;
  registers[A7] = (uint32_t)th->Registers.a7_msp;

  registers[PS] = (uint32_t)th->Registers.sr;
#if 0
  registers[PC] = *(uint32_t*)th->Registers.a7_msp; /* *SP = ret adr */
#else
  registers[PC] = (uint32_t)_CPU_Context_switch;
#endif
}

int rtems_gdb_stub_get_offsets(
  unsigned char **text_addr,
  unsigned char **data_addr,
  unsigned char **bss_addr
)
{
/*
  extern uint32_t   _ftext;
  extern uint32_t   _fdata;
  extern uint32_t   _bss_start;

  *text_addr = &_ftext;
  *data_addr = &_fdata;
  *bss_addr  = &_bss_start;
*/
  *text_addr = 0;
  *data_addr = 0;
  *bss_addr  = 0;
  return 1;
}

#elif defined(__lm32__)

void rtems_gdb_stub_get_registers_from_context(
  unsigned int   *registers,
  Thread_Control *th
)
{
  registers[LM32_REG_R11] = (uint32_t)th->Registers.r11;
  registers[LM32_REG_R12] = (uint32_t)th->Registers.r12;
  registers[LM32_REG_R13] = (uint32_t)th->Registers.r13;
  registers[LM32_REG_R14] = (uint32_t)th->Registers.r14;
  registers[LM32_REG_R15] = (uint32_t)th->Registers.r15;
  registers[LM32_REG_R16] = (uint32_t)th->Registers.r16;
  registers[LM32_REG_R17] = (uint32_t)th->Registers.r17;
  registers[LM32_REG_R18] = (uint32_t)th->Registers.r18;
  registers[LM32_REG_R19] = (uint32_t)th->Registers.r19;
  registers[LM32_REG_R20] = (uint32_t)th->Registers.r20;
  registers[LM32_REG_R21] = (uint32_t)th->Registers.r21;
  registers[LM32_REG_R22] = (uint32_t)th->Registers.r22;
  registers[LM32_REG_R23] = (uint32_t)th->Registers.r23;
  registers[LM32_REG_R24] = (uint32_t)th->Registers.r24;
  registers[LM32_REG_R25] = (uint32_t)th->Registers.r25;
  registers[LM32_REG_GP] = (uint32_t)th->Registers.gp;
  registers[LM32_REG_FP] = (uint32_t)th->Registers.fp;
  registers[LM32_REG_SP] = (uint32_t)th->Registers.sp;
  registers[LM32_REG_RA] = (uint32_t)th->Registers.ra;
  registers[LM32_REG_IE] = (uint32_t)th->Registers.ie;
#if 1
  registers[LM32_REG_PC] = (uint32_t)th->Registers.epc;
#else
  registers[LM32_REG_PC] = (uint32_t)_CPU_Context_switch;
#endif
}

int rtems_gdb_stub_get_offsets(
  unsigned char **text_addr,
  unsigned char **data_addr,
  unsigned char **bss_addr
)
{
  *text_addr = 0;
  *data_addr = 0;
  *bss_addr  = 0;
  return 1;
}

#else
#error "rtems-gdb-stub.c: Unsupported CPU!"
#endif
