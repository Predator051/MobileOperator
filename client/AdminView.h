#ifndef ADMINVIEW_H
#define ADMINVIEW_H

#include <QMainWindow>

namespace Ui {
class AdminView;
}

class AdminView : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminView(QWidget *parent = 0);
    ~AdminView();

signals:
    void onClose();

private:
    Ui::AdminView *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // ADMINVIEW_H
