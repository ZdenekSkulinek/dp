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


#include <AL/alut.h>
#include <string>
#include <cassert>
#include <sstream>
#include <iomanip>
#include "crecorder.h"
#include "cinterthreadqueue.h"
#include "capplication.h"

using namespace std;

#define NUM_BUFFERS 6  //buffers for playing only

#define TEST_ERROR(_msg,_cmd)		\
    error = alGetError();		\
    if (error != AL_NO_ERROR) {	\
        CApplication::displayError("Recorder error",std::string(_msg)+std::string(alutGetErrorString(error)) );	\
        _cmd;\
    \
    }
#define TEST_ERRORC(_msg,_cmd)		\
    error = alcGetError(hCaptureDevice);		\
    if (error != AL_NO_ERROR) {	\
        CApplication::displayError("Recorder error",std::string(_msg)+std::string(alutGetErrorString(error)) );	\
        _cmd;\
    \
    }

CRecorder::CRecorder( ):
    m_oQueue(nullptr),
    m_inputQueue(nullptr),
    m_monitorQueue(nullptr),
    m_iChannels(1),
    m_iBytesPerSample(1),
    m_iSampleFrequency(44100),
    m_iLength(0),
    m_stats(500),
    m_thread(),
    m_bRunning(false),
    m_bPaused(false)
{
    ALboolean enumeration;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) {

        CApplication::displayError("Recorder error","enumeration extension not available",true);
        return;
    }

    setDevice( getDefaultDevice() );
    setCaptureDevice( getDefaultCaptureDevice() );

}


