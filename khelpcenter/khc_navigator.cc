/*
 *  khc_navigator.cc - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <qdir.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlabel.h>
#include <qheader.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <kaction.h>
#include <kapplication.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kprocio.h>
#include <kcharsets.h>
#include <kdialog.h>

#include "khc_navigatoritem.h"
#include "khc_navigatorappitem.h"
#include "searchwidget.h"
#include "khc_factory.h"
#include "khc_infoconsts.h"
#include "khc_infohierarchymaker.h"
#include "khc_infonode.h"
#include "searchengine.h"
#include "khc_view.h"
#include "docmetainfo.h"
#include "docentrytraverser.h"

#include "khc_navigator.h"
#include "khc_navigator.moc"

namespace {

QString langLookup(const QString &fname)
{
    QStringList search;

    // assemble the local search paths
    const QStringList localDoc = KGlobal::dirs()->resourceDirs("html");

    // look up the different languages
    for (int id=localDoc.count()-1; id >= 0; --id)
    {
        QStringList langs = KGlobal::locale()->languageList();
        langs.append("default");
        langs.append("en");
        QStringList::ConstIterator lang;
        for (lang = langs.begin(); lang != langs.end(); ++lang)
            search.append(QString("%1%2/%3").arg(localDoc[id]).arg(*lang).arg(fname));
    }

    // try to locate the file
    QStringList::Iterator it;
    for (it = search.begin(); it != search.end(); ++it)
    {
        kdDebug(1400) << "Looking for help in: " << *it << endl;

        QFileInfo info(*it);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;

        QString file = (*it).left((*it).findRev('/')) + "/index.docbook";
        kdDebug(1400) << "Looking for help in: " << file << endl;
        info.setFile(file);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;
    }

    return QString::null;
}
};

SectionItem::SectionItem(QListViewItem *parent, const QString &text)
        : QListViewItem(parent, text)
{
  setOpen(false);
}

void SectionItem::setOpen(bool open)
{
  if (open)
    setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("contents"), KIcon::Small));
  else
    setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("contents2"), KIcon::Small));

  QListViewItem::setOpen(open);
}

TOCListView::TOCListView( QWidget *parent )
  : KListView( parent )
{
  setFrameStyle( QFrame::Panel | QFrame::Sunken );
  addColumn( QString::null );
	header()->hide();
  setSorting( -1, true );
	setRootIsDecorated( true );
}

TOCListViewItem::TOCListViewItem( TOCListViewItem *parent, const QString &text )
  : QListViewItem( parent, text )
{
}

TOCListViewItem::TOCListViewItem( TOCListViewItem *parent, QListViewItem *after, const QString &text )
  : QListViewItem( parent, after, text )
{
}

TOCListViewItem::TOCListViewItem( TOCListView *parent, const QString &text )
  : QListViewItem( parent, text )
{
}

TOCListViewItem::TOCListViewItem( TOCListView *parent, QListViewItem *after, const QString &text  )
  : QListViewItem( parent, after, text )
{
}

TOCListView *TOCListViewItem::toc() const
{
  return static_cast<TOCListView *>( listView() );
}

TOCChapterItem::TOCChapterItem( TOCListView *parent, const QString &title, const QString &name )
  : TOCListViewItem( parent, title ), m_name( name )
{
  setOpen( false );
}

TOCChapterItem::TOCChapterItem( TOCListView *parent, QListViewItem *after, const QString &title, const QString &name )
  : TOCListViewItem( parent, after, title ), m_name( name )
{
  setOpen( false );
}

void TOCChapterItem::setOpen( bool open )
{
  setPixmap( 0, SmallIcon( open ? "contents" : "contents2" ) );

  QListViewItem::setOpen(open);
}

QString TOCChapterItem::link() const
{
  return "help:" + toc()->application() + "/" + m_name + ".html";
}

TOCSectionItem::TOCSectionItem( TOCChapterItem *parent, const QString &title, const QString &name )
  : TOCListViewItem( parent, title ), m_name( name )
{
  setPixmap( 0, SmallIcon( "document" ) );
}

TOCSectionItem::TOCSectionItem( TOCChapterItem *parent, QListViewItem *after, const QString &title, const QString &name )
  : TOCListViewItem( parent, after, title ), m_name( name )
{
  setPixmap( 0, SmallIcon( "document" ) );
}

QString TOCSectionItem::link() const
{
  if ( static_cast<TOCSectionItem *>( parent()->firstChild() ) == this )
    return static_cast<TOCChapterItem *>( parent() )->link() + "#" + m_name;
  else
    return "help:" + toc()->application() + "/" + m_name + ".html";
}

khcNavigator::khcNavigator( KHCView *view, QWidget *parentWidget,
                            QObject *parent, const char *name )
    : KParts::ReadOnlyPart(parent,name)
{
    kdDebug(1400) << "khcNavigator::khcNavigator\n";
    setInstance( KHCFactory::instance() );

    setWidget( new khcNavigatorWidget( view, parentWidget ) );

    m_extension = new khcNavigatorExtension( this, "khcNavigatorExtension" );
    connect( widget(), SIGNAL( itemSelected(const QString&) ),
             m_extension, SLOT( slotItemSelected(const QString&) ) );
}

khcNavigator::~khcNavigator()
{
  // KParts deletes the widget. Cool.
}

bool khcNavigator::openURL( const KURL & )
{
    emit started( 0 );
    emit completed();
    return true;
}

bool khcNavigator::openFile()
{
  return true; // easy one...
}

void khcNavigatorExtension::slotItemSelected(const QString& url)
{
    KParts::URLArgs urlArgs(true, 0, 0);

    kdDebug(1400) << "request URL " << url << endl;

    emit openURLRequest( url, urlArgs );
}

khcNavigatorWidget::khcNavigatorWidget( KHCView *view, QWidget *parent,
                                        const char *name )
   : QWidget( parent, name ),
     mView( view )
{
    KConfig *config = kapp->config();
    config->setGroup("ScrollKeeper");
    mScrollKeeperShowEmptyDirs = config->readBoolEntry("ShowEmptyDirs",false);

    DocMetaInfo::self()->scanMetaInfo();

    mSearchEngine = new SearchEngine( view );
    connect( mSearchEngine, SIGNAL( searchFinished() ),
             SLOT( slotSearchFinished() ) );

    QBoxLayout *topLayout = new QVBoxLayout( this );

    mSearchFrame = new QFrame( this );
    topLayout->addWidget( mSearchFrame );

    QBoxLayout *searchLayout = new QHBoxLayout( mSearchFrame );
    searchLayout->setMargin( KDialog::spacingHint() );

    mSearchEdit = new QLineEdit( mSearchFrame );
    searchLayout->addWidget( mSearchEdit );
    connect( mSearchEdit, SIGNAL( returnPressed() ), SLOT( slotSearch() ) );
    connect( mSearchEdit, SIGNAL( textChanged( const QString & ) ),
             SLOT( slotSearchTextChanged( const QString & ) ) );

    mSearchButton = new QPushButton( i18n("Search"), mSearchFrame );
    searchLayout->addWidget( mSearchButton );
    connect( mSearchButton, SIGNAL( clicked() ), SLOT( slotSearch() ) );

    mTabWidget = new QTabWidget( this );
    topLayout->addWidget( mTabWidget );

    setupContentsTab();
    setupSearchTab();
    setupGlossaryTab();
    setupTOCTab();

    // compiling the regex used while parsing the info directory (dir) file
    int nResult = regcomp(&compInfoRegEx, "^\\* ([^:]+)\\: \\(([^)]+)\\)([[:space:]]|(([^.]*)\\.)).*$", REG_EXTENDED);
    Q_ASSERT(!nResult);

    // set up the timer which produces signals every 30 sec. The cleanHierarchyMakers function
    // checks entries in the hierarchyMakers map and deletes these which have already finished work.
    connect(&cleaningTimer, SIGNAL(timeout()), SLOT(slotCleanHierarchyMakers()));
    cleaningTimer.start(30000);

    /* Cog-wheel animation handling -- enable after creating the icons
    m_animationTimer = new QTimer( this );
    connect( m_animationTimer, SIGNAL( timeout() ), this, SLOT( slotAnimation() ) );
    */

    buildTree();

    connect( mTabWidget, SIGNAL( currentChanged( QWidget * ) ),
             SLOT( slotShowPage( QWidget * ) ) );

    if ( DocMetaInfo::self()->searchEntries().isEmpty() ) {
      hideSearch();
    } else {
      mSearchWidget->updateScopeList();
      slotSearchTextChanged( mSearchEdit->text() );
    }
}

