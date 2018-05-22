#include "MainWidget.h"

#include "FQGLController.h"
#include "FQGLPrim.h"
#include "FQGLScene.h"
#include "FQGLWidget.h"

#include "CirclePrim.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

FQGL_DECLARE_PTRS(FQGLScene);

MainWidget::MainWidget(QWidget * parent) :
    QWidget(parent),
    FQGLResponder(),
    _controller(new FQGLController)
{
    _controller->SetResponder(this);
    _SetLayout();

    FQGLSceneSharedPtr scene = _widget->GetScene();
    _circle = CirclePrimSharedPtr(new CirclePrim(0.1, 8,
                                                 QVector4D(.25, 0.25,
                                                           0.25, 0.25)));
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        scene->SetShaders(":/410/vshader.glsl",":/410/fshader.glsl");
    } else {
        scene->SetShaders(":/300/vshader.glsl",":/300/fshader.glsl");
    }

    scene->SetPerspective(width(), height());
    scene->AddPrim(_circle);
}

MainWidget::~MainWidget()
{
}

void
MainWidget::HandleSingleTap(const QVector3D& location)
{
    qDebug() << "Received (" << location.x() << ", " << location.y()
             << ", " << location.z() << ")";

    //QVector3D translateVec(location.x(), location.y(), 0.0f);
    QVector3D translateVec = location;
    QMatrix4x4 translateMat;
    translateMat.translate(translateVec);
    _circle->SetTransform(translateMat);
}

void
MainWidget::_SetLayout()
{
    _mainLayout = new QVBoxLayout();

    _topLayout = new QHBoxLayout();
    _label = new QLabel("Entry:");
    _label->setSizePolicy(QSizePolicy::Preferred,
                          QSizePolicy::Maximum);

    _topLayout->addWidget(_label);

    // _text = new QLineEdit();
    // _text->setSizePolicy(QSizePolicy::Preferred,
    //                       QSizePolicy::Maximum);
    // _topLayout->addWidget(_text);

    _bottomLayout = new QHBoxLayout();

     FQGLWidget *widget = new FQGLWidget(_controller, this);
     _widget = FQGLWidgetSharedPtr(widget);
    _controller->SetWidget(_widget);

    _widget->resize(width(), .9*height());
    _bottomLayout->addWidget(widget);

    _mainLayout->addLayout(_topLayout);
    _mainLayout->addLayout(_bottomLayout);
    delete layout();
    setLayout(_mainLayout);
}
