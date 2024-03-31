#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "daisysp.h"

using namespace daisysp;

class Envelope
{
private:
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

public:
    Adsr adsr;
    float depth = .0f;

    void Init(float sampleRate)
    {
        adsr.Init(sampleRate);
        adsr.SetTime(ADSR_SEG_ATTACK, Atk());
        adsr.SetTime(ADSR_SEG_DECAY, Dec());
        adsr.SetSustainLevel(Sus());
        adsr.SetTime(ADSR_SEG_RELEASE, Rel());
    }

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

    void SetShape(float value)
    {
        modAtk = 0.0f;
        modDec = 0.0f;
        modSus = 0.0f;
        modRel = 0.0f;

        if (value > 0)
        {
            modDec = fmap(value * 0.5f, 0, 1, Mapping::EXP);
            modRel = (value > 0.4f) ? fmap(value * 0.3f, 0, 1, Mapping::EXP) : 0;
        }
        else if (value < 0)
        {
            modAtk = fmap(value, 0, 1, Mapping::EXP);
            modRel = fmap(value, 0, 1, Mapping::EXP);
        }
    }
};

#endif // ENVELOPE_H