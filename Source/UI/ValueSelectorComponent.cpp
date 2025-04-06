#include "ValueSelectorComponent.h"

ValueSelectorComponent::ValueSelectorComponent(ValueChangedCallback callback)
    : onValueChanged(std::move(callback)) {
    // Устанавливаем начальное значение без уведомления
    setValue(minValue, juce::sendNotification);
    if (onValueChanged) {
        onValueChanged(minValue);
    }
}

void ValueSelectorComponent::paint(juce::Graphics& g) {
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();

    // Рисуем фон
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    float arrowAreaWidth = bounds.getWidth() * arrowAreaRatio;

    // Забираем область для стрелок справа
    juce::Rectangle<float> arrowBoundsFloat = bounds.removeFromRight(arrowAreaWidth);
    juce::Rectangle<float> textAreaFloat = bounds;

    juce::Rectangle<int> textArea = textAreaFloat.toNearestInt();
    juce::Rectangle<int> arrowBounds = arrowBoundsFloat.toNearestInt(); // Эту область используем для рисования стрелок

    // Рисуем значение
    g.setColour(textColour);
    g.setFont(textArea.getHeight() * 0.6f);
    g.drawText(juce::String(currentValue), textArea, juce::Justification::centred, false);

    // Рисуем стрелки
    g.setColour(arrowColour);
    juce::Rectangle<int> paddedArrowBounds = arrowBounds.reduced(arrowBounds.getWidth() / 4, arrowBounds.getHeight() / 5);

    // Стрелка вверх
    juce::Path upArrow;
    float arrowSize = juce::jmin(paddedArrowBounds.getWidth(), paddedArrowBounds.getHeight() / 2) * 0.8f;
    juce::Point<float> topCenter = paddedArrowBounds.getCentre().toFloat().translated(0, -paddedArrowBounds.getHeight() * 0.25f);
    upArrow.addTriangle(topCenter.x, topCenter.y - arrowSize * 0.5f,
                        topCenter.x - arrowSize * 0.5f, topCenter.y + arrowSize * 0.5f,
                        topCenter.x + arrowSize * 0.5f, topCenter.y + arrowSize * 0.5f);
    g.fillPath(upArrow);


    // Стрелка вниз
    juce::Path downArrow;
    juce::Point<float> bottomCenter = paddedArrowBounds.getCentre().toFloat().translated(0, paddedArrowBounds.getHeight() * 0.25f);
    downArrow.addTriangle(bottomCenter.x, bottomCenter.y + arrowSize * 0.5f,
                          bottomCenter.x - arrowSize * 0.5f, bottomCenter.y - arrowSize * 0.5f,
                          bottomCenter.x + arrowSize * 0.5f, bottomCenter.y - arrowSize * 0.5f);
    g.fillPath(downArrow);
}

void ValueSelectorComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();

    juce::Rectangle<int> arrowBounds = bounds.removeFromRight(bounds.getWidth() * arrowAreaRatio); // Правая часть для стрелок
    // Определяем области клика внутри arrowBounds
    upArrowArea = arrowBounds.removeFromTop(arrowBounds.getHeight() / 2);
    downArrowArea = arrowBounds;
}

void ValueSelectorComponent::mouseDown(const juce::MouseEvent& event) {
    int newValue = currentValue;

    // Смотрим, куда попал клик мышки
    if (upArrowArea.contains(event.getPosition())) { // Если в область верхней стрелки, то инкрементируем значение
        newValue += interval;
    } else if (downArrowArea.contains(event.getPosition())) { // Если в область нижней стрелки, то декрементируем значение
        newValue -= interval;
    } else { // Иначе ничего не делаем
        return;
    }

    // Устанавливаем новое значение
    setValue(newValue);
}

void ValueSelectorComponent::setRange(int newMinValue, int newMaxValue, int newInterval)
{
    minValue = newMinValue;
    maxValue = newMaxValue;
    interval = juce::jmax(1, newInterval); // Интервал не может быть меньше 1
    setValue(currentValue, juce::dontSendNotification); // Перепроверяем текущее значение, чтобы оно было в новом диапазоне
}

void ValueSelectorComponent::setValue (int newValue, juce::NotificationType notification) {
    int clampedValue = juce::jlimit(minValue, maxValue, newValue); // Ограничиваем значение диапазоном

    // Обновляем и перерисовываем, только если значение изменилось
    if (currentValue != clampedValue) {
        currentValue = clampedValue;
        repaint(); // Перерисовка компонента с новым значением

        // Вызываем колбэк, если надо и он есть уведомление
        if (notification == juce::sendNotification && onValueChanged) {
            onValueChanged(currentValue);
        }
    } else if (notification == juce::sendNotificationSync) { // Если значение не изменилось, но нужно обновить отображение
         repaint();
    }

}

int ValueSelectorComponent::getValue() const {
    return currentValue;
}
