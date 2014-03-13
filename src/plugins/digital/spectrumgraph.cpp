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

#include "spectrumgraph.h"

#include <limits>
#include <QPainter>
#include <QtNumeric>
#include <QTimer>

using namespace Digital::Internal;

SpectrumGraph::SpectrumGraph(QWidget* parent)
    : SpectrumWidget(parent),
      m_fontSize(11),
      m_fontBorder(5),
      m_fontColor(Qt::yellow),
      m_gridColor(QColor(47, 47, 57)),
      m_spectrumLineColor(QColor(200, 200, 200)),
      m_spectrumFillColor(QColor(114, 114, 135))
      //m_spectrumFillColor(QColor(114, 114, 135, 150))
{
}

SpectrumGraph::~SpectrumGraph(void)
{

}

void SpectrumGraph::iInit()
{
}

void SpectrumGraph::paint(QPainter& painter)
{
    // draw spectrum with background
    if (!m_showFrequencies) {
        painter.drawImage(0, 0, m_spectrumGraph);
        painter.drawPixmap(m_fontBorder, 0, m_decibels);
    }
    else {
        painter.drawPixmap(0, 0, m_frequencies);
        painter.drawImage(0, m_frequencies.height(), m_spectrumGraph);
        painter.drawPixmap(m_fontBorder, m_frequencies.height(), m_decibels);
    }
}

void SpectrumGraph::beginDraw(QPainter& painter)
{
    //painter.drawPixmap(0, 0, m_background);
}

void SpectrumGraph::drawSpectrum(QPainter& painter, const QRect& rect)
{
    painter.drawImage(rect, m_spectrumGraph);
}

QRect SpectrumGraph::drawFrequencies(QPainter& painter)
{
    return QRect();
    /*if (m_frequencies.isNull())
        return QRect();

    painter.drawPixmap(0, 0, m_frequencies);
    return QRect(m_frequencies.rect());*/
}

void SpectrumGraph::drawFrequencies()
{
}

void SpectrumGraph::drawMarkers(QPainter& painter, qreal frequency, const QColor& frqCol, const QColor& bwCol)
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

void SpectrumGraph::sizeChanged(const QSize& size)
{
    QSize spectrumSize = size;

    // create frequencies and adapt the spectrum size
    if (m_showFrequencies) {
        m_frequencies = QPixmap(size.width(), 30);
        spectrumSize.setHeight(size.height() - m_frequencies.height());
    }

    if (spectrumSize.width() >= 0 && spectrumSize.height() >= 0) {
        // create new spectrograph image
        m_spectrumGraph = QImage(spectrumSize, QImage::Format_RGB32);

        // draw background gradient
        drawBackground(spectrumSize);

        // draw grid and retrieve step sizes needed for text drawing
        int frqStep = 100;
        int dbStep = 10;
        int dbTextWidth = 0;
        drawGrid(spectrumSize, frqStep, dbStep, dbTextWidth);

        // draw frequencies if required
        if (m_showFrequencies) {
            drawFrequencies(frqStep);
        }

        // draw decibel labels
        drawDecibels(dbStep, dbTextWidth);
    }
}

