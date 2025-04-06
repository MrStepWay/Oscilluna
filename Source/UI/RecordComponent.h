#pragma once

#include <JuceHeader.h>

/**
 Компонент для записи проигранной мелодии.
 */
class RecordComponent : public juce::Component {
public:
    // Колбэк для уведомления о начале/остановке записи
    using RecordingStateChangedCallback = std::function<void(bool)>;
    
    RecordComponent(RecordingStateChangedCallback callback);
    
    // Методы juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** Сбрасывает визуальное состояние кнопки на "не записывается" */ 
    void resetRecordingStateVisuals();
    
private:
    RecordingStateChangedCallback onRecordingStateChanged; // Инкапсуляция колбэка для уведомления о начале/остановке записи
    bool isRecording = false; // Флаг состояния записи
    
    // Обработчик клика
   void recordButtonClicked();
    
    // UI элементы
    juce::Label titleLabel { {}, "Recording" };
    juce::TextButton recordButton { "Record" };
    
    // Стилизация
    juce::Colour titleColour = juce::Colours::black;
    juce::Colour backgroundColour = juce::Colour(217, 217, 217);
    juce::Colour recordingColour = juce::Colours::red;
    float cornerSize = 15.0f;
    float padding = 5.f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordComponent);
};
