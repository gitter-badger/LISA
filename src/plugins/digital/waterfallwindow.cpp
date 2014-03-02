/***********************************************************************
 *
 * LISA: Lightweight Integrated System for Amateur Radio
 * Copyright (C) 2013 - 2014
 *      Norman Link (DM6LN)
 *
 * This file is part of LISA.
 *
 * LISA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LISA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You can find a copy of the GNU General Public License in the file
 * LICENSE.GPL contained in the root directory of this project or
 * under <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#include "waterfallwindow.h"
#include "audio/audiodevicein.h"
#include "signalprocessing/spectrum.h"
#include "waterfalltoolbar.h"
#include "waterfall.h"
#include "spectrograph.h"

#include "audio/audiodevicelist.h"

#include <QVBoxLayout>

using namespace Digital::Internal;

WaterfallWindow::WaterfallWindow(QWidget* parent)
    : QWidget(parent),
      m_inputDevice(0),
      m_toolBar(new WaterfallToolBar(this)),
      m_waterfall(new Waterfall(this)),
      m_spectrograph(0)//new Spectrograph(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // TODO: init toolbar signals / slots
    layout->addWidget(m_toolBar);

    // connect spectrum to widget
    m_spectrum = new Spectrum(4096, WT_BLACKMANHARRIS, this);
    connect(m_spectrum, &Spectrum::spectrumLog, m_waterfall, &SpectrumWidget::addSpectrum);
    //connect(m_spectrum, &Spectrum::spectrumLog, m_spectrograph, &Spectrograph::addSpectrumLog);
    //connect(m_spectrum, &Spectrum::spectrumMag, m_spectrograph, &Spectrograph::addSpectrumMag);
    m_spectrum->init();

    // init waterfall
    m_waterfall->init(m_spectrum->getSpectrumSize());

    layout->addWidget(m_waterfall);
    //layout->addWidget(m_spectrograph);

    setMinimumHeight(100);
}

void WaterfallWindow::start(AudioDeviceIn* inputDevice)
{
    m_inputDevice = inputDevice;
    m_inputDevice->registerConsumer(m_spectrum);
}

void WaterfallWindow::stop()
{
    m_inputDevice->unregisterConsumer(m_spectrum);
    m_inputDevice = 0;
}

void WaterfallWindow::reset()
{
    m_waterfall->reset();
}

Waterfall* WaterfallWindow::getWaterfall()
{
    return m_waterfall;
}