#ifndef AFITEM_H
#define AFITEM_H

// application header
#include "m_tweaq.h"

// base class
#include "afmeta.h"

// Qt classes
#include <QVariant>

class AFItem : public AFMeta
{
public:
    // constructors & destructor
    explicit AFItem(const QString path,
                    AFItem* parentItem = nullptr);
    explicit AFItem(const vector<QVariant> data = AFItem::soundFileProperties(),
                    AFItem* parentItem = nullptr);
    AFItem(const AFItem& other);
    virtual ~AFItem(void);


    // DSP object stuff
    enum DSPStatus
    {
        SucceededDSP = 0,
        FailedDSP,
        Unprocessed
    };
    DSPStatus completed;

    // properties
    enum PropertyIndex
    {
        AbsolutePath = 0,
        BaseName,
        Extension,
        Channels,
        Duration,
        SampleRate,
        ParentDirectory,
        NumProperties
    };

    //sets
    void appendChild(AFItem* child);
    void removeChild(t_int row);
    void setProccessingStatus(const bool succeeded);

    // gets
    bool hasBeenProcessed(void) const;
    AFItem* child(t_int row);
    t_int childCount(void) const;
    static t_int columnCount(void);
    QVariant data(t_int column) const;
    t_int isAtRow(void) const;
    AFItem* parentItem(void);
    static vector<QVariant> soundFileProperties(void);
    static t_int visibleColumnCount(void);


private:
    // mvc stuff
    vector<AFItem*> _childItems; // owned array of owned pointers
    vector<QVariant> _itemData;
    AFItem* _parentItem; // non-owned

    // convenience
    bool childIndexIsValid(const t_int index) const;
    void killAllMyChildren(void);
};

#endif // AFITEM_H
