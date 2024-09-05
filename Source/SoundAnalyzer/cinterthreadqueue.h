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


#ifndef CINTERTHREADQUEUE_H
#define CINTERTHREADQUEUE_H

#include <mutex>
#include <condition_variable>

struct SInterThreadQueueItem
{
    unsigned char*          value;
    SInterThreadQueueItem*  next;
};

class CInterThreadQueue
{
public:
    CInterThreadQueue();
    void  push(unsigned char* data);
    unsigned char* pop();
    unsigned char* peek();
    void setLast();

    unsigned char* alloc();
    void free(unsigned char* mem);
    void reset(int buffers, int sizeOfEach);
    ~CInterThreadQueue();
    int buffers() { return m_iBuffers;}
    int segmentSize() { return m_iSizeOfEach;}

protected:
    bool                    m_bLastSegment;
    int                     m_iBuffers;
    int                     m_iSizeOfEach;
    int                     m_iSizeAligned;
    unsigned char*          m_pMemory;
    SInterThreadQueueItem*  m_pItems;
    SInterThreadQueueItem*  m_pFirstFree;
    SInterThreadQueueItem*  m_pFirstWaiting;
    SInterThreadQueueItem*  m_pLastWaiting;
    mutable std::mutex      m_mutex;
    std::condition_variable m_cv;
};

#endif // CINTERTHREADQUEUE_H
