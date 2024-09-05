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


#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include <QApplication>
#include <QQmlApplicationEngine>

#include "csettings.h"
#include "crecorder.h"
#include "cfileio.h"
#include "cinterthreadqueue.h"
#include "copenclgoertzel.h"

class CBarGraph;

const int recorderQueuedTimeMS = 1000;
const int fileIOQueuedTimeMS =   10000;

enum CAppDisplayInfoEnum
{
    DISPLAYINFO_LOG,
    DISPLAYINFO_CORES,
    DISPLAYINFO_TIME,
    DISPLAYINFO_COMPUTE_FREQUENCY,
    DISPLAYINFO_SAMPLING_FREQUENCY
};

class CApplication : public QApplication
{
    Q_OBJECT
public:
    explicit CApplication(int& argc, char** argv);
    ~CApplication();
    void declareQML(QQmlApplicationEngine* engine);
    Q_INVOKABLE void init();


    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void setMode(qint32 mode);
    Q_INVOKABLE void updateDeviceLists();
    Q_INVOKABLE void processMatFile(QString path);



    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged);
    Q_PROPERTY(bool isStereo READ isStereo NOTIFY isStereoChanged);
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY isPausedChanged);
    Q_PROPERTY(int frequencies READ frequencies NOTIFY frequenciesChanged);

    bool isRunning() { return m_recorder.isRunning(); }
    bool isStereo()  { return m_recorder.isStereo(); }
    bool isPaused() { return m_recorder.isPaused(); }
    int frequencies() { return m_iFrequencies;}

    static void displayError(std::string caption, std::string description, bool quit=false);
    static void displayWarning(std::string caption, std::string description);
    static void displayInfo(std::string caption, std::string description, CAppDisplayInfoEnum type = DISPLAYINFO_LOG);
    static void stopRequest();
    static void updateStates();
    static std::string loadFromResources(std::string name);

signals:
    void isRunningChanged();
    void isStereoChanged();
    void isPausedChanged();
    void frequenciesChanged();
    void displayErrorSignal(QString caption, QString description, bool quit);
    void displayInfoSignal(QString caption, QString description, CAppDisplayInfoEnum type);
    void stopSignal();
public slots:
    void displayErrorMyThread(QString caption, QString description, bool quit=false);
    void displayInfoMyThread(QString caption, QString description, CAppDisplayInfoEnum type = DISPLAYINFO_LOG);
    void stopMyThread();
protected:

    CSettings m_settings;
    CSettings m_sharedData;
    CInterThreadQueue m_audioQueue;
    CInterThreadQueue m_frequencyMapQueue;
    CInterThreadQueue m_frequencyPhaseMapQueue;
    CInterThreadQueue m_frequencyMapQueueOS;
    CInterThreadQueue m_frequencyPhaseMapQueueOS;
    CInterThreadQueue m_fileReaderQueue;
    CRecorder m_recorder;
    CFileIO   m_fileIO;
    COpenClGoertzel m_goertzel;
    QQmlApplicationEngine* m_engine;
    CBarGraph* m_topBarGraph;
    CBarGraph* m_bottomBarGraph;
    CBarGraph* m_topBarGraphGoertzel;
    CBarGraph* m_bottomBarGraphGoertzel;
    CBarGraph* m_topBarGraphGoertzelOS;
    CBarGraph* m_bottomBarGraphGoertzelOS;
    qint32       m_iMode;
    int         m_iFrequencies;

    static CApplication* m_instance;
};

#endif // CAPPLICATION_H
