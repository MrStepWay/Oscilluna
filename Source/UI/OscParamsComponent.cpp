#include "OscParamsComponent.h"

void OscParamsComponent::handleVoicesChanged(int count) {
    if (onVoices) {
        onVoices(count);
    }
}

void OscParamsComponent::handleDetuneChanged(float value) {
    if (onDetune) {
        onDetune(value);
    }
}

void OscParamsComponent::handleMainVolChanged(float value) {
    if (onMain) {
        onMain(value);
    }
}

void OscParamsComponent::handleSideVolChanged(float value) {
    if (onSide) {
        onSide(value);
    }
}

void OscParamsComponent::handleOscVolumeChanged(float value) {
    if (onVolume) {
        onVolume(value);
    }
}

void OscParamsComponent::handleOscPitchChanged(float value) {
    if (onPitch) {
        onPitch(value);
    }
}

void OscParamsComponent::handleShapeChanged(int choice) {
    if (onShape) {
        onShape(choice);
    }
}

OscParamsComponent::OscParamsComponent() :
volumeDial([this](float v){ handleOscVolumeChanged(v); }),
pitchDial([this](float v) {handleOscPitchChanged(v); }),
shapeSelectorComponent([this](int v){ handleShapeChanged(v); }, 0),
unisonSettings([this](int v){ handleVoicesChanged(v); }, // Лямбда для колбэка голосов
               [this](float v){ handleDetuneChanged(v); }, // Лямбда для колбэка расстройки
               [this](float v){ handleMainVolChanged(v); }, // Лямбда для колбэка громкости основного голоса(-ов)
               [this](float v){ handleSideVolChanged(v); }) // Лямбда для колбёка сторонних голосов
{
    addAndMakeVisible(volumeDial);
    addAndMakeVisible(pitchDial);
    addAndMakeVisible(shapeSelectorComponent);
    addAndMakeVisible(unisonSettings);
}

void OscParamsComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colour(240, 240, 240));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);
}

void OscParamsComponent::resized() {
    juce::Rectangle<int> area = getLocalBounds();
    int borderPadding = 10;
    int innerPadding = borderPadding / 2;
    
    area.reduce(innerPadding, borderPadding);
    
    volumeDial.setBounds(area.removeFromLeft(area.getWidth() / 5).reduced(innerPadding, 0));
    pitchDial.setBounds(area.removeFromLeft(area.getWidth() / 4).reduced(innerPadding, 0));
    shapeSelectorComponent.setBounds(area.removeFromLeft(area.getWidth() / 2).reduced(innerPadding, 0));
    unisonSettings.setBounds(area.reduced(innerPadding, 0));
}

// Публичные сеттеры
void OscParamsComponent::setVolumeValue(float value, juce::NotificationType notification) {
    volumeDial.setValue(value, notification);
}

void OscParamsComponent::setPitchValue(int value, juce::NotificationType notification) {
    pitchDial.setValue(value, notification);
}

void OscParamsComponent::setShapeIndex(int index, juce::NotificationType notification) {
    shapeSelectorComponent.setSelectedIndex(index, notification);
}

// Делегируем вызовы сеттерам в UnisonSettingsComponent
void OscParamsComponent::setUnisonVoicesValue(int value, juce::NotificationType notification) {
    unisonSettings.setVoicesValue(value, notification);
}

void OscParamsComponent::setUnisonDetuneValue(float value, juce::NotificationType notification) {
    unisonSettings.setDetuneSliderValue(value, notification);
}

void OscParamsComponent::setUnisonMainVolValue(float value, juce::NotificationType notification) {
    unisonSettings.setMainVolSliderValue(value, notification);
}

void OscParamsComponent::setUnisonSideVolValue(float value, juce::NotificationType notification) {
    unisonSettings.setSideVolSliderValue(value, notification);
}
