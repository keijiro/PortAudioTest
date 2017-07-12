#include <cstdio>
#include <cmath>
#include "portaudio.h"
#include "BiquadFilter.h"

namespace
{
    unsigned int kSampleRate = 48000;

    static int TesterCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData
    )
    {
        auto filter = reinterpret_cast<BiquadFilter*>(userData);
        auto in = reinterpret_cast<const float*>(inputBuffer);
        auto out = reinterpret_cast<float*>(outputBuffer);

        for (auto i = 0u; i < framesPerBuffer; i++)
            *out++ = filter->feedSample(*in++);

        return 0;
    }

    PaError RunTest()
    {
        auto err = Pa_Initialize();
        if (err != paNoError) return err;

        BiquadFilter filter;
        filter.setLowpass(0.01f, 1);

        PaStream* stream;

        err = Pa_OpenDefaultStream(&stream,
            1,          // no input channels
            1,          // stereo output
            paFloat32,  // 32 bit floating point output
            kSampleRate,
            paFramesPerBufferUnspecified,
            TesterCallback,
            &filter
        );
        if (err != paNoError) return err;

        err = Pa_StartStream(stream);
        if (err != paNoError) return err;

        Pa_Sleep(10 * 1000);

        err = Pa_StopStream(stream);
        if (err != paNoError) return err;

        err = Pa_CloseStream(stream);
        return err;
    }
}

int main()
{
    auto err = RunTest();

	Pa_Terminate();

    if (err == paNoError)
    {
        std::puts("Test finished.");
    }
    else
    {
        std::fprintf(stderr, "An error occured while using the portaudio stream\n");
        std::fprintf(stderr, "Error number: %d\n", err);
        std::fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    }

	std::getchar();
	return err;
}
