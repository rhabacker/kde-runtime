/* This file is part of the KDE project
   Copyright (c) 2004 Kévin Ottens <ervin ipsquad net>
   Parts of this file are
   Copyright 2003 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapplication.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <QTimer>
#include <kdebug.h>
#include <kglobal.h>
#include <k3process.h>
#include <QtDBus/QtDBus>

#include "kio_media_mounthelper.h"

const Medium MountHelper::findMedium(const QString &name)
{
	QDBusInterface mediamanager("org.kde.kded", "/modules/mediamanager", "org.kde.MediaManager");
	QDBusReply<QStringList> reply = mediamanager.call( "properties", name );

	if ( !reply.isValid() )
	{
		m_errorStr = i18n("The KDE mediamanager is not running.") + '\n';
	}

	return Medium::create(reply);
}

MountHelper::MountHelper() : KApplication()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	m_errorStr = "";

	KUrl url(args->url(0));
	const Medium medium = findMedium(url.fileName());

	if ( medium.id().isEmpty() )
	{
		m_errorStr+= i18n("%1 cannot be found.", url.prettyUrl());
		QTimer::singleShot(0, this, SLOT(error()) );
		return;
	}

	if ( !medium.isMountable() && !args->isSet("e") && !args->isSet("s"))
	{
		m_errorStr = i18n("%1 is not a mountable media.", url.prettyUrl());
		QTimer::singleShot(0, this, SLOT(error()) );
		return;
	}

	QString device = medium.deviceNode();
	QString mount_point = medium.mountPoint();

	m_isCdrom = medium.mimeType().indexOf("dvd")!=-1
	         || medium.mimeType().indexOf("cd")!=-1;

	if (args->isSet("u"))
	{
		KIO::Job * job = KIO::unmount( mount_point );

		connect( job, SIGNAL( result(KJob*) ),
		         this, SLOT( slotResult(KJob*) ) );
	}
	else if (args->isSet("s") || args->isSet("e"))
	{
		if (medium.isMounted())
		{
			KIO::Job * job = KIO::unmount( mount_point );

			m_device = device;
			connect( job, SIGNAL( result(KJob*) ),
			         this, SLOT( slotResultSafe(KJob*) ) );
		}
		else
		{
			invokeEject(device, true);
		}
	}
	else
	{
		 KIO::Job* job = KIO::mount( false, 0, device, mount_point);
		 connect( job, SIGNAL( result(KJob*) ),
		          this, SLOT( slotResult(KJob*) ) );
	}
}

void MountHelper::invokeEject(const QString &device, bool quiet)
{
	K3Process *proc = new K3Process(this);
	*proc << "kdeeject";
	if (quiet)
	{
		*proc << "-q";
	}
	*proc << device;
	proc->start();
	connect( proc, SIGNAL(processExited(K3Process *)),
	         this, SLOT( finished() ) );
}

void MountHelper::slotResultSafe(KJob* job)
{
	if (job->error())
	{
		m_errorStr = job->errorText();
		
		if (m_isCdrom)
		{
			m_errorStr+= i18n("\nPlease check that the disk is"
			                  " entered correctly.");
		}
		else
		{
			m_errorStr+= i18n("\nPlease check that the device is"
			                  " plugged correctly.");
		}
		
		QTimer::singleShot(0, this, SLOT(error()) );
	}
	else
	{
		invokeEject(m_device, true);
	}
}

void MountHelper::slotResult(KJob* job)
{
	if (job->error())
	{
		m_errorStr = job->errorText();
		
		if (m_isCdrom)
		{
			m_errorStr+= i18n("\nPlease check that the disk is"
			                  " entered correctly.");
		}
		else
		{
			m_errorStr+= i18n("\nPlease check that the device is"
			                  " plugged correctly.");
		}
				
		QTimer::singleShot(0, this, SLOT(error()) );
	}
	else
	{
		QTimer::singleShot(0, this, SLOT(finished()) );
	}
}

void MountHelper::error()
{
	KMessageBox::error(0, m_errorStr);
	kapp->exit(1);
}

void MountHelper::finished()
{
	kapp->quit();
}

static KCmdLineOptions options[] =
{
	{ "u", I18N_NOOP("Unmount given URL"), 0 },
	{ "m", I18N_NOOP("Mount given URL (default)"), 0 },
	{ "e", I18N_NOOP("Eject given URL via kdeeject"), 0},
	{ "s", I18N_NOOP("Unmount and Eject given URL (necessary for some USB devices)"), 0},
	{"!+URL",  I18N_NOOP("media:/ URL to mount/unmount/eject/remove"), 0 },
	KCmdLineLastOption
};


int main(int argc, char **argv)
{
	KCmdLineArgs::init(argc, argv, "kio_media_mounthelper",
	                   "kio_media_mounthelper", "kio_media_mounthelper",
	                   "0.1");

	KCmdLineArgs::addCmdLineOptions( options );
	KGlobal::locale()->setMainCatalog("kio_media");

	if (KCmdLineArgs::parsedArgs()->count()==0) KCmdLineArgs::usage();
	KApplication *app = new  MountHelper();

	app->exec();
}

#include "kio_media_mounthelper.moc"
