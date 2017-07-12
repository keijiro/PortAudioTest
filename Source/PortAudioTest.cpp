#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (4)
#define SAMPLE_RATE   (48000)

typedef struct
{
	float phase;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void)inputBuffer; /* Prevent unused variable warning. */

	for (i = 0; i<framesPerBuffer; i++)
	{
        float mul = powf(2.0f, floorf(data->phase));
		*out++ = *out++ = sinf(440.0f * 3.141592f * mul * data->phase) * 0.71f;
        data->phase += 1.0f / SAMPLE_RATE;
	}
	return 0;
}

/*******************************************************************/
static paTestData data;
int main(void);
int main(void)
{
	PaStream *stream;
	PaError err;

	printf("PortAudio Test: output sawtooth wave.\n");
	/* Initialize our data for use by callback. */
	data.phase = 0.0;
	/* Initialize library before making any other calls. */
	err = Pa_Initialize();
	if (err != paNoError) goto error;

	/* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream(&stream,
		0,          /* no input channels */
		2,          /* stereo output */
		paFloat32,  /* 32 bit floating point output */
		SAMPLE_RATE,
		256,        /* frames per buffer */
		patestCallback,
		&data);
	if (err != paNoError) goto error;

	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	/* Sleep for several seconds. */
	Pa_Sleep(NUM_SECONDS * 1000);

	err = Pa_StopStream(stream);
	if (err != paNoError) goto error;
	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;
	Pa_Terminate();
	printf("Test finished.\n");
	getchar();
	return err;
error:
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	getchar();
	return err;
}
