#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include <QtWidgets>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory", ".");
    if (dir.isEmpty())
    {
        return;
    }
    ui->pathField->setText(dir);
}

int NewProjectDialog::GetWidth()
{
    return ui->widthField->value();
}

int NewProjectDialog::GetHeight()
{
    return ui->heightField->value();
}

int NewProjectDialog::GetFps()
{
    return ui->fpsField->value();
}

QString NewProjectDialog::GetPath()
{
    return ui->pathField->text();
}

void NewProjectDialog::on_okButton_clicked()
{
    QDir dir(ui->pathField->text());
    if (!dir.exists())
    {
        QMessageBox::information(this, "Error", "Please select a folder");
        return;
    }
    emit accept();
}
