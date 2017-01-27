#ifndef COMBOBOXREMOVE_H
#define COMBOBOXREMOVE_H

// application header
#include "tweaq.h"

// base class
#include "combobox.h"

using TweaqAction = std::function<void()>;

class ComboBoxRemove : public ComboBox
{
    Q_OBJECT

public:
    explicit ComboBoxRemove(QWidget* const parent = nullptr);

    // sets
    void addNamedAction(const QString name, TweaqAction action);


private slots:
    void doAction(QString name); // todo: see if there's a qt'er way to do this


private:
    std::map<QString, TweaqAction> actionMap;
};

#endif // COMBOBOXREMOVE_H
