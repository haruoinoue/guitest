#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<cstdlib>
#include<ctime>
#include<qrect.h>
#include<qtimer.h>
#include<QKeyEvent>
#include<QDebug>
#include<QFont>
#include<QPixmap>
#include<QMessageBox>
#include<string>
#include<QWidget>
#include<QLCDNumber>
#include<QFileInfo>
#include<QDir>
#include<QFile>
#include<QPainter>
#include<QFileInfoList>
#include<string>
#include <fstream>
#include "qmcdecoder.h"

//extern char* process(char * buffer,long len);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


int check(QString fileload)  //0 文件或文件夹不存在，1 文件，2 文件夹
{
    if(fileload.at(fileload.size() - 1)=='/')
    {
        QDir dir(fileload);
        if(dir.exists()) return 2;
        else return 0;
    }
    else
    {
        QFileInfo fileInfo(fileload);
        if((fileInfo.exists()))  return 1;
        else return 0;
    }
}


QString cd_dir(QString file)
{
    int calcu;
    for(calcu=file.size()-1;file[calcu]!='/';calcu--);
    file=file.left(calcu+1);
    return file;
}


void inter_face(QString fileinfo,QString filename,QString bak,QString position)
{
    std::string str=fileinfo.toStdString();
    std::fstream infile(str,std::ios::in|std::ios::binary);
    auto len = infile.seekg(0,std::ios::end).tellg();
    infile.seekg(0,std::ios::beg);
    char * buffer = new char[len];
    infile.read(buffer,len);
    infile.close();
    Seed seed;
    buffer=seed.process(buffer,len);    //解码

    QString newload=position+filename+".flac";
    QFile newfile(newload);
    newfile.open(QIODevice::ReadWrite);
    newfile.write(buffer,len);
    newfile.close();
    delete[] buffer;
}


QString MainWindow::identifying(QString rawload)
{
    QString outstr=rawload;
    if(outstr.left(7)=="file://")   //用于linux中拖入文件，去除file://前标
        outstr=outstr.mid(7,outstr.size()-7);
    if(outstr.size())
        if(outstr[0]=='"'||outstr[0]=='\'')
            outstr=outstr.right(outstr.size()-1);
    int s=-1;
    if(outstr.size())  s=outstr.size()-1;
    for(int i=s;i>=0&&(outstr[i]==' '||outstr[i]=='\''||outstr[i]=='"');i--)
        outstr=outstr.left(outstr.size()-1);

    int result;
    QString temp=outstr;
    if(!temp.size())  {f1d2=0;return temp;}
    f1d2=check(temp);   //检查文件(夹)存在

    temp+='/';
    result=check(temp);
    if(f1d2<result) outstr=temp;
    f1d2=(f1d2>result)?f1d2:result;

    if(temp.size()>2)
    {
        temp=temp.left(temp.size()-3);   //清除linux中后置转行符
        result=check(temp);
        if(f1d2<result) outstr=temp;
        f1d2=(f1d2>result)?f1d2:result;

        temp+='/';
        result=check(temp);
        if(f1d2<result) outstr=temp;
        f1d2=(f1d2>result)?f1d2:result;
    }
    return outstr;
}



void MainWindow::filecheck(QString fileinfo)   //临时
{
    QFileInfo file(fileinfo);
    QString name=file.fileName();
    name=name.left(name.size()-8);
    //QMessageBox::question(NULL,"test",name);
    if(fileinfo.right(8)==".qmcflac")  inter_face(fileinfo,name,".qmcflac",position);
    else QMessageBox::question(NULL,"Error","无法打开该文件");
}


void MainWindow::filefound(QString dirload)   //临时
{
    QDir dir(dirload);
    QStringList suffix;
    QFileInfoList fileinfolist;
    QFileInfo fileinfo;
    QString file,s,name;

    suffix << "*.qmcflac";   //测试格式
    s=".qmcflac";
    dir.setNameFilters(suffix);   //过滤文件
    fileinfolist = dir.entryInfoList();
    amount=fileinfolist.length();   //进度条分母

    if(amount)
    {
        this->ui->label_4->setText("已找到可转化文件"+tr("%1").arg(amount)+"个，正在尝试转化");
        for(int i=0;i<fileinfolist.length();i++)   //i，进度条分子
        {
             fileinfo = fileinfolist.at(i);
             //QMessageBox::question(NULL,"test",fileinfo.fileName());
             file=dirload+fileinfo.fileName();
             name=fileinfo.fileName();
             name=name.left(name.size()-s.size());
             inter_face(file,name,s,position);
        }
    }
    else QMessageBox::question(NULL,"Error","未能找到可打开的文件");
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)   //源文件(夹)路径
{
    fileload=arg1;
    //QMessageBox::question(NULL,"test",fileload);
    QString get_le2=this->ui->lineEdit_2->text();
    if(fileload.size())   //同步输出路径
    {
        bool a=0,b=0;
        if(fileload.size()) a=(get_le2+fileload.at(fileload.size() - 1)==fileload);
        if(get_le2.size())  b=(fileload+get_le2.at(get_le2.size()-1)==get_le2);
        if(!get_le2.size()||a||b)
            this->ui->lineEdit_2->setText(fileload);
    }
    else if(get_le2.size()==1)  this->ui->lineEdit_2->setText(fileload);
    this->ui->label_4->setText("输入中-----------");
}


void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    loadout= arg1;
    //QMessageBox::question(NULL,"test",loadout);
}


void MainWindow::on_pushButton_clicked()
{
    position=identifying(loadout);
    if(f1d2==1)  position=cd_dir(position);//若是文件，则退到其文件夹
    output=f1d2;
    if(!position.size())  QMessageBox::question(NULL,"Error","输出路径为空");
    this->ui->lineEdit_2->setText(position);

    route=identifying(fileload);   //将路径可识别化
    if(!route.size())  QMessageBox::question(NULL,"Error","输入路径为空");
    this->ui->lineEdit->setText(route);
    //QMessageBox::question(NULL,"test",tr("%1").arg(f1d2)+route+"end");
    if(route.size()||position.size())   //阻断运行
    switch(f1d2)
    {
    case 0:
        this->ui->label_4->setText("未找到指定文件/文件夹，请检查输入路径是否正确");
        break;
    case 1:    //针对单个文件
        this->ui->label_4->setText("找到指定文件,正在尝试转化----");
        if(output)  filecheck(route);
        else  QMessageBox::question(NULL,"Error","路径"+loadout+"不存在");
        break;
    case 2:   //针对文件夹
        this->ui->label_4->setText("找到指定文件夹,正在搜索文件----");
        if(output)  filefound(route);
        else  QMessageBox::question(NULL,"Error","路径"+loadout+"不存在");
        break;
    default:  break;
    }
}

