#ifndef TQLINEEDIT_H
#define TQLINEEDIT_H

// application header
#include "m_tweaq.h"

// base class
#include <QLineEdit>

// drag and drop
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

// directory
#include <QDir>


class TqLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    // constructor
    explicit TqLineEdit(QWidget* parent = Q_NULLPTR);

    // gets
    QString currentValidFileLocation(void);
    bool isCurrentPathValid(void);


protected:
    // drag and drop handling
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);


private slots:
    void validateTextForUser(void);


private:
    // directory
    QDir _currentPath;

    // gets
    QString defaultPath(void) const;

    // look and feel
    static QString _validStyleSheet;
    static QString _invalidStyleSheet;
};

#endif // TQLINEEDIT_H
