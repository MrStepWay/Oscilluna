#include "EnvelopeComponent.h"

EnvelopeComponent::EnvelopeComponent(
    FloatCallback attackChangedCallback,
    FloatCallback decayChangedCallback,
    FloatCallback sustainChangedCallback,
    FloatCallback releaseChangedCallback,
    float initialAttack,
    float initialDecay,
    float initialSustain,
    float initialRelease
) :
    onAttackChanged(std::move(attackChangedCallback)),
    onDecayChanged(std::move(decayChangedCallback)),
    onSustainChanged(std::move(sustainChangedCallback)),
    onReleaseChanged(std::move(releaseChangedCallback))
{
    // Настройка слайдеров
    auto setupSlider = [this](juce::Slider& slider, float initialValue) {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.addListener(this);
        addAndMakeVisible(slider);
        slider.setValue(initialValue, juce::sendNotification); // Устанавливаем начальное значение
    };

    // Attack (время, 0-2 сек)
    attackSlider.setRange(0.0, 2.0, 0.01);
    setupSlider(attackSlider, initialAttack);

    // Decay (время, 0-2 сек)
    decaySlider.setRange(0.0, 2.0, 0.01);
    setupSlider(decaySlider, initialDecay);

    // Sustain (уровень, 0-1)
    sustainSlider.setRange(0.0, 1.0, 0.01);
    setupSlider(sustainSlider, initialSustain);

    // Release (время, например 0-2 сек)
    releaseSlider.setRange(0.0, 2.0, 0.01);
    setupSlider(releaseSlider, initialRelease);
    
    // Заголовок (лейбл)
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);

}

EnvelopeComponent::~EnvelopeComponent() {
    // Отписываемся от слушателей
    attackSlider.removeListener(this);
    decaySlider.removeListener(this);
    sustainSlider.removeListener(this);
    releaseSlider.removeListener(this);
}

void EnvelopeComponent::paint(juce::Graphics& g) {
    // Рисуем фон
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
    bounds.reduce(innerPadding, innerPadding);
    g.setColour(innerBackgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize - innerPadding / 2);

    // Рисуем вертикальные лейблы ADSR
    g.setColour(labelColour);
    g.setFont(juce::Font(14.0f, juce::Font::bold)); // Шрифт для ADSR лейблов

    // Лямбда для отрисовки лейблов вертикально
    auto drawVerticalLabel = [&](const juce::String& text, const juce::Rectangle<int>& area) {
        if (!area.isEmpty()) {
            g.saveState();
            // Поворачиваем текст
            juce::AffineTransform rotation = juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi,
                                                                             area.getCentreX(),
                                                                             area.getCentreY());
            g.addTransform(rotation);
            g.drawText(text, area.transformedBy(rotation), juce::Justification::centred, false);
            g.restoreState();
        }
    };

    drawVerticalLabel(attackLabelText, attackLabelArea);
    drawVerticalLabel(decayLabelText, decayLabelArea);
    drawVerticalLabel(sustainLabelText, sustainLabelArea);
    drawVerticalLabel(releaseLabelText, releaseLabelArea);
}

void EnvelopeComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();

    bounds.reduce(internalPadding, internalPadding);
    bounds.reduce(padding, padding);

    // Заголовок
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    bounds.removeFromTop(topMargin);

    // Область для ADSR
    int numSections = 4;
    int columnWidth = bounds.getWidth() / numSections; // Ширина колонки для каждой секции ADSR

    std::vector<juce::Slider*> sliders = {&attackSlider, &decaySlider, &sustainSlider, &releaseSlider};
    std::vector<juce::Rectangle<int>*> labelAreas = {&attackLabelArea, &decayLabelArea, &sustainLabelArea, &releaseLabelArea};

    // Итерация по секциям
    for (int i = 0; i < numSections; ++i) {
        // Выделяем область для текущей секции, а для последней секции берем всю оставшуюся ширину
        juce::Rectangle<int> sectionArea = bounds.removeFromLeft(i == numSections - 1 ? bounds.getWidth() : columnWidth);

        // Уменьшаем область секции по горизонтали для создания отступов между секциями
        sectionArea.reduce(internalPadding, 0);

        // Разделение секции на лейбл и слайдер
        int currentLabelWidth = sectionArea.getWidth() / 2;
        *(labelAreas[i]) = sectionArea.removeFromLeft(currentLabelWidth); // Левая часть идет под лейбл
        sliders[i]->setBounds(sectionArea); // Оставшаяся правая часть идет под слайдер
    }
}

void EnvelopeComponent::sliderValueChanged(juce::Slider* slider) {
    // Определяем, какой слайдер изменился, и вызываем соответствующий колбэк
    if (slider == &attackSlider) {
        if (onAttackChanged) {
            onAttackChanged(attackSlider.getValue());
        }
    } else if (slider == &decaySlider) {
        if (onDecayChanged) {
            onDecayChanged(decaySlider.getValue());
        }
    } else if (slider == &sustainSlider) {
        if (onSustainChanged) {
            onSustainChanged(sustainSlider.getValue());
        }
    } else if (slider == &releaseSlider) {
        if (onReleaseChanged) {
            onReleaseChanged(releaseSlider.getValue());
        }
    }
}

void EnvelopeComponent::setAttackSliderValue (float value, juce::NotificationType notification) {
    attackSlider.setValue(value, notification);
}

void EnvelopeComponent::setDecaySliderValue  (float value, juce::NotificationType notification) {
    decaySlider.setValue(value, notification);
}

void EnvelopeComponent::setSustainSliderValue(float value, juce::NotificationType notification) {
    sustainSlider.setValue(value, notification);
}

void EnvelopeComponent::setReleaseSliderValue(float value, juce::NotificationType notification) {
    releaseSlider.setValue(value, notification);
}
