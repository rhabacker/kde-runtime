/*  This file is part of the KDE project

    Copyright (c) 2010 Klarälvdalens Datakonsult AB,
                       a KDAB Group company <info@kdab.com>
    Author: Kevin Ottens <kevin.ottens@kdab.com>

    Copyright (c) 2011 Lukas Tinkl <ltinkl@redhat.com>
    Copyright (c) 2011-2012 Lamarque V. Souza <lamarque@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkmanagerstatus.h"

#include <QtDBus/QDBusReply>

#include <NetworkManager.h>

#if !defined(NM_CHECK_VERSION)
    #define NM_CHECK_VERSION(x,y,z) 0
#endif

NetworkManagerStatus::NetworkManagerStatus( QObject *parent )
    : SystemStatusInterface( parent ),
      m_manager( NM_DBUS_SERVICE,
                 NM_DBUS_PATH,
                 NM_DBUS_INTERFACE,
                 QDBusConnection::systemBus() )
{
    connect( &m_manager, SIGNAL(StateChanged(uint)),
             this, SLOT(nmStateChanged(uint)));

    QDBusReply<uint> reply = m_manager.call( "state" );

    if ( reply.isValid() ) {
        m_status = convertNmState( reply );
    } else {
        m_status = Solid::Networking::Unknown;
    }
}

Solid::Networking::Status NetworkManagerStatus::status() const
{
    return m_status;
}

bool NetworkManagerStatus::isSupported() const
{
    return m_manager.isValid();
}

QString NetworkManagerStatus::serviceName() const
{
    return QString(NM_DBUS_SERVICE);
}

void NetworkManagerStatus::nmStateChanged( uint nmState )
{
    m_status = convertNmState( nmState );
    emit statusChanged( m_status );
}

Solid::Networking::Status NetworkManagerStatus::convertNmState( uint nmState )
{
    Solid::Networking::Status status = Solid::Networking::Unknown;

    switch (nmState) {
    case NM_STATE_UNKNOWN:
    case NM_STATE_ASLEEP:
        break;
    case NM_STATE_CONNECTING:
        status = Solid::Networking::Connecting;
        break;
#if NM_CHECK_VERSION(0,8,992)
    case NM_STATE_CONNECTED_LOCAL:
    case NM_STATE_CONNECTED_SITE:
    case NM_STATE_CONNECTED_GLOBAL:
#else
    case NM_STATE_CONNECTED:
#endif
        status = Solid::Networking::Connected;
        break;
    case NM_STATE_DISCONNECTED:
        status = Solid::Networking::Unconnected;
        break;
#if NM_CHECK_VERSION(0,8,992)
    case NM_STATE_DISCONNECTING:
        status = Solid::Networking::Disconnecting;
        break;
#endif
    }

    return status;
}

#include "networkmanagerstatus.moc"