khcNavigatorWidget::~khcNavigatorWidget()
{
  for (std::map<khcNavigatorItem*, khcInfoHierarchyMaker*>::iterator it = hierarchyMakers.begin();
       it != hierarchyMakers.end(); )
  {
    std::map<khcNavigatorItem*, khcInfoHierarchyMaker*>::iterator copyIt(it);
    it++;
    delete copyIt->second;
    hierarchyMakers.erase(copyIt);
  }

  regfree(&compInfoRegEx);
  glossEntries.setAutoDelete( true );
  glossEntries.clear();

  delete mSearchEngine;
}

void khcNavigatorWidget::setupContentsTab()
{
    contentsTree = new KListView( mTabWidget );
    contentsTree->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    contentsTree->addColumn(QString::null);
    contentsTree->setAllColumnsShowFocus(true);
    contentsTree->header()->hide();
    contentsTree->setRootIsDecorated(false);
    contentsTree->setSorting(-1, false);

    connect(contentsTree, SIGNAL(executed(QListViewItem*)),
            SLOT(slotItemSelected(QListViewItem*)));
    connect(contentsTree, SIGNAL(returnPressed(QListViewItem*)),
           SLOT(slotItemSelected(QListViewItem*)));
    connect(contentsTree, SIGNAL(expanded(QListViewItem*)),
            SLOT(slotItemExpanded(QListViewItem*)));
    mTabWidget->addTab(contentsTree, i18n("&Contents"));
}

void khcNavigatorWidget::setupSearchTab()
{
    mSearchWidget = new SearchWidget( mTabWidget );
    connect( mSearchWidget, SIGNAL( searchResult( const QString & ) ),
             SLOT( slotShowSearchResult( const QString & ) ) );
    connect( mSearchWidget, SIGNAL( enableSearch( bool ) ),
             mSearchButton, SLOT( setEnabled( bool ) ) ); 

    mTabWidget->addTab( mSearchWidget, i18n("Search"));
}

void khcNavigatorWidget::slotShowPage(QWidget *w)
{
    if ((w == glossaryTree) && (glossaryHtmlStatus == CacheOk))
    {
       buildGlossary();
    }
    mTabWidget->showPage(w);
}

void khcNavigatorWidget::setupTOCTab()
{
  tocTree = new TOCListView( this );
	connect( tocTree, SIGNAL( executed( QListViewItem * ) ),
	         this, SLOT( slotTOCItemSelected( QListViewItem * ) ) );
	connect( tocTree, SIGNAL( returnPressed( QListViewItem * ) ),
	         this, SLOT( slotTOCItemSelected( QListViewItem * ) ) );

	resetTOCTree();

	mTabWidget->addTab( tocTree, "&Table of contents" );
}

void khcNavigatorWidget::resetTOCTree()
{
	while ( tocTree->firstChild() )
		delete tocTree->firstChild();

	tocTree->insertItem( new QListViewItem( tocTree, i18n( "No manual selected" ) ) );
}

