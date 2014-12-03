#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QDockWidget>
#include <QColor>

namespace Ui {
class ColorPicker;
}

class ColorPicker : public QDockWidget
{
    Q_OBJECT

public:
    explicit ColorPicker(QWidget *parent = 0);
    ~ColorPicker();
    void SetColor(const QColor& color);
signals:
    void valueChanged( const QColor& );
public slots:
    void OnColorChange(const QColor& color);
private:
    Ui::ColorPicker *ui;
    QColor mColor;
};

#endif // COLORPICKER_H
