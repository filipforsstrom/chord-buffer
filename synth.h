#ifndef SYNTH_H
#define SYNTH_H

#include "daisysp.h"
#include "register.h"

using namespace daisysp;

class Voice
{
public:
    Fm2 osc;
    float frequency = 440.f;
    Oscillator vib;
    Adsr adsr;
    Note note;
    bool gate;
    float pan = .0f;
    float rPan = .0f;
    float rAmountPan = .0f;
    float atk = .01f;
    float dec = .01f;
    float sus = .5f;
    float rel = .1f;
    float modAtk = .0f;
    float modDec = .0f;
    float modSus = .0f;
    float modRel = .0f;
    float index = .0f;
    float vibAmount = .0f;

    float Atk() { return atk + modAtk; };
    float Dec() { return dec + modDec; };
    float Sus() { return sus + modSus; };
    float Rel() { return rel + modRel; };
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