void khcNavigatorWidget::fillTOCTree( const QDomDocument &doc )
{
	while ( tocTree->firstChild() )
		delete tocTree->firstChild();
    
	QDomNodeList chapters = doc.documentElement().elementsByTagName( "chapter" );
	for ( unsigned int chapterCount = 0; chapterCount < chapters.count(); chapterCount++ ) {
		QDomElement chapElem = chapters.item( chapterCount ).toElement();
		QDomElement chapTitleElem = chapElem.elementsByTagName( "title" ).item( 0 ).toElement();
		QString chapTitle = chapTitleElem.text().simplifyWhiteSpace();
		QString chapRef = chapElem.attribute( "id" );

    TOCChapterItem *chapItem;
    if ( tocTree->childCount() == 0 )
      chapItem = new TOCChapterItem( tocTree, chapTitle, chapRef );
    else
      chapItem = new TOCChapterItem( tocTree, tocTree->lastChild(), chapTitle, chapRef );

    QDomNodeList sections = chapElem.elementsByTagName( "sect1" );
    for ( unsigned int sectCount = 0; sectCount < sections.count(); sectCount++ ) {
      QDomElement sectElem = sections.item( sectCount ).toElement();
      QDomElement sectTitleElem = sectElem.elementsByTagName( "title" ).item( 0 ).toElement();
      QString sectTitle = sectTitleElem.text().simplifyWhiteSpace();
			QString sectRef = sectElem.attribute( "id" );

      if ( chapItem->childCount() == 0 )
        new TOCSectionItem( chapItem, sectTitle, sectRef );
      else {
        QListViewItem *lastChild = chapItem->firstChild();
        while ( lastChild->nextSibling() )
          lastChild = lastChild->nextSibling();
        new TOCSectionItem( chapItem, lastChild, sectTitle, sectRef );
      }
    }
	}
}

void khcNavigatorWidget::slotTOCItemSelected( QListViewItem *item )
{
  TOCListViewItem *tocItem;
  if ( ( tocItem = dynamic_cast<TOCSectionItem *>( item ) ) ||
       ( tocItem = dynamic_cast<TOCChapterItem *>( item ) ) )
    emit itemSelected( tocItem->link() );
}

void khcNavigatorWidget::setupGlossaryTab()
{
    glossaryTree = new KListView( mTabWidget );
    glossaryTree->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glossaryTree->addColumn(QString::null);
    glossaryTree->header()->hide();
    glossaryTree->setAllColumnsShowFocus(true);
    glossaryTree->setRootIsDecorated(true);
    connect(glossaryTree, SIGNAL(executed(QListViewItem *)),
        SLOT(slotGlossaryItemSelected(QListViewItem *)));
    connect(glossaryTree, SIGNAL(returnPressed(QListViewItem*)),
	    SLOT(slotGlossaryItemSelected(QListViewItem*)));

    byTopicItem = new QListViewItem(glossaryTree, i18n("By topic"));
    byTopicItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("help"), KIcon::Small));

    alphabItem = new QListViewItem(glossaryTree, i18n("Alphabetically"));
    alphabItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("charset"), KIcon::Small));

    mTabWidget->addTab(glossaryTree, i18n("G&lossary"));

    glossaryHtmlFile = locateLocal("cache", "help/glossary.html");
    glossarySource = langLookup(QString::fromLatin1("khelpcenter/glossary/index.docbook"));

    KConfigGroup config(kapp->config(), "Glossary");

    glossaryHtmlStatus = CacheOk;
    if (config.readEntry("CachedGlossary") != glossarySource)
       glossaryHtmlStatus = NeedRebuild;
    if ((glossaryHtmlStatus == CacheOk) && !QFile::exists(glossaryHtmlFile))
       glossaryHtmlStatus = NeedRebuild;

    if (glossaryHtmlStatus == CacheOk)
    {
       struct stat stat_buf;
       if (stat(QFile::encodeName(glossarySource).data(), &stat_buf) != 0)
       {
          // It makes no sense to continue.
          return;
       }
       int ctime = stat_buf.st_ctime;
       if (config.readNumEntry("CachedGlossaryTimestamp") != ctime)
          glossaryHtmlStatus = NeedRebuild;
    }

    if (glossaryHtmlStatus == NeedRebuild)
    {
       KProcess *meinproc = new KProcess();
       connect(meinproc, SIGNAL(processExited(KProcess *)),
               SLOT(meinprocExited(KProcess *)));

       *meinproc << locate("exe", QString::fromLatin1("meinproc"));
       *meinproc << QString::fromLatin1("--output") << glossaryHtmlFile;
       *meinproc << glossarySource;

       meinproc->start(KProcess::NotifyOnExit, KProcess::Stdout);
    }
}

void khcNavigatorWidget::meinprocExited(KProcess *meinproc)
{
  delete meinproc;

  if (!QFile::exists(glossaryHtmlFile))
     return; // Error...

  struct stat stat_buf;
  if (stat(QFile::encodeName(glossarySource).data(), &stat_buf) != 0)
     return; // Error

  int ctime = stat_buf.st_ctime;

  KConfigGroup config(kapp->config(), "Glossary");
  config.writeEntry("CachedGlossary", glossarySource);
  config.writeEntry("CachedGlossaryTimestamp", ctime);
  kapp->config()->sync();
  glossaryHtmlStatus = CacheOk;
  buildGlossary();
}

