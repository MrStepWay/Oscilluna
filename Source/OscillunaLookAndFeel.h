/*
  ==============================================================================

    OscillunaLookAndFeel.h
    Created: 30 Jan 2025 7:51:45pm
    Author:  Степан

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscillunaLookAndFeel : public juce::LookAndFeel_V4 {
public:
    OscillunaLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colours::black);
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::lightgrey);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
        float radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        float centreX = (float)x + (float)width * 0.5f;
        float centreY = (float)y + (float)height * 0.5f;
        float rx = centreX - radius;
        float ry = centreY - radius;
        float rw = radius * 2.0f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        float cornerSize = radius / 31.33f;
        
        // Background: black circle.
        g.setColour (juce::Colours::black);
        g.fillEllipse (rx, ry, rw, rw);
 
        // Pointer: rounded white straight line.
        juce::Path pointerPath;
        float pointerLength = radius;
        float pointerThickness = radius / 15.66f;
        // Taking into account the thickness of the line and move to (0; 0) for fruther rotation tranform.
        pointerPath.addRoundedRectangle(-pointerThickness * 0.5f, -radius + pointerThickness * 0.5f, pointerThickness, pointerLength, cornerSize);
        // Rotate and move to centre of rotary slider.
        pointerPath.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colours::white);
        g.fillPath(pointerPath);
        
        // Serifs.
        // TODO: maybe I should not draw them every time, just make a OscillunaRotarySliderComponent.
        float serifLength = radius / 4.0f;
        float serifThickness = pointerThickness;
        float serifPadding = serifLength / 4.0f;
        g.setColour(juce::Colours::darkgrey);
        
        juce::Path serifPath1;
        serifPath1.addRoundedRectangle(-serifThickness * 0.5f, -radius - serifLength - serifPadding + serifThickness * 0.5f, serifThickness, serifLength, cornerSize);
        serifPath1.applyTransform(juce::AffineTransform::rotation(rotaryStartAngle).translated(centreX, centreY));
        g.fillPath(serifPath1);
        
        juce::Path serifPath2;
        serifPath2.addRoundedRectangle(-serifThickness * 0.5f, -radius - serifLength - serifPadding + serifThickness * 0.5f, serifThickness, serifLength, cornerSize);
        serifPath2.applyTransform(juce::AffineTransform::rotation((rotaryEndAngle + rotaryStartAngle) / 2.0f).translated(centreX, centreY));
        g.fillPath(serifPath2);
        
        juce::Path serifPath3;
        serifPath3.addRoundedRectangle(-serifThickness * 0.5f, -radius - serifLength - serifPadding + serifThickness * 0.5f, serifThickness, serifLength, cornerSize);
        serifPath3.applyTransform(juce::AffineTransform::rotation(rotaryEndAngle).translated(centreX, centreY));
        g.fillPath(serifPath3);
    }
};
