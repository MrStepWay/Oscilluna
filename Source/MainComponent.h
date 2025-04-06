#pragma once

#include <JuceHeader.h>
#include "UI/OscillunaLookAndFeel.h"
#include "UI/OscParamsComponent.h"
#include "UI/EnvelopeComponent.h"
#include "UI/DeviceSelectorComponent.h"
#include "UI/SettingsComponent.h"
#include "Synthesis/SynthAudioSource.h"

class MainComponent : public juce::AudioAppComponent {
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setOscParam(int oscIndex, std::function<void(SynthVoice&)> setter);
    void forEachVoice(std::function<void(SynthVoice&)> fn);

private:
    // Хелперы для пресетов
    void savePreset();
    void loadPreset();
    juce::var getCurrentPresetData(); // Собирает данные с осцилляторов и envelope
    void applyPresetData(const juce::var& data); // Применяет данные к осцилляторам, envelope и UI
    
    // Хелперы для валидации JSON
    float getFloatVarOrDefault(const juce::var& parent, const juce::Identifier& key, float defaultValue);
    int getIntVarOrDefault(const juce::var& parent, const juce::Identifier& key, int defaultValue);
    
    // Запись аудио
    void handleRecordingStateChange(bool isNowRecording); // Обработчик колбэка записи аудио
    void processAudioForRecording(const juce::AudioSourceChannelInfo& bufferToFill); // Метод для обработки аудиоданных для записи
    std::unique_ptr<juce::AudioFormatWriter> audioWriter; // Писатель аудио формата (WAV, AIFF)
    juce::CriticalSection writerLock; // Блокировка для защиты доступа к audioWriter из разных потоков
    std::atomic<bool> isRecordingToFile { false }; // Флаг, что запись активна
    std::unique_ptr<juce::TemporaryFile> tempAudioFile; // Временный файл для записи
    
    OscillunaLookAndFeel lf;
    SynthAudioSource synthAudioSource;
    
    // Midi
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    
    // GUI
    // Осцилляторы
    OscParamsComponent oscParamsComponent1;
    OscParamsComponent oscParamsComponent2;
    OscParamsComponent oscParamsComponent3;
    
    // Envelope
    EnvelopeComponent envelopeComponent;
    
    // Settings
    SettingsComponent settingsComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
