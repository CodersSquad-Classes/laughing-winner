#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>
#include "queueStruct.h"

extern pthread_mutex_t lock;
extern Queue* commandQueue; // Declare commandQueue as an extern variable

#endif
