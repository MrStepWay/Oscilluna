#pragma once

#include <JuceHeader.h>

// Перечисление для форм волны
enum class OscillatorShape
{
    Sine = 0,
    Saw,
    Triangle,
    Square,
    Noise
};

/**
 Компонент для выбора формы волны осциллятора с помощью TextButton, с заголовком "Shape" и фоном.
 */
class ShapeSelectorComponent : public juce::Component,
                               public juce::Button::Listener
{
public:
    // Тип колбэка для обработки выбранной кнопки
    using ShapeChangedCallback = std::function<void(int)>;
    
    ShapeSelectorComponent(ShapeChangedCallback callback, int initialShapeIndex = 0);
    ~ShapeSelectorComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
                                   
    /**Обработка нажатия кнопки*/
    void buttonClicked(juce::Button* button) override;
    
    /**Установка выбранного индекса (формы волны)*/
    void setSelectedIndex(int index, juce::NotificationType notification = juce::sendNotification);

private:
    // Храним колбэк
    ShapeChangedCallback onShapeChanged;

    // UI элементы
    juce::Label titleLabel { {}, "Shape" };
    juce::TextButton sineButton { "Sine" };
    juce::TextButton sawButton { "Saw" };
    juce::TextButton triangleButton { "Triangle" };
    juce::TextButton squareButton { "Square" };
    juce::TextButton noiseButton { "Noise" };

    // Вектор указателей на кнопки
    std::vector<juce::TextButton*> shapeButtons;

    // Стилизация
    juce::Colour backgroundColour = juce::Colour(217, 217, 217); // Cерый фон
    juce::Colour titleColour = juce::Colours::black;
    float cornerSize = 15.0f; // Радиус скругления фона
    int padding = 5;
    float buttonHPadding = 2.5f;

    int currentSelectedIndex = -1;

    // Массив названий для кнопок
    const juce::StringArray shapeNames = { "Sine", "Saw", "Triangle", "Square", "Noise" };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeSelectorComponent)
};
