/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -l UIServer -m -i uiserver.h -a uiserveradaptor_p.h:uiserveradaptor.cpp -- org.kde.KIO.UIServer.xml
 *
 * dbusxml2cpp is Copyright (C) 2006 Trolltech AS. All rights reserved.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef UISERVERADAPTOR_P_H_286871151672286
#define UISERVERADAPTOR_P_H_286871151672286

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "uiserver.h"
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.kde.KIO.UIServer
 */
class UIServerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KIO.UIServer")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.kde.KIO.UIServer\" >\n"
"    <signal name=\"actionPerformed\" >\n"
"      <arg name=\"actionId\" type=\"i\" direction=\"out\" />\n"
"      <arg name=\"jobId\" type=\"i\" direction=\"out\" />\n"
"    </signal>\n"
"    <method name=\"newJob\" >\n"
"      <arg direction=\"in\" type=\"s\" name=\"appServiceName\" />\n"
"      <arg direction=\"in\" type=\"i\" name=\"capabilities\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"showProgress\" />\n"
"      <arg name=\"internalAppName\" type=\"s\" direction=\"in\" />\n"
"      <arg name=\"jobIcon\" type=\"s\" direction=\"in\" />\n"
"      <arg name=\"appName\" type=\"s\" direction=\"in\" />\n"
"      <arg direction=\"out\" type=\"i\" name=\"jobId\" />\n"
"    </method>\n"
"    <method name=\"jobFinished\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"totalSize\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"t\" name=\"size\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"totalFiles\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"u\" name=\"files\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"totalDirs\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"u\" name=\"dirs\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"processedSize\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"t\" name=\"size\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"processedFiles\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"u\" name=\"files\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"processedDirs\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"u\" name=\"dirs\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"percent\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"u\" name=\"ipercent\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"speed\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"bytesPerSecond\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"infoMessage\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"msg\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"progressInfoMessage\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"msg\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"    <method name=\"copying\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"from\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"to\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"moving\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"from\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"to\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"deleting\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"transferring\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"creatingDir\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"stating\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"mounting\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"dev\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"point\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"unmounting\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"id\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"point\" />\n"
"      <arg direction=\"out\" type=\"b\" name=\"res\" />\n"
"    </method>\n"
"    <method name=\"setJobVisible\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"jobId\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"visible\" />\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\" />\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    UIServerAdaptor(UIServer *parent);
    virtual ~UIServerAdaptor();

    inline UIServer *parent() const
    { return static_cast<UIServer *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool copying(int id, const QString &from, const QString &to);
    bool creatingDir(int id, const QString &url);
    bool deleting(int id, const QString &url);
    Q_NOREPLY void infoMessage(int id, const QString &msg);
    Q_NOREPLY void progressInfoMessage(int id, const QString &msg);
    Q_NOREPLY void jobFinished(int id);
    void setJobVisible(int jobId, bool visible);
    bool mounting(int id, const QString &dev, const QString &point);
    bool moving(int id, const QString &from, const QString &to);
    int newJob(const QString &appServiceName, int capabilities, bool showProgress, const QString &internalAppName, const QString &jobIcon, const QString &appName);
    Q_NOREPLY void percent(int id, uint ipercent);
    Q_NOREPLY void processedDirs(int id, uint dirs);
    Q_NOREPLY void processedFiles(int id, uint files);
    Q_NOREPLY void processedSize(int id, qulonglong size);
    Q_NOREPLY void speed(int id, const QString &bytesPerSecond);
    bool stating(int id, const QString &url);
    Q_NOREPLY void totalDirs(int id, uint dirs);
    Q_NOREPLY void totalFiles(int id, uint files);
    Q_NOREPLY void totalSize(int id, qulonglong size);
    bool transferring(int id, const QString &url);
    bool unmounting(int id, const QString &point);
Q_SIGNALS: // SIGNALS
    void actionPerformed(int actionId, int jobId);
};

#endif
