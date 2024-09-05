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


#include "capplication.h"

class CxApplication Q_DECL_FINAL: public QApplication
{

public:
    explicit CxApplication(int& argc, char** argv):QApplication(argc,argv) {}
    ~CxApplication() {}
};

int main(int argc, char *argv[])
{

    CApplication* app = new CApplication(argc, argv);
#ifndef QT_DEBUG
    QStringList sl;
    sl.clear();
    sl.insert(0,QCoreApplication::applicationDirPath()+QString("/plugins"));
    QCoreApplication::setLibraryPaths(sl);
#endif
    int retVal = 0;

    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);



    QQmlApplicationEngine* engine = new QQmlApplicationEngine();
#ifndef QT_DEBUG
    sl.clear();
    sl.insert(0,QCoreApplication::applicationDirPath()+QString("/qml"));
    engine->setImportPathList(sl);
#endif
    app->declareQML(engine);
    engine->load(QUrl("qrc:///main.qml"));
    app->init();
    retVal = app->exec();

    delete engine;
    delete app;

    return retVal;
}
