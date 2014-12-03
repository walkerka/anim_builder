#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QFileDialog>
#include "soundlayer.h"
#include <QWidget>
#include "brushpropertywindow.h"
#include "command.h"
#include "cachedimage.h"
#include "rasterlayer.h"
#include "brushtool.h"
#include "pantool.h"
#include "zoomtool.h"
#include "rotatetool.h"
#include "filltool.h"
#include "regiontool.h"
#include "ringcolorpicker.h"
#include "colorpicker.h"
#include "colortool.h"
#include "splinetool.h"
#include "tracetool.h"
#include "animationfile.h"
#include "newprojectdialog.h"
#include "renderwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mProject(NULL),
    mShowUI(true)
{
    ui->setupUi(this);

    mUndoStack = new QUndoStack();
    mUndoStack->setUndoLimit(10);
    ui->rasterImageEditor->SetUndoStack(mUndoStack);
    ui->timeline->SetUndoStack(mUndoStack);
    ui->timeline->SetEditor(ui->rasterImageEditor);
    mUis.push_back(ui->timeline);

    ui->rasterImageEditor->SetTimeline(ui->timeline);


    QAction* undoAction = mUndoStack->createUndoAction(this);
    undoAction->setShortcut(tr("CTRL+Z"));
    undoAction->setIcon(QIcon(":/icons/undo.png"));
    ui->mainToolBar->addAction(undoAction);


    QAction* redoAction = mUndoStack->createRedoAction(this);
    redoAction->setShortcut(tr("CTRL+Y"));
    redoAction->setIcon(QIcon(":/icons/redo.png"));
    ui->mainToolBar->addAction(redoAction);

//    connect(ui->actionIncreaseBrushSize, SIGNAL(triggered()),
//            ui->rasterImageEditor, SLOT(IncreaseBrushSize()));

//    connect(ui->actionDecreaseBrushSize, SIGNAL(triggered()),
//            ui->rasterImageEditor, SLOT(DecreaseBrushSize()));

    connect(ui->actionNew, SIGNAL(triggered()),
            this, SLOT(NewProject()));

    connect(ui->actionSave, SIGNAL(triggered()),
            this, SLOT(SaveProject()));

    connect(ui->actionExportFrames, SIGNAL(triggered()),
            this, SLOT(ExportFrames()));

    connect(ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(OpenProject()));

    connect(ui->actionImportSound, SIGNAL(triggered()),
            this, SLOT(ImportSound()));

    connect(ui->actionClear, SIGNAL(triggered()),
            ui->rasterImageEditor, SLOT(Clear()));

    connect(ui->actionPrevLayer, SIGNAL(triggered()),
            this, SLOT(PrevLayer()));

    connect(ui->actionNextLayer, SIGNAL(triggered()),
            this, SLOT(NextLayer()));

    connect(ui->actionMoveLayerUp, SIGNAL(triggered()),
            this, SLOT(MoveUpLayer()));

    connect(ui->actionMoveDownLayer, SIGNAL(triggered()),
            this, SLOT(MoveDownLayer()));

    connect(ui->actionPrevFrame, SIGNAL(triggered()),
            this, SLOT(PrevFrame()));

    connect(ui->actionNextFrame, SIGNAL(triggered()),
            this, SLOT(NextFrame()));

    connect(ui->actionPrevImage, SIGNAL(triggered()),
            this, SLOT(PrevImage()));

    connect(ui->actionNextImage, SIGNAL(triggered()),
            this, SLOT(NextImage()));

    connect(ui->actionAddFrame, SIGNAL(triggered()),
            this, SLOT(AddFrame()));

    connect(ui->actionRemoveFrame, SIGNAL(triggered()),
            this, SLOT(RemoveFrame()));

    connect(ui->actionAddExposure, SIGNAL(triggered()),
            this, SLOT(AddExposure()));

    connect(ui->actionDecreaseExposure, SIGNAL(triggered()),
            this, SLOT(DecreaseExposure()));

    connect(ui->actionShowOnion, SIGNAL(triggered()),
            ui->rasterImageEditor, SLOT(ToggleOnionSkin()));

    connect(ui->actionPlay, SIGNAL(triggered()),
            this, SLOT(Play()));

    connect(ui->actionAddRasterLayer, SIGNAL(triggered()),
            this, SLOT(AddRasterLayer()));

    connect(ui->actionAddTraceLayer, SIGNAL(triggered()),
            this, SLOT(AddTraceLayer()));

    connect(ui->actionRemoveLayer, SIGNAL(triggered()),
            this, SLOT(RemoveLayer()));


    connect(ui->actionToggleUI, SIGNAL(triggered()),
            this, SLOT(ToggleUI()));

    connect(ui->actionIdentity, SIGNAL(triggered()),
            ui->rasterImageEditor, SLOT(Identity()));

    mUis.push_back(ui->timelineContainter);

    BrushPropertyWindow* brushWindow = new BrushPropertyWindow();
    this->addDockWidget(Qt::RightDockWidgetArea, brushWindow, Qt::Vertical);
    mUis.push_back(brushWindow);
    connect(brushWindow, SIGNAL(brushSizeChanged(float)),
            this, SLOT(OnBrushSizeChanged(float)));
    connect(brushWindow, SIGNAL(smoothChanged(int)),
            this, SLOT(OnSmoothChanged(int)));
    connect(brushWindow, SIGNAL(modeChanged(QPainter::CompositionMode)),
            this, SLOT(OnModeChanged(QPainter::CompositionMode)));

    ColorPicker* colorPicker = new ColorPicker();
    this->addDockWidget(Qt::RightDockWidgetArea, colorPicker, Qt::Vertical);
    mUis.push_back(colorPicker);
    connect(colorPicker, SIGNAL(valueChanged(QColor)),
            this, SLOT(OnColorChange(QColor)));
    mColorPicker = colorPicker;


    mTimer = new QTimer(this);
    mTimer->setTimerType(Qt::PreciseTimer);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));

    mPenTool = new BrushTool(ui->rasterImageEditor, mUndoStack);
    mPenTool->SetSmooth(2);
    mPenTool->SetMode(QPainter::CompositionMode_DestinationOver);
    ui->rasterImageEditor->SetTool(mPenTool);

    mEraserTool = new RegionTool(ui->rasterImageEditor, mUndoStack);
    mEraserTool->SetMode(QPainter::CompositionMode_Clear);

    mPanTool = new PanTool(ui->rasterImageEditor);
    mZoomTool = new ZoomTool(ui->rasterImageEditor);
    mRotateTool = new RotateTool(ui->rasterImageEditor);

    mFillTool = new FillTool(ui->rasterImageEditor, mUndoStack);
    mFillTool->SetMode(QPainter::CompositionMode_DestinationOver);

    mRegionTool = new RegionTool(ui->rasterImageEditor, mUndoStack);
    mRegionTool->SetMode(QPainter::CompositionMode_DestinationOver);
    mRegionTool->SetSmooth(2);

    mColorTool = new ColorTool(ui->rasterImageEditor, this);

    mSplineTool = new SplineTool(ui->rasterImageEditor, mUndoStack);
    
    mTraceTool = new TraceTool(ui->rasterImageEditor, mUndoStack);

    connect(ui->actionPan, SIGNAL(triggered()),
            this, SLOT(ChangeToolPan()));
    connect(ui->actionZoom, SIGNAL(triggered()),
            this, SLOT(ChangeToolZoom()));
    connect(ui->actionRotate, SIGNAL(triggered()),
            this, SLOT(ChangeToolRotate()));

    connect(ui->actionPen, SIGNAL(triggered()),
            this, SLOT(ChangeToolPen()));

    connect(ui->actionEraser, SIGNAL(triggered()),
            this, SLOT(ChangeToolEraser()));

    connect(ui->actionFill, SIGNAL(triggered()),
            this, SLOT(ChangeToolFill()));

    connect(ui->actionRegion, SIGNAL(triggered()),
            this, SLOT(ChangeToolRegion()));

    connect(ui->actionSpline, SIGNAL(triggered()),
            this, SLOT(ChangeToolSpline()));

    connect(ui->actionSelectColor, SIGNAL(triggered()),
            this, SLOT(ChangeToolColor()));

    connect(ui->actionTraceTool, SIGNAL(triggered()),
        this, SLOT(ChangeToolTrace()));

    ui->rasterImageEditor->SetPanTool(mPanTool);
    ui->rasterImageEditor->SetZoomTool(mZoomTool);
    ui->rasterImageEditor->SetRotateTool(mRotateTool);
    ui->rasterImageEditor->SetColorTool(mColorTool);
    ui->rasterImageEditor->SetEraseTool(mEraserTool);

    //NewProject();
}

