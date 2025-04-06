#include "UnisonOscillator.h"

void UnisonOscillator::prepare(double newSampleRate) {
    sampleRate = newSampleRate;

    if (voices.size() != static_cast<size_t>(numVoices)) {
        voices.resize(numVoices); // Ресайз массива голосов под нужное количество
    }

    for (auto& v : voices) {
        v.setSampleRate(sampleRate);
        v.setMode(mode);
    }
    updateFrequencies(); // Пересчёт частот для каждого голоса
}

// Установка типа волны
void UnisonOscillator::setMode(Oscillator::OscillatorMode m) {
    mode = m;
    for (auto& v : voices) {
        v.setMode(m);
    }
}

// Установка базовой частоты
void UnisonOscillator::setBaseFreq(double hz) {
    baseFreq = hz;
    updateFrequencies();
}

// Установка смещения в полутонах (ограничение 0–48)
void UnisonOscillator::setPitchSemi(int semis) {
    pitchSemi = juce::jlimit(0, 48, semis);
    updateFrequencies();
}

// Установка количества голосов (ограничение 1–16)
void UnisonOscillator::setVoices(int v) {
    numVoices = juce::jlimit(1, 16, v);
    voices.resize(numVoices); // Ресайз массива
    for (auto& vosc : voices) {
        vosc.setSampleRate(sampleRate);
        vosc.setMode(mode);
    }
    updateFrequencies();
}

// Установка детюна между голосами
void UnisonOscillator::setDetune(float c) {
    detuneCent = c;
    updateFrequencies();
}

// Установка громкости центральных голосов
void UnisonOscillator::setMainLevel(float g) {
    mainLevel = g;
}

// Установка громкости боковых голосов
void UnisonOscillator::setSideLevel(float g) {
    sideLevel = g;
}

// Установка общего уровня осциллятора
void UnisonOscillator::setOscLevel(float g) {
    oscLevel = g;
}

// Пересчёт частот всех голосов с учётом детюна и смещения
void UnisonOscillator::updateFrequencies() {
    if (voices.empty()) {
        return;
    }

    const double pitchRatio = std::pow(2.0, pitchSemi / 12.0); // Преобразование полутонов в частотное соотношение
    const double f0 = baseFreq * pitchRatio; // Итоговая базовая частота с учётом смещения

    int idx = 0;

    // Центральный(-ые) голос(-а)
    if (numVoices % 2 == 0) {
        const double r = centsToRatio(detuneCent * 0.5f);
        voices[idx++].setFrequency(f0 * r);
        voices[idx++].setFrequency(f0 / r);
    } else {
        voices[idx++].setFrequency(f0);
    }

    // Боковые голоса
    for (int p = 1; idx < numVoices; ++p) {
        const double r = centsToRatio(detuneCent * p);
        voices[idx++].setFrequency(f0 * r);
        if (idx < numVoices) {
            voices[idx++].setFrequency(f0 / r);
        }
    }
}

// Сброс фаз всех голосов
void UnisonOscillator::resetPhases() {
    for (auto& v : voices) {
        v.reset();
    }
}

// Генерация следующего сэмпла с микшированием всех голосов
float UnisonOscillator::nextSample() {
    if (voices.empty()) {
        return 0.0f;
    }

    // Нормализация амплитуды по количеству голосов
    const float norm = 1.f / std::sqrt(static_cast<float>(numVoices));
    const bool even = (numVoices % 2 == 0);
    const float gainMain = mainLevel * oscLevel * norm; // Усиление центральных голосов
    const float gainSide = sideLevel * oscLevel * norm; // Усиление боковых голосов

    float sum = 0.f;
    for (int i = 0; i < numVoices; ++i) {
        const float g = (even ? (i < 2) : (i == 0)) ? gainMain : gainSide;
        sum += g * static_cast<float>(voices[i].nextSample());
    }
    
    return sum;
}
