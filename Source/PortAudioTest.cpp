#include <cstdio>
#include <cmath>
#include "portaudio.h"

namespace
{
    unsigned int kSampleRate = 48000;

    class Tester
    {
    public:

        Tester() : time_(0) {}

        float getNext()
        {
            time_ += 1.0f / kSampleRate;
            return std::sinf(time_ * 440 * 2) * 0.5f;
        }

    private:
        float time_;
    };

    static int TesterCallback(
        const void* /*inputBuffer*/, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData
    )
    {
        auto tester = reinterpret_cast<Tester*>(userData);
        auto out = reinterpret_cast<float*>(outputBuffer);

        for (auto i = 0u; i < framesPerBuffer; i++)
            *out++ = *out++ = tester->getNext();

        return 0;
    }

    PaError RunTest()
    {
        auto err = Pa_Initialize();
        if (err != paNoError) return err;

        Tester tester;
        PaStream* stream;

        err = Pa_OpenDefaultStream(&stream,
            0,          // no input channels
            2,          // stereo output
            paFloat32,  // 32 bit floating point output
            kSampleRate,
            paFramesPerBufferUnspecified,
            TesterCallback,
            &tester
        );
        if (err != paNoError) return err;

        err = Pa_StartStream(stream);
        if (err != paNoError) return err;

        Pa_Sleep(4 * 1000);

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
