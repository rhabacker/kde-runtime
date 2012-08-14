/*
 * ktar.cpp
 *
 * Copyright (C) 2012 basysKom GmbH <info@basyskom.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "ktar.h"
#include "temporarypath.h"

#include <QDir>

#include <KDebug>

using namespace Kerfuffle;

KTar::KTar(const QString & fileName)
    : KCliArchive(fileName),
      m_state(ReadStateHeader)
{
}

KTar::KTar(QIODevice * dev)
    : KCliArchive(dev),
      m_state(ReadStateHeader)
{
    // TODO: check if it is possible to implement QIODevice support.
    kDebug(7109) << "QIODevice is not supported";
}

KTar::~KTar()
{
    closeArchive();
    kDebug(7109);
}

ParameterList KTar::parameterList() const
{
    static ParameterList p;

    if (p.isEmpty()) {
        //p[CaptureProgress] = true;
        p[ListProgram] = p[ExtractProgram] = p[DeleteProgram] = p[AddProgram] = QLatin1String( "7z" );

        p[ListArgs] = QStringList() << QLatin1String( "l" ) << QLatin1String( "-slt" ) << QLatin1String( "$Archive" );
        p[ExtractArgs] = QStringList() << QLatin1String( "$PreservePathSwitch" ) << QLatin1String( "$PasswordSwitch" ) << QLatin1String( "$Archive" ) << QLatin1String( "$Files" );
        p[PreservePathSwitch] = QStringList() << QLatin1String( "x" ) << QLatin1String( "e" );
        p[PasswordSwitch] = QStringList() << QLatin1String( "-p$Password" );
        p[FileExistsExpression] = QLatin1String( "already exists. Overwrite with" );
        p[WrongPasswordPatterns] = QStringList() << QLatin1String( "Wrong password" );
        p[AddArgs] = QStringList() << QLatin1String( "a" ) << QLatin1String( "$Archive" ) << QLatin1String( "$Files" );
        p[DeleteArgs] = QStringList() << QLatin1String( "d" ) << QLatin1String( "$Archive" ) << QLatin1String( "$Files" );

        p[FileExistsInput] = QStringList()
                             << QLatin1String( "Y" ) //overwrite
                             << QLatin1String( "N" ) //skip
                             << QLatin1String( "A" ) //overwrite all
                             << QLatin1String( "S" ) //autoskip
                             << QLatin1String( "Q" ) //cancel
                             ;

        p[PasswordPromptPattern] = QLatin1String("Enter password \\(will not be echoed\\) :");
    }

    return p;
}

bool KTar::readListLine(const QString& line)
{
    static const QLatin1String archiveInfoDelimiter1("--"); // 7z 9.13+
    static const QLatin1String archiveInfoDelimiter2("----"); // 7z 9.04
    static const QLatin1String entryInfoDelimiter("----------");

    switch (m_state) {
    case ReadStateHeader:
        if (line.startsWith(QLatin1String("Listing archive:"))) {
            kDebug(7109) << "Archive name: "
                     << line.right(line.size() - 16).trimmed();
        } else if ((line == archiveInfoDelimiter1) ||
                   (line == archiveInfoDelimiter2)) {
            m_state = ReadStateArchiveInformation;
        } else if (line.contains(QLatin1String( "Error:" ))) {
            kDebug(7109) << line.mid(6);
        }
        break;

    case ReadStateArchiveInformation:
        if (line == entryInfoDelimiter) {
            m_state = ReadStateEntryInformation;
        } else if (line.startsWith(QLatin1String("Type ="))) {
            const QString type = line.mid(7).trimmed().toLower();
            kDebug(7109) << "Archive type: " << type;

            if (type == QLatin1String("7z")) {
                m_archiveType = ArchiveType7z;
            } else if (type == QLatin1String("bzip2")) {
                m_archiveType = ArchiveTypeBZip2;
            } else if (type == QLatin1String("gzip")) {
                m_archiveType = ArchiveTypeGZip;
            } else if (type == QLatin1String("tar")) {
                m_archiveType = ArchiveTypeTar;
            } else if (type == QLatin1String("zip")) {
                m_archiveType = ArchiveTypeZip;
            } else {
                // Should not happen
                kWarning() << "Unsupported archive type: " << type;
                return false;
            }
        }

        break;

    case ReadStateEntryInformation:
        //kDebug(7109) << "Line: " << line;
        if (line.startsWith(QLatin1String("Path ="))) {
            const QString entryFilename =
                QDir::fromNativeSeparators(line.mid(6).trimmed());
            m_currentArchiveEntry.clear();
            m_currentArchiveEntry[FileName] = entryFilename;
            m_currentArchiveEntry[InternalID] = entryFilename;
        } else if (line.startsWith(QLatin1String("Size = "))) {
            m_currentArchiveEntry[ Size ] = line.mid(7).trimmed();
        } else if (line.startsWith(QLatin1String("Packed Size = "))) {
            // #236696: 7z files only show a single Packed Size value
            //          corresponding to the whole archive.
            if (m_archiveType != ArchiveType7z) {
                m_currentArchiveEntry[CompressedSize] = line.mid(14).trimmed();
            }
        } else if (line.startsWith(QLatin1String("Modified = "))) {
            m_currentArchiveEntry[ Timestamp ] =
                QDateTime::fromString(line.mid(11).trimmed(),
                                      QLatin1String( "yyyy-MM-dd hh:mm:ss" ));
        } else if (line.startsWith(QLatin1String("Attributes = "))) { // 7z and zip
            const QString attributes = line.mid(13).trimmed();

            const bool isDirectory = attributes.startsWith(QLatin1Char( 'D' ));
            m_currentArchiveEntry[ IsDirectory ] = isDirectory;
            if (isDirectory) {
                const QString directoryName =
                    m_currentArchiveEntry[FileName].toString();
                if (!directoryName.endsWith(QLatin1Char( '/' ))) {
                    const bool isPasswordProtected = (line.at(12) == QLatin1Char( '+' ));
                    m_currentArchiveEntry[FileName] =
                        m_currentArchiveEntry[InternalID] = QString(directoryName + QLatin1Char( '/' ));
                    m_currentArchiveEntry[ IsPasswordProtected ] =
                        isPasswordProtected;
                }
            }

            m_currentArchiveEntry[ Permissions ] = attributes.mid(1);
        } else if (line.startsWith(QLatin1String("Folder = "))) { // tar
            const bool isDirectory = line[9] == QLatin1Char('+');
            m_currentArchiveEntry[ IsDirectory ] = isDirectory;
            if (isDirectory) {
                const QString directoryName =
                    m_currentArchiveEntry[FileName].toString();
                if (!directoryName.endsWith(QLatin1Char( '/' ))) {
                    m_currentArchiveEntry[FileName] =
                        m_currentArchiveEntry[InternalID] = QString(directoryName + QLatin1Char( '/' ));
                    m_currentArchiveEntry[ IsPasswordProtected ] =
                        false;
                }
            }
        } else if (line.startsWith(QLatin1String("IsLink = "))) { // 7z
            // we need to patch 7z to print the IsLink attribute line.
            // the patch only exposes the IsLink attribute, not the symlink's target.
            m_currentArchiveEntry[ Link ] = true;
        } else if (line.startsWith(QLatin1String("Mode = "))) { // tar
            m_currentArchiveEntry[ Permissions ] = line.mid(7);
        } else if (line.startsWith(QLatin1String("CRC = "))) {
            m_currentArchiveEntry[ CRC ] = line.mid(6).trimmed();
        } else if (line.startsWith(QLatin1String("Method = "))) {
            m_currentArchiveEntry[ Method ] = line.mid(9).trimmed();
        } else if (line.startsWith(QLatin1String("Encrypted = ")) &&
                   line.size() >= 13) {
            m_currentArchiveEntry[ IsPasswordProtected ] = (line.at(12) == QLatin1Char( '+' ));
        } else if (line.startsWith(QLatin1String("Block = ")) /*    7z */ ||
                   line.startsWith(QLatin1String("Link = "))     /*   tar */ ||
                   line.startsWith(QLatin1String("Version = "))  /*   zip */ ) {

            if (line.startsWith(QLatin1String("Link = "))) { // tar
                QString symLinkTarget = line.mid(7);
                if (!symLinkTarget.isEmpty()) {
                    m_currentArchiveEntry[ Link ] = true;
                    m_currentArchiveEntry[ LinkTarget ] = symLinkTarget;
                }
            }
            if (m_currentArchiveEntry.contains(FileName)) {
                if (m_currentArchiveEntry[ Link ].toBool()) {
                    // the patch for 7z does not exposes the symlink's target,
                    // so we will have to extract the link in KCliArchive::addEntry
                    // and use readlink to read the target. That cannot be done while
                    // CliInterface::list is running, so we will save the list here
                    // and process it when CliInterface::list finishes.
                    symLinksToAdd.append(m_currentArchiveEntry);
                } else {
                    addEntry(m_currentArchiveEntry);
                }
            }
        }
        break;
    }

    return true;
}

