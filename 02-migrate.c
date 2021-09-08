#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <multiverse.h>
#include "mmview.h"


// In this demo, we will demonstrate the incremental migration of
// multiple threads to a different address-space view. We prepare that
// other view with multiverse, but other binary-patching techniques
// (e.g. updates) are possible as well.

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

// We use a global variable to indicate the address-space view
// (mmview) all threads _should_ work in.
volatile mmview_t desired_mmview = 0;


void *worker(void *arg) {
    long thread_num = (long)arg;

    for (unsigned i = 0; i< 100; i++) {
        multiversed_function(thread_num);

        // If we are in the wrong view, we directly migrate to the
        // desired view. This only migrates the current thread.
        //
        // We can do this as this is a local quiescence point, since
        // multiversed_function() is surely not active in this thread
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
    // First, we unshare the text segment, which makes the text
    // segment of this binary a copy-on-write mmview-private mapping
    // in each mmview.
    mmview_unshare_text();

    // We initilize multiverse and commit the variant
    // multiversed_var=0 into the text segment of view 0.
    multiverse_init();
    multiverse_commit();
    desired_mmview = mmview_current();


    // Create three worker threads.
    pthread_t threads[3];
    for (unsigned long i = 0; i < sizeof(threads)/sizeof(*threads); i++) {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    sleep(3);

    // The main thead (we here) creates a new mmview (view 1) and
    // migrates to it. In that view, we use the multiverse machinery
    // to create a different text segment (multiversed_var == 1).
    long view = mmview_create();
    mmview_migrate(view);
    multiversed_var = 1;
    multiverse_commit();

    // Let the worker threads start to migrate.
    desired_mmview = view;

    // Wait for all threads to terminate.
    for (unsigned long i = 0; i < sizeof(threads)/sizeof(*threads); i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
