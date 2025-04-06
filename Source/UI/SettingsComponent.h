#pragma once

#include <JuceHeader.h>
#include "DeviceSelectorComponent.h"
#include "RecordComponent.h"
#include "PresetSelectorComponent.h"

/**
 Компонент, содержащий в себе некоторые настройки синтезатора: выбор MIDI-устройства, выходного аудио устройства. Управление пресетами и запись аудио.
 */
class SettingsComponent : public juce::Component {
public:
    using RecordingStateChangedCallback = std::function<void(bool)>; // Тип колбэка для записи
    using PresetActionCallback = std::function<void()>; // Тип колбэка для пресета
    
    // Конструктор принимает все необходимые объекты для работы внутренних компонентов
    SettingsComponent(juce::AudioDeviceManager& dm,
                      juce::MidiMessageCollector* collector,
                      RecordingStateChangedCallback recordingCallback,
                      PresetActionCallback savePresetCallback,
                      PresetActionCallback loadPresetCallback);
    
    // Методы juce::Conponent
    void paint(juce::Graphics& g) override;
    void resized() override;

    /**Сброс визуального состояния кнопки записи аудио*/
    void resetRecordingStateVisuals();
    
    /**Установка имени пресета**/
    void setPresetName(const juce::String& name);

private:
    // Сохраняем колбэки
    PresetActionCallback onSaveRequest;
    PresetActionCallback onLoadRequest;
    
    // Внутренние компоненты
    DeviceSelectorComponent deviceSelector;
    RecordComponent recordComponent;
    PresetSelectorComponent presetSelector;
    
    // Стилизация
    float cornerSize = 20.f;
    float padding = 10.f;
    juce::Colour backgroundColour = juce::Colour(240, 240, 240);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent);
};
