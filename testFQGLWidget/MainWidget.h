#include <QWidget>
#include "FQGLResponder.h"
#include "FQGLTypes.h"

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QVBoxLayout;

FQGL_DECLARE_PTRS(LoupePrim);
FQGL_DECLARE_PTRS(CubePrim);
FQGL_DECLARE_PTRS(FQGLController);
FQGL_DECLARE_PTRS(FQGLWidget);

class MainWidget : public QWidget,
                   public FQGLResponder
{
    Q_OBJECT

public:
    MainWidget(QWidget * parent = 0);
    virtual ~MainWidget();

    virtual void HandleSingleTap(const QVector2D& location) override;
    virtual void HandleRightTap(const QVector2D& location) override;

    virtual void OnRenderComplete() override;

protected:
    void _SetLayout();

private:

    QVBoxLayout * _mainLayout;
    QHBoxLayout * _topLayout;
    QHBoxLayout * _bottomLayout;

    QLabel * _label;
    QLineEdit * _text;
    FQGLWidgetSharedPtr _widget;

    LoupePrimSharedPtr _loupe;
    CubePrimSharedPtr _cube;

    FQGLControllerSharedPtr _controller;
};