std::list<std::string> CRecorder::list_audio_devices( int specifier) const
{
        const ALCchar *devices = alcGetString(NULL, specifier);
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;
        std::list<std::string> lsDevices;

        while (device && *device != '\0' && next && *next != '\0') {
                lsDevices.push_back( string(device) );
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
        return lsDevices;
}

ALenum CRecorder::to_al_format(short channels, short samples)
{
    bool stereo = (channels > 1);

    switch (samples) {
    case 16:
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 8:
        if (stereo)
            return AL_FORMAT_STEREO8;
        else
            return AL_FORMAT_MONO8;
    default:
        return -1;
    }
}

CRecorder::~CRecorder()
{
    if ( isRunning() ) {

        stop();
    }
}


void CRecorder::start()
{

    if ( isRunning() ) {

        return;
    }

    ALenum format = to_al_format( m_iChannels , 8 * m_iBytesPerSample );
    m_bRunning = true;
    if ( m_thread.joinable() ) m_thread.join();
    if ( ( m_iChannels >= 1 && m_iChannels <=2) &&
         ( m_iBytesPerSample >= 1 && m_iBytesPerSample <= 2) &&
         ( m_iSampleFrequency >=1 && m_iSampleFrequency <= 200000) &&
         ( m_iLength >=1 && m_iLength <= 16384))
    {
        m_thread = std::thread( &CRecorder::doRecord, this,
                                format,
                                m_iSampleFrequency ,
                                m_iLength,
                                m_sDeviceName,
                                m_sCaptureDeviceName,
                                m_oQueue,
                                m_monitorQueue);
        CApplication::displayInfo("Recorder info","Recorder started",DISPLAYINFO_LOG );
    }
    else {

        CApplication::displayWarning("Recorder warning","Bad data from setting" );
    }

}

void CRecorder::play()
{

    if ( isRunning() ) {

        return;
    }

    ALenum format = to_al_format( m_iChannels , 8 * m_iBytesPerSample );
    m_bRunning = true;
    if ( m_thread.joinable() ) m_thread.join();
    if ( ( m_iChannels >= 1 && m_iChannels <=2) &&
         ( m_iBytesPerSample >= 1 && m_iBytesPerSample <= 2) &&
         ( m_iSampleFrequency >=1 && m_iSampleFrequency <= 200000) &&
         ( m_iLength >=1 && m_iLength <= 16384))
    {
        m_thread = std::thread( &CRecorder::doPlay, this,
                                format,
                                m_iSampleFrequency ,
                                m_iLength,
                                m_sDeviceName,
                                m_sCaptureDeviceName,
                                m_oQueue ,
                                m_inputQueue);      
        CApplication::displayInfo("Recorder info","Player started" );
    }
    else {

        CApplication::displayWarning("Recorder warning","Bad data from setting" );
    }

}

void CRecorder::stop()
{

    if ( !isRunning() ) {

        return;
    }
    m_bRunning = false;
    if (m_inputQueue) {

        m_inputQueue->setLast();
    }
    m_thread.join();
    CApplication::displayInfo("Recorder info","Recoder/Player stopped" );
}

void CRecorder::pause()
{

    if ( isPaused() ) {

        m_bPaused = false;
        CApplication::displayInfo("Recorder info","Recorder/Player continued" );
    }
    else{

        m_bPaused = true;
        CApplication::displayInfo("Recorder info","Recorder/Player paused" );
    }
}


bool CRecorder::isRunning()
{
    return m_bRunning;
}


bool CRecorder::isStereo()
{
    return m_iChannels == 2;
}

bool CRecorder::isPaused()
{
    return m_bPaused;
}

void CRecorder::doRecord(   ALenum format,
                            int sampleFrequency,
                            int length,
                            ALchar* device,
                            ALchar* captureDevice,
                            CInterThreadQueue* pqueue,
                            CInterThreadQueue* monitorQueue)
{
    ALCdevice *hDevice;
    ALCdevice *hCaptureDevice;
    ALCcontext *ctx;
    ALint error;
    CInterThreadQueue& queue = *pqueue;
    ALint bytesSample = 1;
    bool refreshStatsText;
    std::string ssstr(256,' ');
    std::stringstream ss;
    ss.str(ssstr);
    int sleepTime = 10;
    switch ( format ) {
    case AL_FORMAT_STEREO16:bytesSample = 4; break;
    case AL_FORMAT_STEREO8:bytesSample = 2; break;
    case AL_FORMAT_MONO16:bytesSample = 2; break;
    case AL_FORMAT_MONO8:bytesSample = 1; break;
    default:
        break;
    }
    ALint bufferLength = length ;
    ALbyte* rbuffer = (ALbyte*) queue.alloc();
    ALint nSamples = 0;
    ALint rSamples = 0;
    bool  lastPaused = false;

    alGetError();
    hDevice = alcOpenDevice(device);
    if ( !hDevice ) {

        TEST_ERROR("OpenAl : open device", goto end);
    }
    ctx = alcCreateContext(hDevice, NULL);
    if ( !ctx ) {

        TEST_ERROR("OpenAl : create context",goto closeDevice);
    }
    alcMakeContextCurrent(ctx);
    {
        TEST_ERROR("OpenAl : make context current",goto destroyContext);
    }




    hCaptureDevice = alcCaptureOpenDevice(captureDevice, sampleFrequency , format , sampleFrequency/2 );

    if ( device == NULL ) {

        TEST_ERRORC("OpenAl : open capture device", goto currentContext);
    }

    m_stats.reset();

    alcCaptureStart(hCaptureDevice);
    TEST_ERRORC("OpenAl : open capture start", goto captureClose);

    while ( m_bRunning ) {

        if ( m_bPaused ) {

            if ( !lastPaused ) {

                if ( rbuffer )
                {
                    if( rSamples >= 0 && rSamples < bufferLength )
                        memset(&rbuffer[rSamples*bytesSample],0,(bufferLength - rSamples)*bytesSample );
                    queue.push((unsigned char*)rbuffer);
                    rbuffer = nullptr;
                }
                alcCaptureStop(hCaptureDevice);
                TEST_ERRORC("OpenAl : capture stop 2", goto captureStop);
                lastPaused = true;
            }
            std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
            continue;
        }
        else {

            if ( lastPaused ) {

                alcCaptureStart(hCaptureDevice);
                TEST_ERROR("OpenAl : open capture start 2", goto captureStop);
                lastPaused = false;
                rbuffer = (ALbyte*) queue.alloc();
            }
        }

        alcGetIntegerv(hCaptureDevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &nSamples);
        TEST_ERRORC("OpenAl : get n of samples", goto captureStop);

        refreshStatsText = m_stats.pushTime(std::chrono::high_resolution_clock::now(),nSamples);

        if ( refreshStatsText ) {

                ss.str(std::string());
                ss<<"Sampling frequency\n"<<std::fixed << std::setprecision( 3 )<< double(m_stats.duration()) / double(m_stats.interval()) * 1000.0 << " kHz"<<std::flush;
                CApplication::displayInfo("",ss.str(),DISPLAYINFO_SAMPLING_FREQUENCY);
        }

        if ( !rbuffer )  {

            rbuffer = (ALbyte*)queue.alloc();
            rSamples = 0;

            std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
            continue;
        }

        while ( rbuffer && (rSamples + nSamples >= bufferLength) ) {

            int samplesToCopy = bufferLength - rSamples;

            alcCaptureSamples(hCaptureDevice, (ALCvoid *)&rbuffer[rSamples*bytesSample], samplesToCopy );
            TEST_ERRORC("OpenAl : capture 1", goto captureStop);

            if ( monitorQueue ) {

                unsigned char* mem = monitorQueue->alloc();
                if ( mem ) {

                    memcpy(mem,rbuffer,bufferLength * bytesSample );
                    monitorQueue->push(mem);
                }
            }

            queue.push((unsigned char*)rbuffer);
            rbuffer = (ALbyte*)queue.alloc();

            nSamples -= samplesToCopy;
            rSamples = 0;

        }
        if ( rbuffer && nSamples != 0 )
        {

            alcCaptureSamples(hCaptureDevice, (ALCvoid *)&rbuffer[rSamples*bytesSample], nSamples );
            TEST_ERRORC("OpenAl : capture 2", goto captureStop);

            rSamples += nSamples;

        }
        std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );

    }

captureStop:
    if ( rbuffer )
    {
        if( rSamples >= 0 && rSamples < bufferLength )
            memset(&rbuffer[rSamples*bytesSample],0,(bufferLength - rSamples)*bytesSample );
        if ( monitorQueue ) {

            unsigned char* mem = monitorQueue->alloc();
            if ( mem ) {

                memcpy(mem,rbuffer,bufferLength * bytesSample );
                monitorQueue->push(mem);
                monitorQueue->setLast();
            }
        }
        queue.push((unsigned char*)rbuffer);

    }

    alcCaptureStop(hCaptureDevice);
    TEST_ERRORC("OpenAl : capture stop", );
captureClose:
    alcCaptureCloseDevice(hCaptureDevice);
    TEST_ERRORC("OpenAl : close capture device",);
currentContext:
    alcMakeContextCurrent(nullptr);
    //TEST_ERROR("OpenAl : make context current to NULL",);
destroyContext:
    alcDestroyContext(ctx);
    //TEST_ERROR("OpenAl : close context",);
closeDevice:
    alcCloseDevice(hDevice);
    //TEST_ERROR("OpenAl : open device",);

end:
    queue.setLast();
    if ( monitorQueue ) monitorQueue->setLast();
    m_bRunning = false;
    CApplication::updateStates();
}

