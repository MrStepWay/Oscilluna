#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SynthSound.h"

/** Класс источника аудиоданных для синтезатора. Обеспечивает связку MIDI сообщений и генерации аудиовыхода */
class SynthAudioSource : public juce::AudioSource {
public:
    // Конструктор принимает ссылку на состояние MIDI-клавиатуры
    SynthAudioSource(juce::MidiKeyboardState& keyState);
    
    // Подготовка источника к воспроизведению (установка sampleRate и сброс MIDI коллектора)
    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override;
    
    // Освобождение ресурсов
    void releaseResources() override;
    
    // Заполнение аудиоблока с учётом MIDI событий
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    // Получение указателя на внутренний MidiMessageCollector для подключения внешних устройств
    juce::MidiMessageCollector* getMidiCollector();
    
    // Доступ к внутреннему синтезатору
    juce::Synthesiser& getSynth() { return synth; }
    const juce::Synthesiser& getSynth() const { return synth; }
    
private:
    juce::MidiKeyboardState& keyboardState; // Состояние клавиатуры
    juce::Synthesiser synth; // JUCE синтезатор (управляет голосами и их рендером)
    juce::MidiMessageCollector midiCollector; // Буферизация MIDI-сообщений для синхронизации с аудио
};
