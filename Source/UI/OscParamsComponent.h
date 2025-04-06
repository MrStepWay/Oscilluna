#pragma once

#include <JuceHeader.h>
#include "VolumeDialComponent.h"
#include "ShapeSelectorComponent.h"
#include "ValueSelectorComponent.h"
#include "UnisonSettingsComponent.h"
#include "PitchDialComponent.h"

/**
 Компонент, содержащий в себе все настройки для одного многоголосного осциллятора.
 */
class OscParamsComponent : public juce::Component {
public:
    OscParamsComponent();
    
    // Методы juce::Comonent
    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Методы для вызова колбэков
    void handleOscVolumeChanged(float value);
    void handleOscPitchChanged(float value);
    void handleVoicesChanged(int count);
    void handleDetuneChanged(float value);
    void handleMainVolChanged(float value);
    void handleSideVolChanged(float value);
    void handleShapeChanged(int choice);
    
    // Хранение колбэков
    std::function<void(float)> onVolume;
    std::function<void(float)> onPitch;
    std::function<void(int)> onVoices;
    std::function<void(float)> onDetune;
    std::function<void(float)> onMain;
    std::function<void(float)> onSide;
    std::function<void(int)> onShape;
    
    // Сеттеры для обновления UI
    void setVolumeValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setPitchValue(int value, juce::NotificationType notification = juce::sendNotification);
    void setShapeIndex(int index, juce::NotificationType notification = juce::sendNotification);
    void setUnisonVoicesValue(int value, juce::NotificationType notification = juce::sendNotification);
    void setUnisonDetuneValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setUnisonMainVolValue(float value, juce::NotificationType notification = juce::sendNotification);
    void setUnisonSideVolValue(float value, juce::NotificationType notification = juce::sendNotification);
private:
    // Внутренние компоненты
    VolumeDialComponent volumeDial; // Компонент для изменения громкости всего осциллятора
    PitchDialComponent pitchDial; // Компонент для изменения частоты (в полутонах) всего осциллятора
    ShapeSelectorComponent shapeSelectorComponent; // Компонент для выбора типа волны
    UnisonSettingsComponent unisonSettings; // Компонент для настройки многоголосия
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscParamsComponent)
};
