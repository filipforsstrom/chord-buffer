// Synth.cpp

#include "synth.h"
#include "register.h"
#include "daisysp.h"

Synth::Synth(int numVoices)
{
    numVoices_ = numVoices;
}

Synth::~Synth()
{
    delete[] voices_;
}

void Synth::Init(float sampleRate)
{
    voices_ = new Voice[numVoices_];
    for (int i = 0; i < numVoices_; i++)
    {
        voices_[i].osc.Init(sampleRate);
        voices_[i].osc.SetFrequency(200 * i);
        voices_[i].osc.SetRatio(.5f);

        voices_[i].noise.Init(sampleRate);
        voices_[i].noise.SetFreq(.5f + (i * 0.1));

        voices_[i].vib.Init(sampleRate);
        voices_[i].vib.SetFreq(3.f + (i * 0.1));
        voices_[i].vib.Reset(i * 0.1);
        voices_[i].vib.SetAmp(voices_[i].vibAmount);
        voices_[i].vib.SetWaveform(Oscillator::WAVE_TRI);

        voices_[i].envAmp.Init(sampleRate);
        voices_[i].envMod.Init(sampleRate);
    }
}

std::pair<float, float> Synth::Process()
{
    float sumLeft = 0;
    float sumRight = 0;

    for (int i = 0; i < numVoices_; i++)
    {
        bool gate = voices_[i].gate;
        float amp = voices_[i].note.amp;
        float env = voices_[i].envAmp.adsr.Process(gate);
        float envMod = voices_[i].envMod.adsr.Process(gate);
        float toneMod = voices_[i].envMod.depth * envMod;

        float tone = std::max(0.0f, voices_[i].index + toneMod);
        voices_[i].osc.SetIndex(tone);

        voices_[i].vib.SetAmp(voices_[i].vibAmount);
        float vibrato = voices_[i].vib.Process();
        float freq = voices_[i].frequency;
        if (vibrato != 0)
        {
            freq = freq + (vibrato * 10);
        }
        voices_[i].osc.SetFrequency(freq);
        float osc = voices_[i].osc.Process();
        osc = osc * amp * env;

        float pan = voices_[i].pan;
        float rPan = voices_[i].noise.Process() * voices_[i].rAmountPan;
        float combinedPan = pan + rPan;
        combinedPan = std::max(-1.0f, std::min(1.0f, combinedPan));

        float left = (1 - combinedPan) * 0.5;
        float right = (1 + combinedPan) * 0.5;

        sumLeft += (osc * env * left) / numVoices_;
        sumRight += (osc * env * right) / numVoices_;
    }

    return {sumLeft, sumRight};
}

void Synth::NoteOn(Note note)
{
    voices_[posVoice_].envAmp.adsr.SetTime(ADSR_SEG_ATTACK, voices_[posVoice_].envAmp.Atk());
    voices_[posVoice_].envAmp.adsr.SetTime(ADSR_SEG_DECAY, voices_[posVoice_].envAmp.Dec());
    voices_[posVoice_].envAmp.adsr.SetSustainLevel(voices_[posVoice_].envAmp.Sus());
    voices_[posVoice_].envAmp.adsr.SetTime(ADSR_SEG_RELEASE, voices_[posVoice_].envAmp.Rel());

    voices_[posVoice_].envMod.adsr.SetTime(ADSR_SEG_ATTACK, voices_[posVoice_].envMod.Atk());
    voices_[posVoice_].envMod.adsr.SetTime(ADSR_SEG_DECAY, voices_[posVoice_].envMod.Dec());
    voices_[posVoice_].envMod.adsr.SetSustainLevel(voices_[posVoice_].envMod.Sus());
    voices_[posVoice_].envMod.adsr.SetTime(ADSR_SEG_RELEASE, voices_[posVoice_].envMod.Rel());

    voices_[posVoice_].note = note;
    voices_[posVoice_].gate = true;
    voices_[posVoice_].frequency = mtof(note.pitch);

    posVoice_ = (posVoice_ + 1) % numVoices_;
}

void Synth::NoteOff(Note note)
{
    for (int i = 0; i < numVoices_; i++)
    {
        if (voices_[i].note.pitch == note.pitch)
        {
            voices_[i].gate = false;
        }
    }
}

void Synth::SetParam(Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f)); // clamp value between 0 and 1

    switch (param)
    {
    case Synth::Param::PAN:
        value = (value * 2) - 1; // convert value from 0-1 to -1-1

        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].pan = value;
        }
        break;
    case Synth::Param::RAMOUNTPAN:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].rAmountPan = value;
        }
        break;
    case Synth::Param::ENVMODDEPTH:
        value = (value * 2) - 1; // convert value from 0-1 to -1-1
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].envMod.depth = pow(value, 3) * 2;
        }
        break;
    case Synth::Param::ENVAMP:
        value = (value * 2) - 1; // convert value from 0-1 to -1-1

        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].envAmp.SetShape(value);
        }
        break;
    case Synth::Param::ENVMOD:
        value = (value * 2) - 1; // convert value from 0-1 to -1-1

        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].envMod.SetShape(value);
        }
        break;
    case Synth::Param::INDEX:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].index = pow(value, 3) * 2;
        }
        break;
    case Synth::Param::SPACE:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].rAmountPan = std::min(value * 2, 1.0f);                 // 0-1 when values is 0-0.5
            voices_[i].vibAmount = (value > 0.5f) ? (value - 0.5f) * 2 : 0.0f; // 0-1 when value is 0.5-1
        }
        break;
    default:
        break;
    }
}
