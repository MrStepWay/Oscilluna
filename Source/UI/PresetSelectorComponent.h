#pragma once

#include <JuceHeader.h>

class PresetSelectorComponent : public juce::Component {
public:
    // Тип колбэка для кнопок
    using PresetActionCallback = std::function<void()>;
    
    PresetSelectorComponent(PresetActionCallback saveCallback, PresetActionCallback loadCallback);
    
    // Методы juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Метод для установки названия пресета
    void setCurrentPresetName(const juce::String& name);

private:
    // Колбэки кнопок сохранения и загрузки пресета
    PresetActionCallback onSave;
    PresetActionCallback onLoad;
    
    // UI элементы
    juce::Label titleLabel { {}, "Preset" };
    juce::Label presetlabel { {}, {"Init Preset"} };
    juce::TextButton saveButton { "Save" };
    juce::TextButton loadButton { "Load" };
    juce::Rectangle<int> presetArea;
    
    // Стилизация
    juce::Colour titleColour = juce::Colours::black;
    juce::Colour backgroundColour = juce::Colour(217, 217, 217);
    juce::Colour presetAreaColour = juce::Colour(240, 240, 240);
    float cornerSize = 15.0f;
    float padding = 5.f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetSelectorComponent);
};
