#pragma once

#include <JuceHeader.h>

/** Класс генератора осциллятора с поддержкой PolyBLEP антиалиасинга */
class Oscillator {
public:
    // Перечисление режимов осциллятора
    enum OscillatorMode {
        Sine = 0, // Синус
        Saw, // Пила
        Square, // Меандр (квадрат)
        Triangle, // Треугольник
        Noise, // Шум
        NumOscillatorModes
    };
    
    // Установка формы генерации волны
    void setMode(OscillatorMode mode);
    
    // Установка базовой частоты
    void setFrequency(float frequency);
    
    // Установка частоты дискретизации
    void setSampleRate(float sampleRate);
    
    // Установка флага отключения генерации
    inline void setMuted(bool muted) { isMuted = muted; }
    
    // Генерация наивной формы волны (без PolyBLEP) для выбранного режима
    float naiveWaveformForMode(OscillatorMode oscMode);
    
    // Генерация следующего сэмпла
    float nextSample();
    
    // PolyBLEP коррекция для устранения алиасинга
    float polyBLEP(float t);
    
    // Конструктор осциллятора
    Oscillator(float sampleRate = 44100.0f);
    
    // Сброс текущего положения фазы
    void reset() { angle = 0.0; }
    
    // Установка модуляции высоты тона
    void setPitchMod(float amount);
    
private:
    // Пересчёт приращения угла при изменении параметров
    void updateAngle();
    
    float sampleRate; // Частота дискретизации
    OscillatorMode mode; // Текущий режим осциллятора
    bool isMuted; // Флаг отключения генерации
    float frequency; // Базовая частота
    float angle; // Текущее положение фазы
    float angleDelta; // Приращение фазы на сэмпл
    float pitchMod; // Модуляция высоты тона
    float lastOutput; // Последнее значение вывода (для триангулярной волны)
    
    juce::Random random; // Генератор случайных чисел для шума
};
