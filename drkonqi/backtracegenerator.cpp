/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * Copyright (C) 2000-2003 Hans Petter Bieker <bieker@kde.org>
 * Copyright (C) 2009  George Kiagiadakis <gkiagia@users.sourceforge.net>
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

#include "backtracegenerator.h"

#include <KDebug>
#include <KStandardDirs>
#include <KMessageBox>
#include <KLocale>
#include <KTemporaryFile>
#include <KShell>

#include <signal.h>

#include "krashconf.h"

BacktraceGenerator::BacktraceGenerator(const KrashConfig *krashconf, QObject *parent)
  : QObject(parent),
    m_krashconf(krashconf), m_proc(NULL), m_temp(NULL)
{
  //stop the process to avoid high cpu usage by other threads (bug 175362)
  ::kill(m_krashconf->pid(), SIGSTOP);
}

BacktraceGenerator::~BacktraceGenerator()
{
  stop();

  //let the app continue, so that it can kill itself when drkonqi exits.
  ::kill(m_krashconf->pid(), SIGCONT);
}

bool BacktraceGenerator::start()
{
  Q_ASSERT(m_proc == NULL && m_temp == NULL); //they should always be null before entering this function.

  QString exec = m_krashconf->tryExec();
  if ( !exec.isEmpty() && KStandardDirs::findExe(exec).isEmpty() )
    return false;

  emit starting();

  m_proc = new KProcess;
  m_proc->setEnv( "LC_ALL", "C" ); // force C locale

  m_temp = new KTemporaryFile;
  m_temp->open();
  m_temp->write(m_krashconf->backtraceCommand().toLatin1());
  m_temp->write("\n", 1);
  m_temp->flush();

  // start the debugger
  QString str = m_krashconf->debuggerBatchCommand();
  m_krashconf->expandString(str, KrashConfig::ExpansionUsageShell, m_temp->fileName());

  *m_proc << KShell::splitArgs(str);
  m_proc->setOutputChannelMode(KProcess::SeparateChannels); // Drop stderr
  m_proc->setNextOpenMode(QIODevice::ReadWrite | QIODevice::Text);
  connect(m_proc, SIGNAL(readyReadStandardOutput()),
          SLOT(slotReadInput()));
  connect(m_proc, SIGNAL(finished(int, QProcess::ExitStatus)),
          SLOT(slotProcessExited(int, QProcess::ExitStatus)));

  //continue running the process so that gdb can inspect it.
  ::kill(m_krashconf->pid(), SIGCONT);

  m_proc->start();
  if (!m_proc->waitForStarted())
  {
      //we mustn't keep these around...
      m_proc->deleteLater();
      m_temp->deleteLater();
      m_proc = NULL;
      m_temp = NULL;

      //stop the process again, as gdb is not running
      ::kill(m_krashconf->pid(), SIGSTOP);

      emit someError();
      return false;
  }
  return true;
}

void BacktraceGenerator::stop()
{
    if (m_proc && m_proc->state() == QProcess::Running)
    {
        m_proc->kill();
        m_proc->waitForFinished();
        ::kill(m_krashconf->pid(), SIGSTOP);
    }

    if ( m_proc ) {
        m_proc->deleteLater();
        m_proc = NULL;
    }

    if ( m_temp ) {
        m_temp->deleteLater();
        m_temp = NULL;
    }
}

void BacktraceGenerator::slotReadInput()
{
    // we do not know if the output array ends in the middle of an utf-8 sequence
    m_output += m_proc->readAllStandardOutput();

    int pos;
    while ((pos = m_output.indexOf('\n')) != -1)
    {
        QString line = QString::fromLocal8Bit(m_output, pos + 1);
        m_output.remove(0, pos + 1);

        emit newLine(line);
    }
}

void BacktraceGenerator::slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    //stop the process again.
    ::kill(m_krashconf->pid(), SIGSTOP);

    QProcess::ProcessError error = m_proc->error();
    
    //these are useless now
    m_proc->deleteLater();
    m_temp->deleteLater();
    m_proc = NULL;
    m_temp = NULL;

    //mark the end of the backtrace for the parser
    emit newLine(QString());

    if (exitStatus != QProcess::NormalExit || exitCode != 0)
    {
        if( error == QProcess::FailedToStart )
        {
            emit failedToStart();
        }
        else
        {
            emit someError();
        }
        
        return;
    }

    emit done();
}

#include "backtracegenerator.moc"
