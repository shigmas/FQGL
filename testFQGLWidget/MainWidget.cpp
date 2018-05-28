#include "MainWidget.h"

#include "FQGLController.h"
#include "FQGLPrim.h"
#include "FQGLScene.h"
#include "FQGLWidget.h"

#include "LoupePrim.h"
#include "CubePrim.h"

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

    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        _widget->SetSceneShaders(":/410/vshader.glsl",":/410/fshader.glsl",
                                 ":/410/ftshader.glsl");
    } else {
        _widget->SetSceneShaders(":/300/vshader.glsl",":/300/fshader.glsl",
                                 ":/300/ftshader.glsl");
    }

    _widget->SetScenePerspective(width(), height());

    QVector4D gray(.75, 0.25, 0.25, 1.0f);
    _loupe = LoupePrimSharedPtr(new LoupePrim(0.3, 4, 1.5, gray));
    _widget->AddPrimToScene(_loupe,true);

    QVector4D green(.1, 0.25, 0.75, 0.50);
    _cube = CubePrimSharedPtr(new CubePrim(0.2, green));
    _cube->SetTranslate(QVector3D(0.0f, 0.0f, -.21f));
    _widget->AddPrimToScene(_cube);
}

MainWidget::~MainWidget()
{
}

void
MainWidget::HandleSingleTap(const QVector2D& location)
{
    qDebug() << "Received (" << location.x() << ", " << location.y() << ")";

    //QVector3D translateVec(location.x(), location.y(), 0.0f);
    QVector3D translateVec = _widget->ToScenePoint(location);
    QMatrix4x4 translateMat;
    translateMat.translate(translateVec);
    _widget->EnableTextureBuffer();
    _loupe->SetTransform(translateMat);
    _loupe->SetTextureOffset(_widget->ToTexPoint(location));
}

void
MainWidget::HandleRightTap(const QVector2D& location)
{
    qDebug() << "Right (" << location.x() << ", " << location.y() << ")";

    //QVector3D translateVec(location.x(), location.y(), 0.0f);
    QVector3D translateVec = _widget->ToScenePoint(location);
    QMatrix4x4 translateMat;
    translateMat.translate(translateVec);
    _widget->EnablePickTestingBuffer();
    _loupe->SetTransform(translateMat);
    _loupe->SetTextureOffset(_widget->ToTexPoint(location));
}

void
MainWidget::OnRenderComplete()
{
    qDebug() << "Render complete";
    GLuint texId = _widget->GetTextureIdFromLastRender();
    if (_widget->IsTextureBufferEnabled() &&
        (texId != GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)) {
        _loupe->SetTextureById(texId);
    }
    _widget->DisablePickTestingBuffer();
    _widget->DisableTextureBuffer();
    
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

     FQGLWidget *widget = new FQGLWidget(_controller);
     _widget = FQGLWidgetSharedPtr(widget);
    _controller->SetWidget(_widget);

    _widget->resize(width(), .9*height());
    _bottomLayout->addWidget(widget);

    _mainLayout->addLayout(_topLayout);
    _mainLayout->addLayout(_bottomLayout);
    delete layout();
    setLayout(_mainLayout);
}
