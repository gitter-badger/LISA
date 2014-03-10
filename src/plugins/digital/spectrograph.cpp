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

#include "spectrograph.h"

#include <limits>
#include <QPainter>
#include <QtNumeric>
#include <QTimer>

using namespace Digital::Internal;

Spectrograph::Spectrograph(QWidget* parent)
    : SpectrumWidget(parent),
      m_refLevel(-10),
      m_ampSpan(90),
      m_mode(MODE_LOG)
{
}

Spectrograph::~Spectrograph(void)
{

}

void Spectrograph::iInit()
{
}

void Spectrograph::setMode(Mode mode)
{
    m_mode = mode;
}

void Spectrograph::setRefLevel(int value)
{
    m_refLevel = value;
}

void Spectrograph::setAmpSpan(int value)
{
    m_ampSpan = value;
}

Spectrograph::Mode Spectrograph::getMode() const
{
    return m_mode;
}

int Spectrograph::getRefLevel() const
{
    return m_refLevel;
}

int Spectrograph::getAmpSpan() const
{
    return m_ampSpan;
}

void Spectrograph::beginDraw(QPainter&)
{
}

void Spectrograph::drawSpectrum(QPainter& painter, const QRect& rect)
{
    painter.drawImage(rect, m_spectrograph);
}

QRect Spectrograph::drawFrequencies(QPainter&)
{
    return QRect();
}

void Spectrograph::drawFrequencies()
{
}