void khcNavigatorWidget::buildGlossary()
{
  glossaryHtmlStatus = ListViewOk;
  QFile htmlFile(glossaryHtmlFile);
  if (!htmlFile.open(IO_ReadOnly))
    return;

  QByteArray bytes = htmlFile.readAll();

  QString htmlData = QString::fromLatin1(bytes.data(), bytes.size());

  QDomDocument doc;
  if (!doc.setContent(decodeEntities(htmlData)))
    return;

  QDomNodeList glossDivNodes = doc.documentElement().elementsByTagName(QString::fromLatin1("div"));
  for (unsigned int i = 0; i < glossDivNodes.count(); i++) {
    QDomNode glossDivNode = glossDivNodes.item(i);
    if (glossDivNode.toElement().attribute(QString::fromLatin1("class"), QString::null) != QString::fromLatin1("glossdiv"))
      continue;

    QString glossDiv = glossDivNode.namedItem(QString::fromLatin1("h3")).toElement().text().simplifyWhiteSpace();
    SectionItem *topicSection = new SectionItem(byTopicItem, glossDiv);

    QDomNodeList glossEntryNodes = glossDivNode.toElement().elementsByTagName(QString::fromLatin1("dt"));
    for (unsigned int j = 0; j < glossEntryNodes.count(); j++) {
      QDomNode glossEntryNode = glossEntryNodes.item(j);
      QString term = glossEntryNode.toElement().text().simplifyWhiteSpace();

      (void) new QListViewItem(topicSection, term);

      SectionItem *alphabSection = 0L;
      for (QListViewItemIterator it(alphabItem); it.current(); it++)
        if (it.current()->text(0) == term[0].upper()) {
          alphabSection = static_cast<SectionItem *>(it.current());
          break;
        }

      if (!alphabSection)
        alphabSection = new SectionItem(alphabItem, term[0].upper());

      (void) new QListViewItem(alphabSection, term);

      glossEntryNode = glossEntryNode.nextSibling();

      QString definition;
      QTextStream defStream(&definition, IO_WriteOnly);
      defStream << glossEntryNode.namedItem(QString::fromLatin1("p")).toElement();

      QStringList seeAlso;

      QDomNodeList seeAlsoNodes = glossEntryNode.lastChild().toElement().elementsByTagName(QString::fromLatin1("a"));
      //kdDebug(1400) << "Moo, text, " << glossEntryNode.toElement().text().latin1()  << " count " << seeAlsoNodes.count() << endl;

      if (seeAlsoNodes.count() > 0)
        for (unsigned int k = 0; k < seeAlsoNodes.count(); k++)
          seeAlso += seeAlsoNodes.item(k).toElement().text().simplifyWhiteSpace();

      glossEntries.insert(term, new GlossaryEntry(term, definition, seeAlso));
    }
  }
}

QString khcNavigatorWidget::decodeEntities(const QString &s) const
{
    QString result = s;
    result.replace(QRegExp(QString::fromLatin1("&amp;")), QString::fromLatin1("&"));
    for (int p = result.find(QString::fromLatin1("&")); p >= 0; p = result.find(QString::fromLatin1("&"), p)) {
        int q = result.find(QString::fromLatin1(";"), p++);
        if (q != -1)
            result.replace(p - 1, q - p + 2, KGlobal::charsets()->fromEntity(result.mid(p, q - p)));
    }
    return result;
}

void khcNavigatorWidget::buildTree()
{
  // supporting KDE
  khcNavigatorItem *ti_support = new khcNavigatorItem(contentsTree, i18n("Supporting KDE"),"document2");
  ti_support->setUrl(QString("help:/khelpcenter/index.html?anchor=support"));

  // kde contacts
  khcNavigatorItem *ti_contact = new khcNavigatorItem(contentsTree, i18n("Contact Information"),"document2");
  ti_contact->setUrl(QString("help:/khelpcenter/index.html?anchor=contact"));

  // kde links
  khcNavigatorItem *ti_links = new khcNavigatorItem(contentsTree, i18n("KDE on the web"),"document2");
  ti_links->setUrl(QString("help:/khelpcenter/index.html?anchor=links"));

  // KDE FAQ
  khcNavigatorItem *ti_faq = new khcNavigatorItem(contentsTree, i18n("The KDE FAQ"),"document2");
  ti_faq->setUrl(QString("help:/khelpcenter/faq/index.html"));

  // scan plugin dir for plugins
  insertPlugins();

  insertScrollKeeperItems();

  // info browser
  khcNavigatorItem *ti_info = new khcNavigatorItem(contentsTree, i18n("Browse info pages"),"document2");
  ti_info->setUrl(QString("info:/dir"));

  // fill the info pages subtree
  buildInfoSubTree(ti_info);

  // unix man pages
  khcNavigatorItem *ti_man = new khcNavigatorItem(contentsTree, i18n("Unix manual pages"),"document2");
  ti_man->setUrl(QString("man:/(index)"));

  // fill the man pages subcontentsTree
  buildManSubTree(ti_man);

  // application manuals
  khcNavigatorItem *ti_manual = new khcNavigatorAppItem(contentsTree, i18n("Application manuals"),"contents2", QString::null);
  ti_manual->setUrl("");

  // KDE user's manual
  khcNavigatorItem *ti_um = new khcNavigatorItem(contentsTree, i18n("KDE user's manual"),"document2");
  ti_um->setUrl(QString("help:/khelpcenter/userguide/index.html"));

  // Welcome page
  khcNavigatorItem *ti_welcome = new khcNavigatorItem(contentsTree, i18n("Welcome to KDE"),"document2");
  ti_welcome->setUrl(QString("help:/khelpcenter/index.html?anchor=welcome"));

  contentsTree->setCurrentItem(ti_welcome);
}

void khcNavigatorWidget::clearTree()
{
  // Remove all children.
  for(QListViewItem *child = contentsTree->firstChild(); child; child = contentsTree->firstChild())
  {
     delete child;
  }
}

