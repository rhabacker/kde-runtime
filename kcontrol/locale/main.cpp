/*
  main.cpp - A KControl Application

  written 1998 by Matthias Hoelzer
  
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

#include <stdio.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qobjectlist.h>

#include <kcontrol.h>
#include <klocale.h>

#include <kglobal.h>

#include "locale.h"
#include "localenum.h"
#include "localemon.h"
#include "main.h"

KLocaleApplication::KLocaleApplication(int &argc, char **argv, const char *name)
  : KControlApplication(argc, argv, name)
{
  locale = 0;

  if (runGUI())
    {
      if (!pages || pages->contains("language"))
        addPage(locale = new KLocaleConfig(dialog, "locale"), 
		i18n("&Locale"), "locale-1.html");
      if (!pages || pages->contains("number"))
        addPage(localenum = new KLocaleConfigNumber(dialog, "number"), 
		i18n("&Numbers"), "locale-2.html");
      if (!pages || pages->contains("money"))
        addPage(localemon = new KLocaleConfigMoney(dialog, "money"), 
		i18n("&Money"), "locale-3.html");

      reTranslate();

      if (locale || localenum || localemon)
        dialog->show();
      else
        {
          fprintf(stderr, i18n("usage: kcmlocale [-init | language | number | money]\n").ascii());
	  justInit = TRUE;
        }
    }
}


void KLocaleApplication::init()
{
}

void KLocaleApplication::apply()
{
  if (locale)
    locale->applySettings();
  if (localenum)
    localenum->applySettings();
  if (localemon)
    localemon->applySettings();
}

void KLocaleApplication::defaultValues()
{
  if (locale)
    locale->defaultSettings();
  if (localenum)
    localemon->defaultSettings();
  if (localemon)
    localemon->defaultSettings();
}

void KLocaleApplication::reTranslate()
{
  QObjectList it;
  it.append(kapp-> mainWidget());
  reTranslate(it);
}

void KLocaleApplication::reTranslate(QObjectListIt it)
{
    QObject *wc;
    while( (wc = it.current()) != 0 ) {
      ++it;
      if (wc->children())
        reTranslate(QObjectListIt(*wc->children()));

      if ( !qstrcmp(wc->name(), "unnamed") )
         continue;
      if ( !wc->isWidgetType() )
         continue;

      if ( !qstrcmp(wc->className(), "QGroupBox"))
      {
        ((QGroupBox *)wc)->setTitle(i18n(wc->name()));
        ((QGroupBox *)wc)->setMinimumSize(((QGroupBox *)wc)->sizeHint());
      }
      else if ( !qstrcmp(wc->className(), "QLabel"))
      {
        ((QLabel *)wc)->setText(i18n(wc->name()));
        ((QLabel *)wc)->setMinimumSize(((QLabel *)wc)->sizeHint());
      }
    }
}

void KLocaleApplication::reset()
{
  resetNum();
  resetMon();
}

void KLocaleApplication::resetMon()
{
  if (localemon)
    localemon->syncWithKLocaleMon();
}

void KLocaleApplication::resetNum()
{
  if (localenum)
    localenum->syncWithKLocaleNum();
}

int main(int argc, char **argv)
{
  KLocaleApplication app(argc, argv, "kcmlocale");
  app.setTitle(i18n("Locale settings"));
  
  if (app.runGUI())
    return app.exec();
  else
    {
      app.init();
      return 0;
    }
}