void Spectrograph::drawMarkers(QPainter& painter, qreal frequency, const QColor& frqCol, const QColor& bwCol)
{
    // check and limit the actual marker frequency to the possible frequency range
    if (frequency < m_lowerPassband)
        frequency = m_lowerPassband;
    else if (frequency > m_upperPassband)
        frequency = m_upperPassband;

    qreal pos = frqToScreen(frequency);

    // the position where the markers start
    //qreal upPos = m_frequencies.height();
    qreal upPos = 0;

    // determine the distance of the bandwidth markers
    qreal bwScreen = bwToScreen(m_bandwidth);
    bwScreen /= 2.0;

    // the width of the bandwidth gradients
    int bwSize = 10;

    // draw center frequency
    QPointF lineStart(pos, upPos);
    QPointF lineEnd(lineStart.x(), m_size.height());
    painter.setPen(frqCol);
    painter.drawLine(lineStart, lineEnd);

    QColor bwAlpha = bwCol;
    bwAlpha.setAlpha(50);

    // draw bandwidth alpha
    /*QRectF bwAlphaRect(pos - bwScreen, upPos, bwScreen * 2.0, m_size.height());
    painter.setBrush(QBrush(QColor(255, 255, 255, 50)));
    painter.setPen(Qt::NoPen);
    painter.drawRect(bwAlphaRect);*/

    // draw upper bandwidth limit
    lineStart.setX(pos + bwScreen);
    lineEnd.setX(lineStart.x());
    painter.setPen(bwCol);
    painter.drawLine(lineStart, lineEnd);

    // draw lower bandwidth limit
    lineStart.setX(pos - bwScreen);
    lineEnd.setX(lineStart.x());
    painter.setPen(bwCol);
    painter.drawLine(lineStart, lineEnd);

    // draw upper bandwidth gradient
    QRectF upperGradientRect(pos + bwScreen - bwSize, upPos, bwSize, m_size.height());
    QLinearGradient upperGradient(upperGradientRect.topLeft(), upperGradientRect.topRight());
    upperGradient.setColorAt(0, QColor(0, 0, 0, 0));
    upperGradient.setColorAt(1, bwAlpha);
    painter.setBrush(QBrush(upperGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRect(upperGradientRect);

    // draw lower bandwidth gradient
    QRectF lowerGradientRect(pos - bwScreen, upPos, bwSize, m_size.height());
    QLinearGradient lowerGradient(lowerGradientRect.topRight(), lowerGradientRect.topLeft());
    lowerGradient.setColorAt(0, QColor(0, 0, 0, 0));
    lowerGradient.setColorAt(1, bwAlpha);
    painter.setBrush(QBrush(lowerGradient));
    painter.drawRect(lowerGradientRect);

}

void Spectrograph::sizeChanged(const QSize& size)
{
    // create new spectrograph image
    m_spectrograph = QImage(size, QImage::Format_RGB32);

    // draw background image
    m_background = QPixmap(size);
    QRect paintRect(0, 0, size.width(), size.height());
    QPainter painter(&m_background);
    QColor startCol(20, 22, 54);
    QColor endCol(0, 0, 0);
    QLinearGradient gradient(paintRect.bottomLeft(), paintRect.topLeft());
    gradient.setColorAt(0, startCol);
    gradient.setColorAt(1, endCol);
    painter.setBrush(QBrush(gradient));
    painter.drawRect(paintRect);
}

void Spectrograph::bresenham(QImage& image, QPoint point1, QPoint point2, const QColor& lineCol, const QColor& fillCol)
{
    int dx = abs(point2.x() - point1.x());
    int dy = abs(point2.y() - point1.y());
    int sx, sy;
    (point1.x() < point2.x()) ? sx = 1 : sx = -1;
    (point1.y() < point2.y()) ? sy = 1 : sy = -1;
    int err = dx - dy;

    const int width = image.width();
    QRgb* data = (QRgb*)image.scanLine(0);

    QRgb fill = 0;
    if (fillCol.isValid())
        fill = qRgb(fillCol.red(), fillCol.green(), fillCol.blue());

    QRgb line = 0;
    if (lineCol.isValid())
        line = qRgb(lineCol.red(), lineCol.green(), lineCol.blue());

    while((point1.x() != point2.x()) || (point1.y() != point2.y())) {
        if (point1.x() >= 0 && point1.x() < image.width()) {

            // draw a bresenham line
            if (point1.y() >= 0 && point1.y() < image.height()) {
                if (lineCol.isValid())
                    (data + width * point1.y())[point1.x()] = line;
            }

            // fill the area under the line
            if (fillCol.isValid()) {
                int y = point1.y() < 0 ? -1 : point1.y();
                // only fill if not already filled before
                if ((data + width * (image.height() - 1))[point1.x()] != fill) {
                    for (int i = y + 1; i < image.height(); i++)
                        (data + width * i)[point1.x()] = fill;
                }
            }
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err = err - dy;
            point1.setX(point1.x() + sx);
        }

        if (e2 <  dx) {
            err = err + dx;
            point1.setY(point1.y() + sy);
        }
    }
}

void Spectrograph::iRedraw()
{
    if (m_spectrum.size() <= 2)
        return;

    // draw background image
    QPainter painter(&m_spectrograph);
    painter.drawPixmap(0, 0, m_spectrograph.width(), m_spectrograph.height(), m_background);
    painter.end();

    // compute array indices for lower and upper passband frequencies
    int lower = m_lowerPassband / m_binSize;
    lower = lower < 0 ? 0 : lower;
    int upper = m_upperPassband / m_binSize;
    const int spectrumSize = m_spectrum.size();
    upper = upper > spectrumSize ? spectrumSize : upper;

    QColor lineCol(255, 255, 255);
    QColor fillCol(114, 114, 135);

    // fill with background color
    QPoint prevPoint;
    for (int j = 0; j < m_spectrograph.width(); j++) {
        int index = ((j / (double)m_spectrograph.width()) * (upper - lower)) + lower;
        const double& value = m_spectrum[index];
        QPoint point(j, (int)valueToDisp(value));

        if (!prevPoint.isNull())
            bresenham(m_spectrograph, prevPoint, point, QColor(), fillCol);
        prevPoint = point;
    }

    // draw spectrum line
    prevPoint = QPoint();
    for (int j = 0; j < m_spectrograph.width(); j++) {
        int index = ((j / (double)m_spectrograph.width()) * (upper - lower)) + lower;
        const double& value = m_spectrum[index];
        QPoint point(j, (int)valueToDisp(value));

        if (!prevPoint.isNull())
            bresenham(m_spectrograph, prevPoint, point, lineCol, QColor());
        prevPoint = point;
    }

    // NOTE: very inefficient
    /*QPolygonF polygon;

    // add a line just outside the drawing area
    polygon.append(QPointF(-1, m_spectrograph.height() + 1));
    polygon.append(QPointF(-1, valueToDisp(m_spectrum.first())));

    for (int i = 0; i < m_spectrograph.width(); i++) {
        int index = ((i / (double)m_spectrograph.width()) * (upper - lower)) + lower;
        const double& value = m_spectrum[index];
        polygon.append(QPointF(i, valueToDisp(value)));
    }

    // add a line just outside the drawing area
    polygon.append(QPointF(m_spectrograph.width() + 1, valueToDisp(m_spectrum.last())));
    polygon.append(QPointF(m_spectrograph.width() + 1, m_spectrograph.height() + 1));

    QPainter painter(&m_spectrograph);

    painter.setBrush(QBrush(fillCol));
    painter.setPen(lineCol);
    m_spectrograph.fill(Qt::transparent);
    painter.drawPolygon(polygon);*/
}

double Spectrograph::valueToDisp(double value) const
{
    if (m_mode == MODE_MAG) {
        value *= -1000000000;
        value += m_spectrograph.height();
    }
    else if (m_mode == MODE_LOG) {
        value = (m_refLevel - value) / m_ampSpan;
        value *= m_spectrograph.height();
    }

    // clamp (important for performance issues)
    value = value < 0 ? -1 : value;
    value = value >= m_spectrograph.height() ? m_spectrograph.height() - 1 : value;

    return value;
}

void Spectrograph::iAddSpectrumMag(const QVector<double>& spectrum, bool changed)
{
    if (m_mode == MODE_MAG) {
        m_spectrum = spectrum;

        if (changed) {
            reset();
            //drawFrequencies();
        }

        requestRedraw();
    }
}

void Spectrograph::iAddSpectrumLog(const QVector<double>& spectrum, bool changed)
{
    if (m_mode == MODE_LOG) {
        m_spectrum = spectrum;

        if (changed) {
            reset();
            //drawFrequencies();
        }

        requestRedraw();
    }
}