void khcNavigatorWidget::buildInfoSubTree(khcNavigatorItem *parent)
{
  QString dirContents;
  if (!readInfoDirFile(dirContents))
    return;

  // actual processing...
  QRegExp reSectionHdr("^[A-Za-z0-9]");
  QTextStream stream(&dirContents, IO_ReadOnly);
  QString sLine;

  sLine = stream.readLine();
  while (!sLine.isNull())
  {
    if (sLine == "* Menu: ")
    {
      // will point to the last added section item
      khcNavigatorItem* pLastSection = 0;

      sLine = stream.readLine();
      while (!sLine.isNull())
      {
        if (reSectionHdr.search(sLine, 0) == 0)
        {
          // add the section header
          khcNavigatorItem* pSectionRoot = new khcNavigatorItem(parent, pLastSection, sLine, "contents2");
          pSectionRoot->setUrl("");

          // will point to the last added subitem
          khcNavigatorItem* pLastChild = 0;

          sLine = stream.readLine();
          while (!sLine.isNull())
          {
            if (sLine.startsWith("* "))
            {
              QString sItemTitle, sItemURL;
              if (parseInfoSubjectLine(sLine, sItemTitle, sItemURL))
              {
                // add subject's root node
                khcNavigatorItem *pItem = new khcNavigatorItem(pSectionRoot, pLastChild, sItemTitle, "document2");
                pItem->setUrl(sItemURL);
                pItem->setExpandable(true);
                pLastChild = pItem;
              }
            }
            else if (sLine.isEmpty())
              break; // go to the next section
            sLine = stream.readLine();
          }

          if (pSectionRoot->childCount() > 0)
          {
            pLastSection = pSectionRoot;
          }
          else
            delete pSectionRoot;
        }
        sLine = stream.readLine();
      }
    }
    sLine = stream.readLine();
  }
}

QString khcNavigatorWidget::findInfoDirFile()
{
  for (uint i = 0; i < INFODIRS; i++)
    if (QFile::exists(INFODIR[i] + "dir"))
      return INFODIR[i] + "dir";
  return QString();
}

bool khcNavigatorWidget::readInfoDirFile(QString& sFileContents)
{
  const QString dirPath = findInfoDirFile();
  if (dirPath.isEmpty())
  {
    kdWarning() << "Info directory (dir) file not found." << endl;
    return false;
  }

  QFile file(dirPath);
  if (!file.open(IO_ReadOnly))
  {
    kdWarning() << "Cannot open info directory (dir) file." << endl;
    return false;
  }

  QTextStream stream(&file);
  sFileContents = stream.read();

  file.close();
  return true;
}

bool khcNavigatorWidget::parseInfoSubjectLine(QString sLine, QString& sItemTitle, QString& sItemURL)
{
  regmatch_t* pRegMatch = new regmatch_t[compInfoRegEx.re_nsub + 1];
  Q_CHECK_PTR(pRegMatch);

  int nResult = regexec(&compInfoRegEx, sLine.latin1(),
                        compInfoRegEx.re_nsub + 1, pRegMatch, 0);
  if (nResult)
  {
    kdWarning() << "Could not parse line \'" << sLine << "\' from the info directory (dir) file; regexec() returned " <<
      nResult << "." << endl;
    delete[] pRegMatch;
    return false;
  }

  Q_ASSERT(pRegMatch[0].rm_so == 0 && pRegMatch[0].rm_eo == int(sLine.length()));

  sItemTitle = sLine.mid(pRegMatch[1].rm_so, pRegMatch[1].rm_eo - pRegMatch[1].rm_so);
  sItemURL = "info:/" + sLine.mid(pRegMatch[2].rm_so, pRegMatch[2].rm_eo - pRegMatch[2].rm_so);
  if (pRegMatch[5].rm_eo - pRegMatch[5].rm_so > 0) // it isn't the main node
    sItemURL += "/" + sLine.mid(pRegMatch[5].rm_so, pRegMatch[5].rm_eo - pRegMatch[5].rm_so);

  //   kdDebug() << "title: " << sItemTitle << "; url: " << sItemURL << endl;

  delete[] pRegMatch;
  return true;
}

void khcNavigatorWidget::buildManSubTree(khcNavigatorItem *parent)
{
  // man(n)
  khcNavigatorItem *ti_man_sn = new khcNavigatorItem(parent, i18n("(n) New"),"document2");
  ti_man_sn->setUrl(QString("man:/(n)"));

  // man(9)
  khcNavigatorItem *ti_man_s9 = new khcNavigatorItem(parent, i18n("(9) Kernel"),"document2");
  ti_man_s9->setUrl(QString("man:/(9)"));

  // man(8)
  khcNavigatorItem *ti_man_s8 = new khcNavigatorItem(parent, i18n("(8) Sys. Administration"),"document2");
  ti_man_s8->setUrl(QString("man:/(8)"));

  // man(7)
  khcNavigatorItem *ti_man_s7 = new khcNavigatorItem(parent, i18n("(7) Miscellaneous"),"document2");
  ti_man_s7->setUrl(QString("man:/(7)"));

  // man(6)
  khcNavigatorItem *ti_man_s6 = new khcNavigatorItem(parent, i18n("(6) Games"),"document2");
  ti_man_s6->setUrl(QString("man:/(6)"));

  // man(5)
  khcNavigatorItem *ti_man_s5 = new khcNavigatorItem(parent, i18n("(5) File Formats"),"document2");
  ti_man_s5->setUrl(QString("man:/(5)"));

  // man(4)
  khcNavigatorItem *ti_man_s4 = new khcNavigatorItem(parent, i18n("(4) Devices"),"document2");
  ti_man_s4->setUrl(QString("man:/(4)"));

  // man(3)
  khcNavigatorItem *ti_man_s3 = new khcNavigatorItem(parent, i18n("(3) Subroutines"),"document2");
  ti_man_s3->setUrl(QString("man:/(3)"));

  // man(2)
  khcNavigatorItem *ti_man_s2 = new khcNavigatorItem(parent, i18n("(2) System calls"),"document2");
  ti_man_s2->setUrl(QString("man:/(2)"));

  // man (1)
  khcNavigatorItem *ti_man_s1 = new khcNavigatorItem(parent, i18n("(1) User commands"),"document2");
  ti_man_s1->setUrl(QString("man:/(1)"));
}


