#ifndef QTWEAQ_H
#define QTWEAQ_H

#ifdef __cplusplus

// libsndfile
#include "sndfile.hh" // libsndfile

// useful c++ things
#include <vector>
#include <memory> // for std::unique_ptr
using std::vector;

// useful Qt things
#ifdef __QTWEAQ_IS_DEFINED
#include <QDebug>
#include <QStringList>
#include <QStringBuilder>
#endif // __QTWEAQ_IS_DEFINED

#endif // __cplusplus

#endif // QTWEAQ_H
