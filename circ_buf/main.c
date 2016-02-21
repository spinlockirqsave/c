#include <stdio.h>
#include <stdint.h>

#include <buffer.h>

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
#define VARIANCE_THRESHOLD (0.001)

int
main()
{
    uint32_t    session_rate = 64000; /* bits per second */
    circ_buffer_t b;
	INIT_CIRC_BUFFER(&b, (size_t)BEEP_LEN(session_rate), (size_t)FRAME_LEN(session_rate));
    return 0;
}