bool KTar::openArchive(QIODevice::OpenMode mode)
{
    if (m_originalFilename.isEmpty() && filename().section(QLatin1Char('.'), -2, -2) == QLatin1String("tar")) {
        createTmpDir();
        copyFiles(QList<QVariant>(), tmpDir, CompressionOptions());
        m_originalFilename = filename();
        QDir dir(tmpDir);
        foreach (const QString & f, dir.entryList()) {
            if (f.endsWith(QLatin1String(".tar"))) {
                m_tmpTarFilename = tmpDir + f;
                setFilename(m_tmpTarFilename);
                break;
            }
        }

        kDebug(1601) << "Temporary tar file:" << m_tmpTarFilename;
    } else {
        m_originalFilename.clear();
    }

    if (mode == QIODevice::WriteOnly) {
        // in case the archive is opened for writing before it has been opened for reading,
        // like in the unit tests.
        CliInterface::cacheParameterList();
        return true;
    }

    // CliInterface is not ready to allow several KProcess running in parallel.
    // We cannot run CliInterface::list() in parallel with other methods of CliInterface,
    // like CliInterface::copyFiles(), which is used to add symbolic links in KCliArchive::addEntry().
    // We will save the symbolic link list and add them after CliInterface::list() finishes.
    symLinksToAdd.clear();
    bool ret = list();
    if (ret) {
        foreach(const Kerfuffle::ArchiveEntry & entry, symLinksToAdd) {
            addEntry(entry);
        }
    }
    symLinksToAdd.clear();
    return ret;
}

