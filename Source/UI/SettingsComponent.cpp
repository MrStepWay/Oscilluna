#include "SettingsComponent.h"

SettingsComponent::SettingsComponent(juce::AudioDeviceManager& dm,
                                     juce::MidiMessageCollector* collector,
                                     RecordingStateChangedCallback recordCallback,
                                     PresetActionCallback savePresetCallback,
                                     PresetActionCallback loadPresetCallback)
: onSaveRequest(std::move(savePresetCallback)),
onLoadRequest(std::move(loadPresetCallback)),
deviceSelector(dm, collector),
recordComponent(recordCallback),
presetSelector([this]() { if(onSaveRequest) onSaveRequest(); }, [this]() { if(onLoadRequest) onLoadRequest(); })
{
    addAndMakeVisible(deviceSelector);
    addAndMakeVisible(recordComponent);
    addAndMakeVisible(presetSelector);
}


void SettingsComponent::paint(juce::Graphics& g) {
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
}

void SettingsComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();
    bounds.reduce(padding, padding);
    
    juce::Rectangle<int> bottomArea = bounds.removeFromBottom(bounds.getHeight() / 2);
    bottomArea.removeFromTop(padding / 2);
    deviceSelector.setBounds(bottomArea); // Выбор аудио устройств снизу
    
    juce::Rectangle<int> topRightArea = bounds.removeFromRight(bounds.getWidth() / 2);
    topRightArea.removeFromLeft(padding / 2);
    topRightArea.removeFromBottom(padding / 2);
    recordComponent.setBounds(topRightArea); // Запись аудио в правом верхнем углу
    
    bounds.removeFromRight(padding / 2);
    bounds.removeFromBottom(padding / 2);
    presetSelector.setBounds(bounds); // Пресеты в левом верхнем углу
}

void SettingsComponent::resetRecordingStateVisuals() {
    recordComponent.resetRecordingStateVisuals();
}

void SettingsComponent::setPresetName(const juce::String& name) {
    presetSelector.setCurrentPresetName(name);
}