void CRecorder::doPlay(          ALenum format,
                                 int sampleFrequency,
                                 int length,
                                 ALchar* device,
                                 ALchar* /*outputDevice*/,
                                 CInterThreadQueue* queue,
                                 CInterThreadQueue* inputQueue)
{
    ALCdevice *hDevice;
    ALCcontext *ctx;
    ALint error;
    ALint bytesSample = 1;
    int sleepTime = 10;
    bool refreshStatsText;
    std::string ssstr(256,' ');
    std::stringstream ss;
    ss.str(ssstr);
    switch ( format ) {
    case AL_FORMAT_STEREO16:bytesSample = 4; break;
    case AL_FORMAT_STEREO8:bytesSample = 2; break;
    case AL_FORMAT_MONO16:bytesSample = 2; break;
    case AL_FORMAT_MONO8:bytesSample = 1; break;
    default:
        break;
    }
    ALint bufferLength = length * bytesSample;

    bool  lastPaused = false;

    ALuint source, buffers[NUM_BUFFERS];
    unsigned char*outputBuffers[NUM_BUFFERS];

    alGetError();
    hDevice = alcOpenDevice(device);
    if ( !hDevice ) {

        TEST_ERROR("OpenAl : open device", goto end);
    }
    ctx = alcCreateContext(hDevice, NULL);
    if ( !ctx ) {

        TEST_ERROR("OpenAl : create context",goto closeDevice);
    }
    alcMakeContextCurrent(ctx);
    TEST_ERROR("OpenAl : make context current", goto destroyContext);



    alGenBuffers(NUM_BUFFERS, buffers);
    TEST_ERROR("OpenAl : gen buffers",goto currentContext);
    for (int i=1;i<NUM_BUFFERS;i++) {
        int j=i;
        while (j > 0 && buffers[j-1] > buffers[j]) {
            ALuint tmp = buffers[j];
            buffers[j]=buffers[j-1];
            buffers[j-1]=tmp;
            j--;
        }
    }

    alGenSources(1, &source);
    TEST_ERROR("OpenAl : gen source", goto deleteBuffers);

    /* Fill OpenAL buffers */
    for (int i = 0; i < NUM_BUFFERS; i++) {
        unsigned char* mem  = inputQueue->pop();
        if ( !mem ) {

            queue->setLast();
            break;
        }
        alBufferData(buffers[i], format , mem, bufferLength , sampleFrequency);
        TEST_ERROR("OpenAl : inti buffer data", goto deleteSources);

        unsigned char* outputMem = queue->alloc();
        if ( outputMem ) {
            memcpy(outputMem,mem,bufferLength);
        }
        outputBuffers[i] = outputMem;

        inputQueue->free(mem);
    }

    m_stats.reset();

    alSourceQueueBuffers(source, NUM_BUFFERS, buffers);
    TEST_ERROR("OpenAl : source queue buffers", goto deleteSources);
    alSourcePlay(source);
    TEST_ERROR("OpenAl : source play",goto sourceUnqueue);

    while ( m_bRunning ) {

        if ( m_bPaused ) {

            if ( !lastPaused ) {

                alSourceStop(source);
                TEST_ERROR("OpenAl : source stop 2",goto sourceStop);
                lastPaused = true;
            }
            std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
            continue;
        }
        else {

            if ( lastPaused ) {

                alSourcePlay(source);
                TEST_ERROR("OpenAl : source play",goto sourceStop);
                lastPaused = false;
            }
        }

        ALuint buffer;
        ALint val;

        alGetSourcei(source, AL_BUFFERS_PROCESSED, &val);
        if (val <= 0) {
            std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
            continue;
        }
        unsigned char * mem = inputQueue->pop();
        if ( !mem ){
            //end od stream
            queue->setLast();
            CApplication::stopRequest();
            break;
        }

        alSourceUnqueueBuffers(source, 1, &buffer);
        TEST_ERROR("OpenAl : unqueue",goto sourceStop);

        refreshStatsText = m_stats.pushTime(std::chrono::high_resolution_clock::now(),length);

        if ( refreshStatsText ) {

                ss.str(std::string());
                ss<<"Playing frequency\n"<<std::fixed << std::setprecision( 3 )<< double(m_stats.duration()) / double(m_stats.interval()) * 1000.0 << " kHz"<<std::flush;
                CApplication::displayInfo("",ss.str(),DISPLAYINFO_SAMPLING_FREQUENCY);
        }
        int l = 0;
        int r = NUM_BUFFERS-1;
        int i;
        do{
            assert( l <= r );
            i = ( l + r ) >> 1;
            if( buffers[i] < buffer ) {
                l = i+1;
                continue;
            }
            if( buffers[i] > buffer ) {
                r = i-1;
                continue;
            }
            break;
        }while(1);

        if (outputBuffers[i]) {
            queue->push(outputBuffers[i]);
            outputBuffers[i] = nullptr;
        }

        alBufferData(buffer, format , mem, bufferLength , sampleFrequency);
        TEST_ERROR("OpenAl : inti buffer data 2",goto sourceStop);
        alSourceQueueBuffers(source, 1, &buffer);
        TEST_ERROR("OpenAl : source queue buffers 2",goto sourceStop);

        alGetSourcei(source, AL_SOURCE_STATE, &val);
        if (val != AL_PLAYING) {

            alSourcePlay(source);
        }

        unsigned char* outputMem = queue->alloc();
        if ( outputMem ) {

            memcpy(outputMem,mem,bufferLength);
            outputBuffers[i] = outputMem;
        }
        inputQueue->free(mem);

    }
sourceStop:
    alSourceStop(source);
    TEST_ERROR("OpenAl : source stop 3", );
sourceUnqueue:
    alSourceUnqueueBuffers(source,NUM_BUFFERS,buffers);
    TEST_ERROR("OpenAl : unqueue 2 ",);
deleteSources:
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (outputBuffers[i]) {

            queue->free(outputBuffers[i]);
        }
    }
    alDeleteSources(1, &source);
    TEST_ERROR("OpenAl : delete source",);
deleteBuffers:
    alDeleteBuffers(NUM_BUFFERS, buffers);
    TEST_ERROR("OpenAl : delete buffers",);
currentContext:
    alcMakeContextCurrent(nullptr);
    //TEST_ERROR("OpenAl : make context current to NULL",);
destroyContext:
    alcDestroyContext(ctx);
    //TEST_ERROR("OpenAl : close context",);
closeDevice:
    alcCloseDevice(hDevice);
    //TEST_ERROR("OpenAl : open device",);
end:
    m_bRunning=false;
    CApplication::updateStates();
}