void SpectrumGraph::drawBackground(const QSize& size)
{
    if (size.width() <= 0 || size.height() <= 0)
        return;

    // draw background gradient
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

void SpectrumGraph::drawGrid(const QSize& size, int& frqStep, int& dbStep, int& dbTextWidth)
{
    if (size.width() <= 0 || size.height() <= 0)
        return;

    QPainter painter(&m_background);

    // set font
    QFont font = painter.font();
    font.setPixelSize(m_fontSize);
    painter.setFont(font);
    const int fontHeight = painter.fontMetrics().height();
    const int maxHeight = m_fontBorder * 2 + fontHeight;

    // set line color
    painter.setPen(m_gridColor);

    QVector<int> dbSteps;
    dbSteps.push_back(5);
    dbSteps.push_back(10);
    dbSteps.push_back(20);
    dbSteps.push_back(50);

    // determine space between two db lines
    dbStep = dbSteps.last();
    for (int i = 0; i < dbSteps.size(); i++) {
        int lineSpace = valueToDisp(m_refLevel - dbSteps[i]);
        if (lineSpace > maxHeight) {
            dbStep = dbSteps[i];
            break;
        }
    }

    // draw decibel grid on y-axis
    for (int i = m_refLevel; i >= m_refLevel - m_ampSpan; i -= dbStep) {
        double dispValue = valueToDisp(i);
        QPointF start(0, dispValue);
        QPointF end(size.width(), dispValue);
        painter.drawLine(start, end);
    }

    QVector<int> frqSteps;
    frqSteps.push_back(100);
    frqSteps.push_back(200);
    frqSteps.push_back(500);
    frqSteps.push_back(1000);

    // determine space between two frequency lines
    frqStep = frqSteps.last();
    for (int i = 0; i < frqSteps.size(); i++) {

        double maxWidth = 0;
        int count = 0;
        for (int j = 0; j < m_upperPassband; j += frqSteps[i]) {
            if (j > m_lowerPassband) {
                QString text = QString::number(j);
                double halfWidth = painter.fontMetrics().width(text) / 2.0;
                maxWidth += halfWidth + m_fontBorder;
                count++;
            }
        }
        maxWidth = (maxWidth * 2) / count;

        int lineSpace = frqToScreen(frqSteps[i]);
        if (lineSpace > maxWidth) {
            frqStep = frqSteps[i];
            break;
        }
    }

    // draw frequencies grid on x-axis
    for (int i = 0; i < m_upperPassband; i += frqStep) {
        if (i > m_lowerPassband) {
            double frq = frqToScreen(i);
            QPointF start(frq, 0);
            QPointF end(frq, size.height());
            painter.drawLine(start, end);
        }
    }

    // get maximum text width of decibel labels
    dbTextWidth = 0;
    for (int i = m_refLevel; i >= m_refLevel - m_ampSpan; i -= dbStep) {
        QString text = QString::number(i);
        text += QLatin1String(" db");
        int width = painter.fontMetrics().width(text);
        if (width > dbTextWidth)
            dbTextWidth = width;
    }
}

void SpectrumGraph::drawFrequencies(int frqStep)
{
    if (!m_frequencies.isNull()) {
        m_frequencies.fill(Qt::black);

        QPainter painter(&m_frequencies);

        // set font
        QFont font = painter.font();
        font.setPixelSize(m_fontSize);
        painter.setFont(font);
        const int fontAscent = painter.fontMetrics().ascent();
        const int fontHeight = painter.fontMetrics().height();

        // draw frequency labels
        for (int i = 0; i < m_upperPassband; i += frqStep) {
            if (i > m_lowerPassband) {
                double frq = frqToScreen(i);
                QString text = QString::number(i);
                double pos = frq - painter.fontMetrics().width(text) / 2.0;

                // draw text
                painter.setPen(m_fontColor);
                painter.drawText(QPoint(pos, fontAscent + m_fontBorder), text);

                // draw line
                painter.setPen(m_gridColor);
                QPointF start(frq, m_fontBorder * 2 + fontHeight);
                QPointF end(frq, m_frequencies.height());
                painter.drawLine(start, end);
            }
        }
    }
}

void SpectrumGraph::drawDecibels(int dbStep, int dbTextWidth)
{
    m_decibels = QPixmap(dbTextWidth, m_spectrumGraph.height());
    m_decibels.fill(Qt::transparent);

    QPainter painter(&m_decibels);
    painter.setPen(m_fontColor);

    // set font
    QFont font = painter.font();
    font.setPixelSize(m_fontSize);
    painter.setFont(font);
    const int fontAscent = painter.fontMetrics().ascent();

    for (int i = m_refLevel; i >= m_refLevel - m_ampSpan; i -= dbStep) {
        double disp = valueToDisp(i);
        QString text = QString::number(i);
        text += QLatin1String(" db");
        painter.drawText(QPoint(0, disp + fontAscent + m_fontBorder), text);
    }
}

void SpectrumGraph::bresenham(QImage& image, QPoint point1, QPoint point2, const QColor& lineCol, const QColor& fillCol)
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
    float fillAlpha = 0;
    float fillAlphaInv = 1;
    if (fillCol.isValid()) {
        fill = qRgba(fillCol.red(), fillCol.green(), fillCol.blue(), fillCol.alpha());
        fillAlpha = qAlpha(fill) / 255.0;
        fillAlphaInv = 1 - fillAlpha;
    }
    // premultiplied color
    int fillRed = fillAlpha * qRed(fill);
    int fillGreen = fillAlpha * qRed(fill);
    int fillBlue = fillAlpha * qRed(fill);

    QRgb line = 0;
    if (lineCol.isValid())
        line = qRgba(lineCol.red(), lineCol.green(), lineCol.blue(), lineCol.alpha());

    while((point1.x() != point2.x()) || (point1.y() != point2.y())) {
        if (point1.x() >= 0 && point1.x() < image.width()) {

            if (point1.y() >= 0 && point1.y() < image.height()) {
                // draw a bresenham line
                if (lineCol.isValid())
                    (data + width * point1.y())[point1.x()] = line;

                // fill the area under the line
                if (fillCol.isValid()) {
                    int y = point1.y() < 0 ? -1 : point1.y();

                    if (fillAlpha == 1) {
                        // only fill if not already filled before
                        if ((data + width * (image.height() - 1))[point1.x()] != fill) {
                            for (int i = y; i < image.height(); i++) {
                                (data + width * i)[point1.x()] = fill;
                            }
                        }
                    }
                    else {
                        // only fill if not already filled before
                        if (qAlpha((data + width * (image.height() - 1))[point1.x()]) == 255) {
                            for (int i = y; i < image.height(); i++) {
                                QRgb val = (data + width * i)[point1.x()];
                                val = qRgba(fillRed + fillAlphaInv * qRed(val),
                                            fillGreen + fillAlphaInv * qGreen(val),
                                            fillBlue + fillAlphaInv * qBlue(val),
                                            254);   // hack: to notify the case when the line has already been filled
                                (data + width * i)[point1.x()] = val;
                            }
                        }
                    }
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

#include <QTime>
#include <QDebug>
QTime t;

void SpectrumGraph::iRedraw()
{
    if (m_spectrum.size() <= 2)
        return;

    t.restart();

    if (!m_spectrumGraph.isNull()) {
        // draw background image
        m_spectrumGraph.fill(Qt::transparent);
        QPainter painter(&m_spectrumGraph);

        if (!m_background.isNull()) {
            painter.drawPixmap(0, 0, m_spectrumGraph.width(), m_spectrumGraph.height(), m_background);
        }

        // compute array indices for lower and upper passband frequencies
        int lower = m_lowerPassband / m_binSize;
        lower = lower < 0 ? 0 : lower;
        int upper = m_upperPassband / m_binSize;
        const int spectrumSize = m_spectrum.size();
        upper = upper > spectrumSize ? spectrumSize : upper;

        // fill with background color
        QPoint prevPoint;
        for (int j = 0; j < m_spectrumGraph.width(); j++) {
            int index = ((j / (double)m_spectrumGraph.width()) * (upper - lower)) + lower;
            const double& value = m_spectrum[index];
            QPoint point(j, (int)valueToDisp(value));

            if (!prevPoint.isNull())
                bresenham(m_spectrumGraph, prevPoint, point, QColor(), m_spectrumFillColor);
            prevPoint = point;
        }

        // draw spectrum line
        prevPoint = QPoint();
        for (int j = 0; j < m_spectrumGraph.width(); j++) {
            int index = ((j / (double)m_spectrumGraph.width()) * (upper - lower)) + lower;
            const double& value = m_spectrum[index];
            QPoint point(j, (int)valueToDisp(value));

            if (!prevPoint.isNull())
                bresenham(m_spectrumGraph, prevPoint, point, m_spectrumLineColor, QColor());
            prevPoint = point;
        }

        /*if (!m_labels.isNull())
            painter.drawPixmap(0, 0, m_spectrumGraph.width(), m_spectrumGraph.height(), m_labels);*/
        painter.end();
    }

    //qDebug() << t.elapsed();
}

double SpectrumGraph::valueToDisp(double value) const
{
    value = (m_refLevel - value) / m_ampSpan;
    value *= m_spectrumGraph.height();

    // clamp (important for performance issues)
    value = value < 0 ? -1 : value;
    value = value >= m_spectrumGraph.height() ? m_spectrumGraph.height() - 1 : value;

    return value;
}

void SpectrumGraph::iAddSpectrumLog(const QVector<double>& spectrum, bool changed)
{
    m_spectrum = spectrum;

    if (changed) {
        reset();
        //drawFrequencies();
    }

    requestRedraw();
}
