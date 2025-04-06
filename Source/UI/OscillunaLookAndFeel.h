#pragma once

#include <JuceHeader.h>

/** Класс, отвечающий за цвета приложения и отрисовку некоторых базовых компонентов.*/
class OscillunaLookAndFeel : public juce::LookAndFeel_V4 {
public:
    OscillunaLookAndFeel();
    
    /**Отрисовка ручки (поворотный регулятор).*/
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override;
    
    /**Отрисовка слайдера (ползунка).*/
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                          float maxSliderPos, juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;
};
