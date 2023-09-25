#include "juce_gui_basics/juce_gui_basics.h"
#include "../UI/DocumentView.h"
#include "../UI/ToolBarStyle.h"
#include "../UI/CustomComponents.h"

#include "EditorView.h"

class CtrlComponent: public juce::Component, 
                     public Button::Listener, 
                     public Slider::Listener,
                     public ComboBox::Listener,
                     public TextEditor::Listener
{

public:

  CtrlComponent() {}

  void setModel(std::shared_ptr<WebWave2Wave> model) {
    mModel = model;
  }

  void populateGui() {
    headerLabel.setText("The controls go here", juce::dontSendNotification);
    headerLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(headerLabel);

    if (mModel == nullptr) {
      DBG("populate gui called, but model is null");
      return;
    }

    auto &ctrlList = mModel->controls();
  
    for (const auto &pair : ctrlList) {
      auto ctrlPtr = pair.second;
      // SliderCtrl
      if (auto sliderCtrl = dynamic_cast<SliderCtrl*>(ctrlPtr.get())) {
          auto slider = std::make_unique<juce::Slider>();
          // Assuming all sliders are horizontal for simplicity
          slider->setSliderStyle(juce::Slider::LinearHorizontal);
          slider->setName(sliderCtrl->id.toString());
          slider->setTitle(sliderCtrl->label);
          slider->setRange(sliderCtrl->minimum, sliderCtrl->maximum, sliderCtrl->step);
          slider->setValue(sliderCtrl->value);
          slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 90, 20);
          slider->addListener(this);
          
          slider->setTextValueSuffix(" " + sliderCtrl->label);
          // slider->setLookAndFeel(&toolbarSliderStyle);
          addAndMakeVisible(*slider);
          sliders.push_back(std::move(slider));
          DBG("Slider: " + sliderCtrl->label + " added");

      // ToggleCtrl
      } else if (auto toggleCtrl = dynamic_cast<ToggleCtrl*>(ctrlPtr.get())) {
          auto toggle = std::make_unique<juce::ToggleButton>();
          toggle->setName(toggleCtrl->id.toString());
          toggle->setTitle(toggleCtrl->label);
          toggle->setButtonText(toggleCtrl->label);
          toggle->setToggleState(toggleCtrl->value, juce::dontSendNotification);
          toggle->addListener(this);
          addAndMakeVisible(*toggle);
          toggles.push_back(std::move(toggle));
          DBG("Toggle: " + toggleCtrl->label + " added");

      // TextBoxCtrl
      } else if (auto textBoxCtrl = dynamic_cast<TextBoxCtrl*>(ctrlPtr.get())) {
          auto textCtrl = std::make_unique<TitledTextBox>();
          textCtrl->setName(textBoxCtrl->id.toString());
          textCtrl->setTitle(textBoxCtrl->label);
          textCtrl->setText(textBoxCtrl->value);
          textCtrl->addListener(this);
          addAndMakeVisible(*textCtrl);
          textCtrls.push_back(std::move(textCtrl));
          DBG("Text Box: " + textBoxCtrl->label + " added");

      // ComboBoxCtrl
      } else if (auto comboBoxCtrl = dynamic_cast<ComboBoxCtrl*>(ctrlPtr.get())) {
          auto comboBox = std::make_unique<juce::ComboBox>();
          comboBox->setName(comboBoxCtrl->id.toString());
          for (const auto &option : comboBoxCtrl->options) {
              comboBox->addItem(option, comboBox->getNumItems() + 1);
          }

          int selectedId = 1; // Default to first item if the desired value isn't found
          for (int i = 0; i < comboBox->getNumItems(); ++i) {
              if (comboBox->getItemText(i).toStdString() == comboBoxCtrl->value) {
                  selectedId = i + 1;  // item IDs start at 1
                  break;
              }
          }
          comboBox->addListener(this);
          comboBox->setTextWhenNoChoicesAvailable("No choices");
          addAndMakeVisible(*comboBox);
          optionCtrls.push_back(std::move(comboBox));
          DBG("Combo Box: " + comboBoxCtrl->label + " added");
      }

      // NumberBoxCtrl (Assuming this should be treated as a slider, but without a visual slider)
      else if (auto numberBoxCtrl = dynamic_cast<NumberBoxCtrl*>(ctrlPtr.get())) {
          std::unique_ptr<juce::Slider> numberBox = std::make_unique<juce::Slider>();
          numberBox->setName(numberBoxCtrl->id.toString());
          numberBox->setRange(numberBoxCtrl->min, numberBoxCtrl->max);
          numberBox->setValue(numberBoxCtrl->value, juce::dontSendNotification);
          numberBox->setTextBoxStyle(juce::Slider::TextBoxAbove, false, 90, 20);
          numberBox->addListener(this);
          numberBox->setTextValueSuffix(" " + numberBoxCtrl->label);
          numberBox->setSliderStyle(juce::Slider::IncDecButtons);
          // numberBox->setLookAndFeel(&toolbarSliderStyle);
          addAndMakeVisible(*numberBox);
          sliders.push_back(std::move(numberBox));
          DBG("Number Box: " + numberBoxCtrl->label + " added");
      }
    }
    repaint();
    resized();
  }

  void resetUI(){
    DBG("CtrlComponent::resetUI called");
    mModel.reset();
    // remove all the widgets and empty the vectors
    for (auto &ctrl : sliders) {
      removeChildComponent(ctrl.get());
    }
    sliders.clear();

    for (auto &ctrl : toggles) {
        removeChildComponent(ctrl.get());
    }
    toggles.clear();

    for (auto &ctrl : optionCtrls) {
        removeChildComponent(ctrl.get());
    }
    optionCtrls.clear();

    for (auto &ctrl : textCtrls) {
        removeChildComponent(ctrl.get());
    }
    textCtrls.clear();
  }


