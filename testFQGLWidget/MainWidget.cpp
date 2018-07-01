#include "MainWidget.h"

#include "FQGLController.h"
#include "FQGLPrim.h"
#include "FQGLScene.h"
#include "FQGLWidget.h"

#include "CubePrim.h"
#include "LoupePrim.h"
#include "SquareLoupePrim.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

FQGL_DECLARE_PTRS(FQGLScene);

MainWidget::MainWidget(QWidget * parent) :
    QWidget(parent),
    FQGLResponder(),
    _controller(std::make_shared<FQGLController>())
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
    //_loupe = std::make_shared<SquareLoupePrim>(0.35, 1.5, gray);
    _loupe = std::make_shared<LoupePrim>(0.35, 16, 1.5, gray);
    _loupe->SetTextureByResourcePath(":/resources/raideen.jpg");
    _widget->AddPrimToScene(_loupe);

    QVector4D green(.1, 0.25, 0.75, 0.20);
    QVector4D blue(.1, 0.25, 0.25, 0.75);
    QVector4D yellow(.200, 0.200, 0.0f, 0.75);
    FQGLPrimSharedPtr cube;
    cube = std::make_shared<CubePrim>(0.35, green);
    cube->SetTranslate(QVector3D(0.3f, 0.0f, -.21f));
    _cubes.push_back(cube);
    _widget->AddPrimToScene(cube);

    cube = std::make_shared<CubePrim>(0.35, blue);
    cube->SetTranslate(QVector3D(-0.3f, 0.0f, -.21f));
    _cubes.push_back(cube);
    _widget->AddPrimToScene(cube);

    cube = std::make_shared<CubePrim>(0.35, yellow);
    cube->SetTranslate(QVector3D(0.0f, 0.33f, -.21f));
    _cubes.push_back(cube);
    _widget->AddPrimToScene(cube);
}

MainWidget::~MainWidget()
{
}

void
MainWidget::HandleSingleTap(const QVector2D& location)
{
    QVector2D coord = _widget->ScreenToCoordinate(location);
    QVector3D translateVec = _widget->CoordToScene(coord, 3.0);
    // qDebug() << "Tap: " << location << "in scene: " << translateVec;
    _widget->EnableTextureBuffer();
    _loupe->SetTranslate(translateVec);
    // Drop the Z.
    //_loupe->SetTextureCenter(_widget->ScreenToCoordinates(location));
    _loupe->SetTextureCenter(location);
}

void
MainWidget::HandleRightTap(const QVector2D& location)
{
    qDebug() << "Right (" << location.x() << ", " << location.y() << ")";

    //QVector3D translateVec(location.x(), location.y(), 0.0f);
    QVector2D coord = _widget->ScreenToCoordinate(location);
    QVector3D translateVec = _widget->CoordToScene(coord);
    QMatrix4x4 translateMat;
    translateMat.translate(translateVec);
    _widget->EnablePickTestingBuffer();
    _loupe->SetTransform(translateMat);
    //_loupe->SetTextureOffset(widget->ToTexPoint(location));
}

void
MainWidget::HandleDrag(const QVector2D& location)
{
    qDebug() << "Drag (" << location.x() << ", " << location.y() << ")";
    QVector2D coord = _widget->ScreenToCoordinate(location);
    QVector3D translateVec = _widget->CoordToScene(coord, 3.0);
    // qDebug() << "Tap: " << location << "in scene: " << translateVec;
    _widget->EnableTextureBuffer();
    _loupe->SetTranslate(translateVec);
    // Drop the Z.
    //_loupe->SetTextureCenter(_widget->ScreenToCoordinates(location));
    _loupe->SetTextureCenter(location);

}

void
MainWidget::OnPreRenderComplete()
{
    GLuint texId = _widget->GetTextureIdFromLastRender();
    if (_widget->IsTextureBufferEnabled() &&
        (texId != GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)) {
        _loupe->SetTextureById(texId);
    }
}

void
MainWidget::OnRenderComplete()
{
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
