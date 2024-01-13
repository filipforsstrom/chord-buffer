#ifndef SEQ_H
#define SEQ_H

#include "daisysp.h"
#include "register.h"
#include "synth.h"
#include "quantizer.h"

using namespace daisysp;

struct NoteClock
{
    Oscillator clock;
    float speed = 15.f;
    bool gate = false;
    bool done = false;
};

struct Clock
{
    Oscillator mainClock;
    bool run = false;
    float speed = 1.f;
    std::vector<NoteClock> noteClocks;
};

class Seq
{
public:
    enum class Param
    {
        SPEED,
        SPREAD,
    };

    Seq(int numNoteClocks);
    void Init(Register *reg, Synth *synth, Quantizer *quantizer, float sampleRate);
    void Process();
    void SetParam(Param param, float value);

private:
    float sampleRate_;
    Clock clock_;
    Register *reg_;
    Synth *synth_;
    Quantizer *quantizer_;
};

#endif // SEQ_H