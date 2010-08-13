#pragma once

#include <qmetatype.h>

#define QT_REGISTER_TYPE(ClassName) \
	Q_DECLARE_METATYPE(ClassName) \
	static int QT_REGISTER_TYPE_##ClassName() { qRegisterMetaType<ClassName>(#ClassName); return 0; } \
	Q_CONSTRUCTOR_FUNCTION(QT_REGISTER_TYPE_##ClassName)
