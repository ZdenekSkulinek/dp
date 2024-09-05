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
#include "capplication.h"
#include <cassert>
#include <QQmlContext>


CApplication::CApplication(int& argc, char** argv) :
    QApplication(argc, argv), m_settings(std::string("setting")),
    m_sharedData(), m_audioQueue(), m_frequencyMapQueue(), m_frequencyPhaseMapQueue(),
    m_frequencyMapQueueOS(), m_frequencyPhaseMapQueueOS(),m_fileReaderQueue(),m_recorder(),
    m_fileIO(),m_goertzel(),m_engine(),
    m_topBarGraph(nullptr), m_bottomBarGraph(nullptr),m_topBarGraphGoertzel(nullptr), m_bottomBarGraphGoertzel(nullptr),
    m_topBarGraphGoertzelOS(nullptr), m_bottomBarGraphGoertzelOS(nullptr),m_iMode(-1),m_iFrequencies(888)
{
    assert(!m_instance);
    m_instance = this;
    qRegisterMetaType<CAppDisplayInfoEnum>("CAppDisplayInfoEnum");
    connect(this,&CApplication::displayErrorSignal, this, &CApplication::displayErrorMyThread);
    connect(this,&CApplication::displayInfoSignal, this, &CApplication::displayInfoMyThread);
    connect(this,&CApplication::stopSignal, this, &CApplication::stopMyThread);
}


void CApplication::declareQML(QQmlApplicationEngine* engine)
{
    m_engine = engine;
    QQmlContext* ctx =  m_engine->rootContext();
    qmlRegisterType<CBarGraph>("main", 1, 0, "CBarGraph");
    ctx->setContextProperty("sharedSettings", &m_settings );
    ctx->setContextProperty("sharedData", &m_sharedData );
    ctx->setContextProperty("application", this );
}

void CApplication::init()
{
    //m_sharedData.setValueForKeyb( std::string("recorder.inFile"), false);
    //m_sharedData.setValueForKeyb( std::string("recorder.outFile"), false);
    //m_sharedData.setValueForKeys( std::string("recorder.fileName"), std::string( "" ) );

    m_topBarGraph = m_engine->rootObjects().first()->findChild<CBarGraph*>("topBarGraph");
    m_bottomBarGraph = m_engine->rootObjects().first()->findChild<CBarGraph*>("bottomBarGraph");
    m_topBarGraphGoertzel = m_engine->rootObjects().first()->findChild<CBarGraph*>("topBarGraphG");
    m_bottomBarGraphGoertzel = m_engine->rootObjects().first()->findChild<CBarGraph*>("bottomBarGraphG");
    m_topBarGraphGoertzelOS = m_engine->rootObjects().first()->findChild<CBarGraph*>("topBarGraphGOS");
    m_bottomBarGraphGoertzelOS = m_engine->rootObjects().first()->findChild<CBarGraph*>("bottomBarGraphGOS");
    updateDeviceLists();
    std::string frequenciesList = m_settings.getValueForKeys( std::string("goertzel.frequencies") );
    m_iFrequencies = m_goertzel.setFrequencyIndexes(frequenciesList);
    emit frequenciesChanged();
    setMode(0);


}


void CApplication::displayErrorMyThread(QString caption, QString description, bool quit)
{
    qWarning()<<caption<<" : "<<description;
    if ( !m_instance || !m_engine ) {

        return;
    }

    QObject *item = m_engine->rootObjects().first()->findChild<QObject*>("messageDialog");

    assert(item);
    item->setProperty("title",  caption);
    item->setProperty("text",  description);
    item->setProperty("quitAfterAccepted",  quit);
    item->setProperty("visible",  true);

}

void CApplication::displayError(std::string caption, std::string description, bool quit)
{
    emit m_instance->displayErrorSignal(
                QString::fromStdString(caption),
                QString::fromStdString(description),
                quit);
}


void CApplication::displayWarning(std::string caption, std::string description)
{
    qWarning()<<QString::fromStdString(caption)<<" : "<<QString::fromStdString(description);
}

