#include "RecordComponent.h"

RecordComponent::RecordComponent(RecordingStateChangedCallback callback) : onRecordingStateChanged(std::move(callback)) // Сохраняем колбэк
{
    // Заголовок (лейбл)
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);
    
    // Кнопка записи
    recordButton.onClick = [this] { recordButtonClicked(); }; // Привязываем обработчик
    recordButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId)); // Цвет по умолчанию
    addAndMakeVisible(recordButton);
}

void RecordComponent::recordButtonClicked() {
    // Инвертируем состояние записи
    isRecording = !isRecording;

    if (isRecording) {
        recordButton.setButtonText("Stop");
        recordButton.setColour(juce::TextButton::buttonColourId, recordingColour); // Меняем цвет кнопки
        recordButton.repaint(); // Перерисовать кнопку
    } else {
        recordButton.setButtonText("Record");
        recordButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId)); // Возвращаем стандартный цвет кнопки
        recordButton.repaint(); // Перерисовать кнопку
    }

    // Уведомляем об изменении состояния
    if (onRecordingStateChanged) {
        onRecordingStateChanged(isRecording);
    }
}

void RecordComponent::paint(juce::Graphics& g) {
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
}

void RecordComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();
    
    bounds.reduce(padding, padding);
    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding);
    
    bounds.reduce(bounds.getWidth() / 5, bounds.getHeight() / 3);
    recordButton.setBounds(bounds);
}

void RecordComponent::resetRecordingStateVisuals() {
    if (!isRecording) {
        return;
    }
    isRecording = false; // Обновляем внутреннее состояние
    recordButton.setButtonText("Record");
    // Возвращаем цвет.
    recordButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
    recordButton.repaint();
}
