#pragma once

#include <JuceHeader.h>

/**
    Компонент для выбора устройств MIDI входа и аудио выхода
*/
class DeviceSelectorComponent  : public juce::Component, public juce::ChangeListener {
public:
    DeviceSelectorComponent(juce::AudioDeviceManager& deviceManager, juce::MidiMessageCollector* collector);
    ~DeviceSelectorComponent() override;

    // Методы juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
private:
    // Взаимодействие с устройствами
    juce::AudioDeviceManager& audioDeviceManager;
    juce::MidiMessageCollector* midiCollector;
    
    int lastMidiInputIndex = -1;

    // UI Элементы
    juce::Label titleLabel { {}, "Devices" };
    juce::Label midiInputLabel { {}, "MIDI" };
    juce::Label audioOutputLabel { {}, "Output" };
    juce::ComboBox midiInputComboBox;
    juce::ComboBox audioOutputComboBox;
    juce::TextButton refreshButton { "Refresh" };

    // Стилизация
    juce::Colour backgroundColour = juce::Colour(217, 217, 217);
    juce::Colour titleColour = juce::Colours::black;
    juce::Colour labelColour = juce::Colours::black;
    float cornerSize = 15.0f;
    float padding = 5.f;

    // Вспомогательные функции
    void populateMidiInputList();
    void populateAudioOutputList();
    void refreshDeviceLists();

    // Обработчики событий
    void midiInputChanged();
    void audioOutputChanged();
    void refreshClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSelectorComponent)
};
