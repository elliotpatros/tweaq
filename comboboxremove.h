#ifndef COMBOBOXREMOVE_H
#define COMBOBOXREMOVE_H

// application header
#include "tweaq.h"

// base class
#include "comboboxtweaq.h"

using TweaqAction = std::function<void()>;

class ComboBoxRemove : public ComboBoxTweaq
{
    Q_OBJECT

public:
    explicit ComboBoxRemove(QWidget* const parent = 0);

    // sets
    void addNamedAction(const QString name, TweaqAction action);


private slots:
    void doAction(QString name); // todo: see if there's a qt'er way to do this


private:
    std::map<QString, TweaqAction> actionMap;
};

#endif // COMBOBOXREMOVE_H