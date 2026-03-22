#pragma once
// ghostmoon UI Catalog: Knob
// Origin: bondsp BoNKnob.h
// Rotary knob with value readout + label, APVTS attachment.

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <limits>

#if __has_include("Typography.h")
#include "Typography.h"
#define GM_HAS_TYPOGRAPHY 1
#endif

namespace gm {

class Knob : public juce::Component, public juce::SettableTooltipClient, private juce::Slider::Listener {
public:
  enum ColourIds {
    valueTextColourId = 0x3100700,
    labelTextColourId = 0x3100701,
    focusRingColourId = 0x3100702
  };

  struct InternalSlider : public juce::Slider {
    bool enableTextEntry = true;
    bool enableContextMenu = true;

    InternalSlider() {
      setDoubleClickReturnValue(false, 0.0);
      setPopupMenuEnabled(false); // We handle our own right-click menu
    }

    void mouseDoubleClick(const juce::MouseEvent&) override {
      if (!enableTextEntry) return;
      // Delegate to parent Knob to show editor over value label
      if (auto* knob = dynamic_cast<Knob*>(getParentComponent()))
        knob->showTextEntry();
    }

    // Optional MIDI learn callbacks — set by the parent plugin
    std::function<void()> onMidiLearn;
    std::function<void()> onMidiUnlearn;

    void mouseDown(const juce::MouseEvent& e) override {
      if (enableContextMenu && e.mods.isPopupMenu()) {
        juce::PopupMenu menu;
        menu.addItem(1, "Set to Default");
        menu.addSeparator();
        menu.addItem(2, "Copy Value");
        menu.addItem(3, "Paste Value", s_clipboard_ != std::numeric_limits<double>::lowest());
        if (onMidiLearn || onMidiUnlearn) {
          menu.addSeparator();
          if (onMidiLearn)   menu.addItem(4, "MIDI Learn");
          if (onMidiUnlearn) menu.addItem(5, "MIDI Unlearn");
        }

        menu.showMenuAsync(juce::PopupMenu::Options(), [this](int result) {
          if (result == 1) setValue(getDoubleClickReturnValue(), juce::sendNotificationSync);
          else if (result == 2) s_clipboard_ = getValue();
          else if (result == 3 && s_clipboard_ != std::numeric_limits<double>::lowest())
            setValue(s_clipboard_, juce::sendNotificationSync);
          else if (result == 4 && onMidiLearn)   onMidiLearn();
          else if (result == 5 && onMidiUnlearn) onMidiUnlearn();
        });
        return;
      }
      juce::Slider::mouseDown(e);
    }

    static inline double s_clipboard_ = std::numeric_limits<double>::lowest();
  };

  Knob(juce::AudioProcessorValueTreeState &apvts, const juce::String &paramId,
       const juce::String &labelText, const juce::String &tooltip = {})
      : labelText_(labelText) {
    setColour(valueTextColourId, juce::Colour(0xff888888));
    setColour(labelTextColourId, juce::Colour(0xff888888));
    setColour(focusRingColourId, juce::Colour(0xff00cccc));

    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setWantsKeyboardFocus(true);
    if (tooltip.isNotEmpty())
      slider.setTooltip(tooltip);
    slider.addListener(this);
    addAndMakeVisible(slider);

    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, findColour(valueTextColourId));
    // Value label passes clicks through so Knob catches double-click
    valueLabel.setInterceptsMouseClicks(false, false);
    setInterceptsMouseClicks(true, true);
    addAndMakeVisible(valueLabel);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, findColour(labelTextColourId));
    addAndMakeVisible(label);

    attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramId, slider);
    updateValueText();
  }

  ~Knob() override { slider.removeListener(this); }

  void mouseEnter(const juce::MouseEvent&) override { isHovered_ = true; repaint(); }
  void mouseExit(const juce::MouseEvent&) override { isHovered_ = false; repaint(); }
  void mouseDoubleClick(const juce::MouseEvent&) override {
    showTextEntry();
  }

  /// Show text entry editor over the value label area.
  void showTextEntry() {
    if (activeEditor_ != nullptr || !slider.enableTextEntry) return;

    activeEditor_ = std::make_unique<juce::TextEditor>();
    auto* ed = activeEditor_.get();
    ed->setMultiLine(false);
    ed->setReturnKeyStartsNewLine(false);
    ed->setText(slider.getTextFromValue(slider.getValue()), juce::dontSendNotification);
    ed->setJustification(juce::Justification::centred);
    auto vlb = valueLabel.getBounds();
    ed->setBounds(vlb.withSizeKeepingCentre(juce::jmax(vlb.getWidth(), 50), 20));
    ed->selectAll();

    ed->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff1e1e24));
    ed->setColour(juce::TextEditor::textColourId, juce::Colour(0xffcccccc));
    ed->setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff00cccc));
    ed->setColour(juce::CaretComponent::caretColourId, juce::Colour(0xff00cccc));

    addAndMakeVisible(ed);
    ed->grabKeyboardFocus();

    ed->onReturnKey = [this] {
      if (activeEditor_) {
        slider.setValue(slider.getValueFromText(activeEditor_->getText()), juce::sendNotificationSync);
      }
      dismissTextEntry();
    };
    ed->onEscapeKey = [this] { dismissTextEntry(); };
    ed->onFocusLost = [this] {
      juce::MessageManager::callAsync([this] { dismissTextEntry(); });
    };
  }

  void dismissTextEntry() {
    if (activeEditor_) { removeChildComponent(activeEditor_.get()); activeEditor_.reset(); }
  }

  void resized() override {
    auto b = getLocalBounds();
    auto nameH = 12;
    auto valH = 11;
    label.setBounds(b.removeFromBottom(nameH));
    valueLabel.setBounds(b.removeFromBottom(valH));
    slider.setBounds(b);
  }

  void paintOverChildren(juce::Graphics& g) override {
    if (isHovered_) {
      g.setColour(findColour(focusRingColourId).withAlpha(0.06f));
      g.fillRoundedRectangle(slider.getBounds().toFloat(), 4.0f);
    }
    if (slider.hasKeyboardFocus(true)) {
      g.setColour(findColour(focusRingColourId).withAlpha(0.5f));
      g.drawRoundedRectangle(slider.getBounds().toFloat().reduced(0.5f), 4.0f, 1.5f);
    }
  }

  void colourChanged() override {
    valueLabel.setColour(juce::Label::textColourId, findColour(valueTextColourId));
    label.setColour(juce::Label::textColourId, findColour(labelTextColourId));
    repaint();
  }

  juce::Slider &getSlider() { return slider; }

private:
  static juce::Font gmFont(float h) {
#ifdef GM_HAS_TYPOGRAPHY
    return gm::Typography::getInstance().getLabelFont(h);
#else
    return juce::Font(juce::FontOptions(h));
#endif
  }
  static juce::Font gmMonoFont(float h) {
#ifdef GM_HAS_TYPOGRAPHY
    return gm::Typography::getInstance().getValueFont(h);
#else
    return juce::Font(juce::FontOptions(h));
#endif
  }

  void sliderValueChanged(juce::Slider *) override {
    updateValueText();
    slider.setTooltip(labelText_ + ": " + slider.getTextFromValue(slider.getValue()));
  }

  void updateValueText() {
    auto text = slider.getTextFromValue(slider.getValue());
    valueLabel.setText(text, juce::dontSendNotification);
  }

  InternalSlider slider;
  juce::Label valueLabel;
  std::unique_ptr<juce::TextEditor> activeEditor_;
  juce::Label label;
  juce::String labelText_;
  bool isHovered_ = false;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      attachment;
};

} // namespace gm
