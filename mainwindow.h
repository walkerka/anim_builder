#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QUndoStack>
#include <QUndoView>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class AnimationProject;
class CachedImage;
class BrushTool;
class PanTool;
class ZoomTool;
class RotateTool;
class FillTool;
class RegionTool;
class ColorTool;
class SplineTool;
class ColorPicker;
class TraceTool;
class RenderWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void NewProject();
    void OpenProject();
    void SaveProject();
    void PrevLayer();
    void NextLayer();
    void MoveUpLayer();
    void MoveDownLayer();
    void PrevFrame();
    void NextFrame();
    void PrevImage();
    void NextImage();
    void AddFrame();
    void RemoveFrame();
    void ExportFrame();
    void ExportFrames();
    void AddExposure();
    void DecreaseExposure();
    void ImportFrame();
    void ImportSound();
    void BrushSizeChanged(int value);
    void Play();
    void OnTimer();
    void AddRasterLayer();
    void AddTraceLayer();
    void RemoveLayer();
    void ToggleUI();
    void ChangeToolPan();
    void ChangeToolZoom();
    void ChangeToolRotate();
    void ChangeToolPen();
    void ChangeToolEraser();
    void ChangeToolFill();
    void ChangeToolRegion();
    void ChangeToolColor();
    void ChangeToolSpline();
    void ChangeToolTrace();
    void OnColorChange(const QColor& color);
    void OnBrushSizeChanged(float value);
    void OnSmoothChanged(int value);
    void OnModeChanged(QPainter::CompositionMode mode);

private:
    Ui::MainWindow *ui;
    AnimationProject* mProject;
    QUndoStack* mUndoStack;
    QTimer* mTimer;
    bool mShowUI;
    std::vector<QWidget*> mUis;
    ColorPicker* mColorPicker;
    BrushTool* mPenTool;
    RegionTool* mEraserTool;
    PanTool* mPanTool;
    ZoomTool* mZoomTool;
    RotateTool* mRotateTool;
    FillTool* mFillTool;
    RegionTool* mRegionTool;
    ColorTool* mColorTool;
    SplineTool* mSplineTool;
    TraceTool* mTraceTool;
};

#endif // MAINWINDOW_H
