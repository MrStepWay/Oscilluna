#pragma once

#include <JuceHeader.h>

/**
 Кастомный компонент для выбора числового значения с помощью стрелок.
*/
class ValueSelectorComponent  : public juce::Component {
public:
    // Callback при изменении значения
    using ValueChangedCallback = std::function<void(int)>;

    ValueSelectorComponent(ValueChangedCallback callback);
    ~ValueSelectorComponent() override = default;

    // Методы juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Метод для отслеживания нажатия мыши
    void mouseDown(const juce::MouseEvent& event) override;

    // Установка диапазона и текущего значения
    void setRange(int minValue, int maxValue, int interval = 1);
    void setValue(int newValue, juce::NotificationType notification = juce::sendNotification);
    int getValue() const;

private:
    ValueChangedCallback onValueChanged;

    int currentValue = 1;
    int minValue = 1;
    int maxValue = 16;
    int interval = 1;

    // Области для кликов по стрелкам 
    juce::Rectangle<int> upArrowArea;
    juce::Rectangle<int> downArrowArea;

    // Стилизация
    juce::Colour backgroundColour = juce::Colour(240, 240, 240);
    juce::Colour textColour = juce::Colours::black;
    juce::Colour arrowColour = juce::Colours::darkgrey;
    float cornerSize = 5.0f;
    float arrowAreaRatio = 0.35f; // Пропорция для области стрелок

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueSelectorComponent)
};
