#ifndef FQGL_GLOBAL_H
#define FQGL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FQGL_LIBRARY)
#  define FQGLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FQGLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // FQGL_GLOBAL_H
