#ifndef __mouse_parser_h__
#define __mouse_parser_h__

#include <rtems/mw_uid.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Use the same definitions as the user interface */
#define RBUTTON      MV_BUTTON_RIGHT
#define MBUTTON      MV_BUTTON_CENTER
#define LBUTTON      MV_BUTTON_LEFT

typedef int		         COORD;		/* device coordinates*/
typedef unsigned int	   BUTTON;		/* mouse button mask*/

/* local routines */
int  	   MOU_Init(void);
int      MOU_Data( int ch, COORD *dx, COORD *dy, COORD *dz, BUTTON *bptr );

/* Mouse Interface */
void register_mou_msg_queue( char * qname, int port );
void unregister_mou_msg_queue( int port );

/* KBD Interface */
void register_kbd_msg_queue( char *qname, int port );
void unregister_kbd_msg_queue( int port );

#ifdef __cplusplus
}
#endif

#endif /*  __mouse_parser_h__  */
