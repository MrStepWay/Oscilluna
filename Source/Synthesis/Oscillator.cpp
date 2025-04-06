#include "Oscillator.h"

// Конструктор с начальной инициализацией параметров
Oscillator::Oscillator(float sampleRate)
    : sampleRate(sampleRate), mode(Sine), isMuted(true), frequency(440.0f), angle(0.0f), pitchMod(0.0f), lastOutput(0.0f) {
    updateAngle(); // Пересчёт angleDelta при создании объекта
};

void Oscillator::setMode(OscillatorMode newMode) {
    mode = newMode;
}

void Oscillator::setFrequency(float newFrequency) {
    frequency = newFrequency;
    updateAngle(); // Обновляем при изменении частоты
}

void Oscillator::setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
    updateAngle(); // Обновляем при изменении частоты дискретизации
}

// Генерация наивной формы волны
float Oscillator::naiveWaveformForMode(OscillatorMode oscMode) {
    float value = 0.0f;
    
    switch (oscMode) {
        case Sine:
            value = std::sin(angle); // Чистая синусоида
            break;
        case Saw:
            value = (2.0f * angle / juce::MathConstants<float>::twoPi) - 1.0f; // Линейная пила
            break;
        case Square:
            if (angle <= juce::MathConstants<float>::pi) {
                value = 1.0;
            } else {
                value = -1.0;
            }
            break;
        case Triangle:
            value = -1.0 + angle / juce::MathConstants<float>::pi;
            value = 2.0 * (std::fabs(value) - 0.5); // Преобразование для треугольной волны
            break;
    }
    return value;
}

// Генерация следующего аудио-сэмпла с учётом PolyBLEP коррекции
float Oscillator::nextSample() {
    if (mode == Noise) {
        return random.nextFloat() * 2.0f - 1.0f; // Генерация белого шума
    }
    
    double value = 0.0;
    double t = angle / juce::MathConstants<float>::twoPi; // Нормализация фазы в диапазоне [0, 1]

    if (mode == Sine) {
        value = naiveWaveformForMode(Sine);
    } else if (mode == Saw) {
        value = naiveWaveformForMode(Saw);
        value -= polyBLEP(t); // PolyBLEP коррекция перехода пилы
    } else {
        value = naiveWaveformForMode(Square);
        value += polyBLEP(t); // PolyBLEP коррекция фронта квадратной волны
        value -= polyBLEP(std::fmod(t + 0.5, 1.0)); // PolyBLEP коррекция заднего фронта

        if (mode == Triangle) {
            // Интеграция квадратной волны для получения триангулярной волны
            value = angleDelta * value + (1 - angleDelta) * lastOutput;
            lastOutput = value;
        }
    }

    // Обновляем фазу
    angle += angleDelta;
    while (angle >= juce::MathConstants<float>::twoPi) {
        angle -= juce::MathConstants<float>::twoPi; // Зацикливаем фазу
    }
    return value;
}

// Функция PolyBLEP для устранения алиасинга
float Oscillator::polyBLEP(float t)
{
    float dt = angleDelta / juce::MathConstants<float>::twoPi; // Нормализованная скорость изменения фазы
    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1.0; // Коррекция для начала перехода
    } else if (t > 1.0 - dt) {
        t = (t - 1.0) / dt;
        return t * t + t + t + 1.0; // Коррекция для конца перехода
    }
    return 0.0; // Вне зоны перехода — без изменений
}

// Установка модуляции высоты тона
void Oscillator::setPitchMod(float amount) {
    pitchMod = amount;
    updateAngle(); // Пересчёт скорости фазы
}

// Пересчёт angleDelta с учётом модуляции высоты тона
void Oscillator::updateAngle() {
    float pitchModAsFrequency = std::pow(2.0, std::fabs(pitchMod) * 14.0) - 1;
    if (pitchMod < 0) {
        pitchModAsFrequency = -pitchModAsFrequency;
    }
    float calculatedFrequency = std::fmin(std::fmax(frequency + pitchModAsFrequency, 0), sampleRate/2.0);
    angleDelta = calculatedFrequency * juce::MathConstants<float>::twoPi / sampleRate; // Вычисляем приращение угла
}