MainWindow::~MainWindow()
{
    delete mProject;

    delete mSplineTool;
    delete mColorTool;
    delete mFillTool;
    delete mRegionTool;
    delete mZoomTool;
    delete mRotateTool;
    delete mPanTool;
    delete mEraserTool;
    delete mPenTool;
    delete mTimer;
    delete ui;
}

void MainWindow::NewProject()
{
    NewProjectDialog* d = new NewProjectDialog(this);
    if (d->exec() == QDialog::Accepted)
    {
        QString path = d->GetPath();
        int width = d->GetWidth();
        int height = d->GetHeight();
        int fps = d->GetFps();
        AnimationProject* project = AnimationProject::New(path, width, height, fps);
        if (project && project->GetScenes().size() > 0)
        {
            SceneModel* scene = project->GetScenes().front();
            ui->timeline->SetScene(scene);

            if (mProject)
            {
                delete mProject;
                mProject = NULL;
            }
            mProject = project;
        }
        else
        {
            delete project;
        }
    }
    delete d;
}

void MainWindow::OpenProject()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory", ".");
    if (dir.isEmpty())
    {
        return;
    }
    AnimationProject* project = AnimationProject::Open(dir);
    if (project && project->GetScenes().size() > 0)
    {
        SceneModel* scene = project->GetScenes().front();
        ui->timeline->SetScene(scene);

        if (mProject)
        {
            delete mProject;
            mProject = NULL;
        }
        mProject = project;
    }
    else
    {
        delete project;
    }
}

