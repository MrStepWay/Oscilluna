#pragma once

#include <JuceHeader.h>

/**
 Компонент, отображающий поворотный регулятор громкости (rotary slider)
 с заголовком "Vol" и скругленным фоном.
*/
class VolumeDialComponent : public juce::Component,
                            public juce::Slider::Listener
{
public:
    // Тип колбэка для уведомления об изменении значения
    using FloatCallback = std::function<void(float)>;

    // Конструктор принимает колбэк и начальное значение
    VolumeDialComponent(FloatCallback valueChangedCallback, float initialValue = 0.5f);
    ~VolumeDialComponent() override;

    // Методы juce::Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    // Метод juce::Slider::Listener
    void sliderValueChanged (juce::Slider* slider) override;

    // Методы для управления значением и диапазоном слайдера
    void setValue(float newValue, juce::NotificationType notification = juce::sendNotification);
    float getValue() const;
    void setRange(double newMinimum, double newMaximum, double newInterval = 0.0);
    
private:
    FloatCallback onValueChanged; // Колбэк

    // UI элементы
    juce::Label titleLabel { {}, "Vol" };
    juce::Slider volumeSlider;

    // Стилизация
    juce::Colour backgroundColour = juce::Colour(217, 217, 217); // Светло-серый фон
    juce::Colour titleColour = juce::Colours::black;
    float cornerSize = 15.f; // Радиус скругления фона
    int padding = 5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeDialComponent)
};
