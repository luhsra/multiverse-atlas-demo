#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <multiverse.h>
#include "mmview.h"


__attribute__((multiverse))
volatile int multiversed_var = 0;

__attribute__((multiverse))
void multiversed_function(int thread_num) {
    if (multiversed_var) {
        printf("thread%d  -> \033[91m view A \033[0m\n", thread_num);
    } else {
        printf("thread%d  -> \033[92m view B \033[0m\n", thread_num);
    }
}

volatile long desired_mmview = 0;


void *worker(void *arg) {
    long thread_num = (long)arg;

    for (unsigned i = 0; i< 100; i++) {
        multiversed_function(thread_num);

        if (mmview_current() != desired_mmview) {
            printf("thread%ld migrate: ", thread_num);
            mmview_migrate(desired_mmview);
        }

        // Sleep for up to a second
        struct timespec time = {0};
        time.tv_nsec = (random() % 1000) * 1e6;
        nanosleep(&time, NULL);
    }

    return NULL;
}


int main(void) {
    mmview_unshare_text();
    multiverse_init();
    multiverse_commit();
    desired_mmview = mmview_current();


    pthread_t threads[3];
    for (unsigned long i = 0; i < sizeof(threads)/sizeof(*threads); i++) {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    sleep(3);

    long view = mmview_create();
    mmview_migrate(view);
    multiversed_var = 1;
    multiverse_commit();
    desired_mmview = view;


    for (unsigned long i = 0; i < sizeof(threads)/sizeof(*threads); i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
