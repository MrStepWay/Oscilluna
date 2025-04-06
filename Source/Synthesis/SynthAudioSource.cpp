#include "SynthAudioSource.h"

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState)
    : keyboardState(keyState)
{
    // Добавляем 16 голосов для полифонии
    for (int i = 0; i < 16; ++i) {
        synth.addVoice(new SynthVoice());
    }

    synth.addSound(new SynthSound());
}

// Установка частоты дискретизации и сброс MIDI коллектора
void SynthAudioSource::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate); // Установка sample rate для всех голосов
    midiCollector.reset(sampleRate); // Сброс и подготовка MIDI коллектора
}

// Очистка ресурсов
void SynthAudioSource::releaseResources() {}

// Главный метод рендеринга следующего блока аудио
void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion(); // Очистка целевого буфера перед записью

    juce::MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples); // Извлекаем MIDI сообщения для этого блока

    keyboardState.processNextMidiBuffer(incomingMidi,
                                        bufferToFill.startSample,
                                        bufferToFill.numSamples,
                                        true); // Обновляем состояние клавиатуры на основе новых MIDI сообщений

    synth.renderNextBlock(*bufferToFill.buffer,
                          incomingMidi,
                          bufferToFill.startSample,
                          bufferToFill.numSamples);
}

// Получение доступа к внутреннему MIDI коллектору
juce::MidiMessageCollector* SynthAudioSource::getMidiCollector() {
    return &midiCollector;
}
