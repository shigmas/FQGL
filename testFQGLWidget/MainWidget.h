#include <QWidget>
#include "FQGLResponder.h"
#include "FQGLTypes.h"

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QVBoxLayout;

FQGL_DECLARE_PTRS(CirclePrim);
FQGL_DECLARE_PTRS(FQGLController);
FQGL_DECLARE_PTRS(FQGLWidget);

class MainWidget : public QWidget,
                   public FQGLResponder
{
    Q_OBJECT

public:
    MainWidget(QWidget * parent = 0);
    ~MainWidget();

    virtual void HandleSingleTap(const QVector3D& location) override;

protected:
    void _SetLayout();

private:
    QVBoxLayout * _mainLayout;
    QHBoxLayout * _topLayout;
    QHBoxLayout * _bottomLayout;

    QLabel * _label;
    QLineEdit * _text;
    FQGLWidgetSharedPtr _widget;

    CirclePrimSharedPtr _circle;

    FQGLControllerSharedPtr _controller;
};
