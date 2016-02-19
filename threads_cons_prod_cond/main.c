/*
 * @file    main.c
 * @brief   Single_producer-multiple_consumers problem
 *          with synchronising of consumers on each row
 *          of the table.
 * @author  Piotr Gregor <piotrek.gregor at gmail.com>
 * @date    18 Feb 2016 2:38 PM
 */


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
#define BUFF_COLS 7
#define INDICES_PER_THREAD 2    /* in each row */

#define INFO_INT(x) fprintf(stderr, "[%s] [%d]\n", #x, x)
#define MIN(x, y) (x) < (y) ? (x) : (y)

struct master_args
{
    int *in;     /* input buffer, data to work on */
    int *out;    /* output buffer, result */
    int threads_n;  /* number of threads */
    int indices_per_thread; /* number of elements single thread will do */

    /* buffer synchronization */
    int row_current;                        /* row being processed now */
    int *shared_buf;

    pthread_mutex_t mutex_row_current_ready;
    pthread_cond_t  cond_row_current_ready;
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
    pthread_mutex_t mutex_row_start; /* this thread's mutex in master's mutexes */
    int             flag;              /* this thread's flag in master's flags */
    /* signals this worker thread to start compute next row */
    pthread_cond_t  cond_row_start;    /* this thread's  cond in master flags */

    pthread_mutex_t *mutex_row_current_ready;
    pthread_cond_t  *cond_row_current_ready;
};

void* worker_func(void *worker_arg);
void* master_func(void *master_arg);

