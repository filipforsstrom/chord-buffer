// Synth.cpp

#include "Synth.h"
#include "register.h"

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

        voices_[i].vib.Init(sampleRate);
        voices_[i].vib.SetFreq(3.f + (i * 0.1));
        voices_[i].vib.Reset(i * 0.1);
        voices_[i].vib.SetAmp(voices_[i].vibAmount);
        voices_[i].vib.SetWaveform(Oscillator::WAVE_TRI);

        voices_[i].adsr.Init(sampleRate);

        voices_[i].adsr.SetTime(ADSR_SEG_ATTACK, voices_[i].atk);
        voices_[i].adsr.SetTime(ADSR_SEG_DECAY, voices_[i].dec);
        voices_[i].adsr.SetSustainLevel(voices_[i].sus);
        voices_[i].adsr.SetTime(ADSR_SEG_RELEASE, voices_[i].rel);
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
        float env = voices_[i].adsr.Process(gate);

        voices_[i].osc.SetIndex(voices_[i].index);
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
        float rPan = voices_[i].rPan;
        float combinedPan = pan + rPan;
        combinedPan = std::max(-1.0f, std::min(1.0f, combinedPan));

        float left = (1 - combinedPan) * 0.5;
        float right = (1 + combinedPan) * 0.5;

        sumLeft += (osc * env * left) / numVoices_;
        sumRight += (osc * env * right) / numVoices_;
    }

    return {sumLeft, sumRight};
}

float randomFloat(float min, float max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

void Synth::NoteOn(Note note)
{
    voices_[posVoice_].adsr.SetTime(ADSR_SEG_ATTACK, voices_[posVoice_].Atk());
    voices_[posVoice_].adsr.SetTime(ADSR_SEG_DECAY, voices_[posVoice_].Dec());
    voices_[posVoice_].adsr.SetSustainLevel(voices_[posVoice_].Sus());
    voices_[posVoice_].adsr.SetTime(ADSR_SEG_RELEASE, voices_[posVoice_].Rel());

    voices_[posVoice_].note = note;
    voices_[posVoice_].gate = true;
    voices_[posVoice_].frequency = mtof(note.pitch);

    float rAmountPan = voices_[posVoice_].rAmountPan;
    voices_[posVoice_].rPan = randomFloat(-1.0f, 1.0f) * rAmountPan;

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
    case Synth::Param::ATK:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].atk = value * 2;
        }
        break;
    case Synth::Param::DEC:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].dec = value * 2;
        }
        break;
    case Synth::Param::SUS:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].sus = value;
        }
        break;
    case Synth::Param::REL:
        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].rel = value * 2;
        }
        break;
    case Synth::Param::ADSR:
        value = (value * 2) - 1; // convert value from 0-1 to -1-1

        for (int i = 0; i < numVoices_; i++)
        {
            voices_[i].modAtk = (value < 0) ? value * -2 : 0; // change when value is negative
            voices_[i].modDec = value * 2;
            voices_[i].modSus = value * -2;
            voices_[i].modRel = (value < 0) ? value * -2 : value * 8; // change when value is negative or positive
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
