#include "PitchDialComponent.h"

PitchDialComponent::PitchDialComponent(IntCallback valueChangedCallback, int initialValue)
    : onValueChanged(std::move(valueChangedCallback)) {
    // Заголовок (лейбл)
    titleLabel.setFont(juce::Font (20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    // Начальный текст установим после настройки слайдера
    addAndMakeVisible(titleLabel);

    // Слайдер
    pitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    // Устанавливаем дискретный диапазон 0-48 с шагом 1
    pitchSlider.setRange(0.0, 48.0, 1.0);
        
    // Устанавливаем начальное значение
    pitchSlider.setValue(initialValue, juce::dontSendNotification);
    pitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);

    // Обновляем текст заголовка с начальным значением
    updateLabelText(initialValue);
}

PitchDialComponent::~PitchDialComponent() {
    pitchSlider.removeListener(this);
}

void PitchDialComponent::paint(juce::Graphics& g) {
    // Рисуем фон
    auto bounds = getLocalBounds().toFloat();
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
}

void PitchDialComponent::resized() {
    // Расположение элементов точно такое же, как в VolumeDialComponent
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(padding);
    
    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding);
    pitchSlider.setBounds(bounds);
}

// Обрабатываем изменение слайдера (кругового регулятора)
void PitchDialComponent::sliderValueChanged(juce::Slider* slider) {
    if (slider == &pitchSlider) {
        // Получаем текущее значение
        int currentValue = static_cast<int>(pitchSlider.getValue());
        // Обновляем текст заголовка
        updateLabelText(currentValue);
        
        // Вызываем колбэк, если он задан
        if (onValueChanged) {
            onValueChanged(currentValue);
        }
    }
}

// Вспомогательная функция для обновления текста заголовка
void PitchDialComponent::updateLabelText(int value) {
    juce::String labelText = "Pitch";
    // Добавляем "+N", если значение не 0
    if (value != 0) {
        // Показываем знак + только для положительных значений
        if (value > 0) {
            labelText << " +"; // Пробел перед знаком
        } else {
            labelText << " "; // Просто пробел для отрицательных
        }
        labelText << juce::String(value);
    }
    
    // Устанавливаем текст
    titleLabel.setText(labelText, juce::dontSendNotification);
}


// Методы для управления слайдером
void PitchDialComponent::setValue(int newValue, juce::NotificationType notification) {
    // Получаем текущее значение перед установкой
    int currentValue = static_cast<int>(pitchSlider.getValue());
    
    // Устанавливаем новое значение на слайдере
    pitchSlider.setValue(newValue, notification);

    // Обновляем текст лейбла, если значение изменилось,
    int updatedValue = static_cast<int>(pitchSlider.getValue());
    if (currentValue != updatedValue || notification != juce::sendNotification) {
        updateLabelText(updatedValue);
    } else if (notification == juce::sendNotificationSync) {
         updateLabelText(updatedValue);
    }
}


int PitchDialComponent::getValue() const {
    return static_cast<int>(pitchSlider.getValue());
}

void PitchDialComponent::setRange(int newMinimum, int newMaximum, int newInterval)
{
    // Устанавливаем диапазон, используя double, но с целочисленным интервалом
    pitchSlider.setRange(static_cast<double>(newMinimum),
                         static_cast<double>(newMaximum),
                         static_cast<double>(juce::jmax(1, newInterval))); // Интервал минимум 1
    // Обновляем лейбл на случай, если текущее значение изменилось из-за нового диапазона
    updateLabelText(getValue());
}
