#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include"code_video.h"
#include<QString>


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

void MainWindow::on_decodePtn_clicked()
{
    QString inputPath = "C:/Users/20818/Desktop/input/炉石传说 .mp4", outputPath = "C:/Users/20818/Desktop/input/炉石传说 .ppm";
    code_video::video_play(inputPath, outputPath);
}

