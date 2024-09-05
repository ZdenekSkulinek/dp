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


#ifndef COPENCLGOERTZEL_H
#define COPENCLGOERTZEL_H
#include <thread>
#include <atomic>
#include <cstats.h>

class CInterThreadQueue;
class COpenClGoertzelData;

#define MAX_BITS_SEGMENT_SIZE 14

class COpenClGoertzel
{

public:
    COpenClGoertzel();
    ~COpenClGoertzel();


    void start();
    void stop();
    void computeOneFile(std::string filename, CInterThreadQueue* amplitudeQueue, CInterThreadQueue* phaseQueue);

    int  channels() const { return m_iChannels; }
    void setChannels(int channels) { m_iChannels = channels; }
    int  bytesperSample() const { return m_iBytesPerSample; }
    void setBytesPerSample(int bytesPerSample) { m_iBytesPerSample = bytesPerSample; }
    int  sampleFrequency() const { return m_iSampleFrequency; }
    void setSampleFrequency(int sampleFrequency) { m_iSampleFrequency = sampleFrequency; }
    int  length() const { return m_iLength; }
    void setLength(int length) { m_iLength =length; }
    int  overlap() const { return m_iOverlap; }
    void setOverlap(int overlap) { m_iOverlap = overlap; }
    bool useCpu() const { return m_bUseCpu;}
    void setUseCpu(bool cpu) { m_bUseCpu = cpu; }
    void setInputQueue(CInterThreadQueue* queue) {m_inputQueue = queue;}
    void setOutputQueue(CInterThreadQueue* queue) {m_outputQueue = queue;}
    void setOutputQueuePhase(CInterThreadQueue* queue) {m_outputQueuePhase = queue;}
    int setFrequencyIndexes(std::string strIndexes);
    int* frequencyIndexes() {return m_arrFrequencyIndexes;}
    int  frequencies() {return m_iFrequencies;}
    bool isRunning();

    static void mutliplyMatrixes22(double* dest, double* a, double*b);
    static void powerOfA(double* dest, double* aPowers, unsigned int power);
    static size_t divideGlobals(size_t* global, size_t* local, int length ,int items);
protected:
    void doGoertzel(COpenClGoertzelData *data, float *oneShotData, float *oneShotResult, float *oneShotResultPhase);
    COpenClGoertzelData* computeData();
    void getInfo();

    COpenClGoertzelData* m_data;

    std::atomic<CInterThreadQueue*> m_inputQueue;
    std::atomic<CInterThreadQueue*> m_outputQueue;
    std::atomic<CInterThreadQueue*> m_outputQueuePhase;
    std::atomic_bool   m_bRunning;
    std::thread        m_thread;

    int                m_iChannels;
    int                m_iBytesPerSample;
    int                m_iSampleFrequency;
    unsigned int       m_iLength;
    int                m_iOverlap;
    unsigned int       m_iPrefferedVectorWidth;
    int                m_iFrequencies;
    int*               m_arrFrequencyIndexes;
    bool               m_bUseCpu;
    CStats             m_stats;
};

#endif // COPENCLGOERTZEL_H
