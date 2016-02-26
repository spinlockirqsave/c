#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "buffer.h"
#include "sma_buf.h"


#define ISNAN(x) (isnan(x))

/*! Calculate how many audio samples per ms based on the rate */
#define SAMPLES_PER_MS(r, m) ((r) / (1000/(m)))
/*! Minimum beep length */
#define BEEP_TIME (100)
/*! How often to evaluate the output of desa2 in ms */
#define SINE_TIME (10)
/*! How long in samples does desa2 results get evaluated */
#define SINE_LEN(r) SAMPLES_PER_MS((r), SINE_TIME)
/*! How long in samples is the minimum beep length */
#define BEEP_LEN(r) SAMPLES_PER_MS((r), BEEP_TIME)
/*! Number of points in desa2 sample */
#define P (5)
/*! Guesstimate frame length in ms */
#define FRAME_TIME (20)
/*! Length in samples of the frame (guesstimate) */
#define FRAME_LEN(r) SAMPLES_PER_MS((r), FRAME_TIME)
/*! Conversion to Hertz */
#define TO_HZ(r, f) (((r) * (f)) / (2.0 * M_PI))
/*! Minimum beep frequency in Hertz */
#define MIN_FREQUENCY (300.0)
#define MIN_FREQUENCY_R(r) ((2.0 * M_PI * MIN_FREQUENCY) / (r))
/*! Maximum beep frequency in Hertz */
#define MAX_FREQUENCY (2500.0)
#define MAX_FREQUENCY_R(r) ((2.0 * M_PI * MAX_FREQUENCY) / (r))
/* decrease this value to eliminate false positives */
#define VARIANCE_THRESHOLD (0.1)

struct frame
{
    int16_t *data;
    size_t  samples;
};

double
desa2(circ_buffer_t *b, size_t i)
{
    double d;
    double n;
    double x0;
    double x1;
    double x2;
    double x3;
    double x4;
    double x2sq;
    double result;

    x0 = GET_SAMPLE((b), (i));
    x1 = GET_SAMPLE((b), ((i) + 1));
    x2 = GET_SAMPLE((b), ((i) + 2));
    x3 = GET_SAMPLE((b), ((i) + 3));
    x4 = GET_SAMPLE((b), ((i) + 4));

    x2sq = x2 * x2;

    d = 2.0 * ((x2sq) - (x1 * x3));
    if (d == 0.0) return 0.0;
    n = ((x2sq) - (x0 * x4)) - ((x1 * x1) - (x0 * x2)) - ((x3 * x3) - (x2 * x4));
    result = 0.5 * acos(n/d);
    if (ISNAN(result)) result = 0.0;

    return result;
}

int
main(void)
{
    struct frame frame;
    float f;
    double v;
    int i, j, k;
    uint32_t    sine_len_i;
    circ_buffer_t b;
    sma_buffer_t   sma_b, sqa_b;
    size_t pos_, pos;
    uint32_t    session_rate = 8000; /* samples per second */

    INIT_CIRC_BUFFER(&b, (size_t)BEEP_LEN(session_rate), (size_t)FRAME_LEN(session_rate));
	INIT_SMA_BUFFER(&sma_b, BEEP_LEN(session_rate) / SINE_LEN(session_rate)); // BEEP_TIME/SINE_TIME
	INIT_SMA_BUFFER(&sqa_b, BEEP_LEN(session_rate) / SINE_LEN(session_rate));

	/*! Precompute values used heavily in the inner loop */
	sine_len_i = SINE_LEN(session_rate);

    printf("<<< sine_len_i [%u] >>>\n", sine_len_i);
    printf("<<< MIN_FREQ [%lf] MAX_FREQ [%lf] MIN_FREQR [%lf] MAX_FREQR [%lf] >>>\n",
            MIN_FREQUENCY, MAX_FREQUENCY, MIN_FREQUENCY_R(session_rate), MAX_FREQUENCY_R(session_rate));

    pos_ = 0;
    i = 0;
    j = 16768;
    for(; i < 6; ++i)
    {
        frame.data = malloc(160 * sizeof(int16_t));
        if (frame.data == NULL) return -1;
        frame.samples = 160;
        k = 0;
        while(k < 160) { frame.data[k] = j + rand() % 1000; k++;}//rand() % 32768;

        /*! Insert frame of 16 bit samples into buffer */
	    INSERT_INT16_FRAME(&b, (int16_t *)(frame.data), frame.samples);

        for (pos = pos_; pos < (GET_CURRENT_POS(&b) - P); pos++)
        {
		    if ((pos % sine_len_i) == 0) {
                f = GET_SAMPLE(&b, pos);//desa2(&b, pos);
			    if (f < MIN_FREQUENCY_R(session_rate) || f > MAX_FREQUENCY_R(session_rate)) {
				    printf("<<< RESET f=[%f] [%f]Hz sma=[%f] sqa=[%f] >>>\n", f, TO_HZ(session_rate, f),
                        sma_b.sma, sqa_b.sma);
				    v = 99999.0;
				    RESET_SMA_BUFFER(&sma_b);
				    RESET_SMA_BUFFER(&sqa_b);
			    } else {
				    APPEND_SMA_VAL(&sma_b, f);
				    APPEND_SMA_VAL(&sqa_b, f * f);

				    /* calculate variance */
				    v = sqa_b.sma - (sma_b.sma * sma_b.sma);

				    printf("<<< IN v=[%f] f=[%f] [%f]Hz sma=[%f] sqa=[%f] >>>\n", v, f, TO_HZ(session_rate, f),
                        sma_b.sma, sqa_b.sma);
			    }

			    /*! If variance is less than threshold then we have detection */
			    if (v < VARIANCE_THRESHOLD) {
                    printf("OK\n");
		        }
		    }
	    }
        free(frame.data);
        frame.samples = 0;
	    pos_ = pos;
        j *= 2;
    }

    DESTROY_SMA_BUFFER(&sma_b);
    DESTROY_SMA_BUFFER(&sqa_b);
    DESTROY_CIRC_BUFFER(&b);

    return 0;
}
