/*
 * disklist.h
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __DISKLIST_H__
#define __DISKLIST_H__

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <qregexp.h>

#include "disks.h"

/***************************************************************************/
typedef QPtrList<DiskEntry>		DisksBase;
typedef QPtrListIterator<DiskEntry>	DisksIterator;

/***************************************************************************/
class Disks : public DisksBase
{
 public:
  Disks(bool deepCopies=TRUE) { dc = deepCopies;};
  ~Disks() { clear(); };
private:
  int compareItems( DiskEntry s1, DiskEntry s2 )
  {
    int ret = s1.deviceName().compare(s2.deviceName());
    if( ret == 0 )
    {
      ret = s1.mountPoint().compare(s2.mountPoint());
    }

    kdDebug() << "compareDISKS " << s1.deviceName() << " vs " << s2.deviceName() << " (" << ret << ")" << endl;
    return( ret );
  }
  bool  dc;
};

/***************************************************************************/
class DiskList : public QObject
{  Q_OBJECT
public:
   DiskList( QObject *parent=0, const char *name=0 );
 ~DiskList();
   void readFSTAB();
   void readMNTTAB();
   int find(const DiskEntry* disk) {return disks->find(disk);};
   DiskEntry*  at(uint index) {return disks->at(index);};
   DiskEntry* first() {return disks->first();};
   DiskEntry* next() {return disks->next();};
   uint count() { return disks->count(); };

public slots:
  void loadExclusionLists();

private:
  void replaceDeviceEntry(DiskEntry *disk);
  void replaceDeviceEntryMounted(DiskEntry *disk);
  bool ignoreDisk(DiskEntry *disk);

  Disks            *disks;
  QPtrList<QRegExp> mountPointExclusionList;
};
/***************************************************************************/


#endif