int main(void)
{
    int err;
    int *in, *out, *shared_buf;
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

    shared_buf = malloc(BUFF_ROWS * BUFF_COLS * sizeof(int));
    if (shared_buf == NULL)
    {
        err =  -2;
        goto fail;
    }
    memset(shared_buf, 0xff, BUFF_ROWS * BUFF_COLS * sizeof(int));

    // allocation of output buffer and initializing to 0
    out = malloc(BUFF_ROWS * BUFF_COLS * sizeof(int));
    if (out == NULL)
    {
       err = -3;
       goto fail;
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
    args.shared_buf = shared_buf;
    if (pthread_create(&master_thread, NULL, master_func, (void *)&args) != 0)
    {
        err = -4;
        goto fail;
    }

    //joining the master thread
    if (pthread_join(master_thread, NULL) != 0)
    {
        fprintf(stderr, "Error joining master thread\n");
        err = -5;
        goto fail;
    }

    //printing the output buffer values
    fprintf(stderr, "Main thread, output buffer:\n");
    for (i = 0; i < BUFF_ROWS; ++i )
    {
        for (j = 0; j < BUFF_COLS; ++j)
        {
            fprintf(stdout, "[%d]\t",out[i * BUFF_COLS + j]);
        }
        fprintf(stdout, "\n");
    }

    fprintf(stderr, "Main thread, shared buffer:\n");
    for (i = 0; i < BUFF_ROWS; ++i )
    {
        for (j = 0; j < BUFF_COLS; ++j)
        {
            fprintf(stdout, "[%d]\t",shared_buf[i * BUFF_COLS + j]);
        }
        fprintf(stdout, "\n");
    }

    /* TODO free all acquired memory */
        free(in);
        free(out);
        free(shared_buf);
    return 0;
fail:
    /* TODO free all acquired memory */
        free(in);
        free(out);
        free(shared_buf);
    return err;
}

/*
 * @return  0 - all flags are 1, 1 - not all flags are 1
 */
static int
not_all_flags_1(struct worker_args *wargs, int threads_n)
{
    int i = 0;
    while (i < threads_n)
    {
        pthread_mutex_lock(&wargs[i].mutex_row_start);
        if (wargs[i].flag != 1)
            goto not_all_1;
        pthread_mutex_unlock(&wargs[i].mutex_row_start);
        ++i;
    }
    return 0;

not_all_1:
    pthread_mutex_unlock(&wargs[i].mutex_row_start);
    return 1;
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

    margs->row_current = 0;
    pthread_mutex_init(&margs->mutex_row_current_ready, NULL);
    pthread_cond_init(&margs->cond_row_current_ready, NULL);

    fprintf(stderr, "Master thread, starting [%d] worker "
            "threads\n", threads_n);

    i = 0;
    for(; i < threads_n; ++i)
    {
        wargs[i].tid = i;
        wargs[i].buff_in = margs->in;
        wargs[i].buff_out = margs->out;
        wargs[i].from = INDICES_PER_THREAD * i;
        wargs[i].to = MIN(wargs[i].from + INDICES_PER_THREAD - 1, BUFF_COLS - 1);
        wargs[i].mutex_row_current_ready = &margs->mutex_row_current_ready;
        wargs[i].cond_row_current_ready = &margs->cond_row_current_ready;
        pthread_cond_init(&wargs[i].cond_row_start, NULL);
        pthread_mutex_init(&wargs[i].mutex_row_start, NULL);
        wargs[i].flag = 1;  /* don't produce yet, wait */
        if (pthread_create(&wargs[i].pid, NULL,
                    worker_func, (void *)&wargs[i]) != 0)
        {
            fprintf(stderr, "Master thread, error [%d]\n", -13);
            return NULL;
        }
    }

    pthread_mutex_lock(&margs->mutex_row_current_ready);
    while (margs->row_current < BUFF_ROWS)
    {
//        pthread_mutex_unlock(&margs->mutex_row_current_ready);
        /* produce */
        /* signal workers to compute this new row */
        i = 0;
        while (i < threads_n)
        {
            pthread_mutex_lock(&wargs[i].mutex_row_start);
            wargs[i].flag = 0;  /* tell worker to continue */
            pthread_cond_signal(&wargs[i].cond_row_start);
            pthread_mutex_unlock(&wargs[i].mutex_row_start);
            ++i;
        }

//    pthread_mutex_lock(&margs->mutex_row_current_ready);
        while (not_all_flags_1(wargs, threads_n))
        {
            /* not all flags 1 */
            pthread_cond_wait(&margs->cond_row_current_ready, &margs->mutex_row_current_ready);
        }
        /* copy row */
        memcpy(margs->shared_buf + margs->row_current * BUFF_COLS,
                margs->out + margs->row_current * BUFF_COLS, BUFF_COLS *sizeof(int));
        /* all flags 1, row has been completed */
        margs->row_current++;
pthread_mutex_unlock(&margs->mutex_row_current_ready);
    }
//    pthread_mutex_unlock(&margs->mutex_row_current_ready);

    i = 0;
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
        //free(retval);
    }
    return NULL;
}


void*
worker_func(void *arg)
{
    int *ret;
    struct worker_args *wargs;
    int row, col;
    int indices_n; /* equals indices_per_thread or less for last thread */
    int *out;

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
    /* pessimistic */
    *ret = -1;

    indices_n = wargs->to - wargs->from + 1;
    out = wargs->buff_out;
    row = col = 0;
    while (row < BUFF_ROWS)
    {
        pthread_mutex_lock(&wargs->mutex_row_start);
        while (wargs->flag == 1)
        {
            pthread_cond_wait(&wargs->cond_row_start, &wargs->mutex_row_start);
        }
        /* master has set flag to 0, compute row */

        col = 0;
        while(col < indices_n && (col <= wargs->to))
        {
            out[row * BUFF_COLS + wargs->from + col] = wargs->tid;
            ++col;
        }

        /* signal row has been completed - master thread checks all flags
         * and sets them to 0 once all become 1 */
        wargs->flag = 1;
        pthread_mutex_unlock(&wargs->mutex_row_start);

        pthread_mutex_lock(wargs->mutex_row_current_ready);
        pthread_cond_signal(wargs->cond_row_current_ready);
        pthread_mutex_unlock(wargs->mutex_row_current_ready);

        ++row;
    }
    // OK
    *ret = 0;

    pthread_exit((void*) ret);
}
