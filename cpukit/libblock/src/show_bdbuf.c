/*===============================================================*\
| Project: RTEMS bdbuf inspector                                  |
+-----------------------------------------------------------------+
| File: show_bdbuf.c
+-----------------------------------------------------------------+
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains functions to enable the monitor              |
| to show bdbuf information                                       |
|                                                                 |
|                    XXX!!! ATTETION!!! XXX!!!                    |
|                                                                 |
| This module inspects the bdbuf data structures,                 |
| assuming they are static, but in fact they are used very        |
| dynamically. Therefore the results show MAY BE INCORRECT in     |
| some cases. And, to cure this a bit, this module may block      |
| preemption for a rather long time and therefore it may          |
| BREAK THE REALTIME BEHAVIOUR OF YOUR SYSTEM (when in use)       |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 26.09.06  creation                                         doe  |
|*****************************************************************|
\*===============================================================*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/bdbuf.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <rtems/libio.h>
#include <inttypes.h>

typedef struct {
  bool bdbuf_modified;
  bool bdbuf_in_progress;
  bool bdbuf_actual;
  bool bdbuf_used;
  bool bdbuf_all;
  rtems_bdpool_id pool_id;
} show_bdbuf_filter_t;

typedef struct {
  bool show_all;
  bool show_node_chain;
  bool show_dev;
  bool show_blocknum;
  bool show_error;
  bool show_state;
  bool show_use_count;
  bool show_pool_id;
  bool show_sema;
} show_bdbuf_selector_t;

typedef enum {bdbuf_chain_ident_none,
	      bdbuf_chain_ident_free,
	      bdbuf_chain_ident_lru,
	      bdbuf_chain_ident_mod} bdbuf_chain_identifier_t;

typedef struct {
  rtems_bdpool_id pool_id;
  int index;
  bdbuf_chain_identifier_t in_chain;
  dev_t dev;
  blkdev_bnum blknum;
  rtems_status_code status;
  int error;
  bool modified;
  bool in_progress;
  bool actual;
  int use_count;
  const CORE_mutex_Control *sema;
} show_bdbuf_bdbuf_info_t;

typedef rtems_mode preemption_key_t;
#define DISABLE_PREEMPTION(key) \
    do {                                                               \
        rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &(key)); \
    } while (0)

#define ENABLE_PREEMPTION(key) \
    do {                                                        \
        rtems_mode temp;                                        \
        rtems_task_mode((key), RTEMS_PREEMPT_MASK, &temp);      \
    } while (0)

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_bdbuf_show_follow_chain_node_to_head
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   follow a given chain to its head                                        |
|   XXX: this is executed with preemption disabled                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const Chain_Node *the_node, /* input: node to track to its head  */
 Chain_Control **the_head    /* storage for pointer to chain head */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  preemption_key_t preempt_key;
  bool preempt_disabled = false;
  /*
   * disable preemption
   */
  if (rc == RTEMS_SUCCESSFUL) {
    DISABLE_PREEMPTION(preempt_key);
  }
  /*
   * follow node to its head
   * XXX: this is highly dependent on the chain implementation
   * in score/src/chain.c and friends
   */
  while (the_node->previous != NULL) {
    the_node = the_node->previous;
  }
  /*
   * reenable preemption, if disabled
   */
  if (preempt_disabled) {
    ENABLE_PREEMPTION(preempt_key);
  }
  /*
   * XXX: this depends n the chain implementation in
   * score/include/rtems/score/chain.h:
   * Chain_Control is overlayed by two Cohain_Nodes
   */
  *the_head = (Chain_Control *)the_node;

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_bdbuf_show_determine_chain_of_bdbuf
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   find out, which chain this bdbuf is linked in                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const bdbuf_buffer *the_bdbuf,         /* this is the bdbuf structure     */
 const bdbuf_pool   *curr_pool,         /* the pool this buffer belongs to */
 bdbuf_chain_identifier_t *chn_ident    /* result: identifier for chain    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  Chain_Control *the_chain_control;


  *chn_ident = bdbuf_chain_ident_none;
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_bdbuf_show_follow_chain_node_to_head(&(the_bdbuf->link),
						    &(the_chain_control));
  }
  if (rc == RTEMS_SUCCESSFUL) {
    if (the_chain_control == &(curr_pool->free)) {
      *chn_ident = bdbuf_chain_ident_free;
    }
    else if (the_chain_control == &(curr_pool->lru)) {
      *chn_ident = bdbuf_chain_ident_lru;
    }
    else if (the_chain_control == &(rtems_bdbuf_ctx.mod)) {
      *chn_ident = bdbuf_chain_ident_mod;
    }
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_bdbuf_show_getargs
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   analyze cmd arguments                                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
    int     argc,
    char  **argv,
    show_bdbuf_filter_t *filter,
    show_bdbuf_selector_t *selector
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int arg_error = 0;
  int i;
  char *tmp_ptr;
  int nm_argc = 0;
  /*
   * set filter and selector to default
   */
  memset(filter,0,sizeof(*filter));
  filter->bdbuf_all  = true;
  memset(selector,0,sizeof(*selector));
  selector->show_all = true;

  /*
   * scan arguments
   */
  for (i = 1;
       (i < argc) && (arg_error == 0);
       i++) {
    if (argv[i][0] == '-') {
      /*
       * modifier arguments
       */
      switch(tolower(argv[i][1])) {
	/*
	 * selection, which bdbufs to show
	 */
      case 'm': /* only show bdbufs modified */
	filter->bdbuf_modified = true ;
	filter->bdbuf_all      = false;
	break;
      case 'i': /* only show bdbufs in progress*/
	filter->bdbuf_in_progress = true ;
	filter->bdbuf_all         = false;
	break;
      case 'v': /* only show bdbufs, which have valid data*/
	filter->bdbuf_actual   = true ;
	filter->bdbuf_all      = false;
	break;
      case 'u': /* only show bdbufs, which are in use */
	filter->bdbuf_used     = true ;
	filter->bdbuf_all      = false;
	break;
      case 'p': /* only show bdbufs, which belong to pool <n> */
	filter->pool_id = strtol(argv[i]+2,&tmp_ptr,0);
	if (tmp_ptr == argv[i]+2) { /* no conversion performed... */
	  arg_error = i;
	}
	filter->bdbuf_all      = false;
	break;
	/*
	 * selection, what fields to show
	 */
      case 'n': /* show bdbuf node_chain */
	selector->show_node_chain = true ;
	selector->show_all        = false;
	break;
      case 'd': /* show device           */
	selector->show_dev        = true ;
	selector->show_all        = false;
	break;
      case 'b': /* show blocknum         */
	selector->show_blocknum   = true ;
	selector->show_all        = false;
	break;
      case 'e': /* show bdbuf error status */
	selector->show_error      = true ;
	selector->show_all        = false;
	break;
      case 's': /* show bdbuf state */
	selector->show_state      = true ;
	selector->show_all        = false;
	break;
      case 'c': /* show bdbuf use count */
	selector->show_use_count  = true ;
	selector->show_all        = false;
	break;
      case 'l': /* show bdbuf pool id   */
	selector->show_pool_id    = true ;
	selector->show_all        = false;
	break;
      case 't': /* show bdbuf transfer sema */
	selector->show_sema       = true ;
	break;
      default:
	arg_error = i;
	break;
      }
    }
    else {
      /*
       * non-modifier arguments
       */
      switch(++nm_argc) {
      default: /* no further arguments defined */
	arg_error = i;
	break;
      }
    }
  }
  if (arg_error) {
    printf("%s: unknown argument %s\n",argv[0],argv[arg_error]);
    sc = RTEMS_NOT_DEFINED;
  }
  return sc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_bdbuf_show_get_bufpool
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get buffer pool information                                             |
|   XXX: this should be coupled closer to the bdbuf.c module               |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct bdbuf_pool **pool_base_pptr,
 int *pool_cnt_ptr
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
#if 0
  rtems_status_code pool_rc = RTEMS_SUCCESSFUL;
  struct bdbuf_pool *curr_pool,*pool_base, *pool_top;
  int pool_cnt;
  int pool_probe_size;
  /*
   * get first buffer pool
   * XXX: this is highly dependent on how pools are defined
   * and maintained in bdbuf.c
   */
  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * try all possible pool sizes, get highest/lowest pool address
     */
    pool_base = NULL;
    pool_top  = NULL;
    curr_pool = NULL;
    for (pool_probe_size = 1;
	 pool_probe_size < (INT_MAX>>1) && (pool_rc == RTEMS_SUCCESSFUL);
	 pool_probe_size <<= 1) {
      pool_rc = rtems_bdbuf_find_pool(pool_probe_size,&curr_pool);
      if (pool_rc == RTEMS_SUCCESSFUL) {
	if (pool_base > curr_pool) {
	  pool_base = curr_pool;
	}
	if (pool_top < curr_pool) {
	  pool_top = curr_pool;
	}
      }
    }
    if (pool_base == NULL) {
      rc = RTEMS_UNSATISFIED;
    }
    else {
      pool_cnt = (pool_top - pool_base) + 1;
    }
  }
  if (rc == RTEMS_SUCCESSFUL) {
    *pool_base_pptr = pool_base;
    *pool_cnt_ptr   = pool_cnt;
  }
