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
   uint32_t   param;
} console_ioctl_request_t;
