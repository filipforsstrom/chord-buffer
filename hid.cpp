#include "daisy_seed.h"
#include "src/dev/mpr121.h"
#include "hid.h"
#include "register.h"
#include "synth.h"
#include <bitset>

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
    static std::bitset<12> previousChannelTouches; // Keep track of the previous touch status
    std::bitset<12> currentChannelTouches(mpr121.Touched());

    // Check each bit of currentChannelTouches. If a bit is set, it means that the corresponding channel is touched.
    for (uint8_t channel = 0; channel < 12; channel++)
    {
        if (currentChannelTouches[channel] && !previousChannelTouches[channel])
        {
            // If channel 10 or 11 is touched, update controlInput state to settings
            if (channel == 10 || channel == 11)
            {
                controlInput_.state = ControlInput::State::SETTINGS;
            }

            HandleTouch(controlInput_.state, channel);
        }
        else if (!currentChannelTouches[channel] && previousChannelTouches[channel])
        {
            // If channel 10 or 11 is released, update controlInput state to keyboard
            if (channel == 10 || channel == 11)
            {
                controlInput_.state = ControlInput::State::KEYBOARD;
            }

            HandleRelease(controlInput_.state, channel);
        }
    }

    previousChannelTouches = currentChannelTouches; // Update the previous touch status for the next call to Process

    controlInput_.slider1 = hw_.adc.GetFloat(6);
    controlInput_.slider2 = hw_.adc.GetFloat(7);
    SetRegisterParam(Register::Param::RAND_PITCH, controlInput_.slider2);
    controlInput_.pot1 = hw_.adc.GetFloat(0);
    SetSynthParam(Synth::Param::ENVMOD, controlInput_.pot1);
    controlInput_.pot2 = hw_.adc.GetFloat(1);
    SetSynthParam(Synth::Param::ENVAMP, controlInput_.pot2);
    controlInput_.pot3 = hw_.adc.GetFloat(2);
    SetSynthParam(Synth::Param::INDEX, controlInput_.pot3);
    controlInput_.pot4 = hw_.adc.GetFloat(3);
    SetSynthParam(Synth::Param::ENVMODDEPTH, controlInput_.pot4);
    controlInput_.pot5 = hw_.adc.GetFloat(4);
    SetSeqParam(Seq::Param::SPEED, controlInput_.pot5);
    SetQuantizerParam(Quantizer::Param::SCALE, 0);
    SetScale(0);
    controlInput_.pot6 = hw_.adc.GetFloat(5);
    SetSeqParam(Seq::Param::SPREAD, controlInput_.pot6);
}

void Hid::AddNoteToRegister(Note note)
{
    callbacks_.addNoteToRegisterCallback(note);
}

void Hid::NoteOn()
{
    callbacks_.noteOnCallback();
}

void Hid::NoteOff()
{
    callbacks_.noteOffCallback();
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

void Hid::SetScale(int index)
{
    scale_ = index;
}

void Hid::HandleTouch(ControlInput::State state, int channel)
{
    switch (state)
    {
    case ControlInput::State::KEYBOARD:
        if (channel < 10)
        {
            Note note;
            int baseNote = controlInput_.BaseNote();
            int octave = controlInput_.Octave();
            note.pitch = baseNote + (scales_[scale_][channel] + (octave * 12));
            note.amp = .5f;
            AddNoteToRegister(note);
            bool trigger = controlInput_.switch1;
            if (trigger)
            {
                NoteOn();
            }
        }
        break;
    case ControlInput::State::SETTINGS:
        if (channel == 0)
        {
            hw_.PrintLine("Settings Channel 0");
        }
        else if (channel == 1)
        {
            hw_.PrintLine("Settings Channel 1");
        }
        else if (channel == 2)
        {
            hw_.PrintLine("Settings Channel 2");
        }
        else if (channel == 3)
        {
            hw_.PrintLine("Settings Channel 3");
        }
        else if (channel == 4)
        {
            hw_.PrintLine("Settings Channel 4");
        }
        else if (channel == 5)
        {
            hw_.PrintLine("Settings Channel 5");
        }
        else if (channel == 6)
        {
            hw_.PrintLine("Settings Channel 6");
        }
        else if (channel == 7)
        {
            hw_.PrintLine("Settings Channel 7");
        }
        else if (channel == 8)
        {
            hw_.PrintLine("Settings Channel 8");
        }
        else if (channel == 9)
        {
            hw_.PrintLine("Settings Channel 9");
        }
        break;
    }
}

void Hid::HandleRelease(ControlInput::State state, int channel)
{
    switch (state)
    {
    case ControlInput::State::KEYBOARD:
        if (channel < 10)
        {
            Note note;
            int baseNote = controlInput_.BaseNote();
            int octave = controlInput_.Octave();
            note.pitch = baseNote + (scales_[scale_][channel] + (octave * 12));
            bool trigger = controlInput_.switch1;
            if (trigger)
            {
                NoteOff();
            }
        }
        break;
    case ControlInput::State::SETTINGS:

        break;
    }
}