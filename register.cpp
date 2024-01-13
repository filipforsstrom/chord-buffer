#include "Register.h"

Register::Register(int size)
{
    notesInput_.resize(size);
    notesOutput_.resize(size);

    for (int i = 0; i < size; i++)
    {
        notesInput_[i].pitch = 63;
        notesInput_[i].amp = 0.5;

        notesOutput_[i].pitch = 63;
        notesOutput_[i].amp = 0.5;
    }

    size_ = size;
    pos_ = 0;
}

void Register::AddNote(Note note)
{
    notesInput_[pos_] = note;
    pos_++;
    if (pos_ >= size_)
    {
        pos_ = 0;
    }
}

std::vector<Note> Register::GetNotes()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> pitch_dist(-randPitchRange_, randPitchRange_);
    for (int i = 0; i < size_; i++)
    {
        if (dist(rng_) < randPitchProbability_)
        {
            notesOutput_[i].pitch = notesInput_[i].pitch + pitch_dist(rng_);
            if (notesOutput_[i].pitch < 0)
                notesOutput_[i].pitch = 0;
            if (notesOutput_[i].pitch > 127)
                notesOutput_[i].pitch = 127; // assuming MIDI pitch range
        }
        else
        {
            notesOutput_[i].pitch = notesInput_[i].pitch; // use the original pitch
        }
    }
    return notesOutput_;
}

void Register::SetParam(Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f)); // clamp value between 0 and 1

    switch (param)
    {
    case Register::Param::RAND_PITCH:
        randPitchProbability_ = pow(value, 2) * 2;
        randPitchRange_ = 7 + value * 12;
        break;
    default:
        break;
    }
}