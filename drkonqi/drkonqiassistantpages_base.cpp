/*******************************************************************
* drkonqiassistantpages_base.cpp
* Copyright 2000-2003 Hans Petter Bieker <bieker@kde.org>     
*           2009    Dario Andres Rodriguez <andresbajotierra@gmail.com>
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

#include "drkonqiassistantpages_base.h"
#include "drkonqi.h"
#include "backtraceparser.h"
#include "backtracegenerator.h"

#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>

#include <kpushbutton.h>
#include <ktextbrowser.h>
#include <ktoolinvocation.h>
#include <kicon.h>
#include <klocale.h>

//BEGIN IntroductionPage

IntroductionPage::IntroductionPage() : 
    DrKonqiAssistantPage()
{
    QLabel * mainLabel = new QLabel(
        i18n("<para>This assistant will analyse the crash information and guide you through the bug reporting process</para><para>You can get help about this bug reporting assistant clicking the \"Help\" button</para><para>To start gathering the crash information press the \"Next\" button</para>")
    );
    mainLabel->setWordWrap( true );

    QVBoxLayout * layout = new QVBoxLayout( this );
    layout->addWidget( mainLabel );
    layout->addStretch();
    setLayout( layout );
}

//END IntroductionPage

//BEGIN CrashInformationPage

CrashInformationPage::CrashInformationPage() 
    : DrKonqiAssistantPage(),
{
    m_backtraceWidget = new GetBacktraceWidget(DrKonqi::instance()->backtraceGenerator());
    
    //connect backtraceWidget signals
    connect( m_backtraceWidget, SIGNAL(stateChanged()) , this, SLOT(emitCompleteChanged()) );
    
    QLabel * subTitle = new QLabel(
        i18n( "This page will fetch some useful information about the crash to generate a better bug report" )
    );
    subTitle->setWordWrap( true ); 
    subTitle->setAlignment( Qt::AlignHCenter );

    QVBoxLayout * layout = new QVBoxLayout( this );
    layout->addWidget( subTitle );
    layout->addWidget( m_backtraceWidget );
    setLayout( layout );    
}

void CrashInformationPage::aboutToShow()
{ 
    m_backtraceWidget->generateBacktrace(); 
    emitCompleteChanged();
}

bool CrashInformationPage::isComplete()
{
    return ( DrKonqi::instance()->backtraceGenerator()->state() != BacktraceGenerator::NotLoaded &&
        DrKonqi::instance()->backtraceGenerator()->state() != BacktraceGenerator::Loading );
}

//END CrashInformationPage

//BEGIN BugAwarenessPage

BugAwarenessPage::BugAwarenessPage(CrashInfo * info) 
    : DrKonqiAssistantPage(),
    m_crashInfo(info)
{
    //Details groupbox
    //QGroupBox * canDetailBox = new QGroupBox();
    //canDetailBox->setFlat( true );
    
    QVBoxLayout * canDetailLayout = new QVBoxLayout( );
    //canDetailBox->setLayout( canDetailLayout );
    
    QLabel * detailQuestionLabel = new QLabel( QString("<strong>%1</strong>").arg( i18n( "Can you give detailed information about what were you doing when the application crashed ?" ) ) );
    detailQuestionLabel->setWordWrap( true );
    canDetailLayout->addWidget( detailQuestionLabel );
    
    QLabel * detailLabel = 
        new QLabel(
        i18n(
        "Including in your bug report what were you doing when the application crashed will help the developers "
        "reproduce and fix the bug. <i>( Important details are: what you were doing in the application and documents you were using )</i>. If you have found a common patterns which causes a crash you can try to write steps to help the developers try to reproduce it." ) );
        
    detailLabel->setWordWrap( true );
    canDetailLayout->addWidget( detailLabel );
    
    m_canDetailCheckBox = new QCheckBox( i18n( "Yes, I can give information about the crash" ) );
    canDetailLayout->addWidget( m_canDetailCheckBox );
    
    //Compromise groupbox
    //QGroupBox * getCompromiseBox = new QGroupBox( i18n( "Are you willing to help the developers? and and and and and and and and and and and and and and and and and and and and and and and" ) );
    
    QVBoxLayout * getCompromiseLayout = new QVBoxLayout();
    //getCompromiseBox->setLayout( getCompromiseLayout );
    
    QLabel * l2 = new QLabel( i18n( "<strong>Are you willing to help the developers?</strong>" ) );
    getCompromiseLayout->addWidget( l2 );
    
    QLabel * compromiseLabel = 
        new QLabel(
        i18n( "Sometimes once the report is created and being investigated, the developers need more information from the reporter in order to fix the bug." )
        );
    compromiseLabel->setWordWrap( true );
    getCompromiseLayout->addWidget( compromiseLabel );
    
    m_willingToHelpCheckBox = 
        new QCheckBox(
        i18n( "Yes, I am willing to help the developers" )
        );
    getCompromiseLayout->addWidget( m_willingToHelpCheckBox );
    
    //Reproduce groupbox
    
    
    /*
    QGroupBox * canReproduceBox = new QGroupBox( i18n( "Advanced Usage" ) );
    QVBoxLayout * canReproduceLayout = new QVBoxLayout( canReproduceBox );
    canReproduceBox->setLayout( canReproduceLayout );
    
    m_canReproduceCheckBox = new QCheckBox( i18n( "I can reproduce the crash and I can provide steps or a testcase and and and and and and and and and and and" ), canReproduceBox );
    canReproduceLayout->addWidget( m_canReproduceCheckBox);
    */
    
    //Main layout
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSpacing( 10 );
    //layout->addWidget( canDetailBox );
    layout->addLayout( canDetailLayout );
    //layout->addWidget( getCompromiseBox );
    layout->addLayout( getCompromiseLayout );
    layout->addStretch();
    //layout->addWidget( canReproduceBox );
    setLayout( layout );
}

