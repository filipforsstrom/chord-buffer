#ifndef SYNTH_INTERFACE_H
#define SYNTH_INTERFACE_H

#include "daisy_seed.h"
#include <functional>
#include "register.h"
#include "synth.h"
#include "seq.h"
#include "quantizer.h"

class SynthInterface
{
public:
    struct Callbacks
    {
        std::function<void(Note)> addNoteToRegisterCallback;
        std::function<void()> noteOnCallback;
        std::function<void()> noteOffCallback;
        std::function<void(Synth::Param, float)> setSynthParamCallback;
        std::function<void(Seq::Param, float)> setSeqParamCallback;
        std::function<void(Register::Param, float)> setRegisterParamCallback;
        std::function<void(Quantizer::Param, float)> setQuantizerParamCallback;
    };

    virtual void Init(const Callbacks &callbacks, daisy::DaisySeed &hw) = 0;
    virtual void Process() = 0;
    virtual void AddNoteToRegister(Note note) = 0;
    virtual void NoteOn() = 0;
    virtual void NoteOff() = 0;
    virtual void SetSynthParam(Synth::Param param, float value) = 0;
    virtual void SetSeqParam(Seq::Param param, float value) = 0;
    virtual void SetRegisterParam(Register::Param param, float value) = 0;
    virtual void SetQuantizerParam(Quantizer::Param param, float value) = 0;
};

#endif // SYNTH_INTERFACE_H