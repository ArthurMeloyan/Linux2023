#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <pthread.h>

#include "queue.h"
#include "error_messages.h"

typedef struct sync_context {
    pthread_cond_t  full_;
    pthread_cond_t  empty_;
    pthread_mutex_t lock_;
    queue_t* queue_;   
} sync_context_t;

void* execute_producer(void* input)
{
    sync_context_t* context = input;

    for (size_t i = 0; i < 25UL; i++) {
        sleep(1);

        const int new_item = rand() % 999;
        
        // Enter critical section
        pthread_mutex_lock(&context->lock_);

        while (queue_size(context->queue_) == queue_empty(context->queue_))
            pthread_cond_wait(&context->empty_, &context->lock_);

        printf("New item %3d is produced and inserted into the storage: ", new_item);

        queue_push(context->queue_, new_item);
        queue_dump(context->queue_);

        // Leave critical section
        pthread_mutex_unlock(&context->lock_);
        pthread_cond_signal(&context->full_);
    }

    return NULL;
}

void* execute_consumer(void* input)
{
    sync_context_t* context = input;

    for (size_t i = 0; i < 25UL; i++) {
        sleep(1);

        // Enter critical section
        pthread_mutex_lock(&context->lock_);

        while (queue_size(context->queue_) == 0)
            pthread_cond_wait(&context->full_, &context->lock_);

        const int item = queue_front(context->queue_);
        printf("New item %3d is consumed and removed from the storage:  ", item);

        queue_pop(context->queue_);
        queue_dump(context->queue_);

        // Leave critical section
        pthread_mutex_unlock(&context->lock_);
        pthread_cond_signal(&context->empty_);
    }

    return NULL;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        printf(ERROR_FORMAT, ARGS_ERROR);
        return 1;
    }   

    // Parse command line arguments
    const size_t queue_size = atoi(argv[1]);
    const size_t producers_count = atoi(argv[2]);
    const size_t consumers_count = atoi(argv[3]);

    // Initialize synchronization context
    sync_context_t sync_context;
    sync_context.queue_ = queue_create();
    pthread_cond_init(&sync_context.full_,  NULL);
    pthread_cond_init(&sync_context.empty_, NULL);
    pthread_mutex_init(&sync_context.lock_, NULL);
    
    // Initialize random machine
    srand(time(NULL));

    // Create consumer threads
    pthread_t consumers[consumers_count];
    for (size_t i = 0; i < consumers_count; i++) {
        pthread_create(&consumers[i], NULL, execute_consumer, &sync_context);
        // join why not?
    }

    // Create producer threads
    pthread_t producers[producers_count];
    for (size_t i = 0; i < producers_count; i++)
        pthread_create(&producers[i], NULL, execute_producer, &sync_context);

    // Join producer threads
    for (size_t i = 0; i < producers_count; i++)
        pthread_join(producers[i], NULL);

    // Join consumer threads 
    for (size_t i = 0; i < consumers_count; i++)
        pthread_join(consumers[i], NULL);

    // Clean up synchronization context
    pthread_cond_destroy(&sync_context.full_);
    pthread_cond_destroy(&sync_context.empty_);
    pthread_mutex_destroy(&sync_context.lock_);
    queue_destroy(sync_context.queue_);

    return 0;
}

