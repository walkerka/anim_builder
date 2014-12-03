#include "brushpropertywindow.h"
#include "ui_brushpropertywindow.h"

BrushPropertyWindow::BrushPropertyWindow(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::BrushPropertyWindow)
{
    ui->setupUi(this);
    connect(ui->sizeControl, SIGNAL(valueChanged(double)),
            this, SLOT(OnBrushSizeChanged(double)));

    connect(ui->sizeControl, SIGNAL(valueChanged(double)),
            this, SLOT(OnBrushSizeChanged(double)));

    connect(ui->smoothControl, SIGNAL(valueChanged(int)),
            this, SLOT(OnSmoothChanged(int)));

    connect(ui->modeControl, SIGNAL(currentTextChanged(QString)),
            this, SLOT(OnModeChanged(QString)));
}

BrushPropertyWindow::~BrushPropertyWindow()
{
    delete ui;
}

void BrushPropertyWindow::OnBrushSizeChanged(double value)
{
    emit brushSizeChanged((float)value);
}

void BrushPropertyWindow::OnSmoothChanged(int value)
{
    emit smoothChanged(value);
}

void BrushPropertyWindow::OnModeChanged(QString mode)
{
    QPainter::CompositionMode cm = QPainter::CompositionMode_SourceOver;
    if (mode == "Normal")
    {
        cm = QPainter::CompositionMode_SourceOver;
    }
    else if (mode == "Behind")
    {
        cm = QPainter::CompositionMode_DestinationOver;
    }
    else if (mode == "Erase")
    {
        cm = QPainter::CompositionMode_Clear;
    }
    else if (mode == "Add")
    {
        cm = QPainter::CompositionMode_Plus;
    }
    else if (mode == "Multiply")
    {
        cm = QPainter::CompositionMode_Multiply;
    }
    else if (mode == "PreserveAlpha")
    {
        cm = QPainter::CompositionMode_SourceAtop;
    }

    emit modeChanged(cm);
}
