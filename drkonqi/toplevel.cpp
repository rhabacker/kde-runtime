/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id$
 *
 * Copyright (C) 2000-2003 Hans Petter Bieker <bieker@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************/

#include <qstring.h>
#include <qlabel.h>
#include <qhbox.h>

#include "netwm.h"

#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kbugreport.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kapplication.h>
#include <dcopclient.h>

#include "backtrace.h"
#include "drbugreport.h"
#include "debugger.h"
#include "krashconf.h"
#include "toplevel.h"
#include "toplevel.moc"

Toplevel :: Toplevel(KrashConfig *krashconf, QWidget *parent, const char *name)
  : KDialogBase( Tabbed,
                 krashconf->programName(),
                 User3 | User2 | User1 | Close,
                 Close,
                 parent,
                 name,
                 true, // modal
                 false, // no separator
                 i18n("&Bug report"),
                 i18n("&Debugger")
                 ),
    m_krashconf(krashconf), m_bugreport(0)
{
  QHBox *page = addHBoxPage(i18n("&General"));
  page->setSpacing(20);

  // picture of konqi
  QLabel *lab = new QLabel(page);
  lab->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QPixmap pix(locate("appdata", QString::fromLatin1("pics/konqi.png")));
  lab->setPixmap(pix);
  lab->setFixedSize( lab->sizeHint() );

  QLabel * info = new QLabel(generateText(), page);
  info->setMinimumSize(info->sizeHint());

  if (m_krashconf->showBacktrace())
  {
    page = addHBoxPage(i18n("&Backtrace"));
    new KrashDebugger(m_krashconf, page);
  }

  showButton( User1, m_krashconf->showBugReport() );
  showButton( User2, m_krashconf->showDebugger() );
  showButton( User3, false );

  connect(this, SIGNAL(closeClicked()), SLOT(accept()));
  connect(m_krashconf, SIGNAL(newDebuggingApplication(const QString&)), SLOT(slotNewDebuggingApp(const QString&)));

  if ( !m_krashconf->safeMode() && kapp->dcopClient()->attach() )
    kapp->dcopClient()->registerAs( kapp->name() );
}

Toplevel :: ~Toplevel()
{
}

QString Toplevel :: generateText() const
{
  QString str;

  if (!m_krashconf->errorDescriptionText().isEmpty())
    str += i18n("<p><b>Short description</b></p><p>%1</p>")
      .arg(m_krashconf->errorDescriptionText());

  if (!m_krashconf->signalText().isEmpty())
    str += i18n("<p><b>What is this?</b></p><p>%1</p>")
      .arg(m_krashconf->signalText());

  if (!m_krashconf->whatToDoText().isEmpty())
    str += i18n("<p><b>What can I do?</b></p><p>%1</p>")
      .arg(m_krashconf->whatToDoText());

  // check if the string is still empty. if so, display a default.
  if (str.isEmpty())
    str = i18n("<p><b>Application crashed</b></p>"
               "<p>The program %appname crashed.</p>");

  // scan the string for %appname etc
  m_krashconf->expandString(str, false);

  return str;
}

// starting bug report
void Toplevel :: slotUser1()
{
  if (m_bugreport)
    return;

  int i = KMessageBox::No;
  if ( m_krashconf->pid() != 0 )
    i = KMessageBox::warningYesNoCancel
      (0,
       i18n("<p>Do you want to generate a "
            "backtrace? This will help the "
            "developers to figure out what went "
            "wrong.</p>\n"
            "<p>Unfortunately this will take some "
            "time on slow machines.</p>"
            "<p><b>Note: A backtrace is not a "
            "substitute for a proper description "
            "of the bug and information on how to "
            "reproduce it. It is not possible "
            "to fix the bug without a proper "
            "description.</b></p>"),
       i18n("Include Backtrace"));

    if (i == KMessageBox::Cancel) return;

  m_bugreport = new DrKBugReport(0, true, m_krashconf->aboutData());

  if (i == KMessageBox::Yes) {
    QApplication::setOverrideCursor ( waitCursor );

    // generate the backtrace
    BackTrace *backtrace = new BackTrace(m_krashconf, this);
    connect(backtrace, SIGNAL(someError()), SLOT(slotBacktraceSomeError()));
    connect(backtrace, SIGNAL(done(const QString &)),
            SLOT(slotBacktraceDone(const QString &)));

    backtrace->start();

    return;
  }

  int result = m_bugreport->exec();
  delete m_bugreport;
  m_bugreport = 0;
  if (result == KDialogBase::Accepted)
     close();
}

void Toplevel :: slotUser2()
{
  QString str = m_krashconf->debugger();
  m_krashconf->expandString(str, true);

  KProcess proc;
  proc.setUseShell(true);
  proc << str;
  proc.start(KProcess::DontCare);
}

void Toplevel :: slotNewDebuggingApp(const QString& launchName)
{
  setButtonText( User3, launchName );
  showButton( User3, true );
}

void Toplevel :: slotUser3()
{
  m_krashconf->acceptDebuggingApp();
}

void Toplevel :: slotBacktraceDone(const QString &str)
{
  // Do not translate.. This will be included in the _MAIL_.
  QString buf = QString::fromLatin1
    ("\n\n\nHere is a backtrace generated by DrKonqi:\n") + str;

  m_bugreport->setText(buf);

  QApplication::restoreOverrideCursor();

  m_bugreport->exec();
  delete m_bugreport;
  m_bugreport = 0;
}

void Toplevel :: slotBacktraceSomeError()
{
  QApplication::restoreOverrideCursor();

  KMessageBox::sorry(0, i18n("It was not possible to generate a backtrace."),
                     i18n("Backtrace Not Possible"));

  m_bugreport->exec();
  delete m_bugreport;
  m_bugreport = 0;
}

