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


#include "cbargraph.h"

CBarGraph::CBarGraph()
    : m_color( Qt::blue ), m_alternateColor(Qt::transparent), m_fMinimum(-1.0), m_fMaximum(1.0),
      m_iStride( 0 ), m_iBytesPerSample( 2 ),
      m_iOffset( 0 ), m_iLength( 0 ), m_inputQueue( nullptr ),
      m_renderer(0),m_timer(this),m_master(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &CBarGraph::handleWindowChanged);
}


void CBarGraph::setInputQueue(CInterThreadQueue* queue)
{
    if (m_renderer) m_renderer->resetInputQueue();
    m_inputQueue = queue;
}

void CBarGraph::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &CBarGraph::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &CBarGraph::cleanup, Qt::DirectConnection);
        connect(&m_timer, &QTimer::timeout, this , &CBarGraph::tick );
        m_timer.start(0);
        //connect(win, &QQuickWindow::sceneGraphInvalidated, win, &QWindow::requestUpdate);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);


    }
}


void CBarGraph::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

void CBarGraph::setVisible(bool newval)
{

    QQuickItem::setVisible( newval);
    if ( m_renderer && window() ) {
        if ( newval ) {

            disconnect(window(), &QQuickWindow::afterRendering, m_renderer, &CBarGraphRenderer::paint);
            connect(window(), &QQuickWindow::afterRendering, m_renderer, &CBarGraphRenderer::paint, Qt::DirectConnection);

        }
        else {

            disconnect(window(), &QQuickWindow::afterRendering, m_renderer, &CBarGraphRenderer::paint);
        }
    }
}

void CBarGraph::sync()
{
    if ( !isVisible() ) return;
    if (!m_renderer && isVisible()) {
        m_renderer = new CBarGraphRenderer();
        connect(window(), &QQuickWindow::afterRendering, m_renderer, &CBarGraphRenderer::paint, Qt::DirectConnection);
    }
    if (m_master) m_renderer->setParentBarGraphRenderer(m_master->getRenderer());
    QPoint qp = mapToItem(nullptr,QPoint(0.0,0.0)).toPoint();
    m_renderer->setItemOrigin( qp * window()->devicePixelRatio());
    m_renderer->setItemSize( QSize(width(),height()) * window()->devicePixelRatio());
    m_renderer->setViewportSize( window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());

    m_renderer->setColor( m_color );
    m_renderer->setAlternateColor(m_alternateColor);
    m_renderer->setMinimum(m_fMinimum);
    m_renderer->setMaximum(m_fMaximum);
    m_renderer->setStride( m_iStride );
    m_renderer->setBytesPerSample( m_iBytesPerSample );
    m_renderer->setOffset( m_iOffset );
    m_renderer->setLength( m_iLength );
    m_renderer->setInputQueue( m_inputQueue );

}


QVariant CBarGraph::getApplicationInitializeFailString()
{
    if ( m_renderer ) return m_renderer->getApplicationInitializeFailString();
    return QVariant();
}
