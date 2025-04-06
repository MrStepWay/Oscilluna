#include "VolumeDialComponent.h"


VolumeDialComponent::VolumeDialComponent(FloatCallback valueChangedCallback, float initialValue)
    : onValueChanged(std::move(valueChangedCallback)) {
    // Заголовок (лейбл)
    titleLabel.setFont(juce::Font (20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);

    // Слайдер
    volumeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeSlider.setRange(0.0, 1.0, 0.01); // Диапазон громкости 0-1
    volumeSlider.setValue(initialValue, juce::sendNotification); // Устанавливаем начальное значение
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.addListener(this); // Регистрируем слушателя изменений
    addAndMakeVisible(volumeSlider);
}

VolumeDialComponent::~VolumeDialComponent() {
    // Отписываемся от слушателя
    volumeSlider.removeListener(this);
}

void VolumeDialComponent::paint (juce::Graphics& g) {
    // Фон
    auto bounds = getLocalBounds().toFloat();
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
}

void VolumeDialComponent::resized() {
    auto bounds = getLocalBounds();

    bounds.removeFromTop(padding);

    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding);
    volumeSlider.setBounds(bounds);
}

void VolumeDialComponent::sliderValueChanged (juce::Slider* slider) {
    // Если изменился слайдер и колбэк задан, вызываем его
    if (slider == &volumeSlider && onValueChanged) {
        onValueChanged(volumeSlider.getValue());
    }
}

// Методы для управления слайдером
void VolumeDialComponent::setValue(float newValue, juce::NotificationType notification) {
    volumeSlider.setValue(newValue, notification);
}

float VolumeDialComponent::getValue() const {
    return volumeSlider.getValue();
}

void VolumeDialComponent::setRange(double newMinimum, double newMaximum, double newInterval) {
    volumeSlider.setRange(newMinimum, newMaximum, newInterval);
}
