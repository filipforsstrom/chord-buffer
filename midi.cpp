#include "midi.h"
#include "daisy_seed.h"
#include "daisysp.h"
#include "synth.h"

using namespace daisy;
using namespace daisysp;

static MidiUsbHandler midiHandler;

void Midi::Init(const SynthInterface::Callbacks &callbacks, DaisySeed &hw)
{
    callbacks_ = callbacks;

    MidiUsbHandler::Config midi_cfg;
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    midiHandler.Init(midi_cfg);
}

void Midi::Process()
{
    midiHandler.Listen();

    while (midiHandler.HasEvents())
    {
        auto msg = midiHandler.PopEvent();

        switch (msg.type)
        {
        case MidiMessageType::NoteOn:
        {
            auto note_msg = msg.AsNoteOn();
            Note note = {note_msg.note, VelocityToAmp(note_msg.velocity)};
            NoteOn(note, true);
            break;
        }
        case MidiMessageType::NoteOff:
        {
            auto note_msg = msg.AsNoteOff();
            Note note = {note_msg.note, 0.f};
            NoteOff(note, true);
            break;
        }
        case MidiMessageType::ControlChange:
        {
            auto cc_msg = msg.AsControlChange();
            Synth::Param param = MapCcToParam(cc_msg.control_number);
            float value = (float)cc_msg.value / 127.f;
            SetSynthParam(param, value);
            break;
        }
        default:
            break;
        }
    }
}

Synth::Param Midi::MapCcToParam(int cc)
{
    return ccToParam_[cc];
}

void Midi::SendNoteOn(Note note)
{
    uint8_t message[3];
    message[0] = 0x90 | (0 & 0x0F);              // Note on message start with 0x9
    message[1] = note.pitch & 0x7F;              // MIDI notes range from 0 to 127
    message[2] = AmpToVelocity(note.amp) & 0x7F; // MIDI velocity range from 0 to 127

    midiHandler.SendMessage(message, sizeof(message));
}

void Midi::SendNoteOff(Note note)
{
    uint8_t message[3];
    message[0] = 0x80 | (0 & 0x0F); // Note off message start with 0x8
    message[1] = note.pitch & 0x7F; // MIDI notes range from 0 to 127
    message[2] = 0 & 0x7F;          // MIDI velocity range from 0 to 127

    midiHandler.SendMessage(message, sizeof(message));
}

void Midi::NoteOn(Note note, bool trigger)
{
    callbacks_.noteOnCallback(note, trigger);
}

void Midi::NoteOff(Note note, bool trigger)
{
    callbacks_.noteOffCallback(note, trigger);
}

void Midi::SetSynthParam(Synth::Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f));
    callbacks_.setSynthParamCallback(param, value);
}

void Midi::SetSeqParam(Seq::Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f));
    callbacks_.setSeqParamCallback(param, value);
}

void Midi::SetRegisterParam(Register::Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f));
    callbacks_.setRegisterParamCallback(param, value);
}

void Midi::SetQuantizerParam(Quantizer::Param param, float value)
{
    value = std::max(0.f, std::min(value, 1.f));
    callbacks_.setQuantizerParamCallback(param, value);
}

float Midi::VelocityToAmp(int velocity)
{
    return (float)velocity / 127.f;
}

int Midi::AmpToVelocity(float amp)
{
    return (int)(amp * 127.f);
}