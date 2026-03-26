#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "CollapsibleComboBox.h"
#include "Engine/ModulationRouter.h"
#include "Typography.h"
#include "BinaryData.h"

// Popout drawer for assigning controller axes (sticks/triggers) to mod targets.
class AxisDrawer : public juce::Component {
public:
    // Callback fired when user changes an axis assignment
    std::function<void(ControlAxis, ModTarget)> onAxisChanged;

    AxisDrawer (ModulationRouter& router)
        : modRouter (router)
    {
        // Build target name list
        for (int t = 0; t < static_cast<int>(ModTarget::Count); ++t)
            targetNames.add (modTargetName (static_cast<ModTarget>(t)));

        // Create a row for each axis
        for (int a = 0; a < static_cast<int>(ControlAxis::Count); ++a)
        {
            auto axis = static_cast<ControlAxis>(a);

            // Label
            auto lbl = std::make_unique<juce::Label>();
            lbl->setText (controlAxisName (axis), juce::dontSendNotification);
            lbl->setColour (juce::Label::textColourId, juce::Colour (0xffaabbcc));
            lbl->setFont (gm::Typography::getInstance().getLabelFont (10.0f));
            lbl->setJustificationType (juce::Justification::centredRight);
            lbl->setTooltip (juce::String ("Assign ") + controlAxisName (axis) + " to a modulation target");
            addAndMakeVisible (*lbl);
            labels.push_back (std::move (lbl));

            // ComboBox
            auto box = std::make_unique<gm::combos::CollapsibleComboBox>();
            for (int t = 0; t < targetNames.size(); ++t)
                box->addItem (targetNames[t], t + 1);  // 1-indexed

            // Set to current assignment
            int currentTarget = static_cast<int>(modRouter.getAxisTarget (axis));
            box->setSelectedId (currentTarget + 1, juce::dontSendNotification);

            box->onChange = [this, axis, a] {
                int sel = combos[a]->getSelectedId() - 1;
                auto target = static_cast<ModTarget>(sel);
                modRouter.setAxisTarget (axis, target);
                if (onAxisChanged)
                    onAxisChanged (axis, target);
            };
            box->setTooltip ("Modulation target for " + juce::String (controlAxisName (axis)));
            addAndMakeVisible (*box);
            combos.push_back (std::move (box));
        }

        // Title label
        titleLabel.setText ("CONTROLLER AXES", juce::dontSendNotification);
        titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff00ccff));
        titleLabel.setFont (gm::Typography::getInstance().getLabelFont (11.0f));
        titleLabel.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (titleLabel);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff111118));

        // Tiled texture
        if (! drawerTile.isValid())
            drawerTile = juce::ImageCache::getFromMemory (
                BinaryData::tile_brushed_metal_png, BinaryData::tile_brushed_metal_pngSize);
        if (drawerTile.isValid())
        {
            int tw = drawerTile.getWidth();
            int th = drawerTile.getHeight();
            g.saveState();
            g.reduceClipRegion (getLocalBounds());
            g.setOpacity (0.15f);
            for (int ty = 0; ty < getHeight(); ty += th)
                for (int tx = 0; tx < getWidth(); tx += tw)
                    g.drawImageAt (drawerTile, tx, ty);
            g.restoreState();
        }

        // Left edge separator
        g.setColour (juce::Colour (0xff00cccc).withAlpha (0.3f));
        g.drawLine (0.0f, 0.0f, 0.0f, static_cast<float> (getHeight()), 1.0f);

        // Section dividers between rows
        g.setColour (juce::Colour (0xff333340));
        auto b = getLocalBounds().reduced (8, 6);
        int startY = b.getY() + 20; // after title
        for (int i = 1; i < static_cast<int>(ControlAxis::Count); ++i)
        {
            int rowY = startY + i * 32;
            g.drawLine (8.0f, (float)(rowY - 2), (float)(getWidth() - 8), (float)(rowY - 2), 0.5f);
        }
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced (8, 6);
        int labelW = 72;
        int rowH = 32;
        int comboH = 22;

        titleLabel.setBounds (b.removeFromTop (18));
        b.removeFromTop (4);

        for (int i = 0; i < static_cast<int>(ControlAxis::Count); ++i)
        {
            auto row = b.removeFromTop (rowH);
            labels[i]->setBounds (row.removeFromLeft (labelW).reduced (0, (rowH - comboH) / 2));
            row.removeFromLeft (4);
            combos[i]->setBounds (row.reduced (0, (rowH - comboH) / 2));
        }
    }

    // Refresh combo selections from the router (e.g., after loading a preset)
    void refreshFromRouter()
    {
        for (int a = 0; a < static_cast<int>(ControlAxis::Count); ++a)
        {
            auto target = modRouter.getAxisTarget (static_cast<ControlAxis>(a));
            combos[a]->setSelectedId (static_cast<int>(target) + 1, juce::dontSendNotification);
        }
    }

private:
    ModulationRouter& modRouter;

    juce::Label titleLabel;
    std::vector<std::unique_ptr<juce::Label>> labels;
    std::vector<std::unique_ptr<gm::combos::CollapsibleComboBox>> combos;
    juce::StringArray targetNames;

    mutable juce::Image drawerTile;
};
