#include <array>
#include "daisy_seed.h"
#include "synthinterface.h"

class Midi : public SynthInterface
{
public:
    void Init(const SynthInterface::Callbacks &callbacks, daisy::DaisySeed &hw) override;
    void Process() override;
    void AddNoteToRegister(Note note) override;
    void NoteOn() override;
    void NoteOff() override;
    void SendNoteOn(Note note);
    void SendNoteOff(Note note);
    void SetSynthParam(Synth::Param param, float value) override;
    void SetSeqParam(Seq::Param param, float value) override;
    void SetRegisterParam(Register::Param param, float value) override;
    void SetQuantizerParam(Quantizer::Param param, float value) override;
    Synth::Param MapCcToParam(int cc);

private:
    float VelocityToAmp(int velocity);
    int AmpToVelocity(float amp);
    SynthInterface::Callbacks callbacks_;
    std::array<Synth::Param, 3> ccToParam_ = {Synth::Param::PAN, Synth::Param::RAMOUNTPAN};
};