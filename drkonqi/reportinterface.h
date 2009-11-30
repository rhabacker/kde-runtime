/*******************************************************************
* reportinterface.h
* Copyright 2009    Dario Andres Rodriguez <andresbajotierra@gmail.com>
* Copyright 2009    George Kiagiadakis <gkiagia@users.sourceforge.net>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************/

#ifndef REPORTINTERFACE__H
#define REPORTINTERFACE__H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class SystemInformation;

class BugzillaManager;
class BugReport;
class ProductMapping;

class ReportInterface : public QObject
{
    Q_OBJECT
public:
    enum Reproducible { ReproducibleUnsure, ReproducibleNever,
        ReproducibleSometimes, ReproducibleEverytime };

    ReportInterface(QObject *parent = 0);

    void setBugAwarenessPageData(bool, Reproducible, bool, bool, bool);
    bool isBugAwarenessPageDataUseful() const;

    int selectedOptionsRating() const;

    QStringList firstBacktraceFunctions() const;
    void setFirstBacktraceFunctions(const QStringList & functions);

    QString backtrace() const;
    void setBacktrace(const QString & backtrace);
    
    QString title() const;
    void setTitle(const QString & text);

    void setDetailText(const QString & text);
    void setPossibleDuplicates(const QStringList & duplicatesList);

    QString generateReport(bool drKonqiStamp) const;

    BugReport newBugReportTemplate() const;
    void sendBugReport() const;

    QStringList relatedBugzillaProducts() const;
    
    bool isWorthReporting() const;

    //Zero means creating a new bug report
    void setAttachToBugNumber(uint);
    uint attachToBugNumber() const;

    void setPossibleDuplicatesByQuery(const QStringList &);

    BugzillaManager * bugzillaManager() const;
    
private Q_SLOTS:
    void sendUsingDefaultProduct() const;
    void addedToCC();
    void attachSent(int, int);

Q_SIGNALS:
    void reportSent(int);
    void sendReportError(const QString &);

private:
    //Information the user can provide
    bool        m_userRememberCrashSituation;
    Reproducible m_reproducible;
    bool        m_provideActionsApplicationDesktop;
    bool        m_provideUnusualBehavior;
    bool        m_provideApplicationConfigurationDetails;
    

    QString     m_backtrace;
    QStringList m_firstBacktraceFunctions;
    
    QString     m_reportTitle;
    QString     m_reportDetailText;
    QStringList m_possibleDuplicates;

    QStringList m_allPossibleDuplicatesByQuery;

    uint     m_attachToBugNumber;
    
    ProductMapping *    m_productMapping;
    BugzillaManager *   m_bugzillaManager;
};

#endif
