/*
 *  $Id$
 */

typedef enum
{
        CON_KBHIT,
        CON_GET_RAW_BYTE,
        CON_SEND_RAW_BYTE
} console_ioctl_t;

typedef struct
{
        console_ioctl_t ioctl_type;
   unsigned32 param;
} console_ioctl_request_t;
