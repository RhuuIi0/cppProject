#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <qmessagebox.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "adddialog.h"
#include "deldialog.h"
#include <QDate>
#include "CenteredTableWidgetItem.h"
#include "addnianjiadialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_Import_clicked();           // 批量导入数据
    void receiveAddData(QString strData);   // 接受增加的数据
    void receiveDelData(QString strData);   // 接受删除的数据

    void on_btn_SearchEntry_clicked();

    void on_btn_Pre_clicked();

    void on_btn_Next_clicked();

    void on_table_Information_doubleClicked(const QModelIndex &index);

private slots:
    void showContextMenu(const QPoint &pos);
    void onAddRow();
    void onDeleteRow();
    void receiveAddData_nianjia(QString strData);       // 接受增加的数据

private:
    QSqlDatabase m_db;

    struct INFORMATION
    {
        QString strName;
        QDate dateIn;
        QString strIsHave;
        float fUsedDays;
        float fUnused;
    };
    QList<INFORMATION> m_listInfo;

    void ConnectSQL();                                          // 连接数据库
    bool WriteToSQL(QString strTable, QString strData);         // 向数据库中写入数据
    bool DelToSQL(QString strTable, QString strData);           // 删除数据库中的数据
    void InitComboBox();                                        // 初始化 combobox
    void InitTableWidget(QTableWidget* tableWidget, const QStringList& strList, int nColumnCount);
                                                                // 初始化 tableWidget
    void InitTableWidget_info();
    void InitTableWidget_nianjia();
    void SearchInformation(QSqlQuery query);
                                            // 查询nianjia表，完善 INFORMATION 信息
    float CalcUsedDays(const QString& strName, const QString& strBeginDate, const QString& strEndDate);
                                            // 计算年假已休天数
    void ShowInfo();
    void CreateRightMenu();                 // 创建右键菜单
    void ClearData();                       // 清理过期数据

private:
    Ui::MainWindow *ui;
    AddDialog* m_addDialog;                 // 增加数据界面
    DelDialog* m_delDialog;                 // 删除数据界面
    int m_nSearchYear;                      // 索引年份
    int m_nSelectRow;                       // 当前选中的序号(list中的序号)
    QModelIndex m_SelectRow;                // 当前选中的行数

    QMenu *m_rightClickMenu;
    QAction *m_addAction;
    QAction *m_deleteAction;
    AddNianjiaDialog* m_addNianjiaDialog;   // 增加年假数据界面
};
#endif // MAINWINDOW_H