void MainWindow::SaveProject()
{
    if (!mProject)
    {
        return;
    }

    mProject->Save();
}

void MainWindow::ExportFrames()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save"), tr("."), tr("png (*.png);; jpg (*.jpg)"));
    if(path.isEmpty())
    {
        return;
    }
    ui->timeline->ExportFrames(path);
}

void MainWindow::ExportFrame()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save"), tr("."), tr("png (*.png);; jpg (*.jpg)"));
    if(path.isEmpty())
    {
        return;
    }
    ui->rasterImageEditor->GetImage()->save(path);
}

void MainWindow::ImportFrame()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open"), tr("."), tr("image files (*.png *.jpg *.gif *.tga)"));
    if(path.isEmpty())
    {
        return;
    }
    QImage img(path);
    if (img.isNull())
    {
        return;
    }
    //ui->rasterImageEditor->Load(img);
}

void MainWindow::ImportSound()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open"), tr("."), tr("image files (*.wav)"));
    if (path.isEmpty())
    {
        return;
    }

    SoundLayer* l = (SoundLayer*)ui->timeline->GetLayerAt(1);
    l->Load(path);
}

void MainWindow::BrushSizeChanged(int value)
{
}

void MainWindow::AddFrame()
{
    Layer* l = ui->timeline->GetLayerAt(ui->timeline->GetLayerIndex());
    if (l && l->GetType() == LayerTypeRaster)
    {
        RasterLayer* rl = (RasterLayer*)l;
        rl->AddFrame(ui->timeline->GetFrameIndex());
    }
}

void MainWindow::RemoveFrame()
{
    Layer* l = ui->timeline->GetLayerAt(ui->timeline->GetLayerIndex());
    if (l && l->GetType() == LayerTypeRaster)
    {
        RasterLayer* rl = (RasterLayer*)l;
        rl->RemoveFrame(ui->timeline->GetFrameIndex());
    }
}

void MainWindow::AddExposure()
{
    Layer* l = ui->timeline->GetLayerAt(ui->timeline->GetLayerIndex());
    if (l && l->GetType() == LayerTypeRaster)
    {
        RasterLayer* rl = (RasterLayer*)l;
        rl->ModExposure(ui->timeline->GetFrameIndex(), 1);
    }
}

void MainWindow::DecreaseExposure()
{
    Layer* l = ui->timeline->GetLayerAt(ui->timeline->GetLayerIndex());
    if (l && l->GetType() == LayerTypeRaster)
    {
        RasterLayer* rl = (RasterLayer*)l;
        rl->ModExposure(ui->timeline->GetFrameIndex(), -1);
    }
}

void MainWindow::PrevFrame()
{
    ui->timeline->ModFrameIndex(-1);
}

void MainWindow::NextFrame()
{
    ui->timeline->ModFrameIndex(1);
}

