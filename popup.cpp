#include "popup.h"

Popup::Popup(QWidget* parent) :
    QDialog(parent),
    _layout(new QVBoxLayout(this)),
    _label(new QLabel(this))
{
    disconnect();
    setWindowFlags(Qt::Popup |
                   Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::WindowDoesNotAcceptFocus);

    setWindowModality(Qt::WindowModal);
    setWindowOpacity(0.85);

    setLayout(_layout);
    _layout->addWidget(_label, 0, Qt::AlignCenter);
    _label->setAlignment(Qt::AlignCenter);

    connect(this, SIGNAL(finished(int)), SLOT(deleteLater()));
    startTimer(1800);
}

// events
void Popup::mousePressEvent(QMouseEvent*)
{
    close();
}

void Popup::keyPressEvent(QKeyEvent*)
{
    close();
}

void Popup::timerEvent(QTimerEvent*)
{
    close();
}

// sets
void Popup::showMessage(const QString message)
{
    if (_label != nullptr)
    {
        _label->setText(message);
    }

    show();
}
