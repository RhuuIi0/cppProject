#ifndef DIALOG_PERSONALINFO_H
#define DIALOG_PERSONALINFO_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class Dialog_PersonalInfo;
}

class Dialog_PersonalInfo : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_PersonalInfo(QWidget *parent = nullptr);
    ~Dialog_PersonalInfo();

signals:
    void sendAddData(QString str);
    void sendDelData(QString str);

private slots:
    void on_btn_delete_clicked();

    void on_btn_Add_clicked();

private:
    Ui::Dialog_PersonalInfo *ui;
    QSqlDatabase m_db;
};

#endif // DIALOG_PERSONALINFO_H