bool KTar::closeArchive()
{
    const bool ret = KCliArchive::closeArchive();

    if (!m_originalFilename.isEmpty()) {
        if (mode() & QIODevice::WriteOnly) {
            QString newArchiveFilename = m_tmpTarFilename + '.' + m_originalFilename.section(QLatin1Char('.'), -1, -1);
            kDebug(7109) << "Will create new archive" << newArchiveFilename; 
            setFilename(newArchiveFilename);

            // FIXME: 7z refuses to add file if one directory appears twice in the archive. I think
            // libarchive plugin in Ark is causing the duplicate directory problem. The tar command
            // does not complaint about that.
            if (addFiles(QStringList() << m_tmpTarFilename, CompressionOptions())) {
                if (QFile::remove(m_originalFilename)) {
                    Q_ASSERT(!QFile::exists(m_originalFilename));

                    if (QFile::copy(newArchiveFilename, m_originalFilename)) {
                        Q_ASSERT(QFile::exists(m_originalFilename));
                        TemporaryPath tmpPath(newArchiveFilename);
                    } else {
                        kDebug(7109) << "Error saving new archive to" << m_originalFilename; 
                    }
                } else {
                    kDebug(7109) << "Error removing original archive" << m_originalFilename; 
                }
            } else {
                kDebug(7109) << "Error adding" << m_tmpTarFilename;
            }
        }

        TemporaryPath tmpPath(m_tmpTarFilename);
        setFilename(m_originalFilename);
        m_originalFilename.clear();
    }

    return ret;
}