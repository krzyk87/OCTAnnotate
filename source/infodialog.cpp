#include "infodialog.h"
#include "ui_infodialog.h"

#include <QFile>
#include <QTextCodec>

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);

    QFile infoFile("./licence.html");
    if (infoFile.open(QIODevice::ReadOnly)){
        QByteArray data = infoFile.readAll();
        QTextCodec *codec = Qt::codecForHtml(data);
        QString str = codec->toUnicode(data);
        ui->textEdit->setText(str);
    }

    ui->puepsLogoLabel->setPixmap(QPixmap::fromImage(QImage("./logoPUEPS.png")));
    ui->puepsLogoLabel->setScaledContents(true);

    ui->putLogoLabel->setPixmap(QPixmap::fromImage(QImage("./logoPUT.png")));
    ui->putLogoLabel->setScaledContents(true);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::on_closeButton_clicked()
{
    this->close();
}
