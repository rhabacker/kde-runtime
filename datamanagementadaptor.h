/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>

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
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.

   The basis for this file was generated by qdbusxml2cpp version 0.7
   Command line was: qdbusxml2cpp -a datamanagementadaptor -c DataManagementAdaptor -m org.kde.nepomuk.DataManagement.xml

   qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 */

#ifndef _DATAMANAGEMENTADAPTOR_H_
#define _DATAMANAGEMENTADAPTOR_H_

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtDBus/QtDBus>
#include <QtCore/QFutureWatcher>

#include "simpleresource.h"

namespace Nepomuk {
class DataManagementModel;
class DataManagementCommand;

/*
 * Adaptor class for interface org.kde.nepomuk.DataManagement
 */
class DataManagementAdaptor: public QDBusAbstractAdaptor, private QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.nepomuk.DataManagement")
    Q_CLASSINFO("D-Bus Introspection", ""
                "  <interface name=\"org.kde.nepomuk.DataManagement\">\n"
                "    <method name=\"addProperty\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"property\"/>\n"
                "      <arg direction=\"in\" type=\"av\" name=\"values\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "    </method>\n"
                "    <method name=\"setProperty\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"property\"/>\n"
                "      <arg direction=\"in\" type=\"av\" name=\"values\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "    </method>\n"
                "    <method name=\"removeProperty\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"property\"/>\n"
                "      <arg direction=\"in\" type=\"av\" name=\"values\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "    </method>\n"
                "    <method name=\"removeProperties\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"as\" name=\"properties\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "    </method>\n"
                "    <method name=\"createResource\">\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "      <arg direction=\"in\" type=\"as\" name=\"types\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"label\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"description\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "    </method>\n"
                "    <method name=\"removeResources\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "      <arg direction=\"in\" type=\"b\" name=\"force\"/>\n"
                "    </method>\n"
                "    <method name=\"removeDataByApplication\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "      <arg direction=\"in\" type=\"b\" name=\"force\"/>\n"
                "    </method>\n"
                "    <method name=\"removeDataByApplication\">\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "      <arg direction=\"in\" type=\"b\" name=\"force\"/>\n"
                "    </method>\n"
                "    <method name=\"removePropertiesByApplication\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"as\" name=\"properties\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "    </method>\n"
                "    <method name=\"mergeResources\">\n"
                "      <arg direction=\"in\" type=\"a(sa{sv})\" name=\"resources\"/>\n"
                "      <annotation value=\"Nepomuk::SimpleResourceGraph\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"app\"/>\n"
                "      <arg direction=\"in\" type=\"a{sv}\" name=\"additionalMetadata\"/>\n"
                "      <annotation value=\"QHash&lt;QString, QVariant>\" name=\"com.trolltech.QtDBus.QtTypeName.In2\"/>\n"
                "    </method>\n"
                "    <method name=\"describeResources\">\n"
                "      <arg direction=\"in\" type=\"as\" name=\"resources\"/>\n"
                "      <arg direction=\"in\" type=\"b\" name=\"includeSubResources\"/>\n"
                "      <arg direction=\"out\" type=\"a(sa{sv})\"/>\n"
                "      <annotation value=\"Nepomuk::SimpleResourceGraph\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                "")
public:
    DataManagementAdaptor(Nepomuk::DataManagementModel* parent);
    virtual ~DataManagementAdaptor();

public Q_SLOTS: // METHODS
    void addProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app);
    QString createResource(const QStringList &types, const QString &label, const QString &description, const QString &app);
    Nepomuk::SimpleResourceGraph describeResources(const QStringList &resources, bool includeSubResources);
    void mergeResources(Nepomuk::SimpleResourceGraph resources, const QString &app, const QHash<QString, QVariant> &additionalMetadata);
    void removeDataByApplication(const QString &app, bool force);
    void removeDataByApplication(const QStringList &resources, const QString &app, bool force);
    void removeProperties(const QStringList &resources, const QStringList &properties, const QString &app);
    void removePropertiesByApplication(const QStringList &resources, const QStringList &properties, const QString &app);
    void removeProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app);
    void removeResources(const QStringList &resources, const QString &app, bool force);
    void setProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app);

private:
    void enqueueCommand(Nepomuk::DataManagementCommand* cmd);

    Nepomuk::DataManagementModel* m_model;
};
}

#endif
