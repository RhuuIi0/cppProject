#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QFont>
#include <QTableWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QTableWidgetItem>
#include <QColor>
#include <QShortcut>
#include "dialog_search.h"
#include "dialog_initdays.h"
#include "dialog_status.h"
#include "dialog_personalinfo.h"
#include "dialog_importdata.h"

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
    void on_btn_Pre_clicked();

    void on_btn_Next_clicked();

    void on_btn_PreYear_clicked();

    void on_btn_NextYear_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    enum class STATUS_TIME{
        AM,
        PM,
        ALLDAY
    };

private:
    Ui::MainWindow *ui;
    QDate m_dateLabel;
    int m_nDaysOfMonth;                                     // 当前月应出勤天数
    QSqlDatabase m_db;

private:
    void Init_labelDate();
    void Init_tableStyle(QTableWidget* tableWidget);        // 初始化table的style
    void Init_tableFormat(QTableWidget* tableWidget);       // 初始化table的大框架（日期）
    void Init_tableFormat_Name(QString strDepartment, QTableWidget* tableWidget);
                                                            // 初始化所有table的大框架（姓名）
    QString SelectWeek(QDate date);                         // 返回星期几
    bool ConnectSQL();                                      // 连接数据库
    void OutputDebug(QString strDebug);                     // 输出调试信息
    void OutputDebug_Box(QString strDebug);

    void ShowInfo();                                        // 显示数据信息(显示 一列/每人次
    void ShowInfo_Update();                                 // 显示数据信息(显示 两行/每人次
    void UpdateShowInfo_Row(int nRow);                      // 只更新两行信息
    QString GetStatus_day(int nId, int day);                // 获取考勤状态(按天获取
    QString GetStatus_month(int nId);                       // 获取考勤装填(按年获取
    int CalcStatus(const int &nId, QString strStatus, STATUS_TIME time);
                                                            // 计算考勤状态数量
    bool IsCanChanged(int nRow, int nCol);                  // 判断是否可以进行改变

private:    // 菜单
    QMenu* m_menuRightClick;
    QAction* m_actionImport;                                // 导入数据菜单
    QAction* m_actionPersonal;                              // 增删用户菜单
    Dialog_PersonalInfo* m_dialogPersonalInfo;
    QAction* m_actionImportData;                            // 导入数据，并进行分析
    Dialog_ImportData* m_dialogImportData;

    void CreateRightMenu();                                 // 创建右键菜单
    void showContextMenu(const QPoint &pos);
    void ImportData();                                      // 导入员工信息数据
    void AddAndDelPersonal();                               // 增删员工
    void onImportPersonalData();                            // 导入员工考勤数据，并进行处理
private slots:
    void receiveAddData(QString str);
    void receiveDelData(QString str);

private:    // map 表（保存table和部门的对应关系）
    std::map<QString, QTableWidget*> m_mapTableAndDepartment;
    void Init_mapTableAndDepartment();

private slots:  // 接收单击信号，高亮选中行和列
    void highlightRowAndColumn(int nRow, int nColumn);          // 单击高亮整行整列的槽函数
private:
    void connectSlots();


private:    // 绑定快捷键 C-F
    Dialog_search* m_searchDialog;
    QShortcut* m_shortcutOpenDialog;
private slots:
    void OpenSearchDialog();
    void receiveSearchData(QString str);

private:    // 绑定快捷键 delete
    QShortcut* m_shortcutDelete;
private slots:
    void onDeleteData();


private:    // 初始化应出勤天数 & 初始化所有成员的 data_kaoqin 数据库表
    Dialog_InitDays* m_InitDaysDialog;

    void InitDays();
    void InitDatakaoqin();
private slots:
    void receiveInitDaysData(int days);


private:    // table的双击事件
    Dialog_Status* m_dialogStatus;
    bool m_bIsodd = false;                                  // 是否是奇数行

    void Init_tableDoubleClickEvent();                      // 绑定tablewidget的双击事件
    int GetId(QString strData);                             // (department,name,date_year_month)
private slots:
    void onDoubleClickEvent(int row, int col);              // 双击事件槽函数
    void receiveStatusData(QString strStatu);               // 接收考勤状态信息


};
#endif // MAINWINDOW_H
