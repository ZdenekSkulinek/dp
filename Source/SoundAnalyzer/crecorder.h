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


#ifndef CRECORDER_H
#define CRECORDER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <cstats.h>
#include <thread>
#include <atomic>
#include <list>
#include <string>
#include <cstring>

class CInterThreadQueue;

class CRecorder
{
    ALint error;


    CInterThreadQueue* m_oQueue;
    CInterThreadQueue* m_inputQueue;
    CInterThreadQueue* m_monitorQueue;
    int         m_iChannels;
    int         m_iBytesPerSample;
    int         m_iSampleFrequency;
    int         m_iLength;
    ALchar      m_sDeviceName[256];
    ALchar      m_sCaptureDeviceName[256];
    CStats      m_stats;

    std::thread             m_thread;
    std::atomic_bool        m_bRunning;
    std::atomic_bool        m_bPaused;

public:
    CRecorder();
    ~CRecorder();



    void start();
    void play();
    void stop();
    void pause();
    bool isRunning();
    bool isStereo();
    bool isPaused();

    int  channels() const { return m_iChannels; }
    void setChannels(int channels) { m_iChannels = channels; }
    int  bytesperSample() const { return m_iBytesPerSample; }
    void setBytesPerSample(int bytesPerSample) { m_iBytesPerSample = bytesPerSample; }
    int  sampleFrequency() const { return m_iSampleFrequency; }
    void setSampleFrequency(int sampleFrequency) { m_iSampleFrequency = sampleFrequency; }
    int  length() const { return m_iLength; }
    void setLength(int length) { m_iLength =length; }
    std::string device() const { return std::string(m_sDeviceName);}
    void setDevice(std::string device) {strncpy(m_sDeviceName, device.c_str(),256);}
    std::string captureDevice() const { return std::string(m_sCaptureDeviceName);}
    void setCaptureDevice(std::string device) {strncpy(m_sCaptureDeviceName, device.c_str(),256);}
    CInterThreadQueue* queue() { return m_oQueue;}
    void setQueue(CInterThreadQueue* queue) { m_oQueue = queue;}
    CInterThreadQueue* monitorQueue() { return m_monitorQueue;}
    void setMonitorQueue(CInterThreadQueue* queue) { m_monitorQueue = queue;}
    CInterThreadQueue* inputQueue() { return m_inputQueue;}
    void setInputQueue(CInterThreadQueue* queue) { m_inputQueue = queue;}

    static ALenum to_al_format(short channels, short samples);

    std::list<std::string> getDeviceList() const { return list_audio_devices(ALC_DEVICE_SPECIFIER);}
    std::list<std::string> getCaptureDeviceList() const { return list_audio_devices(ALC_CAPTURE_DEVICE_SPECIFIER);}
    std::string getDefaultDevice() const { return std::string(alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));}
    std::string getDefaultCaptureDevice() const { return std::string(alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));}

protected:

    void doRecord(          ALenum format,
                            int sampleFrequency,
                            int length,
                            ALchar* device,
                            ALchar* captureDevice,
                            CInterThreadQueue* queue,
                            CInterThreadQueue* monitorQueue);
    void doPlay(          ALenum format,
                            int sampleFrequency,
                            int length,
                            ALchar* device,
                            ALchar* outputDevice,
                            CInterThreadQueue* queue,
                            CInterThreadQueue* inputQueue);
    std::list<std::string> list_audio_devices( int specifier) const;


};

#endif // CRECORDER_H
