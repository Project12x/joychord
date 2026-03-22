#pragma once
// ghostmoon UI Catalog: LEDMeter
// Origin: bondsp BoNMeter.h
// Vertical LED-bar level meter with segmented coloring and glow.

#include <atomic>
#include <juce_gui_basics/juce_gui_basics.h>

#if __has_include("Typography.h")
#include "Typography.h"
#define GM_HAS_TYPOGRAPHY 1
#endif

namespace gm {

class LEDMeter : public juce::Component, private juce::Timer {
public:
  enum ColourIds {
    backgroundColourId = 0x3100730,
    segGreenColourId   = 0x3100731,
    segYellowColourId  = 0x3100732,
    segRedColourId     = 0x3100733,
    segOffColourId     = 0x3100734,
    peakHoldColourId   = 0x3100735,
    scaleTextColourId  = 0x3100736
  };

  LEDMeter() {
    setColour(backgroundColourId, juce::Colour(0xff1a1a1e));
    setColour(segGreenColourId,   juce::Colour(0xff44cc44));
    setColour(segYellowColourId,  juce::Colour(0xffcccc44));
    setColour(segRedColourId,     juce::Colour(0xffcc4444));
    setColour(segOffColourId,     juce::Colour(0xff383840));
    setColour(peakHoldColourId,   juce::Colours::white);
    setColour(scaleTextColourId,  juce::Colour(0xff888888));
  }

  void startMetering() { startTimerHz(30); }
  void stopMetering() { stopTimer(); }

  void setLevel(float newLevel) { level.store(newLevel); }

  void setPeakHoldMs(int ms) { peakHoldFrames_ = (ms * 30) / 1000; }
  void setDecayRate(float rate) { decayRate_ = rate; }
  void setShowScale(bool show) { showScale_ = show; repaint(); }
  void setNumSegments(int n) { numSegments_ = juce::jmax(2, n); repaint(); }

