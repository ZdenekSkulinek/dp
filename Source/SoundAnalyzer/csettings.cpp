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


#include "csettings.h"

#include <QDataStream>
#include <QFile>
#include <QIODevice>

CSettings::CSettings(QObject *parent) : QObject(parent),m_mSettings()
{

}


CSettings::CSettings(std::string filename, QObject *parent) : QObject(parent),m_mSettings()
{

    load(filename);
}

QVariant CSettings::getValueForKey(QString key)
{

    std::string skey = key.toStdString();
    QVariant rv = m_mSettings[skey];
    return rv;
}


void CSettings::setValueForKey(QString key, QVariant value)
{

    std::string skey = key.toStdString();
    m_mSettings[skey] = value;
    emit updated();
}



int CSettings::getValueForKeyi(std::string key)
{
    QVariant rv = m_mSettings[key];
    return rv.toInt();

}



void CSettings::setValueForKeyi(std::string key, int value)
{
    QVariant v = QVariant::fromValue(value);
    m_mSettings[key] = v;
    emit updated();
}


bool CSettings::getValueForKeyb(std::string key)
{
    QVariant rv = m_mSettings[key];
    return rv.toBool();

}



void CSettings::setValueForKeyb(std::string key, bool value)
{
    QVariant v = QVariant::fromValue(value);
    m_mSettings[key] = v;
    emit updated();
}


std::list<std::string> CSettings::getValueForKeysl(std::string key)
{
    std::list<std::string> listStd;
    QStringList listQt = m_mSettings[key].toStringList();
    for(const QString& s : listQt)
    {
        listStd.push_back( s.toStdString() );
    }
    return listStd;
}



void CSettings::setValueForKeysl(std::string key, std::list<std::string> value)
{
    QStringList listQt;
    listQt.reserve( value.size() );
    for(const std::string& s : value)
    {
        listQt.append( QString::fromStdString( s ) );
    }
    m_mSettings[key] = QVariant::fromValue( listQt );
    emit updated();
}



std::string CSettings::getValueForKeys(std::string key)
{
    QVariant rv = m_mSettings[key];
    std::string s = rv.toString().toStdString();
    return s;
}



void CSettings::setValueForKeys(std::string key, std::string value)
{
    QVariant qv = QVariant(QString::fromStdString( value ));
    m_mSettings[key] = qv;
    emit updated();
}



double CSettings::getValueForKeyf(std::string key)
{
    QVariant rv = m_mSettings[key];
    return rv.toDouble();
}



void CSettings::setValueForKeyf(std::string key, double value)
{
    QVariant v = QVariant::fromValue(value);
    m_mSettings[key] = v;
    emit updated();
}



void CSettings::load(std::string filename)
{
    QFile infile( filename.c_str() );
    infile.open( QIODevice::ReadOnly );
    QDataStream in( &infile );
    m_mSettings.clear();
    while ( !in.atEnd() ) {

        QString skey;
        in>>skey;
        QVariant vvalue;
        in>>vvalue;
        m_mSettings[skey.toStdString()] = vvalue;
    }
    infile.close();
    emit updated();
}



void CSettings::save(std::string filename)
{

    QFile outfile( filename.c_str() );
    outfile.open( QIODevice::WriteOnly );
    QDataStream out( &outfile );
    std::map<std::string,QVariant>::iterator it = m_mSettings.begin();
    while ( it != m_mSettings.end() ) {

        out<< QString::fromStdString( it->first );
        out<< it->second;
        it++;
    }
    outfile.close();
}


