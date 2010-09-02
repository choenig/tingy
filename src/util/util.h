#pragma once

#include <qmetatype.h>
#include <QRegExp>
#include <QStringList>

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
