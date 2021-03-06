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

#ifndef QMLJSDELTA_H
#define QMLJSDELTA_H

#include <qmljs/qmljsdocument.h>
#include <qmljs/qmljs_global.h>

namespace QmlJS {

class QMLJS_EXPORT Delta
{
public:
    typedef int DebugId;
    typedef QHash<AST::UiObjectMember*, QList<DebugId> > DebugIdMap;
    DebugIdMap operator()(const QmlJS::Document::Ptr &doc1, const QmlJS::Document::Ptr &doc2, const DebugIdMap &debugIds);

    QSet<AST::UiObjectMember *> newObjects;

    QmlJS::Document::Ptr document() const;
    QmlJS::Document::Ptr previousDocument() const;

private:
    void insert(AST::UiObjectMember *member, AST::UiObjectMember *parentMember,
                const QList<DebugId> &debugReferences, const Document::Ptr &doc);
    void update(AST::UiObjectMember* oldObject, const QmlJS::Document::Ptr& oldDoc,
                AST::UiObjectMember* newObject, const QmlJS::Document::Ptr& newDoc,
                const QList<DebugId>& debugReferences);
    void remove(const QList<DebugId> &debugReferences);
    void reparent(const QList <DebugId> &member, const QList<DebugId> &newParent);

protected:
    virtual void updateScriptBinding(DebugId objectReference,
                             AST::UiObjectMember *parentObject,
                             AST::UiScriptBinding *scriptBinding,
                             const QString &propertyName,
                             const QString &scriptCode);
    virtual void updateMethodBody(DebugId objectReference,
                            AST::UiObjectMember *parentObject,
                            AST::UiScriptBinding *scriptBinding,
                            const QString &methodName,
                            const QString &methodBody);
    virtual void resetBindingForObject(int debugId, const QString &propertyName);
    virtual void removeObject(int debugId);
    virtual void reparentObject(int debugId, int newParent);
    virtual void createObject(const QString &qmlText, DebugId ref,
                              const QStringList &importList, const QString &filename, int order = 0);
    virtual void notifyUnsyncronizableElementChange(AST::UiObjectMember *parent);

private:
    QmlJS::Document::Ptr m_currentDoc;
    QmlJS::Document::Ptr m_previousDoc;
};

} // namespace QmlJS

#endif // QMLJSDELTA_H

