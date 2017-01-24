#ifndef COMBOBOXDSP_H
#define COMBOBOXDSP_H

// application header
#include "tweaq.h"

// base class
#include "comboboxtweaq.h"

class ComboBoxDSP : public ComboBoxTweaq
{
    Q_OBJECT

public:
    explicit ComboBoxDSP(QWidget* const parent = 0);

    // gets
    bool processHasBeenChosen() const;
    int processIndex() const;
};

#endif // COMBOBOXDSP_H