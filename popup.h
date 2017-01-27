#ifndef POPUP_H
#define POPUP_H

// application header
#include "tweaq.h"

// base class
#include <QDialog>

// widgets
#include <QVBoxLayout>
#include <QLabel>

class Popup : public QDialog
{
    Q_OBJECT

public:
    explicit Popup(QWidget* parent = nullptr);

    // sets
    void showMessage(const QString message);


signals:
    void clicked();


protected:
    // events
    void mousePressEvent(QMouseEvent*);
    void timerEvent(QTimerEvent*);
    void keyPressEvent(QKeyEvent*);


private:
    QVBoxLayout* _layout;
    QLabel* _label;
};

#endif // POPUP_H
