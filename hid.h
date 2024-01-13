#include "daisy_seed.h"
#include "synthinterface.h"

class ControlInput
{
public:
    float slider1;
    float slider2;
    float pot1;
    float pot2;
    float pot3;
    float pot4;
    float pot5;
    float pot6;
    bool switch1 = true;
    bool switch2 = true;

    int BaseNote() const
    {
        return baseNote_;
    }

    void SetBaseNote(int change)
    {
        baseNote_ += change;
        if (baseNote_ < 0)
        {
            baseNote_ = 11;
        }
        else if (baseNote_ > 11)
        {
            baseNote_ = 0;
        }
    }

    int Octave() const
    {
        // Map slider1 value to the range between minOctave and maxOctave
        return minOctave_ + static_cast<int>(slider1 * (maxOctave_ - minOctave_ + 1));
    }

private:
    int baseNote_ = 0;
    int minOctave_ = 2;
    int maxOctave_ = 6;
};

class Hid : public SynthInterface
{
public:
    void Init(const SynthInterface::Callbacks &callbacks, daisy::DaisySeed &hw) override;
    void Process() override;
    void NoteOn(Note note, bool trigger) override;
    void NoteOff(Note note, bool trigger) override;
    void SetSynthParam(Synth::Param param, float value) override;
    void SetSeqParam(Seq::Param param, float value) override;
    void SetRegisterParam(Register::Param param, float value) override;
    void SetQuantizerParam(Quantizer::Param param, float value) override;
    void SetScale(float value);

    ControlInput controlInput_;

private:
    daisy::DaisySeed hw_;
    SynthInterface::Callbacks callbacks_;
    int scale_ = 0;
    static constexpr int scales_[3][10] = {
        {21, 23, 24, 26, 28, 29, 31, 33, 35, 36}, // Minor
        {21, 23, 25, 26, 28, 30, 32, 33, 35, 37}, // Major
        {21, 22, 23, 24, 25, 26, 27, 28, 29, 30}  // Chromatic
    };
};