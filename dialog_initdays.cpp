#include "dialog_initdays.h"
#include "ui_dialog_initdays.h"
#include <QMessageBox>

Dialog_InitDays::Dialog_InitDays(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_InitDays)
{
    ui->setupUi(this);
}

Dialog_InitDays::Dialog_InitDays(const QDate& date, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_InitDays)
{
    ui->setupUi(this);

    m_date = date;
    // 设置lab为当前的日期
    QFont font;
    font.setPointSize(18);
    font.setFamily("Source Code Pro");
    ui->lab_date->setFont(font);
    ui->lab_date->setText(m_date.toString("yyyy年MM月"));
}

Dialog_InitDays::~Dialog_InitDays()
{
    delete ui;
}


void Dialog_InitDays::on_btn_Entry_clicked()
{
    // 获取应出勤天数
    QString strTemp = ui->edt_days->text();
    if(!strTemp.isEmpty()){
        int nDays = strTemp.toInt();
        sendAddData(nDays);
    }
    else{
        // 输入为空
        QMessageBox::information(this, "error", "请输入正确的天数");
        return ;
    }

    return ;
}

