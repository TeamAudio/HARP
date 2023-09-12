

/**
 * @file
 * @brief Custom UI components for the TensorJucePlugin
 * @author xribene
 */

#pragma once

#include "juce_audio_basics/juce_audio_basics.h"

using namespace juce;

/**
 * @brief A class for defining the toolbar slider style.
 */
class TitledTextBox : public juce::Component
{
public:
    TitledTextBox()
    {
        // Attach the label to the text box
        // titleLabel.attachToComponent(&textBox, true);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        auto topArea = area.removeFromTop(area.getHeight() * 0.1);
        titleLabel.setBounds(topArea);
        textBox.setBounds(area);
        addAndMakeVisible(textBox);
        addAndMakeVisible(titleLabel);
    }

    void setName(const juce::String &nameId)
    {
        textBox.setName(nameId);
    }

    juce::String getName()
    {
        return textBox.getName();
    }
    
    void setTitle(const juce::String &title)
    {
        titleLabel.setText(title, juce::dontSendNotification);
    }

    void setText(const juce::String &text)
    {
        textBox.setText(text, juce::dontSendNotification);
    }

    void addListener(juce::TextEditor::Listener *listener)
    {
        textBox.addListener(listener);
    }

    juce::String getText()
    {
        return textBox.getText();
    }

private:
    juce::TextEditor textBox;
    juce::Label titleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitledTextBox)
};