#else
  if (rc == RTEMS_SUCCESSFUL) {
    *pool_base_pptr = rtems_bdbuf_ctx.pool;
    *pool_cnt_ptr   = rtems_bdbuf_ctx.npools;
  }
#endif
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_bdbuf_show_pool_header
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   print buffer pool information                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int        pool_idx,
 bdbuf_pool *pool_ptr
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;

  if (rc == RTEMS_SUCCESSFUL) {
    printf("------------------------------------------------------------------------------\n");
    printf(" pool #%03d: blksize=%5u nblks=%5u buf_mem=0x%08" PRIxPTR " bdbuf_mem=0x%08" PRIxPTR "\n",
	   pool_idx,
	   pool_ptr->blksize,
	   pool_ptr->nblks,
	   (intptr_t) pool_ptr->mallocd_bufs,
	   (intptr_t) pool_ptr->bdbufs);
    printf("------------------------------------------------------------------------------\n");
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_show_bdbuf_get_bdbuf_info
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   get buffer pool information                                             |
|   XXX: this should be coupled closer to the bdbuf.c module                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const bdbuf_buffer *the_bdbuf,         /* this is the bdbuf structure     */
 int           bdbuf_idx,               /* index of bdbuf                  */
 const bdbuf_pool   *curr_pool,         /* the pool this buffer belongs to */
 show_bdbuf_bdbuf_info_t *bdbuf_info    /* struct to  store info of bdbuf */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;

  /*
   * determine the chain we are in
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_bdbuf_show_determine_chain_of_bdbuf(the_bdbuf,curr_pool,
        &(bdbuf_info->in_chain));
    if (rc != RTEMS_SUCCESSFUL) {
      bdbuf_info->in_chain = bdbuf_chain_ident_none;
      rc = RTEMS_SUCCESSFUL;
    }
  }

  if (rc == RTEMS_SUCCESSFUL) {
    bdbuf_info->index       = bdbuf_idx;
    bdbuf_info->dev         = the_bdbuf->dev;
    bdbuf_info->blknum      = the_bdbuf->block;
    bdbuf_info->status      = the_bdbuf->status;
    bdbuf_info->error       = the_bdbuf->error;
    bdbuf_info->modified    = the_bdbuf->modified;
    bdbuf_info->in_progress = the_bdbuf->in_progress;
    bdbuf_info->actual      = the_bdbuf->actual;
    bdbuf_info->use_count   = the_bdbuf->use_count;
    bdbuf_info->sema        = &(the_bdbuf->transfer_sema);
    bdbuf_info->pool_id     = the_bdbuf->pool;
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_show_bdbuf_match_filter
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   match bdbuf info with given filter                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const show_bdbuf_bdbuf_info_t *bdbuf_info, /* struct to  store info of bdbuf */
 const show_bdbuf_filter_t *filter,
 bool *is_match
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;
  bool unmatch = false;

  if (rc == RTEMS_SUCCESSFUL) {
    if (filter->bdbuf_all) {
      unmatch = false;
    }
    else {
      unmatch = ((filter->bdbuf_modified    && !bdbuf_info->modified) ||
		 (filter->bdbuf_in_progress && !bdbuf_info->in_progress) ||
		 (filter->bdbuf_actual      && !bdbuf_info->actual) ||
		 (filter->bdbuf_used        && !(bdbuf_info->use_count > 0)));

    }
    *is_match = !unmatch;
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_show_bdbuf_print_wait_chain
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   list tasks waiting in "transfer_sema" chain                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 bdbuf_buffer *the_bdbuf                /* this is the bdbuf structure     */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  Chain_Control *the_chain_head;
  const Chain_Node    *the_chain_node;
  int   thread_cnt = 0;
  const Thread_Control *the_thread;
  Objects_Id      thread_id;
  Objects_Name    thread_name;
  uint32_t        thread_name_nonstring;
  /*
   * get head of (fifo) wait chain
   */
  if (rc == RTEMS_SUCCESSFUL) {
    the_chain_head = &(the_bdbuf->transfer_sema.Wait_queue.Queues.Fifo);
    the_chain_node = _Chain_First(the_chain_head);
  }
  /*
   * walk through thread chain
   */
  while ((rc == RTEMS_SUCCESSFUL) &&
	 (the_chain_node != _Chain_Tail( the_chain_head ))) {
    thread_cnt++;
    the_thread = (const Thread_Control *)the_chain_node;

    thread_id   = the_thread->Object.id;
    thread_name = the_thread->Object.name;
    thread_name_nonstring = (uint32_t)thread_name.name_u32;
    printf("%20s %3d (0x%08" PRIx32 ") %c%c%c%c\n",
	   ((thread_cnt == 1) ? "Threads waiting:" : ""),
	   thread_cnt,thread_id,
	   (char)((thread_name_nonstring >> 24) & 0xff),
	   (char)((thread_name_nonstring >> 16) & 0xff),
	   (char)((thread_name_nonstring >>  8) & 0xff),
	   (char)((thread_name_nonstring >>  0) & 0xff));

    the_chain_node = the_chain_node->next;
  }

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_show_bdbuf_print
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   print requested bdbuffer information                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const show_bdbuf_bdbuf_info_t *bdbuf_info, /* info of bdbuf               */
 show_bdbuf_selector_t * selector,          /* selector, what to show      */
 bool       print_header             /* true: print header, not info    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;

  /*
   * 6 chars: print index of buffer
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (print_header) {
      printf("INDEX ");
    }
    else {
      printf("%5u ",bdbuf_info->index);
    }
  }
  /*
   * 3 chars: print info about the pool id of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_use_count))) {
    if (print_header) {
      printf("PL ");
    }
    else {
      printf("%2u ",bdbuf_info->pool_id);
    }
  }

  /*
   * 4 chars: print info about chain (lru/free/mod) of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_node_chain))) {
    if (print_header) {
      printf("CHN ");
    }
    else {
      printf("%3s ",
	     ((bdbuf_info->in_chain == bdbuf_chain_ident_free)  ? "FRE"
	      : (bdbuf_info->in_chain == bdbuf_chain_ident_lru) ? "LRU"
	      : (bdbuf_info->in_chain == bdbuf_chain_ident_mod) ? "MOD"
	      : "???"));
    }
  }

  /*
   * 7 chars: print info about device of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_dev))) {
    if (print_header) {
      printf("DEVICE ");
    }
    else {
      printf("%3" PRIu32 "%2" PRIu32,
	     ((bdbuf_info->dev == -1)
	      ? 0 : rtems_filesystem_dev_major_t(bdbuf_info->dev)),
	     ((bdbuf_info->dev == -1)
	      ? 0 : rtems_filesystem_dev_minor_t(bdbuf_info->dev)));
    }
  }

  /*
   * 7 chars: print info about block number of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_blocknum))) {
    if (print_header) {
      printf("BLOCK  ");
    }
    else {
      printf("%6" PRIu32,bdbuf_info->blknum);
    }
  }

  /*
   * 4 chars: print info about use count of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_use_count))) {
    if (print_header) {
      printf("USE ");
    }
    else {
      printf("%3u ",bdbuf_info->use_count);
    }
  }

  /*
   * 4 chars: print info about state of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_state))) {
    if (print_header) {
      printf("STA ");
    }
    else {
      printf("%c%c%c ",
	     (bdbuf_info->modified    ? 'M' : '.'),
	     (bdbuf_info->in_progress ? 'P' : '.'),
	     (bdbuf_info->actual      ? 'A' : '.'));
    }
  }

  /*
   * 42 chars: print info about error of this buffer
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((selector->show_all) ||
       (selector->show_error))) {
    if (print_header) {
      printf("%20s:%-10s ","RTEMS STATUS","ERRNO");
    }
    else {
      printf("%20s:%-10s ",
	     ((bdbuf_info->status == RTEMS_SUCCESSFUL)
	      ? "SUCCESSFUL" : rtems_status_text(bdbuf_info->status)),
	     ((bdbuf_info->status == RTEMS_SUCCESSFUL)
	      ? "" : strerror(bdbuf_info->error)));
    }
  }
  /*
   * FIXME: add info about waiting chain
   */
  printf("\n");
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void rtems_bdbuf_show_fnc
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   list all bdbufs with their content                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
    int     argc,
    char  **argv,
    rtems_monitor_command_arg_t* command_arg,
    bool verbose
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  show_bdbuf_filter_t   filter;
  show_bdbuf_selector_t selector;
  show_bdbuf_bdbuf_info_t bdbuf_info;

  bdbuf_pool *curr_pool,*pool_base;
  int pool_cnt,pool_idx;
  int bdbuf_idx;
  bool bdbuf_matches;
  int matched_cnt,un_matched_cnt;

  /*
   * analyze command line options
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_bdbuf_show_getargs (argc,argv,
				   &filter,&selector);
  }

  /*
   * get buffer pool information
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_bdbuf_show_get_bufpool(&pool_base,&pool_cnt);
    if (rc != RTEMS_SUCCESSFUL) {
      printf("%s: ERROR: no buffer pool found\n",argv[0]);
    }
  }
  /*
   * for all or selected buffer pool(s)
   */
  for (pool_idx = 0;
       (rc == RTEMS_SUCCESSFUL) && (pool_idx < pool_cnt);
       pool_idx++) {
    if ((filter.pool_id < 0) ||
	(filter.pool_id == pool_idx)) {
      curr_pool = pool_base + pool_idx;
      /*
       * print pool header
       */
      if (rc == RTEMS_SUCCESSFUL) {
	rc = rtems_bdbuf_show_pool_header(pool_idx,curr_pool);
      }
      if (rc == RTEMS_SUCCESSFUL) {
	matched_cnt = 0;
	un_matched_cnt = 0;
	/*
	 * print header for bdbuf
	 */
	  if (rc == RTEMS_SUCCESSFUL) {
	    rc = rtems_show_bdbuf_print(NULL,&selector,
					true);
	  }
	/*
	 * for all bdbufs in this pool
	 */
	for (bdbuf_idx = 0;
	     ((rc == RTEMS_SUCCESSFUL) &&
	      (bdbuf_idx < curr_pool->nblks));
	     bdbuf_idx++) {
	  /*
	   * get infos about bdbuf
	   */
	  if (rc == RTEMS_SUCCESSFUL) {
	    rc = rtems_show_bdbuf_get_bdbuf_info
	      (&(curr_pool->bdbufs[bdbuf_idx]),
	       bdbuf_idx,
	       curr_pool,
	       &bdbuf_info);
	  }
	  /*
	   * check, if bdbuf matches selection criteria
	   */
	  if (rc == RTEMS_SUCCESSFUL) {
	    rc = rtems_show_bdbuf_match_filter(&bdbuf_info,&filter,
					       &bdbuf_matches);
	  }
	  /*
	   * print info about bdbuf
	   */
	  if (rc == RTEMS_SUCCESSFUL) {
	    if (bdbuf_matches) {
	      rc = rtems_show_bdbuf_print(&bdbuf_info,&selector,
					  false);
	      if ((rc == RTEMS_SUCCESSFUL) &&
		  selector.show_sema) {
		rc = rtems_show_bdbuf_print_wait_chain(&(curr_pool->bdbufs[bdbuf_idx]));
	      }
	      matched_cnt++;
	    }
	    else {
	      un_matched_cnt++;
	    }
	  }
	}
	/*
	 * print match statistics and footer
	 */
	if (rc == RTEMS_SUCCESSFUL) {
	  printf("%d bdbufs printed, %d bdbufs suppressed\n",
		 matched_cnt,un_matched_cnt);
	}
      }
    }
  }
}

static rtems_monitor_command_entry_t rtems_show_bdbuf_cmds[] = {
  {
    "bdbuf_show",
    "usage: bdbuf_show\n",
    0,
    rtems_bdbuf_show_fnc,
    { 0 },
    0
  }
};

#ifndef ARRAY_CNT
#define ARRAY_CNT(arr) (sizeof((arr))/sizeof((arr)[0]))
#endif

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code rtems_bdbuf_show_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   add command(s) to monitor                                               |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void /* none up to now */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int item;

  for (item = 0;
       (sc == RTEMS_SUCCESSFUL) && (item < ARRAY_CNT(rtems_show_bdbuf_cmds));
       item++) {
    if (0 == rtems_monitor_insert_cmd (&rtems_show_bdbuf_cmds[item])) {
      sc = RTEMS_INVALID_NAME;
    }
  }
  return sc;
}
