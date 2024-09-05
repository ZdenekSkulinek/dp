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


#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QObject>
#include <QVariant>
#include <QString>

#include <string>
#include <map>
#include <list>

class CSettings : public QObject
{
    Q_OBJECT


    std::map<std::string,QVariant> m_mSettings;

public:
    explicit CSettings(std::string filename, QObject *parent = 0);
    explicit CSettings(QObject *parent = 0);
    Q_INVOKABLE QVariant getValueForKey(QString key);
    Q_INVOKABLE void setValueForKey(QString key, QVariant value);

    int getValueForKeyi(std::string key);
    void setValueForKeyi(std::string key, int value);
    bool getValueForKeyb(std::string key);
    void setValueForKeyb(std::string key, bool value);
    std::list<std::string> getValueForKeysl(std::string key);
    void setValueForKeysl(std::string key, std::list<std::string> value);
    std::string getValueForKeys(std::string key);
    void setValueForKeys(std::string key, std::string value);
    double getValueForKeyf(std::string key);
    void setValueForKeyf(std::string key, double value);
    void load(std::string filename);
    void save(std::string filename);

signals:
    void updated();
public slots:
};

#endif // CSETTINGS_H
