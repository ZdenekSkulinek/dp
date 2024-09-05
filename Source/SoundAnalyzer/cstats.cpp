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


#include "cstats.h"
#include "capplication.h"

CStats::CStats(int size) :
    m_iIndex(0), m_iSize(size), m_iLength(0), m_llDuration(0LL)
{
    m_arrTimes = new std::chrono::high_resolution_clock::time_point[size];
    if ( !m_arrTimes ) {

        CApplication::displayError("Statistics warning","No memory.", true);
    }
    m_arrDurations = new long long[size];
    if ( !m_arrDurations ) {

        delete m_arrTimes;
        CApplication::displayError("Statistics warning","No memory (2).", true);
    }
    m_tLastUpdateTime = std::chrono::high_resolution_clock::now();
}

CStats::~CStats()
{
    delete m_arrDurations;
    delete m_arrTimes;
}

bool CStats::pushTime(std::chrono::high_resolution_clock::time_point beginTime, long long durationUs)
{
    m_arrTimes[m_iIndex] = beginTime;
    if ( m_iLength < m_iSize )
    {
        m_iLength++;
    }
    else{

        m_llDuration -= m_arrDurations[m_iIndex];
    }
    m_arrDurations[m_iIndex] = durationUs;
    m_llDuration += durationUs;
    m_iIndex++;
    if( m_iIndex == m_iSize) {

        m_iIndex = 0;
    }

    long long refreshTime = std::chrono::duration_cast<std::chrono::microseconds>(beginTime-m_tLastUpdateTime).count();

    if ( refreshTime >= UPDATE_TIME_US ) {

        m_tLastUpdateTime = beginTime;
        return true;
    }
    return false;
}

long long CStats::interval()
{
    int pIndex;
    int sIndex;
    pIndex = m_iIndex-1;
    if( pIndex == -1 ) {

        pIndex = m_iSize-1;
    }
    if ( m_iLength != m_iSize )
    {
        if ( m_iLength <= 2) {

            return 0LL;
        }
        sIndex=0;
    }
    else{

        sIndex = m_iIndex;
    }
    return std::chrono::duration_cast<std::chrono::microseconds>(m_arrTimes[pIndex]-m_arrTimes[sIndex]).count();
}

long long CStats::duration()
{
    return m_llDuration;
}


void CStats::reset()
{
    m_iIndex = 0;
    m_iLength = 0;
    m_llDuration = 0LL;
    m_tLastUpdateTime = std::chrono::high_resolution_clock::now();
}
