/**
 * @file
 * @brief This file is part of the JUCE examples.
 *
 * Copyright (c) 2022 - Raw Material Software Limited
 * The code included in this file is provided under the terms of the ISC license
 * http://www.isc.org/downloads/software-support-policy/isc-license. Permission
 * To use, copy, modify, and/or distribute this software for any purpose with or
 * without fee is hereby granted provided that the above copyright notice and
 * this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
 * WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
 * PURPOSE, ARE DISCLAIMED.
 *
 * @brief Header of the UI above the document view, shows ticks for time
 * and can dispaly other information depending on the DAW.
 * region sequences.
 * @author JUCE, hugo flores garcia, aldo aguilar
 */

#include "RulersHeader.h"

RulersHeader::RulersHeader() {
  chordsLabel.setText("Chords", NotificationType::dontSendNotification);
  addAndMakeVisible(chordsLabel);

  barsLabel.setText("Bars", NotificationType::dontSendNotification);
  addAndMakeVisible(barsLabel);

  timeLabel.setText("Time", NotificationType::dontSendNotification);
  addAndMakeVisible(timeLabel);
}

void RulersHeader::resized() {
  auto bounds = getLocalBounds();
  const auto rulerHeight = bounds.getHeight() / 3;

  for (auto *label : {&chordsLabel, &barsLabel, &timeLabel})
    label->setBounds(bounds.removeFromTop(rulerHeight));
}

void RulersHeader::paint(Graphics &g) {
  auto bounds = getLocalBounds();
  const auto rulerHeight = bounds.getHeight() / 3;
  g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
  g.fillRect(bounds);
  g.setColour(getLookAndFeel()
                  .findColour(ResizableWindow::backgroundColourId)
                  .contrasting());
  g.drawRect(bounds);
  bounds.removeFromTop(rulerHeight);
  g.drawRect(bounds.removeFromTop(rulerHeight));
}
