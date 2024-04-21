#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


enum CMD_TYPE
{
    CONTROL = 0x04, // 控制命令
    PARAMSET = 0x40, // 参数设置
    MODESET = 0x80, // 参数发送
};

enum CONTROL_MODE
{
    ACTIVE = 0,
    PASSIVE = 1,
};

enum FINGER_INDEX
{
    THUMB = 0,
    FOREFINGER = 1,
    MIDDLE = 2,
    RING = 3,
    LITTLE = 4,
    TOTAl =5,
    SINGLE = 6,
};

enum FINGLE_STATUS
{
    STOP = 0,
    CIRCUL = 1,
    STRETCH = 2,
    SHRINK = 3,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 绘图事件
    void paintEvent(QPaintEvent *);

private slots:
    void on_btnSwitch_clicked();

    void serialPortRead_Slot();

//    void on_chkRec_stateChanged(int arg1);

    void on_clean_textRec_clicked();

    void hand_SBtnClicked(int);
    void hand_WBtnClicked(int);
    void on_totall_SButton_clicked();

    void on_totall_WButton_clicked();
    void selectButtonsClick(int);

    void selectButtonsClick2(int id);
    void on_paramSet_Button_clicked();

    void on_active_Button_clicked();

    void on_passive_Button_clicked();


    void realtimeDataSlot();
private:
    Ui::MainWindow *ui;

    QSerialPort *mySerialPort;

    // 发送、接收字节计数
    long sendNum, recvNum;
    QLabel *lblSendNum;
    QLabel *lblRecvNum;
    void setNumOnLabel(QLabel *lbl, QString strS, long num);
    void rx_data_handle(QByteArray *data, uint8_t len);
    void tx_data_handle(QByteArray *data, uint8_t len);
    void cmd_send_control(CMD_TYPE cmd, uint8_t finger_index, uint8_t finger_status);
    void cmd_send_paramset(CMD_TYPE cmd, uint8_t finger_index, uint8_t speed, uint8_t dynamics);
    void cmd_send_modeset(CMD_TYPE cmd, uint8_t mode);
    void Sleep(int msec);

    // 定时发送-定时器
    QTimer *timSend;

    uint8_t rx_data[16];
    uint8_t tx_data[16];

    bool btnStatus[3][5];
    QByteArray sendData;

    uint8_t position[256][5];
    int positionCount;
};
#endif // MAINWINDOW_H
