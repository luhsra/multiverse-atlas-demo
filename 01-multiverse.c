#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <multiverse.h>
#include "mmview.h"

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
volatile int smp = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


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

void work() {
    for (unsigned i = 0; i < (1<<25); i++) {
        lock();
        unlock();
    }
}

#define ROUNDS 5
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

int main(void) {
    multiverse_init();
    multiverse_dump_info();


    printf("\nWithout Multiverse (smp=%d)\n", smp);
    measure();

    smp = 1;
    printf("\nWithout Multiverse (smp=%d)\n", smp);
    measure();


    smp = 0;
    multiverse_commit();
    printf("\nWith Multiverse (smp=%d)\n", smp);
    measure();

    smp = 1;
    multiverse_commit();
    printf("\nWith Multiverse (smp=%d)\n", smp);
    measure();

    return 0;
}
