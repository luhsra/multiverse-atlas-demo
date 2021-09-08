#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
//#include <assert.h>
//#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <multiverse.h>
#include "mmview.h"

/*
  We use a function to measure time with Linux' CLOCK_REALTIME. This
  function always returns the time delta with respect to the last call
  of timedelta();
*/
double timedelta(void) {
    static struct timespec last_now;

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    double delta = now.tv_sec - last_now.tv_sec;
    delta += (now.tv_nsec - last_now.tv_nsec)/1e9;

    last_now = now;

    return delta;
}

__attribute__((multiverse))
volatile int DO_PROFILE = 0;

//#define PROFILE(var) do { if (DO_PROFILE) { __atomic_add_fetch(&var, 1, __ATOMIC_RELAXED); } } while(0)
#define PROFILE(var) do { if (DO_PROFILE) { var++; } } while(0)

unsigned long profile_calls;
unsigned long profile_branches;


__attribute__((multiverse))
unsigned long  fib(int n) {
    PROFILE(profile_calls);

    if (n <= 1) {
        PROFILE(profile_branches);
        return n;
    }
    return fib(n-1) + fib(n-2);
}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

double duration_total = 0.0;
unsigned duration_count = 0;

sem_t profile_view_sem;
mmview_t default_view;
mmview_t profile_view;


#define FIB_CALC 30
#define __STR__(X) #X
#define _STR_(X) __STR__(X)
#define FIB_CALC_STR _STR_(FIB_CALC)


void *worker(void *arg) {
    long thread_num = (long)arg;

    for (unsigned i = 0; i< 100; i++) {
        bool in_profile_view = false;
        if (sem_trywait(&profile_view_sem) == 0) {
            mmview_migrate(profile_view);
            in_profile_view = true;
        }

        // The actual calculation
        timedelta();
        unsigned long result = fib(FIB_CALC);
        double duration = timedelta() * 1000.0;

        // UNCOMMENT to get per run timings
        //printf("thread%ld: %f ms (profile=%d, fib(" FIB_CALC_STR ") = %ld)\n",
        //       thread_num, duration, in_profile_view, result);


        // Sum up run-time and number of fibonacci runs
        pthread_mutex_lock(&lock);
        duration_total += duration;
        duration_count += 1;
        pthread_mutex_unlock(&lock);

        // Switch back to default view, if necessary
        if (in_profile_view) {
            mmview_migrate(default_view);
            sem_post(&profile_view_sem);
        }
    }

    return NULL;
}




int main(int argc, char *argv[]) {
    mmview_unshare_text();
    multiverse_init();
    multiverse_dump_info();

    default_view = mmview_current();
    DO_PROFILE=0;
    multiverse_commit();

    // Prepare a separate view with DO_PROFILE=1
    profile_view = mmview_create();
    mmview_migrate(profile_view);
    DO_PROFILE = 1;
    multiverse_commit();

    // Switch back to the default view
    mmview_migrate(default_view);

    if (argc < 2) {
        printf("%s <THREADCOUNT> [PROFILE_COUNT]\n", argv[0]);
        return -1;
    }

    unsigned nr_threads = atoi(argv[1]);
    
    unsigned nr_profiling = nr_threads;
    if (argc >= 3) {
        nr_profiling = atoi(argv[2]);
    }
    sem_init(&profile_view_sem, false, nr_profiling);

    printf("Starting with %d threads. %d threads are profiling\n", nr_threads, nr_profiling);

    pthread_t *threads = malloc(sizeof(pthread_t) * nr_threads);
    for (unsigned long i = 0; i < nr_threads; i++) {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    for (unsigned long i = 0; i < nr_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Avg. time for fib(" FIB_CALC_STR ") = %f ms; %f branches/call %d\n",
           duration_total/duration_count,
           profile_branches/(double)profile_calls,
        duration_count);

    return 0;
}
