/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbgexcep.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <rtems.h>
#include <rtems/error.h>
#include <assert.h>
#include <errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>

unsigned int NbExceptCtx;
volatile unsigned int NbSerializedCtx;
Exception_context *FirstCtx = NULL;
Exception_context *LastCtx = NULL;

CPU_Exception_frame SavedContext;

/*
 * Save an exception context at the end of a list
 */

  int
PushExceptCtx (Objects_Id Id, Objects_Id semId, CPU_Exception_frame * ctx)
{

  Exception_context *SaveCtx;

  SaveCtx = (Exception_context *) malloc (sizeof (Exception_context));
  if (SaveCtx == NULL)
    rtems_panic ("Can't allocate memory to save Exception context");

  SaveCtx->id = Id;
  SaveCtx->ctx = ctx;
  SaveCtx->semaphoreId = semId;
  SaveCtx->previous = NULL;
  SaveCtx->next = NULL;

  if (FirstCtx == NULL) {       /* initialization */
    FirstCtx = SaveCtx;
    LastCtx = SaveCtx;
    NbExceptCtx = 1;
  } else {
    NbExceptCtx++;
    LastCtx->next = SaveCtx;
    SaveCtx->previous = LastCtx;
    LastCtx = SaveCtx;
  }
  return 0;
}

/*
 * Save an temporary exception context in a global variable
 */

  int
PushSavedExceptCtx (Objects_Id Id, CPU_Exception_frame * ctx)
{
  memcpy (&(SavedContext), ctx, sizeof (CPU_Exception_frame));
  return 0;
}

/*
 * Remove the context of the specified Id thread.
 * If Id = -1, then return the first context
 */

  int
PopExceptCtx (Objects_Id Id)
{

  Exception_context *ExtractCtx;

  if (FirstCtx == NULL)
    return -1;

  if (Id == -1) {
    ExtractCtx = LastCtx;
    LastCtx = LastCtx->previous;
    free (ExtractCtx);
    NbExceptCtx--;
    return 0;
  }

  ExtractCtx = LastCtx;

  while (ExtractCtx->id != Id && ExtractCtx != NULL) {
    ExtractCtx = ExtractCtx->previous;
  }

  if (ExtractCtx == NULL)
    return -1;

  if (ExtractCtx->previous != NULL)
    (ExtractCtx->previous)->next = ExtractCtx->next;

  if (ExtractCtx->next != NULL)
    (ExtractCtx->next)->previous = ExtractCtx->previous;

  if (ExtractCtx == FirstCtx)
    FirstCtx = FirstCtx->next;
  else if (ExtractCtx == LastCtx)
    LastCtx = LastCtx->previous;

  free (ExtractCtx);
  NbExceptCtx--;
  return 0;
}

/*
 * Return the context of the specified Id thread.
 * If Id = -1, then return the first context.
 */

  Exception_context *
GetExceptCtx (Objects_Id Id)
{

  Exception_context *ExtractCtx;

  if (FirstCtx == NULL)
    return NULL;

  if (Id == -1) {
    return LastCtx;
  }

  ExtractCtx = LastCtx;

  while (ExtractCtx->id != Id && ExtractCtx != NULL) {
    ExtractCtx = ExtractCtx->previous;
  }

  if (ExtractCtx == NULL)
    return NULL;

  return ExtractCtx;
}