void resized() override {
    auto area = getLocalBounds();

    headerLabel.setBounds(area.removeFromTop(30));  // Adjust height to your preference

    juce::FlexBox mainBox;
    mainBox.flexDirection = juce::FlexBox::Direction::column;  // Set the main flex direction to column

    // Sliders
    juce::FlexBox sliderBox;
    sliderBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto& slider : sliders) {
        DBG("Adding slider with name: " + slider->getName() + " to sliderBox");
        sliderBox.items.add(juce::FlexItem(*slider).withFlex(1));
    }

    // Toggles
    juce::FlexBox toggleBox;
    toggleBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto& toggle : toggles) {
        DBG("Adding toggle with name: " + toggle->getName() + " to toggleBox");
        toggleBox.items.add(juce::FlexItem(*toggle).withFlex(1));
    }

    // Option Controls
    juce::FlexBox optionBox;
    optionBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto& optionCtrl : optionCtrls) {
        DBG("Adding option control with name: " + optionCtrl->getName() + " to optionBox");
        optionBox.items.add(juce::FlexItem(*optionCtrl).withFlex(1));
    }

    // Text Controls
    juce::FlexBox textBox;
    textBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto& textCtrl : textCtrls) {
        DBG("Adding text control with name: " + textCtrl->getName() + " to textBox");
        textBox.items.add(juce::FlexItem(*textCtrl).withFlex(1));
    }

    // Add each FlexBox to the main FlexBox
    mainBox.items.add(juce::FlexItem(sliderBox).withFlex(1));
    mainBox.items.add(juce::FlexItem(toggleBox).withFlex(1));
    mainBox.items.add(juce::FlexItem(optionBox).withFlex(1));
    mainBox.items.add(juce::FlexItem(textBox).withFlex(1));

    // Perform Layout
    mainBox.performLayout(area);
}



  void buttonClicked(Button *button) {
    auto id = juce::Uuid(button->getName().toStdString());

    CtrlList &ctrls = mModel->controls();
    auto pair = mModel->findCtrlByUuid(id); 
    if (pair == ctrls.end()) {
      DBG("buttonClicked: ctrl not found");
      return;
    }
    auto ctrl = pair->second;
    if (auto toggleCtrl = dynamic_cast<ToggleCtrl*>(ctrl.get())) {
      toggleCtrl->value = button->getToggleState();
    } else {
      DBG("buttonClicked: ctrl is not a toggle");
    }

  }

  void comboBoxChanged(ComboBox *comboBox) {
    auto id = juce::Uuid(comboBox->getName().toStdString());

    CtrlList &ctrls = mModel->controls();
    auto pair = mModel->findCtrlByUuid(id);
    if (pair == ctrls.end()) {
      DBG("comboBoxChanged: ctrl not found");
      return;
    }
    auto ctrl = pair->second;
    if (auto comboBoxCtrl = dynamic_cast<ComboBoxCtrl*>(ctrl.get())) {
      comboBoxCtrl->value = comboBox->getText().toStdString();
    } else {
      DBG("comboBoxChanged: ctrl is not a combobox");
    }
  }

  void textEditorReturnKeyPressed (TextEditor& textEditor) {
    auto id = juce::Uuid(textEditor.getName().toStdString());

    CtrlList &ctrls = mModel->controls();
    auto pair = mModel->findCtrlByUuid(id);
    if (pair == ctrls.end()) {
      DBG("textEditorReturnKeyPressed: ctrl not found");
      return;
    }
    auto ctrl = pair->second;
    if (auto textBoxCtrl = dynamic_cast<TextBoxCtrl*>(ctrl.get())) {
      textBoxCtrl->value = textEditor.getText().toStdString();
    } else {
      DBG("textEditorReturnKeyPressed: ctrl is not a text box");
    }
  }

  void sliderValueChanged(Slider* slider) {
    ignoreUnused(slider);
  }

  void sliderDragEnded(Slider* slider) {
    auto id = juce::Uuid(slider->getName().toStdString());

    CtrlList &ctrls = mModel->controls();
    auto pair = mModel->findCtrlByUuid(id);
    if (pair == ctrls.end()) {
      DBG("sliderDragEnded: ctrl not found");
      return;
    }
    auto ctrl = pair->second;
    if (auto sliderCtrl = dynamic_cast<SliderCtrl*>(ctrl.get())) {
      sliderCtrl->value = slider->getValue();
    } else if (auto numberBoxCtrl = dynamic_cast<NumberBoxCtrl*>(ctrl.get())) {
      numberBoxCtrl->value = slider->getValue();
    } else {
      DBG("sliderDragEnded: ctrl is not a slider");
    }
  }

private:
  // ToolbarSliderStyle toolbarSliderStyle;
  std::shared_ptr<WebWave2Wave> mModel {nullptr};

  juce::Label headerLabel;

  // Vectors of unique pointers to widgets
  std::vector<std::unique_ptr<juce::Slider>> sliders;
  std::vector<std::unique_ptr<juce::ToggleButton>> toggles;
  std::vector<std::unique_ptr<juce::ComboBox>> optionCtrls;
  std::vector<std::unique_ptr<TitledTextBox>> textCtrls;

};

