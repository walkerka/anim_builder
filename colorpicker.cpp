#include "colorpicker.h"
#include "ui_colorpicker.h"

ColorPicker::ColorPicker(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ColorPicker),
    mColor(0, 0, 0, 0xFF)
{
    ui->setupUi(this);
    connect(ui->ringPicker, SIGNAL(valueChanged(const QColor&)),
            this, SLOT(OnColorChange(const QColor&)));
}

ColorPicker::~ColorPicker()
{
    delete ui;
}

void ColorPicker::OnColorChange(const QColor& color)
{
    if (ui->ringPicker->GetColor() != mColor)
    {
        mColor = color;
        emit valueChanged(color);
    }
}

void ColorPicker::SetColor(const QColor& color)
{
    if (color != mColor)
    {
        ui->ringPicker->SetColor(color);
        mColor = color;
    }
}