void CApplication::displayInfoMyThread(QString caption, QString description, CAppDisplayInfoEnum type)
{
    QObject* qobj;
    switch (type) {
    case DISPLAYINFO_LOG:
        qDebug()<<caption<<" : "<<description;
        break;
    case DISPLAYINFO_CORES:
        qobj = m_engine->rootObjects().first()->findChild<QObject*>("coresText");
        qobj->setProperty("text",QVariant::fromValue(description));
        break;
    case DISPLAYINFO_TIME:
        qobj = m_engine->rootObjects().first()->findChild<QObject*>("timeText");
        qobj->setProperty("text",QVariant::fromValue(description));
        break;
    case DISPLAYINFO_COMPUTE_FREQUENCY:
        qobj = m_engine->rootObjects().first()->findChild<QObject*>("computeFrequencyText");
        qobj->setProperty("text",QVariant::fromValue(description));
        break;
    case DISPLAYINFO_SAMPLING_FREQUENCY:
        qobj = m_engine->rootObjects().first()->findChild<QObject*>("samplingFrequencyText");
        qobj->setProperty("text",QVariant::fromValue(description));
        break;
    default:
        break;
    }

}


void CApplication::displayInfo(std::string caption, std::string description, CAppDisplayInfoEnum type)
{
    emit m_instance->displayInfoSignal(
                QString::fromStdString(caption),
                QString::fromStdString(description),
                type);
}

void CApplication::stopRequest()
{
    emit m_instance->stopSignal();
}

std::string CApplication::loadFromResources(std::string name)
{
    QFile file(QString::fromStdString(name));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
            qWarning()<<"CApplication: can't laod from resource file "<<name.c_str();
            return std::string();
    }
    int sz = file.size();
    char pool[sz+1];
    int rrv = file.read(pool,sz);
    file.close();
    if (rrv != sz )
    {
        return std::string();
    }
    pool[sz]=0;
    return std::string(pool);
}


void CApplication::setMode(qint32 mode)
{
    if ( m_iMode == mode ) {

        return;
    }
    m_topBarGraph->setInputQueue(nullptr);
    m_bottomBarGraph->setInputQueue(nullptr);
    m_topBarGraphGoertzel->setInputQueue(nullptr);
    m_bottomBarGraphGoertzel->setInputQueue(nullptr);
    m_goertzel.setInputQueue(nullptr);
    m_goertzel.setOutputQueue(nullptr);
    m_goertzel.setOutputQueuePhase(nullptr);
    m_iMode = mode;

    switch( mode )
    {
        case 0:
            m_topBarGraph->setInputQueue(&m_audioQueue);
            if ( m_recorder.channels() == 2 )
            {
                m_bottomBarGraph->setParentBarGraph(m_topBarGraph);
                m_bottomBarGraph->setInputQueue(&m_audioQueue);
            }
            break;
        case 1:
            m_topBarGraphGoertzel->setInputQueue(&m_frequencyMapQueue);
            m_bottomBarGraphGoertzel->setInputQueue(&m_frequencyPhaseMapQueue);
            m_goertzel.setInputQueue(&m_audioQueue);
            m_goertzel.setOutputQueue(&m_frequencyMapQueue);
            m_goertzel.setOutputQueuePhase(&m_frequencyPhaseMapQueue);
            break;
       case 2:
            break;
    }

}

void CApplication::updateDeviceLists()
{
    std::list<std::string> devicesList = m_recorder.getDeviceList();
    std::list<std::string> captureDevicesList = m_recorder.getCaptureDeviceList();
    m_sharedData.setValueForKeysl( std::string("recorder.devicesList"), devicesList );
    m_sharedData.setValueForKeysl( std::string("recorder.captureDevicesList"), captureDevicesList );

    std::string defaultDevice = m_settings.getValueForKeys( std::string("recorder.defaultDevice") );
    bool notfound = true;
    for (std::list<std::string>::iterator it = devicesList.begin(); it != devicesList.end(); it++) {

        if ( *it == defaultDevice ) {

            notfound = false;
            break;
        }
    }
    if ( notfound ) {

        m_settings.setValueForKeys(std::string("recorder.defaultDevice"), m_recorder.getDefaultDevice() );
    }
    std::string defaultCaptureDevice = m_settings.getValueForKeys( std::string("recorder.defaultCaptureDevice") );
    notfound = true;
    for (std::list<std::string>::iterator it = captureDevicesList.begin(); it != captureDevicesList.end(); it++) {

        if ( *it == defaultCaptureDevice ) {

            notfound = false;
            break;
        }
    }
    if ( notfound ) {

        m_settings.setValueForKeys(std::string("recorder.defaultCaptureDevice"), m_recorder.getDefaultCaptureDevice() );
    }
    QQmlContext* ctx =  m_engine->rootContext();
    ctx->setContextProperty("sharedSettings", &m_settings );
    ctx->setContextProperty("sharedData", &m_sharedData );
}

