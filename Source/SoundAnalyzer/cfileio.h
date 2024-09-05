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

#ifndef CFILEIO_H
#define CFILEIO_H
#include <string>
#include <thread>
#include <atomic>
#include <fstream>
class CInterThreadQueue;

enum WavChunks {
    RiffHeader = 0x46464952,
    WavRiff = 0x54651475,
    Format = 0x020746d66,
    LabeledText = 0x478747C6,
    Instrumentation = 0x478747C6,
    Sample = 0x6C706D73,
    Fact = 0x47361666,
    Data = 0x61746164,
    Junk = 0x4b4e554a,
};

enum WavFormat {
    PulseCodeModulation = 0x01,
    IEEEFloatingPoint = 0x03,
    ALaw = 0x06,
    MuLaw = 0x07,
    IMAADPCM = 0x11,
    YamahaITUG723ADPCM = 0x16,
    GSM610 = 0x31,
    ITUG721ADPCM = 0x40,
    MPEG = 0x50,
    Extensible = 0xFFFE
};

#define RIFFSTYLE 0x45564157

typedef unsigned short int16;
typedef unsigned int   int32;

class CFileIO
{
public:
    CFileIO();
    ~CFileIO();
    int readHeader(std::string name); //0 success, -1 fail
    void read();
    void write(std::string name);
    void stop();
    void setInputQueue(CInterThreadQueue* queue) {m_inputQueue = queue;}
    void setOutputQueue(CInterThreadQueue* queue) {m_outputQueue = queue;}
    int  channels() const { return m_iChannels; }
    void setChannels(int channels) { m_iChannels = channels; }
    int  bytesperSample() const { return m_iBytesPerSample; }
    void setBytesPerSample(int bytesPerSample) { m_iBytesPerSample = bytesPerSample; }
    int  sampleFrequency() const { return m_iSampleFrequency; }
    void setSampleFrequency(int sampleFrequency) { m_iSampleFrequency = sampleFrequency; }
    int  length() const { return m_iLength; }
    void setLength(int length) { m_iLength =length; }
    bool isRunning();

protected:

    void doRead(
            std::ifstream* pstream,
            CInterThreadQueue* outputQueue,
            int32 segmentLength,
            int32 datasize,
            int32 channels,
            int32 bitdepth
            );
    void doWrite(
            std::ofstream* pstream,
            CInterThreadQueue* inputQueue,
            int32 segmentLength,
            int32 channels,
            int32 samplerate,
            int32 bitdepth
            );
    CInterThreadQueue* m_inputQueue;
    CInterThreadQueue* m_outputQueue;
    std::atomic_bool   m_bRunning;
    std::thread        m_thread;

    int                m_iChannels;
    int                m_iBytesPerSample;
    int                m_iSampleFrequency;
    int                m_iLength;
    int                m_iDataSize;
    std::ifstream*     m_ifstream;

};

#endif // CFILEIO_H
