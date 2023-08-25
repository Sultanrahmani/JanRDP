

#ifndef __WASM_UPDATE_THREAD_H__
#define __WASM_UPDATE_THREAD_H__

#include <winpr/collections.h>
#include <winpr/wtypes.h>
#include <pthread.h>

struct update_thread {
  wQueue* PendingQueue;
  pthread_t thread;
  BOOL stop;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};
typedef struct update_thread UpdateThread;

typedef VOID (*UpdateThreadCallbackFunc)(void* args);
struct update_thread_callback_instance {
  UpdateThreadCallbackFunc func;
  void* param;
};
typedef struct update_thread_callback_instance UpdateThreadCallbackInstance;



UpdateThread *createUpdateThread();
void updateThreadFree(UpdateThread *uThread);
BOOL addUpdateTask(UpdateThread *uThread, UpdateThreadCallbackFunc *func, void *param);

#endif