class PluginTraverser : public DocEntryTraverser
{
  public:
    PluginTraverser( QListView *listView  ) :
      mListView( listView ), mParentItem( 0 ), mCurrentItem( 0 ) {}
    PluginTraverser( khcNavigatorItem *listViewItem  ) :
      mListView( 0 ), mParentItem( listViewItem ), mCurrentItem( 0 ) {}

    void process( DocEntry *entry )
    {
      if ( mListView ) {
        mCurrentItem = new khcNavigatorItem( mListView, entry->name(),
                                             entry->icon() );
      } else if ( mParentItem ) {
        mCurrentItem = new khcNavigatorItem( mParentItem, entry->name(),
                                             entry->icon() );
      } else {
        kdDebug() << "ERROR! Neither mListView nor mParentItem is set." << endl;
        return;
      }
        
      mCurrentItem->setName( entry->name() );
      mCurrentItem->setUrl( entry->docPath() );
      if ( entry->icon().isEmpty() ) {
        if ( entry->isDirectory() ) {
          mCurrentItem->setIcon( "contents2" );
        } else {
          mCurrentItem->setIcon( "document2" );
        }
      } else {
        mCurrentItem->setIcon( entry->icon() );
      }
    }

    DocEntryTraverser *createChild( DocEntry * )
    {
      if ( mCurrentItem ) return new PluginTraverser( mCurrentItem );
      kdDebug() << "ERROR! mCurrentItem is not set." << endl;
      return 0;
    }

  private:
    QListView *mListView;
    khcNavigatorItem *mParentItem;
    khcNavigatorItem *mCurrentItem;
};


void khcNavigatorWidget::insertPlugins()
{
  PluginTraverser t( contentsTree );
  DocMetaInfo::self()->traverseEntries( &t );

  kdDebug() << "<docmetainfo>" << endl;
  DocEntry::List entries = DocMetaInfo::self()->docEntries();
  DocEntry::List::ConstIterator it;
  for( it = entries.begin(); it != entries.end(); ++it ) {
    (*it)->dump();
  }
  kdDebug() << "</docmetainfo>" << endl;
}

void khcNavigatorWidget::insertScrollKeeperItems()
{
    KProcIO proc;
    proc << "scrollkeeper-get-content-list";
    proc << KGlobal::locale()->language();
    connect(&proc,SIGNAL(readReady(KProcIO *)),SLOT(getScrollKeeperContentsList(KProcIO *)));
    if (!proc.start(KProcess::Block)) {
      kdDebug(1400) << "Could not execute scrollkeeper-get-content-list" << endl;
      return;
    }

    if (!QFile::exists(mScrollKeeperContentsList)) {
      kdDebug(1400) << "Scrollkeeper contents file '" << mScrollKeeperContentsList
                    << "' does not exist." << endl;
      return;
    }

    QDomDocument doc("ScrollKeeperContentsList");
    QFile f(mScrollKeeperContentsList);
    if ( !f.open( IO_ReadOnly ) )
        return;
    if ( !doc.setContent( &f ) ) {
        f.close();
        return;
    }
    f.close();

    // Create top-level item
    khcNavigatorItem *topItem = new khcNavigatorItem(contentsTree, i18n("Scrollkeeper"),"contents2");
    topItem->setUrl("");
    scrollKeeperItems.append(topItem);

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "sect") {
              insertScrollKeeperSection(topItem,e);
            }
        }
        n = n.nextSibling();
    }
}

void khcNavigatorWidget::getScrollKeeperContentsList(KProcIO *proc)
{
    QString filename;
    proc->readln(filename,true);

    mScrollKeeperContentsList = filename;
}

int khcNavigatorWidget::insertScrollKeeperSection(khcNavigatorItem *parentItem,QDomNode sectNode)
{
    khcNavigatorItem *sectItem = new khcNavigatorItem(parentItem,"","contents2");
    sectItem->setUrl("");
    scrollKeeperItems.append(sectItem);

    int numDocs = 0;  // Number of docs created in this section

    QDomNode n = sectNode.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "title") {
                sectItem->setText(0,e.text());
            } else if (e.tagName() == "sect") {
                numDocs += insertScrollKeeperSection(sectItem,e);
            } else if (e.tagName() == "doc") {
                insertScrollKeeperDoc(sectItem,e);
                ++numDocs;
            }
        }
        n = n.nextSibling();
    }

    // Remove empty sections
    if (!mScrollKeeperShowEmptyDirs && numDocs == 0) delete sectItem;

    return numDocs;
}

void khcNavigatorWidget::insertScrollKeeperDoc(khcNavigatorItem *parentItem,QDomNode docNode)
{
    khcNavigatorItem *docItem = new khcNavigatorItem(parentItem,"","document2");
    scrollKeeperItems.append(docItem);

    QString url;

    QDomNode n = docNode.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "doctitle") {
                docItem->setText(0,e.text());
            } else if (e.tagName() == "docsource") {
                url.append(e.text());
            } else if (e.tagName() == "docformat") {
                QString mimeType = e.text();
                if (mimeType == "text/html") {
                    // Let the HTML part figure out how to get the doc
                } else if (mimeType == "text/xml") {
                    // Should probably check for the DTD here
                    url.prepend("help:");
                } else if (mimeType == "text/sgml") {
                    // GNOME docs use this type. We don't have a real viewer for this.
                    url.prepend("file:");
                } else if (mimeType.left(5) == "text/") {
                    url.prepend("file:");
                }
            }
        }
        n = n.nextSibling();
    }

    docItem->setUrl(url);
}

