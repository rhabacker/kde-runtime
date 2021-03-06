/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Vishesh Handa <me@vhanda.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DETAILSWIDGET_H
#define DETAILSWIDGET_H

#include <kdialog.h>
#include <QLabel>

#include <Nepomuk2/ResourceManager>

namespace Soprano {
    namespace Util {
        class AsyncQuery;
    }
}

namespace Nepomuk2 {

class DetailsWidget : public KDialog
{
    Q_OBJECT
public:
    explicit DetailsWidget(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~DetailsWidget();

private slots:
    void refresh();
    void slotFileCountFinished(Soprano::Util::AsyncQuery* query);
    void slotEmailCountFinished(Soprano::Util::AsyncQuery* query);

private:
    QLabel* m_fileCountLabel;
    QLabel* m_emailCountLabel;

};

}
#endif // DETAILSWIDGET_H
