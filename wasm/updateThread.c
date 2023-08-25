
#include <winpr/pool.h>
#include <winpr/wtypes.h>

#include "updateThread.h"

static void *threadFunc(void *arg)
{
  UpdateThread *uThread = (UpdateThread *)arg;
  if (!uThread || !uThread->PendingQueue)
    return NULL;
  UpdateThreadCallbackInstance *callbackInstance;

  while (1)
  {
    if (uThread->stop)
      break;
    pthread_mutex_lock(&uThread->mutex);
    callbackInstance = (UpdateThreadCallbackInstance *)Queue_Dequeue(uThread->PendingQueue);
    if (!callbackInstance)
    {
      pthread_cond_wait(&uThread->cond, &uThread->mutex);
    }
    pthread_mutex_unlock(&uThread->mutex);
    if (callbackInstance)
    {
      callbackInstance->func(callbackInstance->param);
      free(callbackInstance);
    }
  }
}

BOOL addUpdateTask(UpdateThread *uThread, UpdateThreadCallbackFunc *func, void *param)
{
  if (!uThread)
    return FALSE;
  UpdateThreadCallbackInstance *callbackInstance = (UpdateThreadCallbackInstance *)malloc(sizeof(UpdateThreadCallbackInstance));
  if (!callbackInstance)
    return FALSE;
  callbackInstance->func = func;
  callbackInstance->param = param;
  pthread_mutex_lock(&uThread->mutex);
  if (!Queue_Enqueue(uThread->PendingQueue, callbackInstance))
  {
    free(callbackInstance);
    pthread_mutex_unlock(&uThread->mutex);
    return FALSE;
  }
  pthread_mutex_unlock(&uThread->mutex);
  pthread_cond_signal(&uThread->cond);
  return TRUE;
}

void updateThreadFree(UpdateThread *uThread)
{
  if (!uThread)
    return;

  uThread->stop = TRUE;
  pthread_mutex_lock(&uThread->mutex);
  Queue_Free(uThread->PendingQueue);
  pthread_mutex_unlock(&uThread->mutex);
  free(uThread);
}

UpdateThread *createUpdateThread()
{
  UpdateThread *uThread = (UpdateThread *)malloc(sizeof(UpdateThread));
  if (!uThread)
    return NULL;
  uThread->stop = FALSE;
  uThread->PendingQueue = Queue_New(TRUE, -1, -1);
  if (!uThread->PendingQueue)
    goto PendingQueueFail;
  if (pthread_create(&uThread->thread, NULL, threadFunc, uThread) != 0)
    goto createThreadFail;
  return uThread;
createThreadFail:
  Queue_Free(uThread->PendingQueue);
PendingQueueFail:
  free(uThread);
  return NULL;
}
