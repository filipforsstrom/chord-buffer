#ifndef QUANTIZER_H
#define QUANTIZER_H

#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

class Quantizer
{
public:
    enum Scale
    {
        MINOR,
        MAJOR,
        CHROMATIC
    };

    enum class Param
    {
        SCALE
    };

    Quantizer(Scale scale = CHROMATIC) : scale_(scale) {}
    ~Quantizer() = default;
    void SetParam(Param param, float value)
    {
        switch (param)
        {
        case Param::SCALE:
            SetScale(static_cast<Scale>(value * 3));
            break;
        }
    }

    int Quantize(int note)
    {
        // Calculate the octave and the note within the octave
        int octave = note / 12;
        int noteInOctave = note % 12;

        // Find the closest note in the scale
        int closestNoteInOctave = *std::min_element(scales_[scale_].begin(), scales_[scale_].end(), [noteInOctave](int a, int b)
                                                    { return std::abs(a - noteInOctave) < std::abs(b - noteInOctave); });

        // Add the original octave back to the result
        int closestNote = octave * 12 + closestNoteInOctave;

        return closestNote;
    }

    void SetScale(Scale scale)
    {
        scale_ = scale;
    };

private:
    Scale scale_;
    std::map<Scale, std::vector<int>> scales_ = {
        {MINOR, {0, 2, 3, 5, 7, 8, 10, 12}},                    // C minor scale MIDI notes
        {MAJOR, {0, 2, 4, 5, 7, 9, 11, 12}},                    // C major scale MIDI notes
        {CHROMATIC, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}} // C chromatic scale MIDI notes
    };
};

#endif // QUANTIZER_H