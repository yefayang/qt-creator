/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef QMLJSIMPORTCACHE_H
#define QMLJSIMPORTCACHE_H

#include "qmljsconstants.h"
#include "qmljsdialect.h"

#include <languageutils/componentversion.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QSet>
#include <QSharedPointer>

#include <functional>

QT_BEGIN_NAMESPACE
class QCryptographicHash;
QT_END_NAMESPACE

namespace QmlJS {
class ImportInfo;
class ViewerContext;
namespace Internal { class ImportDependenciesPrivate; }
class ImportDependencies;

// match strenght wrt to the selectors of a ViewerContext
// this is valid only within a ViewerContext
class QMLJS_EXPORT ImportMatchStrength
{
public:
    explicit ImportMatchStrength() {}
    ImportMatchStrength(QList<int> match);

    int compareMatch(const ImportMatchStrength &o) const;

    bool hasNoMatch();

    bool hasMatch();
private:
    friend bool operator ==(const ImportMatchStrength &m1, const ImportMatchStrength &m2);
    QList<int> m_match;
};
bool operator ==(const ImportMatchStrength &m1, const ImportMatchStrength &m2);
bool operator !=(const ImportMatchStrength &m1, const ImportMatchStrength &m2);
bool operator <(const ImportMatchStrength &m1, const ImportMatchStrength &m2);

/*!
 * \brief The ImportKey class represent an import (or export), and can be used as hash key
 *
 * This represent only what is to be imported, *not* how (i.e. no as clause)
 *
 * Order is defined so that files in the same directory are contiguous, and different
 * ImportKind are separated.
 * This is used to efficiently iterate just on library imports, or just on a directory
 * while preserving space.
 */
class QMLJS_EXPORT ImportKey
{
public:
    enum DirCompareInfo {
        SameDir,
        FirstInSecond,
        SecondInFirst,
        Different,
        Incompatible
    };

    explicit ImportKey();
    explicit ImportKey(const ImportInfo &info);
    ImportKey(ImportType::Enum type, const QString &path,
              int majorVersion = LanguageUtils::ComponentVersion::NoVersion,
              int minorVersion = LanguageUtils::ComponentVersion::NoVersion);

    ImportType::Enum type;
    QStringList splitPath;
    int majorVersion;
    int minorVersion;

    QString path() const;
    QString libraryQualifiedPath() const;

    void addToHash(QCryptographicHash &hash) const;
    ImportKey flatKey() const;

    // wrap QList in a special type?
    ImportMatchStrength matchImport(const ImportKey &o, const ViewerContext &vContext) const;
    int compare(const ImportKey &other) const;
    bool isDirectoryLike() const;
    DirCompareInfo compareDir(const ImportKey &other) const;
    QString toString() const;
};

uint qHash(const ImportKey &info);
bool operator ==(const ImportKey &i1, const ImportKey &i2);
bool operator !=(const ImportKey &i1, const ImportKey &i2);
bool operator <(const ImportKey &i1, const ImportKey &i2);

class QMLJS_EXPORT Export
{
public:
    static const QString LibraryTypeName;
    Export();
    Export(ImportKey exportName, QString pathRequired, bool intrinsic = false,
           const QString &typeName = LibraryTypeName);
    ImportKey exportName;
    QString pathRequired;
    QString typeName;
    bool intrinsic;
    bool visibleInVContext(const ViewerContext &vContext) const;
};
bool operator ==(const Export &i1, const Export &i2);
bool operator !=(const Export &i1, const Export &i2);

class QMLJS_EXPORT CoreImport
{
public:
    CoreImport();
    CoreImport(const QString &importId, const QList<Export> &possibleExports = QList<Export>(),
               Dialect language = Dialect::Qml, const QByteArray &fingerprint = QByteArray());
    QString importId;
    QList<Export> possibleExports;
    Dialect language;
    QByteArray fingerprint;
    bool valid();
};

class QMLJS_EXPORT DependencyInfo
{
public:
    typedef QSharedPointer<const DependencyInfo> ConstPtr;
    typedef QSharedPointer<DependencyInfo> Ptr;

    QByteArray calculateFingerprint(const ImportDependencies &deps);

    ImportKey rootImport;
    QSet<QString> allCoreImports;
    QSet<ImportKey> allImports;
    QByteArray fingerprint;
};

class QMLJS_EXPORT MatchedImport
{
public:
    MatchedImport();
    MatchedImport(ImportMatchStrength matchStrength, ImportKey importKey,
                  const QString &coreImportId);

    ImportMatchStrength matchStrength;
    ImportKey importKey;
    QString coreImportId;
    int compare(const MatchedImport &o) const;
};
bool operator ==(const MatchedImport &m1, const MatchedImport &m2);
bool operator !=(const MatchedImport &m1, const MatchedImport &m2);
bool operator <(const MatchedImport &m1, const MatchedImport &m2);

class QMLJS_EXPORT ImportDependencies
{
public:
    typedef QMap<ImportKey, QList<MatchedImport> > ImportElements;

    explicit ImportDependencies();
    ~ImportDependencies();

    void filter(const ViewerContext &vContext);

    CoreImport coreImport(const QString &importId) const;
    void iterateOnCandidateImports(const ImportKey &key, const ViewerContext &vContext,
                                   std::function<bool(const ImportMatchStrength &,
                                                        const Export &,
                                                        const CoreImport &)> const &iterF) const;
    ImportElements candidateImports(const ImportKey &key, const ViewerContext &vContext) const;

    QList<DependencyInfo::ConstPtr> createDependencyInfos(const ImportKey &mainDoc,
                                                          const ViewerContext &vContext) const;

    void addCoreImport(const CoreImport &import);
    void removeCoreImport(const QString &importId);

    void addExport(const QString &importId, const ImportKey &importKey,
                     const QString &requiredPath, const QString &typeName = Export::LibraryTypeName);
    void removeExport(const QString &importId, const ImportKey &importKey,
                      const QString &requiredPath, const QString &typeName = Export::LibraryTypeName);

    void iterateOnCoreImports(const ViewerContext &vContext,
                              std::function<bool(const CoreImport &)> const &iterF) const;
    void iterateOnLibraryImports(const ViewerContext &vContext,
                                 std::function<bool(const ImportMatchStrength &,
                                                      const Export &,
                                                      const CoreImport &)> const &iterF) const;
    void iterateOnSubImports(const ImportKey &baseKey, const ViewerContext &vContext,
                             std::function<bool(const ImportMatchStrength &,
                                                  const Export &,
                                                  const CoreImport &)> const &iterF) const;

    QSet<ImportKey> libraryImports(const ViewerContext &viewContext) const;
    QSet<ImportKey> subdirImports(const ImportKey &baseKey, const ViewerContext &viewContext) const;
    void checkConsistency() const;
private:
    void removeImportCacheEntry(const ImportKey &importKey, const QString &importId);

    QMap<ImportKey, QStringList> m_importCache;
    QMap<QString, CoreImport> m_coreImports;
};

} // namespace QmlJS

#endif // QMLJSIMPORTCACHE_H
