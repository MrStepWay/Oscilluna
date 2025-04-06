#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"
#include "Oscillator.h"
#include "UnisonOscillator.h"

/**Класс SynthVoice реализует отдельный голос синтезатора*/
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    // Проверяет, может ли голос воспроизводить данный звук
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    
    // Старт новой ноты
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    
    // Остановка ноты
    void stopNote(float velocity, bool allowTailOff) override;
    
    // Реакция на движение pitch wheel (пока не используется)
    void pitchWheelMoved(int) override {}

    // Реакция на MIDI-контроллеры (пока не используется)
    void controllerMoved(int, int) override {}

    // Генерация следующего блока аудио данных
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    // Установка частоты дискретизации воспроизведения
    void setCurrentPlaybackSampleRate(double newRate) override;

    // Доступ для GUI: получить осциллятор по индексу
    UnisonOscillator& getOsc(size_t i) {
        return oscillators[i];
    }

    // Изменение параметров ADSR
    void setAttack(float value = 0.1f);
    void setDecay(float value = 0.1f);
    void setSustain(float value = 1.f);
    void setRelease(float value = 0.1f);
    
    // Получение текущих параметров ADSR
    const juce::ADSR::Parameters& getAdsrParams() const { return adsrParams; }

private:
    static constexpr size_t numOsc {3}; // Количество осцилляторов в одном голосе
    std::array<UnisonOscillator, numOsc> oscillators; // Массив многоголосных осцилляторов
    juce::ADSR adsr; // ADSR огибающая
    juce::ADSR::Parameters adsrParams { 0.01f, 0.1f, 1.f, 0.1f }; // Стартовые параметры огибающей
    float level; // Уровень ноты (velocity)
    bool paramsDirty = false; // Флаг изменения параметров ADSR
};
