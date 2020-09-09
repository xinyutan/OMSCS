#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

static int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    // without the sleep(1), the whole program runs much slower than the
    // slow_worker: makes sense, because context switching takes time.
    // However, by adding the sleep(1), this program take half the
    // time as the slow_worker
    int loops = *((int *)arg);
    int j, s=0;
    for (j = 0; j < loops; j++) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0) {
            fprintf(stderr, "error: lock the mutex\n");
            exit(1);
        }
        glob++;
        s = pthread_mutex_unlock(&mtx);
        if (s != 0) {
            fprintf(stderr, "error: unlock the mutex\n");
            exit(1);
        }
        sleep(1);
    }
    return NULL;
}

void *slow_worker(void *arg) {
    // by putting the mutex outside the for loop
    // this is no longer multi-threading
    // in fact, it takes as much time as if there
    // were run in serial. (mainly due to the wait sleep (1), used to
    // simulate the time-consuming computations)
    int loops = *((int *)arg);
    int j, s=0;
    s = pthread_mutex_lock(&mtx);
    if (s != 0) {
        fprintf(stderr, "error: lock the mutex\n");
        exit(1);
    }
    for (j = 0; j < loops; j++) {

        glob++;

        sleep(1);
    }
    s = pthread_mutex_unlock(&mtx);
    if (s != 0) {
        fprintf(stderr, "error: unlock the mutex\n");
        exit(1);
    }
    return NULL;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: threads <value>\n");
        exit(1);
    }

    pthread_t p1, p2;
    int s, loops;
    struct timeval start, end;

    loops  = atoi(argv[1]);

    printf("Initial value: %d\n", glob);

    gettimeofday(&start, NULL);
    s = pthread_create(&p1, NULL, worker, &loops);
    if (s != 0) {
        fprintf(stderr, "error create the thread\n");
        exit(1);
    }

    s = pthread_create(&p2, NULL, worker, &loops);
    if (s != 0) {
        fprintf(stderr, "error create the thread\n");
        exit(1);
    }

    s = pthread_join(p1, NULL);
    if (s != 0) {
        fprintf(stderr, "error join the thread\n");
        exit(1);
    }
    s = pthread_join(p2, NULL);
    if (s != 0) {
        fprintf(stderr, "error join the thread\n");
        exit(1);
    }
    gettimeofday(&end, NULL);

    long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

	printf("Time elpased is %ld seconds and %ld micros\n", seconds, micros);

    printf("Final value: %d\n", glob);
    return 0;
}
