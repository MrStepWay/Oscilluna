#include "ShapeSelectorComponent.h"

ShapeSelectorComponent::ShapeSelectorComponent(ShapeChangedCallback callback, int initialShapeIndex)
    : onShapeChanged(std::move(callback)) {
    // Заголовок
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);

    // Добавляем кнопки в вектор
    shapeButtons = { &sineButton, &sawButton, &triangleButton, &squareButton, &noiseButton };
    jassert(shapeButtons.size() == shapeNames.size()); // Проверка консистентности

    // Настраиваем каждую кнопку
    int radioGroupId = 1; // Уникальный ID для группы радио-кнопок
    for (size_t i = 0; i < shapeButtons.size(); ++i) {
        auto* button = shapeButtons[i];
        button->setButtonText(shapeNames[i]);
        button->setRadioGroupId(radioGroupId);
        button->setClickingTogglesState(true);
        button->addListener(this);
        addAndMakeVisible(button);
    }

    setSelectedIndex(initialShapeIndex, juce::dontSendNotification);
}

ShapeSelectorComponent::~ShapeSelectorComponent() {
    // Отписываемся от слушателей
    for (auto* button : shapeButtons)
    {
        button->removeListener(this);
    }
}

void ShapeSelectorComponent::paint (juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
}

void ShapeSelectorComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();

    bounds.reduce(padding, padding);

    // Заголовок
    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding); // Пространство после заголовка

    // Область для кнопок
    juce::Rectangle<int> buttonArea = bounds;
    float rowVPadding = 5.0f; // Вертикальный отступ между рядами
    float totalRowHeight = (buttonArea.getHeight() - rowVPadding) / 2.0f;
    // Рассчитываем высоту кнопки
    float buttonHeight = totalRowHeight * 0.5f; // Половина высоты строки
    float buttonWidth = 80.0f; // Фиксированная ширина для сохранения вида

    // Верхний ряд (3 кнопки)
    juce::FlexBox topRowFlex;
    topRowFlex.flexDirection = juce::FlexBox::Direction::row; // Располагаем в ряд
    topRowFlex.justifyContent = juce::FlexBox::JustifyContent::center; // Центрируем группу кнопок
    topRowFlex.alignItems = juce::FlexBox::AlignItems::center; // Центрируем кнопки по вертикали в ряду

    // Добавляем первые 3 кнопки
    for (int i = 0; i < 3; ++i) {
        // Создаем FlexItem для кнопки
        juce::FlexItem item(*shapeButtons[i]);
        // Устанавливаем фиксированную ширину и динамическую высоту
        item.width = buttonWidth;
        item.height = buttonHeight;
        // Добавляем горизонтальные отступы слева и справа
        item.margin = juce::FlexItem::Margin(0.0f, buttonHPadding, 0.0f, buttonHPadding);
        topRowFlex.items.add(item);
    }

    // Выполняем компоновку для верхнего ряда в верхней половине доступной области
    juce::Rectangle<int> topRowBounds = buttonArea.removeFromTop((int)totalRowHeight);
    topRowFlex.performLayout(topRowBounds.toFloat());


    // Нижний ряд (2 кнопки)
    juce::FlexBox bottomRowFlex;
    bottomRowFlex.flexDirection = juce::FlexBox::Direction::row;
    bottomRowFlex.justifyContent = juce::FlexBox::JustifyContent::center; // Центрируем
    bottomRowFlex.alignItems = juce::FlexBox::AlignItems::center; // Центрируем

    // Добавляем последние 2 кнопки
    for (int i = 3; i < 5; ++i) {
        juce::FlexItem item(*shapeButtons[i]);
        item.width = buttonWidth;
        item.height = buttonHeight;
        item.margin = juce::FlexItem::Margin(0.0f, buttonHPadding, 0.0f, buttonHPadding);
        bottomRowFlex.items.add(item);
    }

    // Выполняем компоновку для нижнего ряда в нижней половине области
    auto bottomRowBounds = buttonArea.removeFromBottom((int)totalRowHeight);
    bottomRowFlex.performLayout(bottomRowBounds.toFloat());
}

void ShapeSelectorComponent::buttonClicked(juce::Button* clickedButton) {
    for (int i = 0; i < shapeButtons.size(); ++i)
    {
        if (clickedButton == shapeButtons[i]) {
            // Проверяем, изменился ли индекс, и вызываем колбэк, если да.
            if (currentSelectedIndex != i && onShapeChanged) {
                 onShapeChanged(i); // Уведомляем об изменении выбора
            }
            // Обновляем текущий индекс
            currentSelectedIndex = i;
            break;
        }
    }
}

void ShapeSelectorComponent::setSelectedIndex(int index, juce::NotificationType notification) {
    if (index >= 0 && index < shapeButtons.size()) {
        // Устанавливаем состояние нужной кнопки.
        shapeButtons[index]->setToggleState(true, juce::dontSendNotification);

        // Обновляем внутренний индекс.
        currentSelectedIndex = index;
    } else {
        jassertfalse; // Неверный индекс
    }
}
