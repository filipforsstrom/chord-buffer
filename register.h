#ifndef REGISTER_H
#define REGISTER_H

#include "daisysp.h"

struct Note
{
    int pitch = 69;
    float amp = .0f;
};

class Register
{
public:
    enum class Param
    {
        RAND_PITCH,
    };

    Register(int size);
    void AddNote(Note note);
    std::vector<Note> GetNotes();
    void SetParam(Param param, float value);

private:
    float randPitchProbability_ = 0.f;
    int randPitchRange_ = 7;
    std::mt19937 rng_;
    std::vector<Note> notesInput_;
    std::vector<Note> notesOutput_;
    int size_;
    int pos_;
};

#endif // REGISTER_H