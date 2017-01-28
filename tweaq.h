#ifndef TWEAQ_H
#define TWEAQ_H

// c++
#include <vector>
#include <algorithm>
#include <functional>
#include <map>

// Qt classes
#include <QDebug>
#include <QStringList>
#include <QStringBuilder>

// typedefs
using std::unique_ptr;
using std::vector;
using FileList = vector<QString>;

// algorithms
template <typename Container, typename Type>
static bool in_container(const Type t, const Container c)
{
    return c.end() != std::find(c.begin(), c.end(), t);
}

template <typename Container, typename Type>
static bool in_sorted(const Type t, const Container c)
{
    return std::binary_search(c.begin(), c.end(), t);
}

template <typename Container, typename Predicate>
static void erase_if(Container& c, Predicate p)
{
    c.erase(std::remove_if(c.begin(), c.end(), p), c.end());
}

#endif // TWEAQ_H
