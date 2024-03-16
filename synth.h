#ifndef SYNTH_H
#define SYNTH_H

#include "daisysp.h"
#include "register.h"

using namespace daisysp;

class Voice
{
public:
    Fm2 osc;
    SmoothRandomGenerator noise;
    float frequency = 440.f;
    Oscillator vib;
    Adsr adsr;
    Note note;
    bool gate;
    float pan = .0f;
    float rAmountPan = .0f;
    float atk = 3.01f;
    float dec = 1.f;
    float sus = .0f;
    float rel = 50.f;
    float modAtk = .0f;
    float modDec = .0f;
    float modSus = .0f;
    float modRel = .0f;
    const float lowAtk = .003f;
    const float lowDec = .003f;
    const float lowSus = .0f;
    const float lowRel = .003f;
    float index = .0f;
    float vibAmount = .0f;

    float Atk()
    {
        float val = atk * modAtk;
        return (val < lowAtk) ? lowAtk : val;
    };
    float Dec()
    {
        float val = dec * modDec;
        return (val < lowDec) ? lowDec : val;
    };
    float Sus()
    {
        float val = sus * modSus;
        return (val < lowSus) ? lowSus : val;
    };
    float Rel()
    {
        float val = rel * modRel;
        return (val < lowRel) ? lowRel : val;
    };
};

class Synth
{
public:
    enum class Param
    {
        PAN,
        RAMOUNTPAN,
        ATK,
        DEC,
        SUS,
        REL,
        ADSR,
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