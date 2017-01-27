#ifndef COMBOBOXDSP_H
#define COMBOBOXDSP_H

// application header
#include "tweaq.h"

// base class
#include "combobox.h"

class ComboBoxDSP : public ComboBox
{
    Q_OBJECT

public:
    explicit ComboBoxDSP(QWidget* const parent = nullptr);

    // gets
    bool processHasBeenChosen() const;
    int processIndex() const;
};

#endif // COMBOBOXDSP_H
