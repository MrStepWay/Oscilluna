#pragma once

#include <JuceHeader.h>

/**
 Класс, содержащий информацию о звуке, который будет воспроизводит синтезатор
*/
class SynthSound : public juce::SynthesiserSound {
public:
    SynthSound();

    bool appliesToNote(int) override;
    bool appliesToChannel(int) override;
};
