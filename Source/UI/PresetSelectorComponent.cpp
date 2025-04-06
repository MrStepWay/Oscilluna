#include "PresetSelectorComponent.h"

PresetSelectorComponent::PresetSelectorComponent(PresetActionCallback saveCallback, PresetActionCallback loadCallback)
: onSave(std::move(saveCallback)), // Сохраняем колбэки
  onLoad(std::move(loadCallback))
{
    // Заголовок (лейбл)
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);
    
    // Название пресета (лейбл)
    presetlabel.setFont(juce::Font(16.0f));
    presetlabel.setJustificationType(juce::Justification::centred);
    presetlabel.setColour(juce::Label::textColourId, titleColour);
    // Уменьшим размер шрифта, если текст не влезает
    presetlabel.setMinimumHorizontalScale(0.7f);
    addAndMakeVisible(presetlabel);
    
    // Кнопки (сохранить и загрузить)
    // Привязываем к колбэкам
    saveButton.onClick = [this] { if (onSave) onSave(); };
    loadButton.onClick = [this] { if (onLoad) onLoad(); };
    
    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);
}

void PresetSelectorComponent::paint(juce::Graphics& g) {
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
    
    if (!presetArea.isEmpty()) {
        g.saveState();
        g.setColour(presetAreaColour);
        g.fillRoundedRectangle(presetArea.toFloat(), cornerSize / 3);
    }
}

void PresetSelectorComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();
    
    bounds.reduce(padding, padding);
    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding);
    
    presetArea = bounds.removeFromTop(bounds.getHeight() / 2);
    presetArea.removeFromBottom(padding / 2);
    presetlabel.setBounds(presetArea.reduced(padding, 0));
    
    bounds.removeFromTop(padding / 2);
    
    juce::Rectangle<int> saveArea = bounds.removeFromLeft(bounds.getWidth() / 2);
    saveArea.removeFromRight(padding / 2);
    saveArea.removeFromBottom(padding);
    bounds.removeFromLeft(padding / 2);
    bounds.removeFromBottom(padding);
    
    saveButton.setBounds(saveArea);
    loadButton.setBounds(bounds);
}

void PresetSelectorComponent::setCurrentPresetName(const juce::String& name) {
    presetlabel.setText(name, juce::dontSendNotification);
}