void BugAwarenessPage::aboutToHide()
{
    //Save data
    m_crashInfo->setUserCanDetail( m_canDetailCheckBox->checkState() == Qt::Checked );
    //m_crashInfo->setUserCanReproduce( m_canReproduceCheckBox->checkState() == Qt::Checked );
    m_crashInfo->setUserIsWillingToHelp( m_willingToHelpCheckBox->checkState() == Qt::Checked );
}

//END BugAwarenessPage

//BEGIN ConclusionPage

ConclusionPage::ConclusionPage(CrashInfo * info) 
    : DrKonqiAssistantPage(),
    m_crashInfo(info),
    isBKO(false),
    needToReport(false)
{
    m_reportEdit = new KTextBrowser();
    m_reportEdit->setReadOnly( true );

    m_saveReportButton = new KPushButton( KGuiItem( i18nc( "Button action", "&Save to File" ), KIcon("document-save"), i18nc("button explanation", "Use this button to save the generated report and conclusions about this crash to a file. You can use this option to report the bug later."), i18nc("button explanation", "Use this button to save the generated report and conclusions about this crash to a file. You can use this option to report the bug later.") ) );
    connect(m_saveReportButton, SIGNAL(clicked()), this, SLOT(saveReport()) );
    
    m_reportButton = new KPushButton( KGuiItem( i18nc( "button action", "&Report bug to the application maintainer" ), KIcon("document-new"), i18nc( "button explanation", "Use this button to report this bug to the application maintainer. This will launch the browser or the mail client using the assigned bug report address" ), i18nc( "button explanation", "Use this button to report this bug to the application maintainer. This will launch the browser or the mail client using the assigned bug report address" ) ) );
    m_reportButton->setVisible( false );
    connect( m_reportButton, SIGNAL(clicked()), this , SLOT(reportButtonClicked()) );

    m_explanationLabel = new QLabel();
    m_explanationLabel->setWordWrap( true );
    
    QHBoxLayout *bLayout = new QHBoxLayout();
    bLayout->addStretch();
    bLayout->addWidget( m_saveReportButton );
    bLayout->addWidget( m_reportButton );

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing( 10 );
    layout->addWidget( m_reportEdit );
    layout->addWidget( m_explanationLabel );
    layout->addLayout( bLayout );
    setLayout( layout );
}

void ConclusionPage::saveReport()
{
    DrKonqi::saveReport(m_reportEdit->toPlainText(), this);
}

void ConclusionPage::reportButtonClicked()
{
    if( m_crashInfo->isReportMail() )
    {
        QString subject = QString( "Automatic crash report generated by DrKonqi for " + m_crashInfo->getProductName() );
        QString body = m_crashInfo->generateReportTemplate();
        KToolInvocation::invokeMailer( m_crashInfo->getReportLink(), "","" ,subject, body);
    }
    else
    {
        KToolInvocation::invokeBrowser( m_crashInfo->getReportLink() );
    }
    
    emit finished();
}

