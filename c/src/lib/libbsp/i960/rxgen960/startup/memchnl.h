/*-------------------------------------*/
/* memchnl.h                           */
/* Last change : 16. 5.95              */
/*-------------------------------------*/ 
/*
 *  $Id$
 */

#ifndef _MEMCHNL_H_
#define _MEMCHNL_H_

/* The following is a dummy for now to be filled in
	as the message passing stuff gets migrated to 
	I20 (or other standard )
*/

typedef
struct memMsg{
	int * foo;
} IOTrCBlk, * iotrcb_ptr_t;

typedef void * iotrcb_sgm_t;

  /* Interrupt arrived from X processor.
   * This is invoked from low-level interrupt dispatcher.
   */
extern void memChnlIntrFromHost(void);
  /* This IOT's processing is completed.
   * Signal the host, it can take it back.
   */
extern void memChnlIotFinished(IOTrCBlk *);
  /* A fault has just occured. Pass a signal over
   * memory channel.
   */
extern void memChnlI960Fault(void);
  /* IOT handler procedure.
   */   
typedef void (* IotHandler)(IOTrCBlk *, int tr_req);
  /* Register handler to process IOT's.
   */   
extern void memChnlRegisterHndl(IotHandler);
  /* Intermediate image buffer.
   * Defined in *.ld
   */
extern unsigned int downloadStart[];   

typedef struct {
  int ptr_len;
  iotrcb_ptr_t ptr_loc;
  iotrcb_ptr_t * ptr_crnt;
  iotrcb_sgm_t * sgm_crnt;
} BufLookupHndl;
  /* Procedures to travel throughout the buffer page list.
   * It should be accomplished as below:
   *    ...
   *    BufLookupHndl hndl;
   *    char * buf;
   *    if (memBufLookupInit(& hndl, iot) != 0)  {
   *       while ((buf = memBufLookupNext(& hndl)) != 0)  {
   *         ... buf is a buffer page address.
   *       }
   *    }
   */
extern BufLookupHndl * memBufLookupInit(BufLookupHndl *, IOTrCBlk *);
extern void * memBufLookupNext(BufLookupHndl *);

#endif
/*-------------*/
/* End of file */
/*-------------*/

