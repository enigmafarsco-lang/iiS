// ============================================================================
// File: spurepage.h
// Subsystem: eLynxSDR pages
// Description: Header definitions and interface declarations for spurepage.h.
// ============================================================================

#ifndef SPUREPAGE_H
#define SPUREPAGE_H

#include <QObject>

class SpurePage : public QObject
{
    Q_OBJECT
public:
    explicit SpurePage(QObject *parent = nullptr);

signals:

};

#endif // SPUREPAGE_H