void khcNavigatorWidget::slotURLSelected(QString url)
{
    emit itemSelected(url);
}

void khcNavigatorWidget::slotGlossaryItemSelected(QListViewItem *item)
{
  if (!item)
    return;

  if (dynamic_cast<SectionItem *>(item->parent())) {
    GlossaryEntry *entry = glossEntries[item->text(0)];
    kdDebug(1400) << "Emitting entry " << entry->term << endl;
    emit glossSelected(*entry);
  }

  item->setOpen(!item->isOpen());
}

void khcNavigatorWidget::slotItemSelected(QListViewItem* currentItem)
{
  if (!currentItem)
    return;
  khcNavigatorItem *item = static_cast<khcNavigatorItem*>(currentItem);

  if (item->childCount() > 0 || item->isExpandable())
  {
      if (item->isOpen())
        item->setOpen(false);
      else
        item->setOpen(true);
  }

#if 0
  if (pluginItems.find(item) > -1)
  {
    QString url = item->url();
    kdDebug() << "--ITEM: " << url << endl;
  
    if (!url.isEmpty()) {
      if ( url.left(1) == "/" ) {
        url = "file:" + url;
      } else {
        int colonPos = url.find(':');
        int slashPos = url.find('/');
        if ( colonPos < 0 || ( colonPos > url.find('/') && slashPos > 0 ) ) {
          url = "file:" + langLookup(url);
        }
      }
      
      kdDebug() << "--URL: " << url << endl;
      
      emit itemSelected(url);
    }
    return;
  }
#endif

  if (!item->url().isEmpty()) {
    KURL u = item->url();
    if ( u.protocol() == "help" ) {
      tocTree->setApplication( u.directory() );
      QString doc = langLookup( u.path() );
      // Enforce the original .docbook version, in case langLookup returns a
      // cached version
      doc.replace( doc.find( ".html" ), 5, ".docbook" );
      kdDebug( 1400 ) << "slotURLSelected(): doc = " << doc << endl;

      QFile f( doc );
      if ( f.open( IO_ReadOnly ) ) {
        QDomDocument domDoc;
        if ( domDoc.setContent( &f ) ) {
          fillTOCTree( domDoc );
          mTabWidget->setCurrentPage( mTabWidget->indexOf( tocTree ) );
        }
        f.close();
      }
    }
    emit itemSelected(item->url());
  }
}

void khcNavigatorWidget::slotItemExpanded(QListViewItem* index)
{
  if (!index)
    return;

  QListViewItem* parent;
  if ((parent = index->parent())) // it _is_ an assignment, not a comparison !
    if ((parent = parent->parent())) // it _is_ an assignment, not a comparison !
      // item is at least on the third level of the tree
      if (parent->text(0) == i18n("Browse info pages") && index->childCount() == 0)
        // it is an unexpanded info subject's root node. Let's check if we are have already started to create the hierarchy.
      {
        khcNavigatorItem* item = static_cast<khcNavigatorItem*>(index);
          // const QString itemName(item->name());
        if (hierarchyMakers.find(item) == hierarchyMakers.end())
          // no. We must create one.
        {
          khcInfoHierarchyMaker* pMaker = new khcInfoHierarchyMaker;
          Q_CHECK_PTR(pMaker);
          hierarchyMakers[item] = pMaker;

          QString sURL = item->url();
          Q_ASSERT(!sURL.isEmpty());

          regex_t reInfoURL;
          int nResult = regcomp(&reInfoURL, "^info:/([^/]*)(/(.*))?$", REG_EXTENDED);
          Q_ASSERT(!nResult);
          Q_ASSERT(reInfoURL.re_nsub == 3);

          regmatch_t regMatch[4];

          nResult = regexec(&reInfoURL, sURL.latin1(), reInfoURL.re_nsub + 1, regMatch, 0);
          if (nResult)
          {
            kdWarning() << "Could not parse URL \'" << sURL << "\'; regexec() returned " << nResult << "." << endl;
            hierarchyMakers.erase(item);
            item->setExpandable(false);
            return;
          }

          Q_ASSERT(regMatch[0].rm_so == 0 && regMatch[0].rm_eo == int(sURL.length()));

          QString sTopic = sURL.mid(regMatch[1].rm_so, regMatch[1].rm_eo - regMatch[1].rm_so);
          QString sNode = sURL.mid(regMatch[3].rm_so, regMatch[3].rm_eo - regMatch[3].rm_so);

          kdDebug() << "sTopic: \'" << sTopic << "\'; sNode: \'" << sNode << "\'" << endl;

          // begin creating hierarchy!

          /* Cog-wheel animation handling -- enable after creating the icons
             startAnimation(item);
          */
          connect(pMaker, SIGNAL(hierarchyCreated(uint, uint, const khcInfoNode*)),
                  SLOT(slotInfoHierarchyCreated(uint, uint, const khcInfoNode*)));
          pMaker->createHierarchy((uint) item, sTopic, sNode);

          regfree(&reInfoURL);
        }
        else
          kdDebug() << "Hierarchy creation already in progress..." << endl;
      }
}

