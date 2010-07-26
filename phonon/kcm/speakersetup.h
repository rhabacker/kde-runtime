/*  This file is part of the KDE project
    Copyright (C) 2010 Colin Guthrie <cguthrie@mandriva.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.

*/

#ifndef PHONON_SPEAKERSETUP_H
#define PHONON_SPEAKERSETUP_H

#include <canberra.h>
#include <pulse/pulseaudio.h>

#include "ui_speakersetup.h"
#define KDE3_SUPPORT
#include <kcmodule.h>
#undef KDE3_SUPPORT
#include <kconfig.h>

//class QLabel;

class SpeakerSetup : public QWidget, private Ui::SpeakerSetup
{
    Q_OBJECT
    public:
        SpeakerSetup(QWidget *parent = 0);
        ~SpeakerSetup();

        void load();
        void save();
        void defaults();
        uint32_t getCurrentSinkIndex();
        void updateCard(const pa_card_info*);
        void removeCard(uint32_t idx);
        void updateSink(const pa_sink_info*);
        void removeSink(uint32_t idx);
        void updateFromPulse();
        void updateIndependantDevices();

    public Q_SLOTS:
        void cardChanged();
        void profileChanged();
        void sinkChanged();
        void portChanged();

    Q_SIGNALS:
        void changed();
        void ready();

    private:
        void _updatePlacementTester();

        QLabel* m_Icon;
        int m_OutstandingRequests;
        ca_context* m_Canberra;
};

#endif // PHONON_SPEAKERSETUP_H
