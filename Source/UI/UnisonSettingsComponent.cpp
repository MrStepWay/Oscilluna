#include "UnisonSettingsComponent.h"

UnisonSettingsComponent::UnisonSettingsComponent(
    IntCallback voicesChangedCallback,
    FloatCallback detuneChangedCallback,
    FloatCallback mainVolChangedCallback,
    FloatCallback sideVolChangedCallback,
    int initialVoices,
    float initialDetune,
    float initialMainVol,
    float initialSideVol
) :
    onVoicesChanged(std::move(voicesChangedCallback)),
    onDetuneChanged(std::move(detuneChangedCallback)),
    onMainVolChanged(std::move(mainVolChangedCallback)),
    onSideVolChanged(std::move(sideVolChangedCallback)),
    voicesSelector([this](int v){ if(onVoicesChanged) onVoicesChanged(v); }) // Передаем колбэк в ValueSelector
{
    // Заголовок
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);

    // Голоса (Voices)
    voicesLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    voicesLabel.setJustificationType(juce::Justification::centredLeft);
    voicesLabel.setColour(juce::Label::textColourId, labelColour);
    addAndMakeVisible(voicesLabel);

    voicesSelector.setRange(1, 16); // Диапазон 1-16
    voicesSelector.setValue(initialVoices);
    addAndMakeVisible(voicesSelector);

    // Detune
    detuneLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    detuneLabel.setJustificationType(juce::Justification::centredLeft);
    detuneLabel.setColour(juce::Label::textColourId, labelColour);
    addAndMakeVisible(detuneLabel);

    detuneSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    detuneSlider.setRange(0.0, 1.0, 0.01); // Диапазон 0-1
    detuneSlider.setValue(initialDetune);
    detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    detuneSlider.addListener(this);
    addAndMakeVisible(detuneSlider);

    // Громкость основного голоса (Main Vol)
    mainVolSlider.setSliderStyle(juce::Slider::LinearVertical);
    mainVolSlider.setRange(0.0, 1.0, 0.01); // Диапазон 0-1
    mainVolSlider.setValue(initialMainVol);
    mainVolSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mainVolSlider.addListener(this);
    addAndMakeVisible(mainVolSlider);

    // Громкость боковых голосов (Side Vol)
    sideVolSlider.setSliderStyle(juce::Slider::LinearVertical);
    sideVolSlider.setRange(0.0, 1.0, 0.01);
    sideVolSlider.setValue(initialSideVol);
    sideVolSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sideVolSlider.addListener(this);
    addAndMakeVisible(sideVolSlider);
}

UnisonSettingsComponent::~UnisonSettingsComponent() {
    // Отписываемся от слушателей
    detuneSlider.removeListener(this);
    mainVolSlider.removeListener(this);
    sideVolSlider.removeListener(this);
}

void UnisonSettingsComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    float cornerSize = 15.0f;
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Рисуем вертикальные лейблы
    g.setColour(labelColour);
    g.setFont(juce::Font(15.0f, juce::Font::bold)); // Шрифт для вертикальных лейблов

    // Main Vol Label
    if (!mainVolLabelArea.isEmpty()) {
        g.saveState(); // Сохраняем текущее состояние графики
        
        // Поворот на 90 градусов.
        juce::AffineTransform rotation = juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi,
                                                                         mainVolLabelArea.getCentreX(),
                                                                         mainVolLabelArea.getCentreY());
        g.addTransform(rotation);
        g.drawText(mainVolLabelText, mainVolLabelArea.transformedBy(rotation), juce::Justification::centred, false);
        
        g.restoreState(); // Восстанавливаем состояние
    }

    // Side Vol Label
    if (!sideVolLabelArea.isEmpty()) {
        g.saveState();
        
        juce::AffineTransform rotation = juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi,
                                                                         sideVolLabelArea.getCentreX(),
                                                                         sideVolLabelArea.getCentreY());
        g.addTransform(rotation);
        g.drawText(sideVolLabelText, sideVolLabelArea.transformedBy(rotation), juce::Justification::centred, false);
        g.restoreState();
    }
}

void UnisonSettingsComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();
    
    bounds.reduce(padding, padding);

    // Заголовок
    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding); // Пространство после заголовка

    // Разделение на левую и правую части
    juce::Rectangle<int> leftArea = bounds.removeFromLeft(bounds.getWidth() / 2);
    juce::Rectangle<int> rightArea = bounds;
    
    juce::Rectangle<int> voicesArea = leftArea.removeFromTop(leftArea.getHeight() / 2).reduced(padding, padding);
    juce::Rectangle<int> detuneArea = leftArea.reduced(padding, padding);
    
    // Voices
    voicesLabel.setBounds(voicesArea.removeFromTop(15));
    voicesSelector.setBounds(voicesArea.withWidth(50).withTrimmedTop(5));
    
    // Detune
    detuneLabel.setBounds(detuneArea.removeFromTop(15));
    detuneSlider.setBounds(detuneArea.withTrimmedTop(5));

    // Правая часть (вертикальные слайдеры)
    juce::Rectangle<int> mainVolArea = rightArea.removeFromLeft(rightArea.getWidth() / 2).reduced(padding, padding);
    juce::Rectangle<int> sideVolArea = rightArea.reduced(padding, padding);
    
    mainVolLabelArea = mainVolArea.removeFromLeft(mainVolArea.getWidth() / 2);
    mainVolSlider.setBounds(mainVolArea);
    
    sideVolLabelArea = sideVolArea.removeFromLeft(sideVolArea.getWidth() / 2);
    sideVolSlider.setBounds(sideVolArea);
}

void UnisonSettingsComponent::sliderValueChanged (juce::Slider* slider) {
    // Определяем, какой слайдер изменился, и вызываем соответствующий колбэк
    if (slider == &detuneSlider) {
        if (onDetuneChanged) {
            onDetuneChanged(detuneSlider.getValue());
        }
    } else if (slider == &mainVolSlider) {
        if (onMainVolChanged) {
            onMainVolChanged(mainVolSlider.getValue());
        }
    } else if (slider == &sideVolSlider) {
        if (onSideVolChanged) {
            onSideVolChanged(sideVolSlider.getValue());
        }
    }
}

void UnisonSettingsComponent::setVoicesValue(int value, juce::NotificationType notification) {
    voicesSelector.setValue(value, notification);
}

void UnisonSettingsComponent::setDetuneSliderValue(float value, juce::NotificationType notification) {
    detuneSlider.setValue(value, notification);
}

void UnisonSettingsComponent::setMainVolSliderValue(float value, juce::NotificationType notification) {
    mainVolSlider.setValue(value, notification);
}

void UnisonSettingsComponent::setSideVolSliderValue(float value, juce::NotificationType notification) {
    sideVolSlider.setValue(value, notification);
}
