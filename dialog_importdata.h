#ifndef DIALOG_IMPORTDATA_H
#define DIALOG_IMPORTDATA_H

#include <QDialog>

namespace Ui {
class Dialog_ImportData;
}

class Dialog_ImportData : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ImportData(QWidget *parent = nullptr);
    ~Dialog_ImportData();

private slots:
    void on_btn_selectFile_clicked();

private:
    Ui::Dialog_ImportData *ui;
};

#endif // DIALOG_IMPORTDATA_H
