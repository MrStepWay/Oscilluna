#include "SynthVoice.h"

SynthVoice::SynthVoice() {
    for (auto& o : oscillators) {
        o.setVoices(1); // По одному голосу в каждом осцилляторе по умолчанию
    }
    
    adsr.setParameters(adsrParams); // Инициализация ADSR параметров
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* s) {
    return dynamic_cast<SynthSound*>(s) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) {
    const double freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // Преобразование MIDI ноты в герцы
    level = velocity; // Сохранение силы нажатия

    for (auto& o : oscillators) {
        o.setBaseFreq(freq); // Настройка частоты
        o.resetPhases(); // Сброс фаз осцилляторов для чистого старта
    }

    if (paramsDirty) { // Если параметры ADSR были изменены
        adsr.reset();
        adsr.setParameters(adsrParams);
        paramsDirty = false;
    }

    adsr.noteOn(); // Запуск атаки ADSR
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
    if (allowTailOff) {
        adsr.noteOff(); // Плавное затухание
    } else {
        clearCurrentNote(); // Мгновенная остановка
    }
}

void SynthVoice::setCurrentPlaybackSampleRate(double newSampleRate) {
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newSampleRate);

    for (auto& o : oscillators) {
        o.prepare(newSampleRate); // Передача нового sample rate осцилляторам
    }
}

// Генерация следующего блока аудио данных
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples) {
    if (!isVoiceActive()) // Если голос не активен — пропускаем
        return;

    for (int s = 0; s < numSamples; ++s) {
        float env = adsr.getNextSample(); // Получение текущего значения ADSR огибающей
        float cursamp = 0.f;

        for (auto& osc : oscillators) {
            cursamp += osc.nextSample(); // Суммирование сигналов всех осцилляторов
        }

        // Применение уровня velocity, огибающей и нормализация по количеству осцилляторов
        cursamp = env * cursamp * level / numOsc;
        
        // Запись значения в каждый канал аудиовыхода
        for (int ch = 0; ch < output.getNumChannels(); ++ch) {
            output.addSample(ch, startSample + s, cursamp);
        }
    }

    // Если ADSR закончилась — остановить голос
    if (!adsr.isActive()) {
        clearCurrentNote();
    }
}

// Установка новых параметров ADSR и пометка их как изменённых
void SynthVoice::setAttack(float value) {
    adsrParams.attack = value;
    paramsDirty = true;
}

void SynthVoice::setDecay(float value) {
    adsrParams.decay = value;
    paramsDirty = true;
}

void SynthVoice::setSustain(float value) {
    adsrParams.sustain = value;
    paramsDirty = true;
}

void SynthVoice::setRelease(float value) {
    adsrParams.release = value;
    paramsDirty = true;
}
