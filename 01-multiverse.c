#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <multiverse.h>


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

// measure() runs work() ROUND times, records its run-time and
// provides an average over all runs.
#define ROUNDS 5
extern void work(void);
double measure() {
    double duration_total = 0.0;
    for(unsigned __rounds = 0; __rounds < ROUNDS; __rounds++) {
        timedelta();
        work();
        double duration = timedelta() *1000;
        printf(" work() -> %f ms\n", duration);
        duration_total += duration;
    }
    printf("Average (n=%d): %f\n\n", ROUNDS, duration_total/ROUNDS);
    return duration_total/ROUNDS;
}

////////////////////////////////////////////////////////////////
// The actual example
////////////////////////////////////////////////////////////////

// We use a single multiverse variable. The compiler plugin determines
// a value range of {0, 1} per default and specializes functions for
// these values.

__attribute__((multiverse)) int smp = 0;

// This mutex is only used to provide a workload. It does not protect
// anything.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// We attribute the lock()/unlock() function with the multiverse
// attribute to instruct the plugin to generate variants. Since lock()
// uses the multiverse variable `smp' and that variables has the
// domain {0,1}, the following variants will be generated:
//
// - lock  -- The unmodified general-purpose lock with the dynamic switch
// - lock.multiverse.smp_0  -- Specialized for smp == 0
// - lock.multiverse.smp_1  -- Specialized for smp == 1
__attribute__((multiverse))
void lock() {
    if (smp) {
        pthread_mutex_lock(&mutex);
    }
}

__attribute__((multiverse))
void unlock() {
    if (smp) {
        pthread_mutex_unlock(&mutex);
    }
}

// Our workload runs lock()/unlock in a tight loop.
void work() {
    for (unsigned i = 0; i < (1<<25); i++) {
        lock();
        unlock();
    }
}

int main(void) {
    // First, we have to initialize the multiverse run-time library.
    // This does not commit any specialized variant to the text
    // segment!
    multiverse_init();

    // Dump Information about all known function variants.
    multiverse_dump_info();


    // Run general-purpose lock()/unlock()
    printf("\nWithout Multiverse (smp=%d)\n", smp);
    measure();

    smp = 1;
    printf("\nWithout Multiverse (smp=%d)\n", smp);
    measure();

    ////////////////////////////////////////////////////////////////
    // With Multiverse
    ////////////////////////////////////////////////////////////////
    
    // We set the global variable, and "commit" all multiversed
    // functions according to the current state of the multiverse 
    // variables.
    //
    // multiverse_commit() patches all known call sites and inserts a
    // jump in the general-purpose variants. Thereby, the semantic of
    // lock() becomes consistently fixed to smp==0, even if you change
    // smp to 1.
    smp = 0;
    multiverse_commit();
    printf("\nWith Multiverse (smp=%d)\n", smp);
    measure();

    smp = 1;
    // <- At this point: still comitteted to smp==0!
    multiverse_commit();
    // <- At this point: now committed to smp==1/
    printf("\nWith Multiverse (smp=%d)\n", smp);
    measure();

    return 0;
}
