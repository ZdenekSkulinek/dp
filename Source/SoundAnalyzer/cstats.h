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


#ifndef CSTATS_H
#define CSTATS_H

#include <chrono>

#define UPDATE_TIME_US 500000LL

class CStats
{
public:

    CStats(int size);
    ~CStats();

    bool pushTime(std::chrono::high_resolution_clock::time_point beginTime, long long durationUs);
    long long interval();
    long long duration();
    void reset();
    int length() const { return m_iLength;}

protected:
    std::chrono::high_resolution_clock::time_point* m_arrTimes;
    long long *                        m_arrDurations;
    int                                m_iIndex;
    int                                m_iSize;
    int                                m_iLength;
    long long                          m_llDuration;
    std::chrono::high_resolution_clock::time_point m_tLastUpdateTime;
};

#endif // CSTATS_H
