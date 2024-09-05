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


#ifndef CBARGRAPHRENDERER_H
#define CBARGRAPHRENDERER_H

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QQuickWindow>
#include "cinterthreadqueue.h"

class CBarGraphRenderer : public QObject, protected QOpenGLFunctions
{
public:
    CBarGraphRenderer() : m_program(-1),m_fragmentShader(-1),m_vertexShader(-1),
        m_majorVersion(-1),m_minorVersion(-1),
        m_fminimum(-1.0f),m_fmaximum(1.0f),m_oldData(nullptr),m_arrCoords(nullptr),
        m_parent(nullptr)
        { }
    ~CBarGraphRenderer();

    void setColor(QColor color) { m_color = color; }
    void setAlternateColor(QColor color) { m_alternateColor = color; }
    void setMinimum(float minimum) { m_fminimum = minimum; };
    void setMaximum(float maximum) { m_fmaximum = maximum; };
    void setStride(int stride) { m_iStride = stride; }
    void setBytesPerSample(int bytesPerSample) { m_iBytesPerSample = bytesPerSample; }
    void setOffset(int offset) { m_iOffset = offset; }
    void setLength(int length);
    void setInputQueue(CInterThreadQueue* queue);
    void resetInputQueue();
    unsigned char* getDataForMonitor();
    void returnDataForMonitor(unsigned char* data);
    void setParentBarGraphRenderer(CBarGraphRenderer* parent) {m_parent = parent;}

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setItemSize(const QSize &size) { m_itemSize = size; }
    void setItemOrigin(const QPoint &origin) {m_itemOrigin = origin; };
    void setWindow(QQuickWindow *window) { m_window = window; }

    void printShaderInfoLog(GLuint obj);
    void printProgramInfoLog(GLuint obj);
    QVariant getApplicationInitializeFailString();
public slots:
    void paint();

private:

    GLuint m_program;
    GLuint m_fragmentShader;
    GLuint m_vertexShader;
    QSize m_viewportSize;
    QSize m_itemSize;
    QPoint m_itemOrigin;
    GLint m_majorVersion;
    GLint m_minorVersion;

    QColor  m_color;
    QColor  m_alternateColor;
    float   m_fminimum;
    float   m_fmaximum;
    int     m_iStride;
    int     m_iBytesPerSample;
    int     m_iOffset;
    int     m_iLength;
    CInterThreadQueue* m_inputQueue;
    unsigned char*     m_oldData;
    float*             m_arrCoords;


    QQuickWindow *m_window;
    CBarGraphRenderer* m_parent;
};


#endif // CBARGRAPHRENDERER_H
