#include "daisy_seed.h"
#include "daisysp.h"
#include "register.h"
#include "synth.h"
#include "synthinterface.h"
#include "midi.h"
#include "quantizer.h"
#include "seq.h"
#include "hid.h"

using namespace daisy;
using namespace daisysp;

const int MAX_VOICES = 8;
const int MAX_REGISTERS = 8;

DaisySeed hw;
MidiUsbHandler midiHandler;
Midi midi;
Register reg(MAX_VOICES);
Synth synth(MAX_REGISTERS);
Quantizer quantizer(Quantizer::CHROMATIC);
Seq seq(MAX_REGISTERS);
Hid hid;
bool keyMode = true;
bool seqMode = true;

void NoteOnCallback(Note note, bool trigger)
{
	note.pitch = quantizer.Quantize(note.pitch);
	reg.AddNote(note);

	if (trigger)
	{
		std::vector<Note> notes = reg.GetNotes();
		for (size_t i = 0; i < notes.size(); i++)
		{
			synth.NoteOn(notes[i]);
		}
	}
}

void NoteOffCallback(Note note, bool trigger)
{
	if (trigger)
	{
		std::vector<Note> notes = reg.GetNotes();
		for (size_t i = 0; i < notes.size(); i++)
		{
			synth.NoteOff(notes[i]);
		}
	}
}

void SetSynthParamCallback(Synth::Param param, float value)
{
	synth.SetParam(param, value);
}

void SetSeqParamCallback(Seq::Param param, float value)
{
	seq.SetParam(param, value);
}

void SetRegisterParamCallback(Register::Param param, float value)
{
	reg.SetParam(param, value);
}

void SetQuantizerParamCallback(Quantizer::Param param, float value)
{
	quantizer.SetParam(param, value);
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	std::pair<float, float> sig;
	for (size_t i = 0; i < size; i++)
	{
		if (seqMode)
		{
			seq.Process();
		}

		sig = synth.Process();
		out[0][i] = sig.first;
		out[1][i] = sig.second;
	}
}

int main(void)
{
	hw.Init();
	hw.StartLog();
	hw.SetAudioBlockSize(4);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	float sampleRate = hw.AudioSampleRate();

	synth.Init(sampleRate);
	seq.Init(&reg, &synth, &quantizer, sampleRate);

	SynthInterface::Callbacks callbacks;
	callbacks.noteOnCallback = NoteOnCallback;
	callbacks.noteOffCallback = NoteOffCallback;
	callbacks.setSynthParamCallback = SetSynthParamCallback;
	callbacks.setSeqParamCallback = SetSeqParamCallback;
	callbacks.setRegisterParamCallback = SetRegisterParamCallback;
	callbacks.setQuantizerParamCallback = SetQuantizerParamCallback;

	midi.Init(callbacks, hw);
	hid.Init(callbacks, hw);

	Switch switch1;
	switch1.Init(hw.GetPin(7), 1000);

	AdcChannelConfig adcConfig[8];
	adcConfig[0].InitSingle(hw.GetPin(15));
	adcConfig[1].InitSingle(hw.GetPin(16));
	adcConfig[2].InitSingle(hw.GetPin(17));
	adcConfig[3].InitSingle(hw.GetPin(18));
	adcConfig[4].InitSingle(hw.GetPin(19));
	adcConfig[5].InitSingle(hw.GetPin(20));
	adcConfig[6].InitSingle(hw.GetPin(21));
	adcConfig[7].InitSingle(hw.GetPin(22));
	hw.adc.Init(adcConfig, 8);
	hw.adc.Start();

	hw.StartAudio(AudioCallback);
	while (1)
	{
		midi.Process();
		hid.Process();

		switch1.Debounce();
		hid.controlInput_.switch1 = switch1.Pressed();
	}
}