void CApplication::start()
{
    if ( m_recorder.isRunning() ) {

        qWarning()<<"Trying to run runned recorder";
        return;
    }

    if ( m_fileIO.isRunning() ) {

        qWarning()<<"Trying to run runned file IO";
        return;
    }
    if ( m_goertzel.isRunning() ) {

        qWarning()<<"Trying to run runned file Goertzel";
        return;
    }


    bool infile = m_settings.getValueForKeyb("recorder.inFile");
    bool outfile = m_settings.getValueForKeyb("recorder.outFile");
    std::string filename = m_settings.getValueForKeys("recorder.fileName");
    int length = m_settings.getValueForKeyi( std::string("recorder.segmentLength") );
    int overlap = m_settings.getValueForKeyi( std::string("goertzel.overlap") );
    std::string frequenciesList = m_settings.getValueForKeys( std::string("goertzel.frequencies") );
    bool useCpu = m_settings.getValueForKeyb( std::string("goertzel.cpu") );
    int channels;
    int bytesPerSample;
    int sampleFrequency;

    if ( infile ) {

        if ( m_fileIO.readHeader( filename ) != 0 ) {

            return;
        }
        m_fileIO.setOutputQueue(&m_fileReaderQueue);
        m_recorder.setInputQueue(&m_fileReaderQueue);
        m_fileIO.setLength(length);
        channels = m_fileIO.channels();
        bytesPerSample = m_fileIO.bytesperSample();
        sampleFrequency = m_fileIO.sampleFrequency();
    }
    else {

        channels = m_settings.getValueForKeyi( std::string("recorder.nChannels") );
        bytesPerSample = m_settings.getValueForKeyi( std::string("recorder.nBytesSample") );
        sampleFrequency = m_settings.getValueForKeyi( std::string("recorder.samplingFrequency") );

        if ( outfile ) {

            m_fileIO.setInputQueue(&m_fileReaderQueue);
            m_recorder.setMonitorQueue(&m_fileReaderQueue);
            m_fileIO.setLength(length);
            m_fileIO.setChannels(channels);
            m_fileIO.setBytesPerSample(bytesPerSample);
            m_fileIO.setSampleFrequency(sampleFrequency);
        }
        else {

            m_recorder.setMonitorQueue(nullptr);
        }
    }

    std::string device = m_settings.getValueForKeys( std::string("recorder.defaultDevice") );
    std::string captureDevice = m_settings.getValueForKeys( std::string("recorder.defaultCaptureDevice") );

    if ( ( channels >= 1 && channels <=2) &&
         ( bytesPerSample >= 1 && bytesPerSample <= 2) &&
         ( sampleFrequency >=1 && sampleFrequency <= 2000000000) &&
         ( length >=1 && length <= 16384))
    {
        int segLen = length * channels * bytesPerSample;
        int segments = recorderQueuedTimeMS * sampleFrequency / length / 1000;
        if (segments<10) segments=10;
        int fileSegments = fileIOQueuedTimeMS * sampleFrequency / length / 1000;
        if (fileSegments<10) fileSegments=10;
        m_audioQueue.reset(segments,segLen);

        int numfrequencies = m_goertzel.setFrequencyIndexes(frequenciesList);
        m_iFrequencies = numfrequencies;
        emit frequenciesChanged();
        m_frequencyMapQueue.reset(segments,numfrequencies*channels*sizeof(float));
        m_frequencyPhaseMapQueue.reset(segments,numfrequencies*channels*sizeof(float));
        if ( infile || outfile) {

            m_fileReaderQueue.reset(fileSegments,segLen);
        }

        m_recorder.setDevice(device);
        m_recorder.setCaptureDevice(captureDevice);
        m_recorder.setChannels(channels);
        m_recorder.setBytesPerSample(bytesPerSample);
        m_recorder.setSampleFrequency(sampleFrequency);
        m_recorder.setLength(length);
        m_recorder.setQueue(&m_audioQueue);

        m_goertzel.setSampleFrequency(sampleFrequency);
        m_goertzel.setLength(length);
        m_goertzel.setOverlap(overlap);
        m_goertzel.setChannels(channels);
        m_goertzel.setBytesPerSample(bytesPerSample);
        m_goertzel.setUseCpu(useCpu);

        if ( channels == 1 ) {

            m_topBarGraph->setStride(0);
            m_topBarGraph->setBytesPerSample(bytesPerSample);
            m_topBarGraph->setOffset(0);
            m_topBarGraph->setLength(length);
        }
        else{
            m_topBarGraph->setStride(bytesPerSample);
            m_topBarGraph->setBytesPerSample(bytesPerSample);
            m_topBarGraph->setOffset(0);
            m_topBarGraph->setLength(length);
            m_bottomBarGraph->setStride(bytesPerSample);
            m_bottomBarGraph->setBytesPerSample(bytesPerSample);
            m_bottomBarGraph->setOffset(bytesPerSample);
            m_bottomBarGraph->setLength(length);
        }
        m_topBarGraphGoertzel->setStride(0);
        m_topBarGraphGoertzel->setBytesPerSample(4);
        m_topBarGraphGoertzel->setOffset(0);
        m_topBarGraphGoertzel->setLength(channels * numfrequencies);
        m_bottomBarGraphGoertzel->setStride(0);
        m_bottomBarGraphGoertzel->setBytesPerSample(4);
        m_bottomBarGraphGoertzel->setOffset(0);
        m_bottomBarGraphGoertzel->setLength(channels * numfrequencies);

        int mode = m_iMode;
        m_iMode = -1;
        setMode(mode);

        if ( infile ) {

            m_recorder.play();
            m_fileIO.read();
        }
        else  if ( outfile ) {

            m_fileIO.write( filename );
            m_recorder.start();
        }
        else{

            m_recorder.start();

        }
        m_goertzel.start();


        emit isRunningChanged();
        emit isStereoChanged();
        emit isPausedChanged();
    }
    else {

        CApplication::displayError("Error"," Application -> Recoder - Bad data from setting");

    }

}

