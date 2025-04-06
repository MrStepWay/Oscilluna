#pragma once

#include <JuceHeader.h>

/**
 Компонент, отображающий поворотный регулятор высоты тона (Pitch).
 Использует дискретный шаг в полутонах.
*/
class PitchDialComponent  : public juce::Component,
                            public juce::Slider::Listener
{
public:
    // Тип колбэка для уведомления об изменении значения
    using IntCallback = std::function<void(int)>;

    // Конструктор принимает колбэк и начальное значение (int)
    PitchDialComponent(IntCallback valueChangedCallback, int initialValue = 0);
    ~PitchDialComponent() override;

    // Методы juce::Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    // Метод juce::Slider::Listener
    void sliderValueChanged (juce::Slider* slider) override;

    // Методы для управления значением и диапазоном слайдера извне
    void setValue(int newValue, juce::NotificationType notification = juce::sendNotification);
    int getValue() const;
    // Задание диапазона
    void setRange(int newMinimum, int newMaximum, int newInterval = 1);

private:
    IntCallback onValueChanged; // Колбэк

    // UI элементы
    juce::Label titleLabel { {}, "Pitch" };
    juce::Slider pitchSlider;

    // Стилизация
    juce::Colour backgroundColour = juce::Colour(217, 217, 217);
    juce::Colour titleColour = juce::Colours::black;
    float cornerSize = 15.0f;
    int padding = 5;

    // Вспомогательная функция для обновления текста заголовка
    void updateLabelText(int value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchDialComponent)
};
