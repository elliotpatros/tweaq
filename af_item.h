#ifndef AF_ITEM_H
#define AF_ITEM_H

// application header
#include "tweaq.h"

// base class
#include "af_properties.h"

// Qt classes
#include <QColor>
#include <QThread>

// tweaq classes
#include "externalinterface.h"


class AF_Item : public AF_Properties
{
public:
    // constructors and destructors
    explicit AF_Item(const QString path, AF_Item* const parent = nullptr);
    explicit AF_Item(const vector<QVariant> data = audioFileProperties(),
                     AF_Item* const parent = nullptr);
    explicit AF_Item(const AF_Item& other);
    AF_Item& operator=(const AF_Item& other);

    // sets
    void appendChildren(const FileList list);
    void removeChildren(const vector<int> list = vector<int>());
    void filterDuplicates(FileList& list) const;
    void wasProcessed(const bool succeeded);

    // gets
    bool hasUnprocessedChildren() const;
    bool hasChildPath(const QString path) const;
    int index() const;
    int nChildren() const;
    AF_Item* child(const int row);
    AF_Item* parent() const;
    QVariant statusColor() const;
    QVariant data(const int column) const;
    vector<int> processedChildren() const;
    bool wasProcessed() const;


private:
    vector<AF_Item> _children;
    AF_Item* _parent; // non-owned
    DSP_Status _dsp_status;

    // helpers
    void copy(const AF_Item &other);
    bool childIndexIsValid(const int index) const;
};

#endif // AF_ITEM_H