void CApplication::updateStates()
{
    emit m_instance->isPausedChanged();
    emit m_instance->isRunningChanged();
    emit m_instance->isStereoChanged();
}

void CApplication::stop()
{
    m_recorder.stop();
    m_fileIO.stop();
    m_topBarGraph->setInputQueue(nullptr);
    m_bottomBarGraph->setInputQueue(nullptr);
    m_goertzel.stop();
    emit isRunningChanged();
}


void CApplication::stopMyThread()
{
    stop();
}

void CApplication::pause()
{
    m_recorder.pause();
    emit isPausedChanged();
}

void CApplication::processMatFile(QString path)
{
    COpenClGoertzel g;
    m_topBarGraphGoertzelOS->setInputQueue(nullptr);
    m_bottomBarGraphGoertzelOS->setInputQueue(nullptr);

    g.computeOneFile(path.toStdString(),&m_frequencyMapQueueOS,&m_frequencyPhaseMapQueueOS);
    m_topBarGraphGoertzelOS->setStride(0);
    m_topBarGraphGoertzelOS->setBytesPerSample(4);
    m_topBarGraphGoertzelOS->setOffset(0);
    m_topBarGraphGoertzelOS->setLength(m_frequencyMapQueueOS.segmentSize() / sizeof(float) );
    m_bottomBarGraphGoertzelOS->setStride(0);
    m_bottomBarGraphGoertzelOS->setBytesPerSample(4);
    m_bottomBarGraphGoertzelOS->setOffset(0);
    m_bottomBarGraphGoertzelOS->setLength(m_frequencyPhaseMapQueueOS.segmentSize() / sizeof(float) );
    m_topBarGraphGoertzelOS->setInputQueue(&m_frequencyMapQueueOS);
    m_bottomBarGraphGoertzelOS->setInputQueue(&m_frequencyPhaseMapQueueOS);
    QObject* gauge = m_engine->rootObjects().first()->findChild<QObject*>("horizontalGaugeGOS");
    gauge->setProperty("maximumText",QVariant::fromValue(QString::number(m_frequencyPhaseMapQueueOS.segmentSize() / sizeof(float) )));
}

CApplication::~CApplication()
{
    m_recorder.stop();
    m_goertzel.stop();
    m_fileIO.stop();
    m_settings.save( std::string("setting") );
    m_instance = nullptr;
    disconnect(this,&CApplication::displayErrorSignal, this, &CApplication::displayErrorMyThread);
    disconnect(this,&CApplication::displayInfoSignal, this, &CApplication::displayInfoMyThread);
    disconnect(this,&CApplication::stopSignal, this, &CApplication::stopMyThread);
}

CApplication* CApplication::m_instance = nullptr;
