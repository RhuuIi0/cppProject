#include "dialog_search.h"
#include "ui_dialog_search.h"
#include <QMessageBox>

Dialog_search::Dialog_search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_search)
{
    ui->setupUi(this);
}

Dialog_search::Dialog_search(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_search)
{
    ui->setupUi(this);
    m_db = db;
}

Dialog_search::~Dialog_search()
{
    delete ui;
}

void Dialog_search::on_btn_Entry_clicked()
{
    if(ui->edt_Search->text().isEmpty()){
        QMessageBox::information(this, "error", "姓名不能为空", "OK");
        return ;
    }
    else {
        // 清除 list widget 中的数据
        ui->listWidget->clear();
        m_listInfo.clear();

        // 从数据库中查询数据(id, name, department)
        //select * from information_kaoqin where name="?";
        QSqlQuery* query = new QSqlQuery(m_db);
        QString strSqlExec = "select * from information_kaoqin where name=\"" +
                             ui->edt_Search->text() + "\";";
        if(!query->exec(strSqlExec)){
            return ;
        }
        if(!query->next()){
            return ;
        }

        // 将数据插入到list中
        QString strTemp;
        do{
            strTemp.clear();
            strTemp = query->value(0).toString() + "-" +
                      query->value(1).toString() + "-" +
                      query->value(2).toString();
            m_listInfo.append(strTemp);
        }while(query->next());

    }

    // 将数据显示到 listwidget
    for(auto it = m_listInfo.begin(); it != m_listInfo.end(); ++it){
        ui->listWidget->addItem(*it);
    }

    return ;
}


void Dialog_search::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
//    QMessageBox::information(this, "tip", item->text());
    sendAddData(item->text());

    return ;
}

