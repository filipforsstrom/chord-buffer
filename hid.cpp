#include "daisy_seed.h"
#include "src/dev/mpr121.h"
#include "hid.h"
#include "register.h"
#include "synth.h"

using namespace daisy;
using namespace daisy::seed;

constexpr int Hid::scales_[3][10];

Mpr121<Mpr121I2CTransport> mpr121;
GPIO s0, s1;
GPIO k0, k1, k2, k3, k4, k5;

void Hid::Init(const SynthInterface::Callbacks &callbacks, DaisySeed &hw)
{
    hw_ = hw;
    callbacks_ = callbacks;

    Mpr121<Mpr121I2CTransport>::Config mpr121_config;
    // Set any necessary fields in mpr121_config here

    mpr121.Init(mpr121_config);

    s0.Init(A6, GPIO::Mode::ANALOG, GPIO::Pull::NOPULL, GPIO::Speed::LOW);
}

void Hid::Process()
{
    static uint16_t prevTouchStatus = 0; // Keep track of the previous touch status
    uint16_t touchStatus = mpr121.Touched();

    // Check each bit of touchStatus. If a bit is set, it means that the corresponding channel is touched.
    for (uint8_t i = 0; i < 12; i++)
    {
        if (((touchStatus >> i) & 1) && !((prevTouchStatus >> i) & 1))
        {
            // The i-th channel is touched and was not touched in the previous call to Process.
            if (i == 10)
            {
                controlInput_.SetBaseNote(-1);
            }
            else if (i == 11)
            {
                controlInput_.SetBaseNote(1);
            }
            else
            {
                // This means that a touch has begun. Create a Note object and call NoteOn.
                Note note;
                int baseNote = controlInput_.BaseNote();
                int octave = controlInput_.Octave();
                bool trigger = controlInput_.switch1;
                note.pitch = baseNote + (scales_[scale_][i] + (octave * 12));
                note.amp = .5f;
                NoteOn(note, trigger);
            }
        }
        else if (!((touchStatus >> i) & 1) && ((prevTouchStatus >> i) & 1))
        {
            // The i-th channel was touched in the previous call to Process but is no longer touched.
            if (i != 10 && i != 11)
            {
                // This means that a touch has ended. Create a Note object and call NoteOff.
                Note note;
                int baseNote = controlInput_.BaseNote();
                int octave = controlInput_.Octave();
                bool trigger = controlInput_.switch1;
                note.pitch = baseNote + (scales_[scale_][i] + (octave * 12));
                NoteOff(note, trigger);
            }
        }
    }

    prevTouchStatus = touchStatus; // Update the previous touch status

    controlInput_.slider1 = hw_.adc.GetFloat(6);
    controlInput_.slider2 = hw_.adc.GetFloat(7);
    SetRegisterParam(Register::Param::RAND_PITCH, controlInput_.slider2);
    controlInput_.pot1 = hw_.adc.GetFloat(0);
    SetSeqParam(Seq::Param::SPEED, controlInput_.pot1);
    controlInput_.pot2 = hw_.adc.GetFloat(1);
    SetSynthParam(Synth::Param::ADSR, controlInput_.pot2);
    controlInput_.pot3 = hw_.adc.GetFloat(2);
    SetSynthParam(Synth::Param::INDEX, controlInput_.pot3);
    controlInput_.pot4 = hw_.adc.GetFloat(3);
    SetSynthParam(Synth::Param::SPACE, controlInput_.pot4);
    controlInput_.pot5 = hw_.adc.GetFloat(4);
    SetQuantizerParam(Quantizer::Param::SCALE, controlInput_.pot5);
    SetScale(controlInput_.pot5);
    controlInput_.pot6 = hw_.adc.GetFloat(5);
    SetSeqParam(Seq::Param::SPREAD, controlInput_.pot6);
}

void Hid::NoteOn(Note note, bool trigger)
{
    callbacks_.noteOnCallback(note, trigger);
}

void Hid::NoteOff(Note note, bool trigger)
{
    callbacks_.noteOffCallback(note, trigger);
}

void Hid::SetSynthParam(Synth::Param param, float value)
{
    callbacks_.setSynthParamCallback(param, value);
}

void Hid::SetSeqParam(Seq::Param param, float value)
{
    callbacks_.setSeqParamCallback(param, value);
}

void Hid::SetRegisterParam(Register::Param param, float value)
{
    callbacks_.setRegisterParamCallback(param, value);
}

void Hid::SetQuantizerParam(Quantizer::Param param, float value)
{
    callbacks_.setQuantizerParamCallback(param, value);
}

void Hid::SetScale(float value)
{
    scale_ = static_cast<int>(value * 3);
}