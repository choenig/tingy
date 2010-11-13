/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of Tingy.
**
** Tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with Tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#pragma once

#include <qmetatype.h>
#include <QRegExp>
#include <QStringList>
#include <QTextDocument>

#define QT_REGISTER_TYPE(ClassName) \
	Q_DECLARE_METATYPE(ClassName) \
	static int QT_REGISTER_TYPE_##ClassName() { qRegisterMetaType<ClassName>(#ClassName); return 0; } \
	Q_CONSTRUCTOR_FUNCTION(QT_REGISTER_TYPE_##ClassName)

inline QString dot(QString str) {
	return str.replace('*', QString::fromUtf8("\xe2\x80\xa2"));
}

inline QStringList split(const QString & startTag, const QString & endTag, const QString & src)
{
    QRegExp re(startTag+"(.*)"+endTag);
    re.setMinimal(true);

    QStringList retval;
    int idx = 0;
    while ((idx = src.indexOf(re, idx)) >= 0) {
        retval << re.cap(1);
        ++idx;
    }
    return retval;
}

inline QString unescapeFromXml(const QString & escapedString)
{
    QString unescapedString = escapedString;
    unescapedString.replace("&quot;", "\"");
    unescapedString.replace("&lt;",   "<");
    unescapedString.replace("&gt;",   ">");
    unescapedString.replace("&amp;",  "&");

    QRegExp asciiRE("&#([0-9]+);");
    while (unescapedString.indexOf(asciiRE) != -1) {
        unescapedString.replace(QString("&#%1;").arg(asciiRE.cap(1)),
                                QChar(asciiRE.cap(1).toInt()));
    }

    return unescapedString;
}
