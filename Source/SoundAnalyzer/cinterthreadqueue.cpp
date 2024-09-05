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


#include "cinterthreadqueue.h"
#include "capplication.h"
#include <new>
#include <thread>
#include <sstream>

#define OUT_OF_POOL (SInterThreadQueueItem *)(-2)



CInterThreadQueue::CInterThreadQueue() :
    m_bLastSegment(false),m_iBuffers(0), m_iSizeOfEach(0), m_iSizeAligned(0), m_pMemory(nullptr), m_pItems(nullptr),
    m_pFirstFree(nullptr), m_pFirstWaiting(nullptr),  m_pLastWaiting(nullptr)
{
}


void  CInterThreadQueue::push(unsigned char* data)
{
    {
        std::lock_guard<std::mutex> lock( m_mutex );

        if (m_bLastSegment) {

            free(data);
            return;
        }

        int mmdiff = ((unsigned char *)(data) - m_pMemory );
        int index = mmdiff / m_iSizeAligned;
        if ( (mmdiff % m_iSizeAligned != 0) || index<0 || index>=m_iBuffers ) {

            std::stringstream ss;
            ss<<"Bad pointer -"<<data<<" ("<<this<<")";
            CApplication::displayWarning("Inter thread queue",ss.str());
            return;
        }
        if ( m_pItems[index].next != OUT_OF_POOL ) {

            std::stringstream ss;
            ss<<"Trying push non given pointer-"<<data<<" ("<<this<<")";
            CApplication::displayWarning("Inter thread queue",ss.str());
            return;
        }
        m_pItems[index].next = nullptr;
        if ( m_pFirstWaiting ) {

            m_pLastWaiting->next = &m_pItems[index];
        }
        else {

            m_pFirstWaiting = &m_pItems[index];
        }
        m_pLastWaiting =  &m_pItems[index];
    }
    m_cv.notify_one();
}

void CInterThreadQueue::setLast()
{

    std::lock_guard<std::mutex> lock( m_mutex );
    m_bLastSegment = true;
    m_cv.notify_one();
}

unsigned char* CInterThreadQueue::pop()
{

    std::unique_lock<std::mutex> lock( m_mutex );
    while ( !m_pFirstWaiting ) {

        if ( m_iBuffers == 0 || m_bLastSegment) {

            return nullptr;
        }
        m_cv.wait( lock );
    }

    SInterThreadQueueItem* p = m_pFirstWaiting;
    m_pFirstWaiting = p->next;
    p->next = OUT_OF_POOL;
    return p->value;
}


unsigned char* CInterThreadQueue::peek()
{

    std::unique_lock<std::mutex> lock( m_mutex );
    if ( !m_pFirstWaiting ) {

        return nullptr;
    }

    SInterThreadQueueItem* p = m_pFirstWaiting;
    m_pFirstWaiting = p->next;
    p->next = OUT_OF_POOL;
    return p->value;
}


unsigned char* CInterThreadQueue::alloc()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if ( m_bLastSegment ) {

        return nullptr;
    }

    if ( !m_pFirstFree ) {

        return nullptr;
    }
    SInterThreadQueueItem* p = m_pFirstFree;
    m_pFirstFree = m_pFirstFree->next;
    p->next = OUT_OF_POOL;
    return p->value;

}


void CInterThreadQueue::free(unsigned char* mem)
{
    std::lock_guard<std::mutex> lock( m_mutex );

    int mmdiff = ((unsigned char *)(mem) - m_pMemory );
    int index = mmdiff / m_iSizeAligned;
    if ( (mmdiff % m_iSizeAligned != 0) || index<0 || index>=m_iBuffers ) {

        std::stringstream ss;
        ss<<"Bad pointer -"<<mem<<" ("<<this<<")";
        CApplication::displayWarning("Inter thread queue",ss.str());
        return;
    }
    if ( m_pItems[index].next != OUT_OF_POOL ) {

        std::stringstream ss;
        ss<<"Trying return non given pointer-"<<mem<<" ("<<this<<")";
        CApplication::displayWarning("Inter thread queue",ss.str());
        return;
    }
    m_pItems[index].next = m_pFirstFree;
    m_pFirstFree = &m_pItems[index];
}


void CInterThreadQueue::reset(int buffers, int sizeOfEach)
{

    std::lock_guard<std::mutex> lock( m_mutex );

    int msec = 2000;
    do {
        int cnt = 0;
        SInterThreadQueueItem *p = m_pFirstFree;
        while( p ) {

            cnt++;
            p= p->next;
        }
        p = m_pFirstWaiting;
        while( p ) {

            cnt++;
            p= p->next;
        }
        if ( cnt != m_iBuffers ) {

            if ( msec > 0 ) {

                msec -= 20;
                m_mutex.unlock();
                std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
                m_mutex.lock();
                continue;
            }
            std::stringstream ss;
            ss<<"Reset while some packet is out ("<<this<<")";
            CApplication::displayWarning("Inter thread queue",ss.str());
            return;
        }
        else {
            break;
        }
    }while ( msec<=0 );
    if ( m_pMemory ) {

        delete m_pMemory;
        m_pMemory = nullptr;
    }
    if ( m_pItems ) {

        delete m_pItems;
        m_pItems = nullptr;
    }
    m_iBuffers = buffers;
    m_iSizeOfEach = sizeOfEach;
    m_iSizeAligned = ( m_iSizeOfEach + 7 ) & ~7;
    m_bLastSegment = false;
    if ( buffers == 0 ) {

        //unblock waited
        m_cv.notify_one();
        return;
    }
    m_pItems = new (std::nothrow) SInterThreadQueueItem[buffers];
    m_pMemory = new (std::nothrow) unsigned char[buffers * m_iSizeAligned];
    if ( !m_pItems || !m_pMemory) {

        std::stringstream ss;
        ss<<"Queue-pool No memory ("<<this<<")";
        CApplication::displayWarning("Inter thread queue",ss.str());
        m_mutex.unlock();
        reset(0,0);
        m_mutex.lock();
        return;
    }
    for (int i = 0 ; i < buffers ; i++ ) {

        m_pItems[i].value = m_pMemory + i * m_iSizeAligned;
        if ( i == buffers - 1 ) m_pItems[i].next = nullptr;
        else m_pItems[i].next = &m_pItems[i+1];
    }
    m_pFirstFree = m_pItems;
    m_pFirstWaiting = nullptr;
    m_pLastWaiting = nullptr;
}

CInterThreadQueue::~CInterThreadQueue()
{
    reset( 0, 0 );
}

