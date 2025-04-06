#include "OscillunaLookAndFeel.h"

OscillunaLookAndFeel::OscillunaLookAndFeel() {
    // Определение используемых цветов.
    
    // Окно
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::lightgrey);
    
    // Слайдер
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::black);
    setColour(juce::Slider::trackColourId, juce::Colours::black);
    setColour(juce::Slider::backgroundColourId, juce::Colours::grey);
    setColour(juce::Slider::thumbColourId, juce::Colours::white);
    
    // Кнопка
    setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    setColour(juce::TextButton::buttonOnColourId, juce::Colours::black);
    
    // Выпадающий список (ComboBox)
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(240, 240, 240));
    setColour(juce::ComboBox::textColourId, juce::Colours::black);
    setColour(juce::ComboBox::arrowColourId, juce::Colours::black);
    setColour(juce::ComboBox::buttonColourId, juce::Colours::black);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(240, 240, 240));
    setColour(juce::PopupMenu::textColourId, juce::Colours::black);
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::black);
}

void OscillunaLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                       const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) {
    float availableSpace = (float)juce::jmin(width / 2, height / 2);
    float outerBoundsRadius = juce::jmax(0.0f, availableSpace - 2.0f);
    float radius = outerBoundsRadius * (16.0f / 21.0f);

    if (radius <= 0.0f) {
        return; // Ничего не рисуем, если область слишком маленькая
    }

    // Определение координат, радиуса и толщин.
    float centreX = (float)x + (float)width * 0.5f;
    float centreY = (float)y + (float)height * 0.5f;
    float rx = centreX - radius;
    float ry = centreY - radius;
    float rw = radius * 2.0f;
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float cornerSize = radius / 31.33f;
    float pointerThickness = radius / 15.66f;
    float serifLength = radius / 4.0f;
    float serifThickness = pointerThickness;
    float serifPadding = serifLength / 4.0f;

    // Тело слайдера — круг.
    g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    g.fillEllipse(rx, ry, rw, rw);

    // Указатель — белая закруглённая линия.
    juce::Path pointerPath;
    float pointerLength = radius; // Указатель доходит до края основного круга
    // Учитываем толщину линии и перемещаем в (0; 0) для последующего поворота
    pointerPath.addRoundedRectangle(-pointerThickness * 0.5f, -radius + pointerThickness * 0.5f, pointerThickness, pointerLength, cornerSize);
    // Поворачиваем и перемещаем в центр слайдера
    pointerPath.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(juce::Colours::white);
    g.fillPath(pointerPath);

    // Засечки
    g.setColour(juce::Colours::darkgrey);
    float serifStartY = -radius - serifPadding; // Начало отрисовки — за пределами радиуса + отступ

    juce::Path serifPath; // Используем один путь для эффективности
    // Создаём фигуру в начале координат, направленную вверх
    serifPath.addRoundedRectangle(-serifThickness * 0.5f, serifStartY - serifLength, serifThickness, serifLength, cornerSize);

    g.fillPath(serifPath, juce::AffineTransform::rotation(rotaryStartAngle).translated(centreX, centreY));
    g.fillPath(serifPath, juce::AffineTransform::rotation((rotaryEndAngle + rotaryStartAngle) / 2.0f).translated(centreX, centreY));
    g.fillPath(serifPath, juce::AffineTransform::rotation(rotaryEndAngle).translated(centreX, centreY));
}

void OscillunaLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                      float maxSliderPos, juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) {
    if (sliderStyle == juce::Slider::LinearVertical) { // Отрисовка вертикального слайдера.
        // Вычисляем ширину трека
        float trackWidth = juce::jmin(8.0f, width * 0.25f);

        // Определяем стартовую и конечную точки трека
        juce::Point<float> startPoint(x + width * 0.5f, y + height);
        juce::Point<float> endPoint(startPoint.x, y);
        
        // Рисуем фон трека (весь диапазон)
        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId)); // Цвет фона трека
        g.strokePath(backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded}); // Кривая, с закруглёнными концами

        // Рисуем заполненную часть трека (до текущего положения ползунка)
        juce::Path valueTrack;
        juce::Point<float> thumbPoint(startPoint.x, sliderPos); // Позиция ползунка по вертикали
        valueTrack.startNewSubPath(startPoint);
        valueTrack.lineTo(thumbPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId)); // Цвет активной части трека
        g.strokePath(valueTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        // Параметры внешнего и внутреннего круга ползунка
        float thumbOuterRadiusFactor = 3.0f;
        float thumbInnerRadiusFactor = 1.0f;
        float outerThumbDiameter = trackWidth * thumbOuterRadiusFactor;
        float innerThumbDiameter = trackWidth * thumbInnerRadiusFactor;

        // Готовим внешний эллипс для отрисовки с тенью
        juce::Rectangle<float> outerThumbBounds(outerThumbDiameter, outerThumbDiameter);
        outerThumbBounds = outerThumbBounds.withCentre(thumbPoint); // Центрируем эллипс на позиции ползунка
        juce::Path outerThumbPath;
        outerThumbPath.addEllipse(outerThumbBounds);
        
        // Настройка параметров тени
        juce::DropShadow shadowProperties;
        shadowProperties.colour = juce::Colours::black.withAlpha(0.5f); // Чёрная тень с полупрозрачностью
        shadowProperties.radius = 10; // Радиус размытия тени
        shadowProperties.offset = {0, 0}; // Без смещения
        shadowProperties.drawForPath(g, outerThumbPath); // Применение тени к пути

        // Заливка внешнего эллипса цветом ползунка
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillPath(outerThumbPath);

        // Отрисовка внутреннего круга ползунка
        g.setColour(slider.findColour(juce::ResizableWindow::backgroundColourId)); // Цвет внутреннего круга — фон окна
        g.fillEllipse(juce::Rectangle<float>(innerThumbDiameter, innerThumbDiameter).withCentre(thumbPoint));
    } else if (sliderStyle == juce::Slider::LinearHorizontal) { // Отрисовка горизонтально слайдера
        // Толщина трека зависит от высоты компонента
        float trackWidth = juce::jmin(8.0f, height * 0.25f);
        // Начальная точка — слева, по центру по Y
        juce::Point<float> startPoint(x, y + height * 0.5f);
        // Конечная точка — справа, по центру по Y
        juce::Point<float> endPoint(x + width, startPoint.y);

        // Рисуем фоновый трек
        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        // Точка положения ползунка (X = sliderPos, Y фиксирован)
        juce::Point<float> thumbPoint(sliderPos, startPoint.y);

        // Рисуем трек значения (от начала до ползунка)
        juce::Path valueTrack;
        valueTrack.startNewSubPath(startPoint);
        valueTrack.lineTo(thumbPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});

        // Расчёт размеров ползунка (зависит от толщины трека)
        float thumbOuterRadiusFactor = 3.0f;
        float thumbInnerRadiusFactor = 1.0f;
        float outerThumbDiameter = trackWidth * thumbOuterRadiusFactor;
        float innerThumbDiameter = trackWidth * thumbInnerRadiusFactor;

        // Рисуем внешний круг ползунка с тенью
        juce::Rectangle<float> outerThumbBounds(outerThumbDiameter, outerThumbDiameter);
        outerThumbBounds = outerThumbBounds.withCentre (thumbPoint);
        juce::Path outerThumbPath;
        outerThumbPath.addEllipse(outerThumbBounds);

        juce::DropShadow shadowProperties;
        shadowProperties.colour = juce::Colours::black.withAlpha(0.5f);
        shadowProperties.radius = 10;
        shadowProperties.offset = {0, 0};
        shadowProperties.drawForPath(g, outerThumbPath);

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillPath(outerThumbPath);

        // Рисуем внутренний круг ползунка
        g.setColour(slider.findColour(juce::ResizableWindow::backgroundColourId)); // Используем цвет фона окна для "дырки"
        g.fillEllipse(juce::Rectangle<float>(innerThumbDiameter, innerThumbDiameter).withCentre(thumbPoint));
    } else {
        // Для других стилей (например, горизонтального) используем стандартную отрисовку
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, sliderStyle, slider);
    }
}
