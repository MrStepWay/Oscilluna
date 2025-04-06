#pragma once

#include <JuceHeader.h>

/**
 Компонент, отображающий несколько слайдеров, отвечающих за настройку ADSR огибающий (Envelope)
 */
class EnvelopeComponent : public juce::Component,
                          public juce::Slider::Listener // Слушатель для слайдеров
{
public:
    // Тип колбэка для уведомления об изменениях
    using FloatCallback = std::function<void(float)>;
    
    EnvelopeComponent(
        FloatCallback attackChangedCallback,
        FloatCallback decayChangedCallback,
        FloatCallback sustainChangedCallback,
        FloatCallback releaseChangedCallback,
        float initialAttack = 0.1f,
        float initialDecay = 0.1f,
        float initialSustain = 1.f,
        float initialRelease = 0.1f
    );
    
    ~EnvelopeComponent() override;
    
    // Методы juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Метод juce::Slider::Listener
    void sliderValueChanged(juce::Slider* slider) override;

    // Установка значения слайдеров
    void setAttackSliderValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setDecaySliderValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setSustainSliderValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setReleaseSliderValue(float value, juce::NotificationType notification = juce::sendNotification);
private:
    // Колбэки
    FloatCallback onAttackChanged;
    FloatCallback onDecayChanged;
    FloatCallback onSustainChanged;
    FloatCallback onReleaseChanged;
    
    // Слайдеры
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    
    // Заголовок
    juce::Label titleLabel { {}, "Envelope" };
    
    // Лейблы (вертикальные)
    juce::String attackLabelText = "Attack";
    juce::String decayLabelText = "Decay";
    juce::String sustainLabelText = "Sustain";
    juce::String releaseLabelText = "Release";
    
    // Области для отрисовки вертикальных лейблов (вычисляются в resized)
    juce::Rectangle<int> attackLabelArea;
    juce::Rectangle<int> decayLabelArea;
    juce::Rectangle<int> sustainLabelArea;
    juce::Rectangle<int> releaseLabelArea;
    
    // Цвета
    juce::Colour backgroundColour = juce::Colour(240, 240, 240); // Светло-серый фон
    juce::Colour innerBackgroundColour = juce::Colour(217, 217, 217); // Cерый внутренний фон
    juce::Colour titleColour = juce::Colours::black;
    juce::Colour labelColour = juce::Colours::black;
    
    // Различные отступы
    float cornerSize = 20.f;
    float innerPadding = 10.f;
    int padding = 10; // Общие отступы
    int titleHeight = 20; // Высота заголовка
    int topMargin = 5; // Отступ после заголовка
    int internalPadding = 5; // Горизонтальный отступ внутри каждой секции ADSR
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeComponent)
};
