#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"
#include <vector>
#include <cmath>

/**
 Многоголосный осциллятор, до 16 голосов Oscillator на один осциллятор
*/
class UnisonOscillator
{
public:
    // Конструктор по умолчанию создаёт 1 голос
    UnisonOscillator() { setVoices(1); }

    // Инициализация sample rate и голосов
    void prepare(double newSampleRate);

    void setMode(Oscillator::OscillatorMode m); // Установка режима волны
    void setBaseFreq(double hz); // Базовая частота в герцах
    void setPitchSemi(int semis); // Смещение в полутонах (0–48)
    void setVoices(int v); // Количество голосов (1–16)
    void setDetune(float cents); // Детюн в центах между парами
    void setMainLevel(float g); // Громкость центральных голосов
    void setSideLevel(float g); // Громкость боковых голосов
    void setOscLevel(float g); // Общий уровень осциллятора

    void resetPhases(); // Сброс фаз всех голосов
    float nextSample(); // Генерация следующего сэмпла (смешение всех голосов)

    // Геттеры для текущих параметров
    Oscillator::OscillatorMode getMode() const { return mode; }
    int getPitchSemi() const { return pitchSemi; }
    int getVoices() const { return numVoices; }
    float getDetune() const { return detuneCent; }
    float getMainLevel() const { return mainLevel; }
    float getSideLevel() const { return sideLevel; }
    float getOscLevel() const { return oscLevel; }

private:
    void updateFrequencies(); // Пересчёт частот для всех голосов с учётом детюна и смещения
    static inline double centsToRatio(float c) { return std::pow(2.0, c / 1200.0); } // Перевод центов в частотное отношение

    std::vector<Oscillator> voices; // Массив голосов (реальных осцилляторов)
    double sampleRate { 44100.0 }; // Частота дискретизации

    Oscillator::OscillatorMode mode = Oscillator::OscillatorMode::Sine; // Текущий режим волны
    double baseFreq { 440.0 }; // Базовая частота
    int pitchSemi { 0 }; // Смещение в полутонах
    int numVoices { 1 }; // Количество голосов
    float detuneCent { 5.f }; // Детюн между голосами в центах
    float mainLevel { 1.f }; // Уровень центральных голосов
    float sideLevel { 0.5f }; // Уровень боковых голосов
    float oscLevel { 1.0f }; // Общий уровень осциллятора
};
