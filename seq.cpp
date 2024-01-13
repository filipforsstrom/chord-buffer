#include "seq.h"
#include "daisy_seed.h"

Seq::Seq(int numNoteClocks)
{
    clock_.noteClocks.resize(numNoteClocks);
}

void Seq::Init(Register *reg, Synth *synth, Quantizer *quantizer, float sampleRate)
{
    reg_ = reg;
    synth_ = synth;
    quantizer_ = quantizer;
    sampleRate_ = sampleRate;
    clock_.mainClock.Init(sampleRate_);
    clock_.mainClock.SetFreq(1.f);
    clock_.mainClock.SetWaveform(Oscillator::WAVE_SQUARE);
    for (size_t i = 0; i < clock_.noteClocks.size(); i++)
    {
        clock_.noteClocks[i].clock.Init(sampleRate_);
        clock_.noteClocks[i].clock.SetFreq(1.f);
        clock_.noteClocks[i].clock.SetWaveform(Oscillator::WAVE_SQUARE);
    }

    clock_.run = true;
}

void Seq::Process()
{
    if (!clock_.run)
    {
        return;
    }

    for (size_t i = 0; i < clock_.noteClocks.size(); i++)
    {
        clock_.noteClocks[i].clock.Process();
        if (clock_.noteClocks[i].clock.IsEOC() && !clock_.noteClocks[i].gate)
        {
            std::vector<Note> notes = reg_->GetNotes();
            notes[i].pitch = quantizer_->Quantize(notes[i].pitch);
            synth_->NoteOn(notes[i]);
            clock_.noteClocks[i].gate = true;
        }

        if (clock_.noteClocks[i].clock.IsEOR() && clock_.noteClocks[i].gate)
        {
            std::vector<Note> notes = reg_->GetNotes();
            notes[i].pitch = quantizer_->Quantize(notes[i].pitch);
            synth_->NoteOff(notes[i]);
            clock_.noteClocks[i].done = true;
            clock_.noteClocks[i].gate = false;
        }
    }
}

void Seq::SetParam(Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f)); // clamp value between 0 and 1

    switch (param)
    {
    case Param::SPEED:
        clock_.run = !(value < 0.1f);

        value = pow(value, 3) * 60.f;
        clock_.mainClock.SetFreq(value);
        clock_.speed = value;
        break;
    case Param::SPREAD:
        for (size_t i = 0; i < clock_.noteClocks.size(); i++)
        {
            clock_.noteClocks[i].clock.SetFreq(clock_.speed - (i * (value * .05f)));
        }
        break;
    default:
        break;
    }
}