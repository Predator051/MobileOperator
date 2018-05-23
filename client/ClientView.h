#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QMainWindow>

namespace Ui {
class ClientView;
}

class ClientView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientView(QWidget *parent = 0);
    ~ClientView();

signals:
    void onClose();

private:
    Ui::ClientView *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // CLIENTVIEW_H
