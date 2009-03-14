/*******************************************************************
* getbacktracewidget.h
* Copyright 2000-2003 Hans Petter Bieker <bieker@kde.org>     
*           2009      Dario Andres Rodriguez <andresbajotierra@gmail.com>
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

#ifndef GETBACKTRACEWIDGET__H
#define GETBACKTRACEWIDGET__H

#include <QtGui/QWidget>

#include "ui_getbacktracewidget.h"

class CrashInfo;
class KTextBrowser;
class QLabel;
class KPushButton;
class UsefulnessMeter;

class GetBacktraceWidget: public QWidget
{
    Q_OBJECT
  
    public:
        GetBacktraceWidget( CrashInfo* );
        
        void generateBacktrace();

    Q_SIGNALS:
        void stateChanged();
        
    private:
        CrashInfo * crashInfo;
        Ui::Form    ui;
        UsefulnessMeter *   m_usefulnessMeter;
        
    private Q_SLOTS:
        void backtraceGenerated();
        
        void setAsLoading();
        void regenerateBacktrace();
        
        void saveClicked();
        void copyClicked();

};

#endif