  void paint(juce::Graphics &g) override {
    auto fullBounds = getLocalBounds().toFloat().reduced(1.0f);

    // Reserve space for scale labels on the right if enabled
    auto bounds = fullBounds;
    float scaleWidth = 0.0f;
    if (showScale_) {
      scaleWidth = 28.0f;
      bounds = bounds.withTrimmedRight(scaleWidth);
    }

    g.setColour(findColour(backgroundColourId));
    g.fillRoundedRectangle(bounds, 2.0f);

    const int numSeg = numSegments_;
    float segH = bounds.getHeight() / (float)numSeg;
    float gap = 1.0f;
    int litSegments = (int)(displayLevel_ * (float)numSeg);

    // Threshold indices for color zones (scale proportionally to segment count)
    int greenEnd  = (int)(numSeg * 10.0f / 16.0f);  // green zone
    int yellowEnd = (int)(numSeg * 13.0f / 16.0f);   // yellow zone
    int glowStart = numSeg / 2;

    // Glow behind lit area when above 50%
    if (litSegments > glowStart) {
      juce::Colour glowColor;
      if (litSegments >= yellowEnd + 1)
        glowColor = findColour(segRedColourId);
      else if (litSegments >= greenEnd + 1)
        glowColor = findColour(segYellowColourId);
      else
        glowColor = findColour(segGreenColourId);

      float glowAlpha = (float)(litSegments - glowStart) / (float)(numSeg - glowStart) * 0.15f;
      float litHeight = (float)litSegments * segH;
      auto glowBounds = juce::Rectangle<float>(
          bounds.getX() - 3.0f, bounds.getBottom() - litHeight - 2.0f,
          bounds.getWidth() + 6.0f, litHeight + 4.0f);
      g.setColour(glowColor.withAlpha(glowAlpha));
      g.fillRoundedRectangle(glowBounds, 4.0f);
    }

    // Draw segments
    for (int i = 0; i < numSeg; ++i) {
      float yPos = bounds.getBottom() - (float)(i + 1) * segH;
      auto segBounds = juce::Rectangle<float>(bounds.getX(), yPos + gap,
                                              bounds.getWidth(), segH - gap);
      if (i < litSegments) {
        juce::Colour segColor;
        if (i < greenEnd)       segColor = findColour(segGreenColourId);
        else if (i < yellowEnd) segColor = findColour(segYellowColourId);
        else                    segColor = findColour(segRedColourId);
        g.setColour(segColor);
      } else {
        g.setColour(findColour(segOffColourId).withAlpha(0.3f));
      }
      g.fillRoundedRectangle(segBounds, 1.0f);
    }

    // Peak hold line
    if (peakHoldLevel_ > 0.01f) {
      int peakSeg = (int)(peakHoldLevel_ * (float)numSeg);
      peakSeg = juce::jmin(peakSeg, numSeg);
      float peakY = bounds.getBottom() - (float)peakSeg * segH;

      // Use accent color at peak position for the line color
      juce::Colour peakColor;
      if (peakSeg < greenEnd)       peakColor = findColour(segGreenColourId);
      else if (peakSeg < yellowEnd) peakColor = findColour(segYellowColourId);
      else                          peakColor = findColour(segRedColourId);

      g.setColour(peakColor.withAlpha(0.9f));
      g.fillRect(bounds.getX() + 1.0f, peakY - 1.0f, bounds.getWidth() - 2.0f, 2.0f);
    }

    // dB scale markings
    if (showScale_) {
      auto scaleArea = juce::Rectangle<float>(
          bounds.getRight() + 2.0f, fullBounds.getY(),
          scaleWidth - 2.0f, fullBounds.getHeight());

      g.setFont(gmMonoFont(9.0f));
      g.setColour(findColour(scaleTextColourId));

      // dB values and their normalized positions (0 dB = 1.0, -48 dB ~ 0.0)
      struct ScaleMark { float db; const char* text; };
      const ScaleMark marks[] = {
        {  0.0f, "0"   },
        { -3.0f, "-3"  },
        { -6.0f, "-6"  },
        { -12.0f, "-12" },
        { -24.0f, "-24" },
        { -48.0f, "-48" }
      };

      for (auto &m : marks) {
        // Convert dB to normalized 0..1 range (using -48 dB as floor)
        float norm = juce::jlimit(0.0f, 1.0f, (m.db + 48.0f) / 48.0f);
        float y = bounds.getBottom() - norm * bounds.getHeight();
        // Tick mark
        g.drawHorizontalLine((int)y, bounds.getRight(), bounds.getRight() + 3.0f);
        // Label
        g.drawText(m.text,
                   juce::Rectangle<float>(scaleArea.getX() + 3.0f, y - 5.0f, scaleWidth - 5.0f, 10.0f),
                   juce::Justification::centredLeft, false);
      }
    }
  }

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

  void timerCallback() override {
    float target = level.load();

    // Fast attack / slow decay
    if (target > displayLevel_)
      displayLevel_ = target;
    else
      displayLevel_ *= decayRate_;

    // Clamp near-zero
    if (displayLevel_ < 0.001f)
      displayLevel_ = 0.0f;

    // Peak hold tracking
    if (displayLevel_ > peakHoldLevel_) {
      peakHoldLevel_ = displayLevel_;
      peakHoldCounter_ = 0;
    } else {
      peakHoldCounter_++;
      if (peakHoldCounter_ > peakHoldFrames_) {
        // Decay the peak hold after hold time expires
        peakHoldLevel_ *= decayRate_;
        if (peakHoldLevel_ < 0.001f)
          peakHoldLevel_ = 0.0f;
      }
    }

    repaint();
  }

  std::atomic<float> level{0.0f};

  // Display state (timer thread only)
  float displayLevel_   = 0.0f;
  float peakHoldLevel_  = 0.0f;
  int   peakHoldCounter_ = 0;

  // Configuration
  float decayRate_      = 0.92f;
  int   peakHoldFrames_ = 30;   // 1000ms at 30Hz
  bool  showScale_      = false;
  int   numSegments_    = 16;
};

} // namespace gm
