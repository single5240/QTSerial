#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QSignalMapper"
#include <QDebug>


QString fingerName[5] = {"拇指","食指","中指","无名指","小指"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Qt Serial Debugger");

    // 发送、接收计数清零
    sendNum = 0;
    recvNum = 0;
    // 状态栏
    QStatusBar *sBar = statusBar();
    // 状态栏的收、发计数标签
    lblSendNum = new QLabel(this);
    lblRecvNum = new QLabel(this);
    // 设置标签最小大小
    lblSendNum->setMinimumSize(100, 20);
    lblRecvNum->setMinimumSize(100, 20);
    setNumOnLabel(lblSendNum, "S: ", sendNum);
    setNumOnLabel(lblRecvNum, "R: ", recvNum);
    // 从右往左依次添加
    sBar->addPermanentWidget(lblSendNum);
    sBar->addPermanentWidget(lblRecvNum);

    QSignalMapper * btnSMapper;
    btnSMapper = new QSignalMapper(this);
    QPushButton * Sbutton[5]={ui->thumb_SButton,ui->forefinger_SButton,ui->middle_SButton,ui->ring_SButton,
                             ui->little_SButton};
       for(int i = 0;i<5;i++)
       {
            connect(Sbutton[i], SIGNAL(clicked(bool)), btnSMapper, SLOT(map()));
            btnSMapper->setMapping(Sbutton[i], i);
       }
    connect(btnSMapper, SIGNAL(mapped(int)), this, SLOT(hand_SBtnClicked(int)));


    QSignalMapper * btnWMapper;
    btnWMapper = new QSignalMapper(this);
    QPushButton * Wbutton[5]={ui->thumb_WButton,ui->forefinger_WButton,ui->middle_WButton,ui->ring_WButton,
                             ui->little_WButton};
       for(int i = 0;i<5;i++)
       {
            connect(Wbutton[i], SIGNAL(clicked(bool)), btnWMapper, SLOT(map()));
            btnWMapper->setMapping(Wbutton[i], i);
       }
    connect(btnWMapper, SIGNAL(mapped(int)), this, SLOT(hand_WBtnClicked(int)));

    // 新建一串口对象
    mySerialPort = new QSerialPort(this);

    // 串口接收，信号槽关联
    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(serialPortRead_Slot()));

    ui->TotalRadioButton->setChecked(true);
    ui->singleControlWidget->setVisible(false);
    ui->totalControlWidget->setVisible(true);
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(selectButtonsClick(int)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

// 绘图事件
void MainWindow::paintEvent(QPaintEvent *)
{
    // 绘图
    // 实例化画家对象，this指定绘图设备
    QPainter painter(this);

    // 设置画笔颜色
    QPen pen(QColor(0,0,0));
    // 设置画笔线宽（只对点线圆起作用，对文字不起作用）
    pen.setWidth(1);
    // 设置画笔线条风格，默认是SolidLine实线
    // DashLine虚线，DotLine点线，DashDotLine、DashDotDotLine点划线
    pen.setStyle(Qt::DashDotDotLine);
    // 让画家使用这个画笔
    painter.setPen(pen);

    painter.drawLine(QPoint(ui->txtRec->x() + ui->txtRec->width(), ui->txtRec->y()), QPoint(this->width(), ui->txtRec->y()));
    painter.drawLine(QPoint(ui->statusbar->x(), ui->statusbar->y()-2), QPoint(this->width(), ui->statusbar->y()-2));

}

// 串口接收显示，槽函数
void MainWindow::serialPortRead_Slot()
{
    /*QString recBuf;
    recBuf = QString(mySerialPort->readAll());*/

    QByteArray recBuf;
    recBuf = mySerialPort->readAll();
    rx_data_handle(&recBuf,recBuf.size());
    // 接收字节计数
    recvNum += recBuf.size();
    // 状态栏显示计数值
    setNumOnLabel(lblRecvNum, "R: ", recvNum);

    // 判断是否为16进制接收，将以后接收的数据全部转换为16进制显示（先前接收的部分在多选框槽函数中进行转换。最好多选框和接收区组成一个自定义控件，方便以后调用）
    if(ui->chkRec->checkState() == false){

        // 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
        ui->txtRec->insertPlainText(recBuf);
    }else{
        // 16进制显示，并转换为大写
        QString str1 = recBuf.toHex().toUpper();//.data();
        // 添加空格
        QString str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        ui->txtRec->insertPlainText(str2);
        //ui->txtRec->insertPlainText(recBuf.toHex());
    }

    // 移动光标到文本结尾
    ui->txtRec->moveCursor(QTextCursor::End);

    // 将文本追加到末尾显示，会导致插入的文本换行
    /*ui->txtRec->appendPlainText(recBuf);*/

    /*// 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
    ui->txtRec->insertPlainText(recBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/

    // 利用一个QString去获取消息框文本，再将新接收到的消息添加到QString尾部，但感觉效率会比当前位置插入低。也不会发生换行
    /*QString txtBuf;
    txtBuf = ui->txtRec->toPlainText();
    txtBuf += recBuf;
    ui->txtRec->setPlainText(txtBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/

    // 利用一个QString去缓存接收到的所有消息，效率会比上面高一点。但清空接收的时候，要将QString一并清空。
    /*static QString txtBuf;
    txtBuf += recBuf;
    ui->txtRec->setPlainText(txtBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/
}

// 打开/关闭串口 槽函数
void MainWindow::on_btnSwitch_clicked()
{

    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity   checkBits;

    // 获取串口波特率
    // 有没有直接字符串转换为 int的方法？？？
    //baudRate = ui->cmbBaudRate->currentText().toInt();
    if(ui->cmbBaudRate->currentText() == "9600"){
        baudRate = QSerialPort::Baud9600;
    }else if(ui->cmbBaudRate->currentText() == "38400"){
        baudRate = QSerialPort::Baud38400;
    }else if(ui->cmbBaudRate->currentText() == "115200"){
        baudRate = QSerialPort::Baud115200;
    }else{

    }

    // 获取串口数据位
    if(ui->cmbData->currentText() == "5"){
        dataBits = QSerialPort::Data5;
    }else if(ui->cmbData->currentText() == "6"){
        dataBits = QSerialPort::Data6;
    }else if(ui->cmbData->currentText() == "7"){
        dataBits = QSerialPort::Data7;
    }else if(ui->cmbData->currentText() == "8"){
        dataBits = QSerialPort::Data8;
    }else{

    }

    // 获取串口停止位
    if(ui->cmbStop->currentText() == "1"){
        stopBits = QSerialPort::OneStop;
    }else if(ui->cmbStop->currentText() == "1.5"){
        stopBits = QSerialPort::OneAndHalfStop;
    }else if(ui->cmbStop->currentText() == "2"){
        stopBits = QSerialPort::TwoStop;
    }else{

    }

    // 获取串口奇偶校验位
    if(ui->cmbCheck->currentText() == "无"){
        checkBits = QSerialPort::NoParity;
    }else if(ui->cmbCheck->currentText() == "奇校验"){
        checkBits = QSerialPort::OddParity;
    }else if(ui->cmbCheck->currentText() == "偶校验"){
        checkBits = QSerialPort::EvenParity;
    }else{

    }

    // 想想用 substr strchr怎么从带有信息的字符串中提前串口号字符串
    // 初始化串口属性，设置 端口号、波特率、数据位、停止位、奇偶校验位数
    mySerialPort->setBaudRate(baudRate);
    mySerialPort->setDataBits(dataBits);
    mySerialPort->setStopBits(stopBits);
    mySerialPort->setParity(checkBits);
    //mySerialPort->setPortName(ui->cmbSerialPort->currentText());// 不匹配带有串口设备信息的文本
    // 匹配带有串口设备信息的文本
    QString spTxt = ui->cmbSerialPort->currentText();
    spTxt = spTxt.section(':', 0, 0);//spTxt.mid(0, spTxt.indexOf(":"));
    //qDebug() << spTxt;
    mySerialPort->setPortName(spTxt);

    // 根据初始化好的串口属性，打开串口
    // 如果打开成功，反转打开按钮显示和功能。打开失败，无变化，并且弹出错误对话框。
    if(ui->btnSwitch->text() == "打开串口"){
        if(mySerialPort->open(QIODevice::ReadWrite) == true){
            //QMessageBox::
            ui->btnSwitch->setText("关闭串口");
            // 让端口号下拉框不可选，避免误操作（选择功能不可用，控件背景为灰色）
            ui->cmbSerialPort->setEnabled(false);
            ui->cmbBaudRate->setEnabled(false);
            ui->cmbStop->setEnabled(false);
            ui->cmbData->setEnabled(false);
            ui->cmbCheck->setEnabled(false);
        }else{
            QMessageBox::critical(this, "错误提示", "串口打开失败！！！\r\n该串口可能被占用\r\n请选择正确的串口");
        }
    }else{
        mySerialPort->close();
        ui->btnSwitch->setText("打开串口");
        // 端口号下拉框恢复可选，避免误操作
        ui->cmbSerialPort->setEnabled(true);
        ui->cmbBaudRate->setEnabled(true);
        ui->cmbStop->setEnabled(true);
        ui->cmbData->setEnabled(true);
        ui->cmbCheck->setEnabled(true);
    }

}

void MainWindow::hand_SBtnClicked(int index){
    QPushButton *SBtn_temp;
    QPushButton *WBtn_temp;
    switch (index) {
    case 0:
        SBtn_temp = ui->thumb_SButton;
        WBtn_temp = ui->thumb_WButton;
        break;
    case 1:
        SBtn_temp = ui->forefinger_SButton;
        WBtn_temp = ui->forefinger_WButton;
        break;
    case 2:
        SBtn_temp = ui->middle_SButton;
        WBtn_temp = ui->middle_WButton;
        break;
    case 3:
        SBtn_temp = ui->ring_SButton;
        WBtn_temp = ui->ring_WButton;
        break;
    case 4:
        SBtn_temp = ui->little_SButton;
        WBtn_temp = ui->little_WButton;
        break;
    }
    if(btnStatus[0][index] == false){
        btnStatus[0][index] = true;
        SBtn_temp->setText("循环");
        //向mcu发送命令 todo
        ui->txtRec->appendPlainText("[系统]:"+fingerName[index]+"循环命令已发送！");
        WBtn_temp->setEnabled(false);
    } else {
        btnStatus[0][index] = false;
        SBtn_temp->setText("停止");
        //向mcu发送命令 todo
        ui->txtRec->appendPlainText("[系统]:"+fingerName[index]+"停止命令已发送！");
        WBtn_temp->setEnabled(true);
    }
}

void MainWindow::hand_WBtnClicked(int index){
    QPushButton *WBtn_tmep;
    switch (index) {
    case 0:
        WBtn_tmep = ui->thumb_WButton;
        break;
    case 1:
        WBtn_tmep = ui->forefinger_WButton;
        break;
    case 2:
        WBtn_tmep = ui->middle_WButton;
        break;
    case 3:
        WBtn_tmep = ui->ring_WButton;
        break;
    case 4:
        WBtn_tmep = ui->little_WButton;
        break;
    }
    if(btnStatus[1][index] == 0){
        btnStatus[1][index] = 1;
        WBtn_tmep->setText("收缩");
        //向mcu发送命令 todo
        ui->txtRec->appendPlainText("[系统]:"+fingerName[index]+"收缩命令已发送！");
    } else {
        btnStatus[1][index] = 0;
        WBtn_tmep->setText("伸展");
        //向mcu发送命令 todo
        ui->txtRec->appendPlainText("[系统]:"+fingerName[index]+"伸展命令已发送！");
    }

}
// 发送按键槽函数
// 如果勾选16进制发送，按照asc2的16进制发送
//void MainWindow::on_btnSend_clicked()
//{
//    QByteArray sendData;
//    // 判断是否为16进制发送，将发送区全部的asc2转换为16进制字符串显示，发送的时候转换为16进制发送
//    if(ui->chkSend->checkState() == false){
//        // 字符串形式发送
//        sendData = ui->txtSend->toPlainText().toLocal8Bit().data();
//    }else{
//        // 16进制发送
//        sendData = QByteArray::fromHex(ui->txtSend->toPlainText().toUtf8()).data();
//    }

//    // 如发送成功，会返回发送的字节长度。失败，返回-1。
//    int a = mySerialPort->write(sendData);
//    // 发送字节计数并显示
//    if(a > 0)
//    {
//        // 发送字节计数
//        sendNum += a;
//        // 状态栏显示计数值
//        setNumOnLabel(lblSendNum, "S: ", sendNum);
//    }

//}

// 状态栏标签显示计数值
void MainWindow::setNumOnLabel(QLabel *lbl, QString strS, long num)
{
    // 标签显示
    QString strN;
    strN.sprintf("%ld", num);
    QString str = strS + strN;
    lbl->setText(str);
}


//void MainWindow::on_btnClearSend_clicked()
//{
//    ui->txtSend->clear();
//    // 清除发送字节计数
//    sendNum = 0;
//    // 状态栏显示计数值
//    setNumOnLabel(lblSendNum, "S: ", sendNum);
//}

// 先前接收的部分在多选框状态转换槽函数中进行转换。（最好多选框和接收区组成一个自定义控件，方便以后调用）
void MainWindow::on_chkRec_stateChanged(int arg1)
{
    // 获取文本字符串
    QString txtBuf = ui->txtRec->toPlainText();

    // 获取多选框状态，未选为0，选中为2
    // 为0时，多选框未被勾选，接收区先前接收的16进制数据转换为asc2字符串格式
    if(arg1 == 0){

        QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());
        // 文本控件清屏，显示新文本
        ui->txtRec->clear();
        ui->txtRec->insertPlainText(str1);
        // 移动光标到文本结尾
        ui->txtRec->moveCursor(QTextCursor::End);

    }else{// 不为0时，多选框被勾选，接收区先前接收asc2字符串转换为16进制显示

        QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();
        // 添加空格
        QByteArray str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->txtRec->clear();
        ui->txtRec->insertPlainText(str2);
        // 移动光标到文本结尾
        ui->txtRec->moveCursor(QTextCursor::End);

    }
}

//// 先前发送区的部分在多选框状态转换槽函数中进行转换。（最好多选框和发送区组成一个自定义控件，方便以后调用）
//void MainWindow::on_chkSend_stateChanged(int arg1)
//{
//    // 获取文本字符串
//    QString txtBuf = ui->txtSend->toPlainText();

//    // 获取多选框状态，未选为0，选中为2
//    // 为0时，多选框未被勾选，将先前的发送区的16进制字符串转换为asc2字符串
//    if(arg1 == 0){

//        QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());
//        // 文本控件清屏，显示新文本
//        ui->txtSend->clear();
//        ui->txtSend->insertPlainText(str1);
//        // 移动光标到文本结尾
//        ui->txtSend->moveCursor(QTextCursor::End);

//    }else{// 多选框被勾选，将先前的发送区的asc2字符串转换为16进制字符串

//        QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();
//        // 添加空格
//        QByteArray str2;
//        for(int i = 0; i<str1.length (); i+=2)
//        {
//            str2 += str1.mid (i,2);
//            str2 += " ";
//        }
//        // 文本控件清屏，显示新文本
//        ui->txtSend->clear();
//        ui->txtSend->insertPlainText(str2);
//        // 移动光标到文本结尾
//        ui->txtSend->moveCursor(QTextCursor::End);

//    }
//}

// 定时发送开关 选择复选框
//void MainWindow::on_chkTimSend_stateChanged(int arg1)
//{
//    // 获取复选框状态，未选为0，选中为2
//    if(arg1 == 0){
//        timSend->stop();
//        // 时间输入框恢复可选
//        ui->txtSendMs->setEnabled(true);
//    }else{
//        // 对输入的值做限幅，小于10ms会弹出对话框提示
//        if(ui->txtSendMs->text().toInt() >= 10){
//            timSend->start(ui->txtSendMs->text().toInt());// 设置定时时长，重新计数
//            // 让时间输入框不可选，避免误操作（输入功能不可用，控件背景为灰色）
//            ui->txtSendMs->setEnabled(false);
//        }else{
//            ui->chkTimSend->setCheckState(Qt::Unchecked);
//            QMessageBox::critical(this, "错误提示", "定时发送的最小间隔为 10ms\r\n请确保输入的值 >=10");
//        }
//    }
//}
void MainWindow::rx_data_handle(QByteArray * data, uint8_t len){
    Q_UNUSED(data)
    Q_UNUSED(len)
}

void MainWindow::tx_data_handle(QByteArray *data, uint8_t len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
}

void MainWindow::cmd_send(CMD_TYPE cmd, uint8_t *data)
{
    Q_UNUSED(cmd)
    Q_UNUSED(data)
}
void MainWindow::on_clean_textRec_clicked()
{
    ui->txtRec->clear();
    // 清除发送、接收字节计数
    sendNum = 0;
    recvNum = 0;
    // 状态栏显示计数值
    setNumOnLabel(lblSendNum, "S: ", sendNum);
    setNumOnLabel(lblRecvNum, "R: ", recvNum);
}



void MainWindow::on_totall_SButton_clicked()
{
    if(btnStatus[2][0] == false){
        btnStatus[2][0] = true;
        ui->totall_SButton->setText("循环");
        ui->totall_WButton->setEnabled(false);
        //向mcu发送命令 todo
        ui->txtRec->appendPlainText("[系统]:所有手指循环命令已发送！");
    } else {
        btnStatus[2][0] = false;
        ui->totall_SButton->setText("停止");
        ui->totall_WButton->setEnabled(true);
        //向mcu发送命令 todo
        ui->txtRec->appendPlainText("[系统]:所有手指收停止令已发送！");
    }
}

void MainWindow::on_totall_WButton_clicked()
{
    if(btnStatus[2][1] == false){
        btnStatus[2][1] = true;
        ui->totall_WButton->setText("伸展");

        //向mcu发送命令 todo

        ui->txtRec->appendPlainText("[系统]:所有手指伸展命令已发送！");
    } else {
        btnStatus[2][1] = false;
        ui->totall_WButton->setText("收缩");

        //向mcu发送命令 todo

        ui->txtRec->appendPlainText("[系统]:所有手指收缩命令已发送！");
    }
}
void MainWindow::selectButtonsClick(int id)
{
    if(id == ui->buttonGroup->id(ui->TotalRadioButton)){
        ui->singleControlWidget->setVisible(false);
        ui->totalControlWidget->setVisible(true);
    } else if(id == ui->buttonGroup->id(ui->SingleRadioButton)){
        ui->singleControlWidget->setVisible(true);
        ui->totalControlWidget->setVisible(false);
    }

    //向mcu发送停止命令 todo
}
