/*
    fixhostfilter.cpp

    This file is part of the KDE project
    Copyright (C) 2007 Lubos Lunak <llunak@suse.cz>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config-runtime.h>

#include "fixhosturifilter.h"

#include <QtCore/QTimer>

#include <kdebug.h>
#include <kurl.h>

/**
 * IMPORTANT: If you change anything here, please run the regression test
 * ../tests/kurifiltertest
 */
 
FixHostUriFilter::FixHostUriFilter( QObject *parent, const QVariantList & /*args*/ )
    : KUriFilterPlugin( "fixhosturifilter", parent ), m_hostExists( false )
{
}

bool FixHostUriFilter::filterUri( KUriFilterData& data ) const
{
    KUrl url = data.uri();
    QString cmd = url.url();

    kDebug(7023) <<  url;

    KUrl url2 = url;
    url2.setHost( "www." + url.host());

    if(( url.protocol() == "http" || url.protocol() == "https" )    
        && !url.host().startsWith( QLatin1String("www.") ) && !exists( url ) && exists( url2 ))
    {
        setFilteredUri( data, url2 );
        setUriType( data, KUriFilterData::NetProtocol );

        kDebug() << "filtered to" << data.uri();
        return true;
    }

    return false;
}

void FixHostUriFilter::lookedUp( const QHostInfo &hostInfo )
{
    if (hostInfo.lookupId() == m_lookupId) {
        m_hostExists = ( hostInfo.error() == QHostInfo::NoError );
        if (m_eventLoop) {
            m_eventLoop->exit();
        }
    }
}

bool FixHostUriFilter::exists( const KUrl& url ) const
{
    QEventLoop eventLoop;

    m_hostExists = false;
    m_eventLoop = &eventLoop;

    FixHostUriFilter *self = const_cast<FixHostUriFilter*>( this );
    m_lookupId = QHostInfo::lookupHost( url.host(), self, SLOT(lookedUp(QHostInfo)) );

    QTimer t;
    connect( &t, SIGNAL(timeout()), &eventLoop, SLOT(quit()) );
    t.start(1000);

    eventLoop.exec();
    m_eventLoop = 0;

    t.stop();

    QHostInfo::abortHostLookup( m_lookupId );

    return m_hostExists;
}

K_PLUGIN_FACTORY(FixHostUriFilterFactory, registerPlugin<FixHostUriFilter>();)
K_EXPORT_PLUGIN(FixHostUriFilterFactory("kcmkurifilt"))

#include "fixhosturifilter.moc"