void MainWindow::PrevImage()
{
    Layer* l = ui->timeline->GetLayerAt(ui->timeline->GetLayerIndex());
    if (l && l->GetType() == LayerTypeRaster)
    {
        RasterLayer* rl = (RasterLayer*)l;
        ui->timeline->SetFrameIndex(rl->GetPrevImageIndex(ui->timeline->GetFrameIndex()));
    }
}

void MainWindow::NextImage()
{
    Layer* l = ui->timeline->GetLayerAt(ui->timeline->GetLayerIndex());
    if (l && l->GetType() == LayerTypeRaster)
    {
        RasterLayer* rl = (RasterLayer*)l;
        ui->timeline->SetFrameIndex(rl->GetNextImageIndex(ui->timeline->GetFrameIndex()));
    }
}

void MainWindow::PrevLayer()
{
    ui->timeline->ModLayerIndex(-1);
}

void MainWindow::NextLayer()
{
    ui->timeline->ModLayerIndex(1);
}

void MainWindow::MoveUpLayer()
{
    ui->timeline->MoveLayer(1);
}

void MainWindow::MoveDownLayer()
{
    ui->timeline->MoveLayer(-1);
}

void MainWindow::Play()
{
    if (mTimer->isActive())
    {
        mTimer->stop();
    }
    else
    {
        mTimer->start((int)roundf(1000.0f / ui->timeline->GetFps()));
    }
}

void MainWindow::OnTimer()
{
    ui->timeline->ModFrameIndex(1);
    if (ui->timeline->GetFrameIndex() >= ui->timeline->GetMaxFrames())
    {
        ui->timeline->SetFrameIndex(0);
    }
}

void MainWindow::AddRasterLayer()
{
    int layerIndex = ui->timeline->GetLayerIndex();
    ui->timeline->AddRasterLayer(layerIndex + 1);
    ui->timeline->SetLayerIndex(layerIndex + 1);
}

void MainWindow::AddTraceLayer()
{
    int layerIndex = ui->timeline->GetLayerIndex();
    ui->timeline->AddTraceLayer(layerIndex + 1);
}

void MainWindow::RemoveLayer()
{
    ui->timeline->RemoveLayer();
}

void MainWindow::ToggleUI()
{
    mShowUI = !mShowUI;
    for (size_t i = 0; i < mUis.size(); ++i)
    {
        if (mShowUI)
        {
            mUis[i]->show();
        }
        else
        {
            mUis[i]->hide();
        }
    }
}

void MainWindow::ChangeToolPan()
{
    ui->rasterImageEditor->SetTool(mPanTool);
}

void MainWindow::ChangeToolZoom()
{
    ui->rasterImageEditor->SetTool(mZoomTool);
}

void MainWindow::ChangeToolRotate()
{
    ui->rasterImageEditor->SetTool(mRotateTool);
}

void MainWindow::ChangeToolPen()
{
    ui->rasterImageEditor->SetTool(mPenTool);
}

void MainWindow::ChangeToolEraser()
{
    ui->rasterImageEditor->SetTool(mEraserTool);
}

void MainWindow::ChangeToolFill()
{
    ui->rasterImageEditor->SetTool(mFillTool);
}

void MainWindow::ChangeToolRegion()
{
    ui->rasterImageEditor->SetTool(mRegionTool);
}

void MainWindow::ChangeToolColor()
{
    ui->rasterImageEditor->SetTool(mColorTool);
}

void MainWindow::ChangeToolSpline()
{
    ui->rasterImageEditor->SetTool(mSplineTool);
}

void MainWindow::ChangeToolTrace()
{
    ui->rasterImageEditor->SetTool(mTraceTool);
}

void MainWindow::OnColorChange(const QColor& color)
{
    mPenTool->SetColor(color);
    mFillTool->SetColor(color);
    mRegionTool->SetColor(color);
    mSplineTool->SetColor(color);
    mColorPicker->SetColor(color);
}

void MainWindow::OnBrushSizeChanged(float value)
{
    mPenTool->SetBrushSize(value);
    mSplineTool->SetBrushSize(value);
}

void MainWindow::OnSmoothChanged(int value)
{
    mPenTool->SetSmooth(value);
    mRegionTool->SetSmooth(value);
}

void MainWindow::OnModeChanged(QPainter::CompositionMode mode)
{
    mPenTool->SetMode(mode);
    mFillTool->SetMode(mode);
    mRegionTool->SetMode(mode);
    mSplineTool->SetMode(mode);
}

