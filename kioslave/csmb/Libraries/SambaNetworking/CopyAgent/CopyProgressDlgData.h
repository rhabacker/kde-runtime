/* Name:  CopyProgressDlgData.h

   Description: This file is a part of the CopyAgent.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.


*/

/**********************************************************************

	--- Qt Architect generated file ---

	File: CopyProgressDlgData.h
	Last generated: Mon Nov 29 00:57:03 1999

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef CCopyProgressDlgData_included
#define CCopyProgressDlgData_included

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprogressbar.h>

class CCopyProgressDlgData : public QDialog
{
    Q_OBJECT

public:

    CCopyProgressDlgData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~CCopyProgressDlgData();

public slots:


protected slots:


protected:
    QLabel* m_pCopyMovieLabel;
    QLabel* m_pTextLabel1;
    QLabel* m_pTextLabel2;
    QProgressBar* m_ByteProgress;
    QPushButton* m_pCancelButton;
    QProgressBar* m_FileProgress;
    QLabel* m_FilesLabel;
    QLabel* m_BytesLabel;

};

#endif // CCopyProgressDlgData_included
