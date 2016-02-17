#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdbool.h>


#define BUFF_ROWS 4
#define BUFF_COLS 100
#define INDICES_PER_THREAD 50

#define INFO_INT(x) fprintf(stderr, "[%s] [%d]\n", #x, x)

struct master_args
{
    int *in;     /* input buffer, data to work on */
    int *out;    /* output buffer, result */
    int threads_n;  /* number of threads */
    int indices_per_thread; /* number of elements single thread will do */
};

struct worker_args
{
    pthread_t pid;
    int tid;        /* thread id */
    int *buff_in;   /* input buffer */
    int *buff_out;  /* output buffer */
    int from, to;   /* start and end index in each row of input buffer
                       that this thread works on */
    size_t in_size; /* buffer size */
    size_t out_size;/* buffer size */
};

void* worker_func(void *worker_arg);
void* master_func(void *master_arg);

int main(int argc, char* argv[])
{

    int *in, *out;
    int threads_n;
    int i,j;
    pthread_t master_thread;
    struct master_args  args;

    INFO_INT(BUFF_ROWS);
    INFO_INT(BUFF_COLS);
    INFO_INT(INDICES_PER_THREAD);

    // allocation of input buffer and initializing to 0
    in = malloc(BUFF_ROWS * BUFF_COLS * sizeof(int));
    if (in == NULL) return -1;
    memset(in, 0, BUFF_ROWS * BUFF_COLS * sizeof(int));

    // allocation of output buffer and initializing to 0
    out = malloc(BUFF_ROWS * BUFF_COLS * sizeof(int));
    if (out == NULL)
    {
        free(in);
        return -2;
    }
    memset(out, 0, BUFF_ROWS * BUFF_COLS * sizeof(int));

    /* calc number of threads */
    threads_n = BUFF_COLS / INDICES_PER_THREAD
                + (BUFF_COLS % INDICES_PER_THREAD ? 1 : 0);

    /* master */
    args.in = in;
    args.out = out;
    args.threads_n = threads_n;
    args.indices_per_thread = INDICES_PER_THREAD;
    if (pthread_create(&master_thread, NULL, master_func, (void *)&args) != 0)
        return -3;

    //joining the master thread
    if (pthread_join(master_thread, NULL) != 0)
    {
        fprintf(stderr, "Error joining master thread\n");
        return -4;
    }

    //printing the output buffer values
    for (i = 0; i < BUFF_ROWS; ++i )
    {
        for (j = 0; j < BUFF_COLS; ++j)
        {
            fprintf(stderr, "[%d]\t",out[i * INDICES_PER_THREAD + j]);
        }
        fprintf(stderr, "\n");
    }

    return 0;
}

void*
master_func(void *arg)
{
    int i;
    struct master_args *margs;
    int threads_n;
    struct worker_args *wargs;
    int ret;
    int *retval;

    margs = (struct master_args *) arg;
    if (margs == NULL)
    {
        fprintf(stderr, "Master thread, error [%d]\n", -11);
        return NULL;
    }

    threads_n = margs->threads_n;
    wargs = malloc(threads_n * sizeof(struct worker_args));
    if (wargs == NULL)
    {
        fprintf(stderr, "Master thread, error [%d]\n", -12);
        return NULL;
    }

    i = 0;
    for(; i < threads_n; ++i)
    {
        wargs[i].buff_in = margs->in;
        wargs[i].buff_out = margs->out;
        wargs[i].from = INDICES_PER_THREAD * i;
        wargs[i].to = wargs->from + INDICES_PER_THREAD - 1;
        if (pthread_create(&wargs[i].pid, NULL,
                    worker_func, (void *)&wargs[i]) != 0)
        {
            fprintf(stderr, "Master thread, error [%d]\n", -13);
            return NULL;
        }
    }

    //joining worker threads
    for (; i < threads_n; ++i)
    {
        ret = pthread_join(wargs[i].pid, (void **) &retval);
        if (*retval != 0)
        {
            fprintf(stderr, "Master thread, thread [%d] exit "
                    "error [%d]\n", wargs[i].tid, *retval);
            return NULL;
        }
        if (ret != 0)
        {
            fprintf(stderr, "Master thread, error [%d] "
                    "joining thread [%d]\n", ret, wargs[i].tid);
            return NULL;
        }
        free(retval);
    }

    return NULL;
}


void*
worker_func(void *arg)
{
    int *ret;
    struct worker_args *wargs;

    wargs = (struct worker_args*) arg;
    if (wargs == NULL)
    {
        fprintf(stderr, "Worker thread: NULL argument passed "
                "to function\n");
        return NULL;
    }

    ret = malloc(sizeof(int));
    if (ret == NULL)
    {
        fprintf(stderr, "Worker thread [%d]: malloc failed\n",
                wargs->tid);
        return NULL;
    }
    *ret = 0;

    pthread_exit((void*) ret);
}
