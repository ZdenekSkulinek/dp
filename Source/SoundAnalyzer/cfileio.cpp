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


#include "cfileio.h"
#include "cinterthreadqueue.h"
#include "capplication.h"
#include <fstream>
#include <thread>
#include <cassert>

CFileIO::CFileIO():
    m_inputQueue(nullptr),
    m_outputQueue(nullptr),
    m_bRunning(false),
    m_thread(),
    m_iChannels(1),
    m_iBytesPerSample(1),
    m_iSampleFrequency(44100),
    m_iLength(512),
    m_iDataSize(0),
    m_ifstream(nullptr)
{

}


CFileIO::~CFileIO()
{    
    stop();
}


bool CFileIO::isRunning()
{
    return m_bRunning;
}


int CFileIO::readHeader(std::string name)
{
    assert(m_ifstream==nullptr);
    m_ifstream = new std::ifstream();
    if ( !m_ifstream ) {

        CApplication::displayError("Wav reading","I can't allocate streeam, memory full?");
        return -1;
    }

    int32 formatsize;
    int16 format;
    int16 channels;
    int32 samplerate;
    int32 bitspersecond;
    int16 formatblockalign;
    int16 bitdepth;
    int32 memsize;
    int32 riffstyle;
    int32 datasize;

    m_ifstream->open(name,std::ifstream::in | std::ifstream::binary);
    if ( !(*m_ifstream) ) {

        CApplication::displayError("Wav reading","Open, bad rights?");
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }

    int32 chunkid = 0;
    bool datachunk = false;
    while ( !datachunk ) {
        m_ifstream->read((char*)&chunkid,4);
        switch ( (WavChunks)chunkid ) {
        case WavChunks::Format:
            m_ifstream->read((char*)&formatsize,4);//16
            m_ifstream->read((char*)&format,2);//1-pcm
            m_ifstream->read((char*)&channels,2);
            m_ifstream->read((char*)&samplerate,4);
            m_ifstream->read((char*)&bitspersecond,4);//bytes
            m_ifstream->read((char*)&formatblockalign,2);//2
            m_ifstream->read((char*)&bitdepth,2);
            if ( formatsize == 18 ) {
                int32 extradata;
                m_ifstream->read((char*)&extradata,4);
                m_ifstream->seekg( extradata, std::ios::cur );
            }
            break;
        case WavChunks::RiffHeader:
            //headerid = chunkid;
            m_ifstream->read((char*)&memsize,4);//datasize +36
            m_ifstream->read((char*)&riffstyle,4);
            break;
        case WavChunks::Data:
            datachunk = true;
            m_ifstream->read((char*)&datasize,4);
            break;
        default:
            int32 skipsize;
            m_ifstream->read((char*)&skipsize,4);
            m_ifstream->seekg( skipsize, std::ios::cur );
            break;
        }
    }

    if ( !(*m_ifstream) ) {

        CApplication::displayError("Wav reading","Header corrupted, is Wav file?");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if ( formatsize!=16 && formatsize!=18 ) {

        CApplication::displayError("Wav reading","Invalid formatsize");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if ( channels!=1 && channels!=2 ) {

        CApplication::displayError("Wav reading","Invalid channels");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if ( format!=1 ) {//PCM

        CApplication::displayError("Wav reading","Invalid format - not PCM");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if ( bitdepth!=16 && bitdepth!=8 ) {

        CApplication::displayError("Wav reading","Invalid bitdepth");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if((channels * ( bitdepth>>3 ) * samplerate) != bitspersecond) {

        CApplication::displayError("Wav reading","Invalid bitspersecond");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if(riffstyle != RIFFSTYLE) {

        CApplication::displayError("Wav reading","Invalid riffstyle");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }
    if( memsize != datasize+36) {

        CApplication::displayError("Wav reading","Invalid memsize");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return -1;
    }

    m_iBytesPerSample = bitdepth>>3;
    m_iChannels = channels;
    m_iSampleFrequency = samplerate;
    m_iDataSize = datasize;
    return 0;
}


void CFileIO::read()
{

    assert( !m_bRunning );
    assert( m_ifstream );

    if ( m_iLength<=0 || m_iLength>16384 ) {

        CApplication::displayError("Wav reading","Segment length is invalid");
        m_ifstream->close();
        delete m_ifstream;
        m_ifstream = nullptr;
        return;
    }
    m_bRunning = true;
    if ( m_thread.joinable() ) m_thread.join();
    m_thread = std::thread( &CFileIO::doRead, this,
                            m_ifstream,
                            m_outputQueue,
                            (int32)m_iLength,
                            (int32)m_iDataSize,
                            (int32)m_iChannels,
                            (int32)m_iBytesPerSample
                            );
    m_ifstream = nullptr;

}


void CFileIO::stop()
{

    if ( !isRunning() ) {

        return;
    }
    m_bRunning = false;
    if (m_outputQueue) {

        m_outputQueue->setLast();
    }
    m_thread.join();
    CApplication::displayInfo(std::string("FileIO info"),std::string("FileIO stopped."));
}


void CFileIO::doRead(
        std::ifstream* pstream,
        CInterThreadQueue* outputQueue,
        int32 segmentLength,
        int32 datasize,
        int32 channels,
        int32 bytedepth
        )
{

    assert( pstream );
    assert( pstream->is_open() );
    assert(outputQueue);
    std::ifstream& stream = *pstream;

    CApplication::displayInfo(std::string("FileIO info"),std::string("Reading started."));

    int32 length = datasize;
    int32 totalBytesPerSample = channels *  bytedepth ;

    segmentLength = segmentLength * totalBytesPerSample;

    while( length > 0 ) {

        if (!m_bRunning) {


            break;
        }

        unsigned char * mem = outputQueue->alloc();

        if (!mem) {

            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            continue;
        }

        if ( length > segmentLength ) {

            stream.read((char*)mem,segmentLength);
            if ( !stream ) {
                CApplication::displayError("Wav reading","File is corrupted - invalid read data");
                outputQueue->free(mem);
                goto cleanup;
            }
            length -= segmentLength;
            outputQueue->push(mem);
        }
        else{
            stream.read((char*)mem, length);
            if ( !stream ) {
                CApplication::displayError("Wav reading","File is corrupted - invalid read data (2)");
                outputQueue->free(mem);
                goto cleanup;
            }
            memset(&mem[length], 0, (segmentLength - length));
            length = 0;
            outputQueue->push(mem);
            outputQueue->setLast();
        }

    }
cleanup:
    stream.close();
    delete pstream;
    m_bRunning = false;
}

void CFileIO::write(std::string name)
{
    assert( !m_bRunning );
    assert( m_inputQueue );

    std::ofstream* stream = new std::ofstream();
    if ( !stream ) {

        CApplication::displayError("Wav writing","I can't allocate streeam, memory full?");
        return;
    }
    stream->open(name,std::ofstream::out | std::ofstream::binary);
    if ( !(*stream) ) {

        CApplication::displayError("Wav writing","Open, bad rights?");
        delete stream;
        return;
    }
    m_bRunning = true;
    m_thread = std::thread( &CFileIO::doWrite, this,
                            stream,
                            m_inputQueue,
                            (int32)m_iLength,
                            (int32)m_iChannels,
                            (int32)m_iSampleFrequency,
                            (int32)m_iBytesPerSample
                            );
}

void CFileIO::doWrite(
        std::ofstream* pstream,
        CInterThreadQueue* inputQueue,
        int32 segmentLength,
        int32 channels,
        int32 samplerate,
        int32 bytedepth
        )
{
    std::ofstream& stream = *pstream;
    int32 totalBytesPerSample = channels * ( bytedepth );
    segmentLength = segmentLength * totalBytesPerSample;
    int32 length = 0;

    CApplication::displayInfo(std::string("FileIO info"),std::string("Writing started."));

    int32 header[] = {
        WavChunks::RiffHeader,
        36,                 //length +36
        RIFFSTYLE,
        WavChunks::Format,
        16,
        int32((channels<<16) + 1),   //channel +PCM
        samplerate,
        int32(channels * bytedepth * samplerate),
        int32((bytedepth<<19) + 2),
        WavChunks::Data,
        0                   //length
    };
    stream.write((const char *)header,sizeof(header));
    if ( !stream ) {

        CApplication::displayError("Wav writing","Header");
        goto cleanup;
    }



    for(;;) {

        unsigned char *mem = inputQueue->pop();
        if ( !mem ) {

            break;
        }
        stream.write((char*)mem,segmentLength);
        inputQueue->free(mem);
        if ( !stream ) {
            CApplication::displayError("Wav writing","Write data");
            goto cleanup;
        }
        length += segmentLength;
    }
    header[1] = length +36;
    header[10] = length;
    stream.seekp( 0 );
    if ( !stream ) {
        CApplication::displayError("Wav writing","Seek");
        goto cleanup;
    }
    stream.write((const char *)header,sizeof(header));
    if ( !stream ) {
        CApplication::displayError("Wav writing","Header - finish");
        goto cleanup;
    }
cleanup:
    stream.close();
    delete pstream;
    m_bRunning = false;
}
