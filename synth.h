#ifndef SYNTH_H
#define SYNTH_H

#include "daisysp.h"
#include "register.h"
#include "envelope.h"

using namespace daisysp;

class Voice
{
public:
    Fm2 osc;
    SmoothRandomGenerator noise;
    float frequency = 440.f;
    Oscillator vib;
    Envelope env;
    Envelope envMod;
    Note note;
    bool gate;
    float pan = .0f;
    float rAmountPan = 1.f;
    float index = .0f;
    float vibAmount = .1f;
};

class Synth
{
public:
    enum class Param
    {
        PAN,
        RAMOUNTPAN,
        ADSR,
        ENVMOD,
        ENVMODDEPTH,
        INDEX,
        SPACE,
    };
    Synth(int numVoices);
    ~Synth();

    void Init(float sampleRate);
    std::pair<float, float> Process();
    void NoteOn(Note note);
    void NoteOff(Note note);
    void SetParam(Param param, float value);

private:
    Voice *voices_;
    int numVoices_;
    int posVoice_ = 0;
};

#endif // SYNTH_H