/*
  toplevel.cpp - A KControl Application

  Copyright 1998 Matthias Hoelzer
  Copyright 1999-2000 Hans Petter Bieker <bieker@kde.org>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qtabwidget.h>
#include <qevent.h>
#include <qwidgetintdict.h>

#include <kcharsets.h>
#include <kconfig.h>
#include <kcmodule.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kapp.h>

#include "klocaleadv.h"
#include "locale.h"
#include "localenum.h"
#include "localemon.h"
#include "localetime.h"
#include "klocalesample.h"
#include "toplevel.h"
#include "toplevel.moc"

KLocaleAdvanced *locale = 0;

KLocaleApplication::KLocaleApplication(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  locale = new KLocaleAdvanced(QString::fromLatin1("kcmlocale"));
  QVBoxLayout *l = new QVBoxLayout(this, 5);

  tab = new QTabWidget(this);
  l->addWidget(tab);

  localemain = new KLocaleConfig(this);
  tab->addTab( localemain, QString::null);
  localenum = new KLocaleConfigNumber(this);
  tab->addTab( localenum, QString::null);
  localemon = new KLocaleConfigMoney(this);
  tab->addTab( localemon, QString::null);
  localetime = new KLocaleConfigTime(this);
  tab->addTab( localetime, QString::null);

  connect(localemain, SIGNAL(resample()),                   SLOT(update()));
  connect(localenum,  SIGNAL(resample()),                   SLOT(update()));
  connect(localemon,  SIGNAL(resample()),                   SLOT(update()));
  connect(localetime, SIGNAL(resample()),                   SLOT(update()));
  connect(localemain, SIGNAL(countryChanged()),             SLOT(reset()) );
  connect(localemain, SIGNAL(moneyChanged()),   localemon,  SLOT(reset()) );
  connect(localemain, SIGNAL(numberChanged()),  localenum,  SLOT(reset()) );
  connect(localemain, SIGNAL(timeChanged()),    localetime, SLOT(reset()) );
  connect(localemain, SIGNAL(chsetChanged()),               SLOT(newChset()) );

  // Examples
  gbox = new QGroupBox(this);
  l->addWidget(gbox);
  sample = new KLocaleSample(gbox);

  load();
  update();
}

KLocaleApplication::~KLocaleApplication()
{
    delete locale;
}

void KLocaleApplication::load()
{
    localemain->load();
    localenum->load();
    localemon->load();
    localetime->load();
}

void KLocaleApplication::save()
{
    localemain->save();
    localenum->save();
    localemon->save();
    localetime->save();

    // temperary use of our locale as the global locale
    KLocale *lsave = KGlobal::_locale;
    KGlobal::_locale = locale;

    KMessageBox::information(this,
			     locale->translate("Changed language settings apply only to newly started "
				  "applications.\nTo change the language of all "
				  "programs, you will have to logout first."),
                             locale->translate("Applying language settings"));
    // restore the old global locale
    KGlobal::_locale = lsave;
}

void KLocaleApplication::defaults()
{
    localemain->defaults();
    localenum->defaults();
    localemon->defaults();
    localetime->defaults();
}

void KLocaleApplication::moduleChanged(bool state)
{
  emit changed(state);
}

QString KLocaleApplication::quickHelp()
{
  return i18n("<h1>Locale</h1>\n"
	      "<p>From here you can configure language, numberic, and time "
	      "settings for your particular region. In most cases it will be "
	      "suffient to choose the country you live in. For instance KDE "
	      "will automatically choose \"German\" as language if you choose "
	      "\"Germany\" from the list. It will also change the time format "
	      "to use 24 hours and and use comma as decimal separator.</p>"
	      "<p><b>Note:</b> The default charset is ISO 8859-1. That "
	      "charset is used when choosing fonts. You will have to change "
	      "it if you are using a non-Western European language.");
}

void KLocaleApplication::updateSample()
{
    sample->update();
    moduleChanged(true);
}

void KLocaleApplication::reTranslate()
{
  // The untranslated string for QLabel are stored in 
  // the name() so we use that when retranslating
  QObjectList *list = queryList("QLabel");
  QObjectListIt it(*list);
  QObject *wc;
  while ( (wc = it.current()) != 0 ) {
    ++it;
    
    // unnamed labels will cause errors and should not be 
    // retranslated. E.g. the example box should not be
    // retranslated from here.
    if (strcmp(wc->name(), "unnamed") == 0) continue;
    
    ((QLabel *)wc)->setText( locale->translate( wc->name() ) );
  }
  delete list;

  // Here we have the pointer
  gbox->setCaption(i18n("Examples"));
  tab->changeTab(localemain, locale->translate("&Locale"));
  tab->changeTab(localenum, locale->translate("&Numbers"));
  tab->changeTab(localemon, locale->translate("&Money"));
  tab->changeTab(localetime, locale->translate("&Time && dates"));

  // retranslate some other widgets
  localemain->reTranslate();
  localenum->reTranslate();
  localemon->reTranslate();
  localetime->reTranslate();

  // FIXME: All widgets are done now. However, there are 
  // still some problems. Popup menus from the QLabel are 
  // not retranslated.
}

void KLocaleApplication::reset()
{
  localenum->reset();
  localemon->reset();
  localetime->reset();
}

void KLocaleApplication::newChset()
{
  // figure out which font to use
  KConfig *c = KGlobal::config();
  c->setGroup( QString::fromLatin1("General") );

  QFont font(QString::fromLatin1("helvetica"), 12, QFont::Normal);
  KGlobal::charsets()->setQFont(font, locale->charset());
  font = c->readFontEntry(QString::fromLatin1("font"), &font);

  // set the font for all subwidgets
  QObjectList *list = queryList("QWidget");
  QObjectListIt it(*list);

  QObject *w;
  while ( (w = it.current()) != 0 ) {
    ++it;
    ((QWidget *)w)->setFont(font);
  }
  delete list;

  // FIXME: the context help are still using the old charset

  moduleChanged(true);
}
