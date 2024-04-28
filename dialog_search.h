#ifndef DIALOG_SEARCH_H
#define DIALOG_SEARCH_H

#include <QDialog>
#include <QSqlDatabase>
#include <QListWidgetItem>
#include <QSqlQuery>

namespace Ui {
class Dialog_search;
}

class Dialog_search : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_search(QWidget *parent = nullptr);
    explicit Dialog_search(QSqlDatabase db, QWidget *parent = nullptr);
    ~Dialog_search();

signals:
    void sendAddData(QString);

private slots:
    void on_btn_Entry_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::Dialog_search *ui;
    QSqlDatabase m_db;
    QList<QString> m_listInfo;
};

#endif // DIALOG_SEARCH_H
