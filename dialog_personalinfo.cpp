#include "dialog_personalinfo.h"
#include "ui_dialog_personalinfo.h"
#include <QDate>
#include <QSqlQuery>
#include <QMessageBox>

Dialog_PersonalInfo::Dialog_PersonalInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_PersonalInfo)
{
    ui->setupUi(this);

    // 初始化 date
    QDate date = QDate::currentDate();
    ui->edt_date->setDate(date);

    // 初始化 combo
    //select * form department;
    QSqlQuery* query = new QSqlQuery(m_db);
    QString strSqlExec = "select * from department;";
    if(!query->exec(strSqlExec)){
        return ;
    }
    if(!query->next()){
        return ;
    }
    do{
        ui->combo_department->addItem(query->value(0).toString());
    }while(query->next());
}

Dialog_PersonalInfo::~Dialog_PersonalInfo()
{
    delete ui;
}

void Dialog_PersonalInfo::on_btn_delete_clicked()
{
    // 获取数据
    QDate date = ui->edt_date->date();
    QString strName = ui->edt_name->text();
    QString strDepartment = ui->combo_department->currentText();
    if(strName == nullptr || strDepartment == nullptr){
        QMessageBox::information(this, "error", "输入姓名");
        return ;
    }

    sendDelData(strDepartment + "," + strName + "," + date.toString("yyyy/MM/dd"));
}


void Dialog_PersonalInfo::on_btn_Add_clicked()
{
    // 获取数据
    QDate date = ui->edt_date->date();
    QString strName = ui->edt_name->text();
    QString strDepartment = ui->combo_department->currentText();
    if(strName == nullptr || strDepartment == nullptr){
        QMessageBox::information(this, "error", "输入姓名");
        return ;
    }

    sendAddData(strDepartment + "," + strName + "," + date.toString("yyyy/MM/dd"));

    return ;
}

