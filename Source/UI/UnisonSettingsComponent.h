#pragma once

#include <JuceHeader.h>
#include "ValueSelectorComponent.h"

/**
 Компонент для настроек унисона (голоса, расстройка, громкость)
*/
class UnisonSettingsComponent : public juce::Component,
                                public juce::Slider::Listener // Слушатель для слайдеров
{
public:
    // Типы колбэков для уведомления об изменениях
    using IntCallback = std::function<void(int)>;
    using FloatCallback = std::function<void(float)>;

    // Конструктор принимает колбэки и начальные значения
    UnisonSettingsComponent(
        IntCallback voicesChangedCallback,
        FloatCallback detuneChangedCallback,
        FloatCallback mainVolChangedCallback,
        FloatCallback sideVolChangedCallback,
        int initialVoices = 1,
        float initialDetune = 0.0f,
        float initialMainVol = 0.8f,
        float initialSideVol = 0.5f
    );

    ~UnisonSettingsComponent() override;

    // Методы juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Метод juce::Slider::Listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Методы для внешней установки значений
    void setVoicesValue(int value, juce::NotificationType notification = juce::sendNotification);
    void setDetuneSliderValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setMainVolSliderValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setSideVolSliderValue(float value, juce::NotificationType notification = juce::sendNotification);

private:
    // Колбэки
    IntCallback onVoicesChanged;
    FloatCallback onDetuneChanged;
    FloatCallback onMainVolChanged;
    FloatCallback onSideVolChanged;

    // Основные компоненты
    juce::Label titleLabel { {}, "Unison" };
    juce::Label voicesLabel { {}, "Voices" };
    juce::Label detuneLabel { {}, "Detune" };
    
    // Вертикальные лейблы
    juce::String mainVolLabelText = "main vol";
    juce::String sideVolLabelText = "side vol";

    ValueSelectorComponent voicesSelector;
    juce::Slider detuneSlider;
    juce::Slider mainVolSlider;
    juce::Slider sideVolSlider;

    // Области для отрисовки вертикальных лейблов (вычисляются в resized)
    juce::Rectangle<int> mainVolLabelArea;
    juce::Rectangle<int> sideVolLabelArea;

    // Стилизация
    juce::Colour backgroundColour = juce::Colour(217, 217, 217); // Светло-серый фон
    juce::Colour titleColour = juce::Colours::black;
    juce::Colour labelColour = juce::Colours::black;
    int padding = 5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnisonSettingsComponent)
};