void ConclusionPage::aboutToShow()
{
    isBKO = false;
    needToReport = false;
    emitCompleteChanged();
    
    QString report;
    
    BacktraceParser::Usefulness use = DrKonqi::instance()->backtraceGenerator()->parser()->backtraceUsefulness();
    bool canDetails = m_crashInfo->getUserCanDetail();
    //bool canReproduce = m_crashInfo->getUserCanReproduce();
    bool willingToHelp = m_crashInfo->getUserIsWillingToHelp();
    
    switch( use )
    {
        case BacktraceParser::ReallyUseful:
        {
            needToReport = ( canDetails ); //TODO discuss: always true ?
            report = i18n( "* The crash information is really useful and worth reporting" );
            break;
        }
        case BacktraceParser::MayBeUseful:
        {
            needToReport = ( canDetails || willingToHelp );
            report = i18n( "* The crash information lacks some details but may be useful" ) ;
            break;
        }
        case BacktraceParser::ProbablyUseless:
        {
            needToReport = ( canDetails && willingToHelp );
            report = i18n( "* The crash information lacks a lot of important details and it is probably useless" );
            break;
        }           
        case BacktraceParser::Useless:
        case BacktraceParser::InvalidUsefulness:
        {
            needToReport =  ( canDetails && willingToHelp );
            report = i18n( "* The crash information is completely useless" ) ;
     break;
        }
    }
    
    report.append( QLatin1String( "<br />" ) );
    if( canDetails )
        report += i18n( "* You can explain in detail what were you doing when the application crashed" );
    else
            report += i18n( "* You aren't sure what were you doing when the application crashed" ) ;
       /*        report += i18n( "* You can't say what were you doing when the application crashed" ) ;
        There's something between "very detailed" and "no clue"... */
    
    /* FIXME
    report.append( QLatin1String( "<br />" ) );
    if( canReproduce )
        report += i18n( "* You can reproduce the crash at will and you can provide steps or a testcase" );
    else
        report += i18n( "* You can't reproduce the crash at will, but you can provide steps or a testcase" ); // is that possible for newbies?
    */  
    
    report.append( QLatin1String( "<br />" ) );
    if( willingToHelp )
        report += i18n( "* You are willing to help the developers" );
    else
        report += i18n( "* You are not willing to help the developers" );
        
    if ( needToReport )
    {
        QString reportMethod;
        QString reportLink;
        
        isBKO = m_crashInfo->isKDEBugzilla();
        m_reportButton->setVisible( !isBKO );
        if ( isBKO )
        {
            emitCompleteChanged();
            m_explanationLabel->setText( i18n( "This application is supported in the KDE Bugtracker, press Next to start the report assistant" ) );
            
            reportMethod = i18n( "You need to file a new bug report, press Next to start the report assistant" );
            reportLink = i18nc( "address to report the bug", "Report at http://bugs.kde.org" );
        }
        else
        {
            m_explanationLabel->setText( i18n( "<strong>Notice:</strong> This application isn't supported in the KDE Bugtracker, you need to report the bug to the maintainer" ) );
            
            reportMethod = i18n( "You need to file a new bug report with the following information:" );
            reportLink =  i18nc( "address(mail or web) to report the bug", "Report to %1", m_crashInfo->getReportLink() );
            
            if( m_crashInfo->isReportMail() )
            {
                m_reportButton->setGuiItem( KGuiItem( i18n("Send a mail to the application maintainer" ), KIcon("mail-message-new"), i18n( "Launches the mail client to send an email to the application maintainer with the crash information" ) , i18n( "Launch the mail client to send an email to the application maintainer with the crash information" ) ) ) ;
            }
            else
            {
                m_reportButton->setGuiItem( KGuiItem( i18n("Open the application maintainer web site" ), KIcon("internet-web-browser"), i18n( "Launches the web browser showing the application web page in order to contact the maintainer" ) , i18n( "Launches the web browser showing the application web page in order to contact the maintainer" ) ) ) ;
            }
        }
        
        report += QString("<br /><strong>%1</strong><br />%2<br /><br />--------<br /><br />%3").arg( i18n( "The crash is worth reporting" ), reportMethod, m_crashInfo->generateReportTemplate() );
        
        report += QString("<br /><br />") + reportLink;
    }
    else
    {
        m_reportButton->setVisible( false );
        
        report += QString("<br /><strong>%1</strong><br />%2<br />--------<br /><br />%3").arg( i18n( "The crash is not worth reporting" ), i18n( "However you can report it on your own if you want, using the following information:" ), m_crashInfo->generateReportTemplate() );
        
        report.append( QString( "<br /><br />" ) );
        if ( m_crashInfo->isKDEBugzilla() )
        {
            report += i18nc( "address to report the bug", "Report at http://bugs.kde.org" );
            m_explanationLabel->setText( i18n( "This application is supported in the KDE Bugtracker, you can report this bug at https://bugs.kde.org" ) );
        }
        else
        {
            report += i18nc( "address(mail or web) to report the bug", "Report to %1", m_crashInfo->getReportLink() );
            m_explanationLabel->setText( i18n( "This application isn't supported in the KDE Bugtracker, you can report thia bug to its maintainer : <i>%1</i>", m_crashInfo->getReportLink() ) );
        }
        
        emit finished();
    }
    
    m_reportEdit->setHtml( report );
}

bool ConclusionPage::isComplete()
{
    return ( isBKO && needToReport );
}

//END ConclusionPage
