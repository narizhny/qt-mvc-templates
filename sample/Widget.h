#ifndef WIDGET_H
#define WIDGET_H

#include <QTabWidget>

class Widget: public QTabWidget
{
public:
    explicit Widget(QWidget *parent = nullptr);
};

#endif // WIDGET_H
