#ifndef FKAPPLICATION_GLOBAL_H
#define FKAPPLICATION_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef FK_NO_CORE_LIBRARY
#  define FKCORESHARED_EXPORT
#else
#if defined(FKCORE_LIBRARY)
#  define FKCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define FKCORESHARED_EXPORT Q_DECL_IMPORT
#endif
#endif

#endif
