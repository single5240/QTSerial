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
    CONTROL = 0,

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

    void on_chkRec_stateChanged(int arg1);

    void on_clean_textRec_clicked();

    void hand_SBtnClicked(int);
    void hand_WBtnClicked(int);
    void on_totall_SButton_clicked();

    void on_totall_WButton_clicked();
    void selectButtonsClick(int);

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
    void cmd_send(CMD_TYPE cmd, uint8_t *data);

    // 定时发送-定时器
    QTimer *timSend;

    uint8_t rx_data[16];
    uint8_t tx_data[16];

    bool btnStatus[3][5];
};
#endif // MAINWINDOW_H
