/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: http://www.qt-project.org/
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**************************************************************************/

#include "settings.h"
#include "operation.h"

#include <app/app_version.h>

#include <iostream>

#include <QCoreApplication>
#include <QDir>

Settings *Settings::m_instance = 0;

Settings *Settings::instance()
{
    return m_instance;
}

Settings::Settings() :
    operation(0)
{
    Q_ASSERT(!m_instance);
    m_instance = this;

    // autodetect sdk dir:
    sdkPath = Utils::FileName::fromString(QCoreApplication::applicationDirPath());
    sdkPath.appendPath(QLatin1String(DATA_PATH));
    sdkPath = Utils::FileName::fromString(QDir::cleanPath(sdkPath.toString()));
    sdkPath.appendPath(QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR)
                       + QLatin1String("/qtcreator"));
}

Utils::FileName Settings::getPath(const QString &file)
{
    Utils::FileName result = sdkPath;
    if (file == QLatin1String("profiles") || file == QLatin1String("kits"))
        result.appendPath(QLatin1String("profiles"));
    else if (file == QLatin1String("qtversions") || file == QLatin1String("qtversion"))
        result.appendPath(QLatin1String("qtversion"));
    else if (file == QLatin1String("toolchains") || file == QLatin1String("toolChains"))
        result.appendPath(QLatin1String("toolchains"));
    else if (file == QLatin1String("toolchains") || file == QLatin1String("toolChains"))
        result.appendPath(QLatin1String("toolchains"));
    else if (file == QLatin1String("devices"))
        result.appendPath(QLatin1String("devices"));
    else if (file == QLatin1String("android"))
        result.appendPath(QLatin1String("android"));
    else
        return Utils::FileName();
    result.append(QLatin1String(".xml"));
    return result;
}
