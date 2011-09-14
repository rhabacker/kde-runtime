/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -m -i dbustypes.h -p datamanagementinterface /home/trueg/kde/dev/kde/src/kde-runtime/nepomuk/interfaces/org.kde.nepomuk.DataManagement.xml
 *
 * qdbusxml2cpp is Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 *
 * It contains one small change: it is derived from AbstractTimeoutDbusInterface instead of QDBusAbstractInterface and uses a big timeout.
 */

#ifndef DATAMANAGEMENTINTERFACE_H_1308595912
#define DATAMANAGEMENTINTERFACE_H_1308595912

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "dbustypes.h"

#include "abstracttimeoutdbusinterface.h"

/*
 * Proxy class for interface org.kde.nepomuk.DataManagement
 */
class OrgKdeNepomukDataManagementInterface: public AbstractTimeoutDBusInterface
{
    Q_OBJECT

    /// we use a big timeout (10 min) since commands are queued in the DMS
    static const int s_defaultTimeout = 600000;

public:
    static inline const char *staticInterfaceName()
    { return "org.kde.nepomuk.DataManagement"; }

public:
    OrgKdeNepomukDataManagementInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgKdeNepomukDataManagementInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> addProperty(const QString &resource, const QString &property, const QDBusVariant &value, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(value) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("addProperty"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> addProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(property) << qVariantFromValue(values) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("addProperty"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<QString> createResource(const QString &type, const QString &label, const QString &description, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type) << qVariantFromValue(label) << qVariantFromValue(description) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("createResource"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<QString> createResource(const QStringList &types, const QString &label, const QString &description, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(types) << qVariantFromValue(label) << qVariantFromValue(description) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("createResource"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<QList<Nepomuk::SimpleResource> > describeResources(const QStringList &resources, int flags, const QStringList &targetParties)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(flags) << qVariantFromValue(targetParties);
        return asyncCallWithArgumentList(QLatin1String("describeResources"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> importResources(const QString &url, const QString &serialization, int identificationMode, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(url) << qVariantFromValue(serialization) << qVariantFromValue(identificationMode) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("importResources"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> importResources(const QString &url, const QString &serialization, int identificationMode, int flags, Nepomuk::PropertyHash additionalMetadata, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(url) << qVariantFromValue(serialization) << qVariantFromValue(identificationMode) << qVariantFromValue(flags) << qVariantFromValue(additionalMetadata) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("importResources"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> mergeResources(const QString &resource1, const QString &resource2, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource1) << qVariantFromValue(resource2) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("mergeResources"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeDataByApplication(int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeDataByApplication"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeDataByApplication(const QStringList &resources, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeDataByApplication"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeProperties(const QString &resource, const QString &property, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperties"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeProperties(const QStringList &resources, const QStringList &properties, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(properties) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperties"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeProperty(const QString &resource, const QString &property, const QDBusVariant &value, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(value) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperty"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(property) << qVariantFromValue(values) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperty"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeResources(const QString &resource, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeResources"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> removeResources(const QStringList &resources, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeResources"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> setProperty(const QString &resource, const QString &property, const QDBusVariant &value, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(value) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("setProperty"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> setProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(property) << qVariantFromValue(values) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("setProperty"), argumentList, s_defaultTimeout);
    }

    inline QDBusPendingReply<> storeResources(const QList<Nepomuk::SimpleResource> &resources, int identificationMode, int flags, Nepomuk::PropertyHash additionalMetadata, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(identificationMode) << qVariantFromValue(flags) << qVariantFromValue(additionalMetadata) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("storeResources"), argumentList, s_defaultTimeout);
    }

Q_SIGNALS: // SIGNALS
};

namespace org {
  namespace kde {
    namespace nepomuk {
      typedef ::OrgKdeNepomukDataManagementInterface DataManagement;
    }
  }
}
#endif
