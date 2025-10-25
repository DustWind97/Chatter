#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    qputenv("QT_LOGGING_RULES", "qt.imageio.png=false");//处理图片sRGB问题：通过程序运行前直接忽略此错误
    QApplication a(argc, argv);
    //a.setStyle(QStyleFactory::create("Fusion"));//设置风格为fusion

    //导入QSS样式表
    QFile qss(":style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("QSS Open Success");
        //QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(qss.readAll());
        qss.close();
    }else{
        qDebug("QSS Open Failed");
    }


    //读入配置文件
    QString fileName = "config.ini";//配置文件名
    QString app_path = QCoreApplication::applicationDirPath(); //应用执行目录
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);//配置文件完整路径名
    QSettings settings(config_path,QSettings::IniFormat);//指定读取文件类型为ini文件
    QString gate_host = settings.value("GateServer/Host").toString();
    QString gate_port = settings.value("GateServer/Port").toString();
    //拼接url

    gate_url_prefix = "http://" + gate_host + ":" + gate_port;

    MainWindow w;
    w.show();
    return a.exec();
}
