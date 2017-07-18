#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>

class ToolButton : public QPushButton {
    Q_OBJECT
public:
    ToolButton(QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
    }
    ToolButton(QString text, QWidget* parent = 0) {
        Q_UNUSED(parent);
        setFixedSize(24, 24);
        setCheckable(true);
        setText(text);
    }
    ~ToolButton() {}
};

class TopToolbar : public QWidget {
    Q_OBJECT
public:
    TopToolbar(QWidget* parent = 0);
    ~TopToolbar();

    void initStackWidget();

private:
    QStackedWidget* m_stackWidget;

    QWidget* m_cutWidget;
    QWidget* m_drawShapeWidget;
    QWidget* m_picWidget;
    QWidget* m_textWidget;
    QWidget* m_blurWidget;

};

#endif // TOPTOOLBAR_H