void khcNavigatorWidget::slotInfoHierarchyCreated(uint key, uint nErrorCode, const khcInfoNode* pRootNode)
{
  Q_ASSERT(key);
  khcNavigatorItem* pItem = (khcNavigatorItem*) key;

  kdDebug() << "Info hierarchy for subject \'" << pItem->name() << "\'created! Result: " << nErrorCode << endl;

  if (!nErrorCode)
  {
    if (pRootNode->m_lChildren.empty())
      // "Hierarchy" consists of only one element (pRootNode has no children)
    {
      pItem->setExpandable(false);
      pItem->repaint();
    }
    else
      addChildren(pRootNode, pItem);
    /* Cog-wheel animation handling -- enable after creating the icons
    stopAnimation(pItem);
    */
  }
  else
  {
    /* Cog-wheel animation handling -- enable after creating the icons
    stopAnimation(pItem);
    */

    QString sErrMsg;
    switch (nErrorCode)
    {
    case ERR_FILE_UNAVAILABLE:
      sErrMsg = i18n("One or more files containing info nodes belonging to the subject '%1' do(es) not exist.").arg(pItem->name());
      break;
    case ERR_NO_HIERARCHY:
      sErrMsg = i18n("Info nodes belonging to the subject '%1' seem to be not ordered in a hierarchy.").arg(pItem->name());
      break;
    default:
      sErrMsg = i18n("An unknown error occurred while creating the hierarchy of info nodes belonging to the subject '%1'.").arg(pItem->name());
    }
    KMessageBox::sorry(0, sErrMsg, i18n("Cannot Create Hierarchy of Info Nodes"));
    pItem->setExpandable(false);
    pItem->repaint();
  }
}

void khcNavigatorWidget::addChildren(const khcInfoNode* pParentNode, khcNavigatorItem* pParentTreeItem)
{
  khcNavigatorItem* pLastChild = 0;
  for (std::list<khcInfoNode*>::const_iterator it = pParentNode->m_lChildren.begin();
       it != pParentNode->m_lChildren.end(); ++it)
  {
    //    khcNavigatorItem *pItem = new khcNavigatorItem(pParentTreeItem, pLastChild, (*it)->m_sTitle, "document2");
    khcNavigatorItem *pItem = new khcNavigatorItem(pParentTreeItem, pLastChild,
                                                   (*it)->m_sTitle.isEmpty() ? (*it)->m_sName : (*it)->m_sTitle, "document2");
    pItem->setUrl("info:/" + (*it)->m_sTopic + "/" + (*it)->m_sName);
    pLastChild = pItem;

    addChildren(*it, pItem);
  }
}

void khcNavigatorWidget::slotCleanHierarchyMakers()
{
//  kdDebug() << "--- slotCleanHierarchyMakers ---" << endl;
  for (std::map<khcNavigatorItem*, khcInfoHierarchyMaker*>::iterator it = hierarchyMakers.begin();
       it != hierarchyMakers.end(); )
  {
    std::map<khcNavigatorItem*, khcInfoHierarchyMaker*>::iterator copyIt(it);
    it++;
    if (!copyIt->second->isWorking())
    {
      kdDebug() << "Deleting a not-working hierarchy maker..." << endl;
      delete copyIt->second;
    }
    hierarchyMakers.erase(copyIt);
  }
}

/* Cog-wheel animation handling -- enable after creating the icons
void khcNavigatorWidget::slotAnimation()
{
    MapCurrentOpeningSubjects::Iterator it = m_mapCurrentOpeningSubjects.begin();
    MapCurrentOpeningSubjects::Iterator end = m_mapCurrentOpeningSubjects.end();
    for (; it != end; ++it )
    {
        uint & iconNumber = it.data().iconNumber;
        QString icon = QString::fromLatin1( it.data().iconBaseName ).append( QString::number( iconNumber ) );
        it.key()->setPixmap( 0, SmallIcon( icon, KHCFactory::instance() ) );

        iconNumber++;
        if ( iconNumber > it.data().iconCount )
            iconNumber = 1;
    }
}

void khcNavigatorWidget::startAnimation( khcNavigatorItem * item, const char * iconBaseName, uint iconCount )
{
    m_mapCurrentOpeningSubjects.insert( item, AnimationInfo( iconBaseName, iconCount, *item->pixmap(0) ) );
    if ( !m_animationTimer->isActive() )
        m_animationTimer->start( 50 );
}

void khcNavigatorWidget::stopAnimation( khcNavigatorItem * item )
{
    MapCurrentOpeningSubjects::Iterator it = m_mapCurrentOpeningSubjects.find(item);
    if ( it != m_mapCurrentOpeningSubjects.end() )
    {
        item->setPixmap( 0, it.data().originalPixmap );
        m_mapCurrentOpeningSubjects.remove( item );
    }
    if (m_mapCurrentOpeningSubjects.isEmpty())
        m_animationTimer->stop();
}
*/


void khcNavigatorWidget::slotSearch()
{
  QString words = mSearchEdit->text();
  QString method = mSearchWidget->method();
  int pages = mSearchWidget->pages();
  QString scope = mSearchWidget->scope();

  if ( words.isEmpty() || scope.isEmpty() ) return;

  // disable search Button during searches
  mSearchButton->setEnabled(false);
  QApplication::setOverrideCursor(waitCursor);

  if ( !mSearchEngine->search( words, method, pages, scope ) ) {
    slotSearchFinished();
  }
}

void khcNavigatorWidget::slotShowSearchResult( const QString &url )
{
  QString u = url;
  u.replace( QRegExp( "%k" ), mSearchEdit->text() );

  slotURLSelected( u );
}

void khcNavigatorWidget::slotSearchFinished()
{
  mSearchButton->setEnabled(true);
  QApplication::restoreOverrideCursor();

  kdDebug() << "Search finished." << endl;
}

void khcNavigatorWidget::slotSearchTextChanged( const QString &text )
{
//  kdDebug() << "khcNavigatorWidget::slotSearchTextCanged() '" << text << "'"
//            << endl;

  mSearchButton->setEnabled( !text.isEmpty() );
}

void khcNavigatorWidget::hideSearch()
{
  mSearchFrame->hide();
  mTabWidget->removePage( mSearchWidget );
}

// vim:et:ts=2:sw=2
