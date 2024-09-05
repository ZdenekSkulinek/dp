/* SoundAnalyzer
 *
 * This program demonstrates usage of OpenCL for signal processing.
 * Simply computes spectrum of signal with Goertzel algorithm.
 *
 * Copyright (C) 2017 Zdeněk Skulínek
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef CBARGRAPH_H
#define CBARGRAPH_H

#include <QtQuick/QQuickItem>
#include <QTimer>
#include <QTime>

#include "cbargraphrenderer.h"
#include "cinterthreadqueue.h"


class CBarGraph : public QQuickItem, protected QOpenGLFunctions
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor alternateColor READ alternateColor WRITE setAlternateColor NOTIFY alternateColorChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)

public:
    CBarGraph();

    void setColor(QColor color) { m_color = color; emit colorChanged();}
    void setAlternateColor(QColor color) { m_alternateColor = color; emit alternateColorChanged();}
    void setMinimum(qreal minimum) { m_fMinimum = minimum; emit minimumChanged();};
    void setMaximum(qreal maximum) { m_fMaximum = maximum; emit maximumChanged();};
    void setStride(int stride) { m_iStride = stride; }
    void setBytesPerSample(int bytesPerSample) { m_iBytesPerSample = bytesPerSample; }
    void setOffset(int offset) { m_iOffset = offset; }
    void setLength(int length) { m_iLength = length; }
    void setInputQueue(CInterThreadQueue* queue);
    QColor color() const { return m_color;}
    QColor alternateColor() const { return m_alternateColor;}
    qreal minimum() const { return m_fMinimum;}
    qreal maximum() const { return m_fMaximum;}
    int stride() const { return m_iStride;}
    int bytesPerSample() const { return m_iBytesPerSample;}
    int offset() const { return m_iOffset;}
    int length() const { return m_iLength;}
    CInterThreadQueue* inputQueue() const { return m_inputQueue;}

    void setVisible(bool);
    bool isVisible() const { return QQuickItem::isVisible();}

    Q_INVOKABLE QVariant getApplicationInitializeFailString();

    void tick() {

        setFlag(QQuickItem::ItemHasContents, true);
        window()->update();
    }

    CBarGraphRenderer* getRenderer() { return m_renderer;}
    void setParentBarGraph(CBarGraph* parent) {m_master=parent;}


signals:
    void colorChanged();
    void alternateColorChanged();
    void visibleChanged();
    void minimumChanged();
    void maximumChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    QColor  m_color;
    QColor  m_alternateColor;
    qreal   m_fMinimum;
    qreal   m_fMaximum;
    int     m_iStride;
    int     m_iBytesPerSample;
    int     m_iOffset;
    int     m_iLength;
    CInterThreadQueue* m_inputQueue;

    CBarGraphRenderer *m_renderer;
    QTimer m_timer;
    CBarGraph* m_master;
};



#endif // CBARGRAPH_H
