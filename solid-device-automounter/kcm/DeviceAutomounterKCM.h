/***************************************************************************
*   Copyright (C) 2009 by Trever Fischer <wm161@wm161.net>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
***************************************************************************/

#ifndef DEVICEAUTOMOUNTERKCM_H
#define DEVICEAUTOMOUNTERKCM_H

#include <KCModule>

#include "ui_DeviceAutomounterKCM.h"

class QStandardItemModel;

class DeviceAutomounterKCM : public KCModule, public Ui::DeviceAutomounterKCM {
    Q_OBJECT
    public:
        explicit DeviceAutomounterKCM(QWidget *parent = 0, const QVariantList &args = QVariantList());
        virtual ~DeviceAutomounterKCM();
    public slots:
        void load();
        void save();
    private slots:
        void emitChanged();
        void enabledChanged();
        void updateForgetDeviceButton();
        void forgetSelectedDevices();
        void addNewDevice();
    private:
        void addNewDevice(const QString &udi);
        QStandardItemModel *m_devices;
};

#endif
