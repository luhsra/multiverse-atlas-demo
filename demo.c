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
        printf("thread%d  -> \033[91m A \033[0m\n", thread_num);
    } else {
        printf("thread%d  -> \033[92m B \033[0m\n", thread_num);
    }
}


void *worker(void *arg) {
    long thread_num = (long)arg;

    long view = mmview_create();
    mmview_migrate(view);
    multiversed_var = 1;
    multiverse_commit();

    sleep(2);

    multiversed_function(thread_num);
    printf("multiversed_var = %d\n", multiversed_var);

    return NULL;
}


int main(void) {
    mmview_unshare_text();
    multiverse_init();

    pthread_t thread;
    pthread_create(&thread, NULL, worker, (void*)1);

    sleep(1);
    multiversed_var = 0;

    multiversed_function(0);

    sleep(2);

    return 0;
}
