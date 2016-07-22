#include "oglwidget.h"

#include <iostream>

#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtGui/qevent.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

#include "common.h"
#include "hairloader.h"
#include "arcballcontroller.h"

static constexpr int SHADER_POSITION_LOC = 0;
static constexpr int SHADER_TANGENT_LOC = 1;

static const QVector3D LIGHT_POS = QVector3D(-200.0f, -100.0f, 100.0f);

static QMatrix4x4 depthMVP;

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
    arcball = std::make_unique<ArcballController>(this);

    QMatrix4x4 mMat, vMat;
    mMat.setToIdentity();
    vMat.lookAt(QVector3D(0.0f, 0.0f, 200.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    arcball->initModelView(mMat, vMat);
}

OGLWidget::~OGLWidget() {
    grabFramebuffer().save(QString(SOURCE_DIRECTORY) + "result.jpg");
}

void OGLWidget::setHairModel(const QString& filename) {
    makeCurrent();

    // Load hair.
    hair = std::make_unique<Hair>();
    if (!loadHair(filename.toStdString(), hair.get())) {
        std::abort();
    }
    
    // Prepare VAO.
    vao = std::make_unique<QOpenGLVertexArrayObject>(this);
    vao->create();
    vao->bind();

    vBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vBuffer->create();
    vBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vBuffer->bind();
    vBuffer->allocate(&hair->segments[0], sizeof(Segment) * hair->segments.size());

    auto f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnableVertexAttribArray(SHADER_POSITION_LOC);
    f->glEnableVertexAttribArray(SHADER_TANGENT_LOC);
    f->glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Segment), (void*)0);
    f->glVertexAttribPointer(SHADER_TANGENT_LOC,  3, GL_FLOAT, GL_FALSE, sizeof(Segment), (void*)(sizeof(float) * 3));

    std::vector<unsigned int> indices;
    for (int i = 0; i < hair->segments.size(); i++) {
        indices.push_back(i);
    }

    iBuffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    iBuffer->create();
    iBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    iBuffer->bind();
    iBuffer->allocate(&indices[0], indices.size() * sizeof(unsigned int));

    vao->release();
}

void OGLWidget::setMethod(RefModel model) {
    hair->refmodel = model;
    update();
}

void OGLWidget::initializeGL() {
    // General OpenGL settings.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set color to clear window (black).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Prepare shader.
    shader = std::make_unique<QOpenGLShaderProgram>(this);
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(SOURCE_DIRECTORY) + "render.vs");
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(SOURCE_DIRECTORY) + "render.fs");
    shader->link();
    if (!shader->isLinked()) {
        std::cerr << "Failed to compile or link shaders." << std::endl;
        std::exit(1);
    }
}

void OGLWidget::paintGL() {
    if (!hair) return;

    // Clear window color.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrices.
    QMatrix4x4 mMat, vMat, pMat;
    mMat = arcball->modelMat();
    vMat = arcball->viewMat();
    pMat.perspective(45.0f, (float)width() / (float)height(), 0.1f, 1000.0f);
    QMatrix4x4 mvMat  = arcball->modelViewMat(); //vMat * mMat;
    QMatrix4x4 mvpMat = pMat * mvMat;

    // Draw color triangle.
    shader->bind();
    vao->bind();

    shader->setUniformValue("uMVPMat", mvpMat);
    shader->setUniformValue("uMVMat", mvMat);
    shader->setUniformValue("uLightPos", LIGHT_POS);
    shader->setUniformValue("uDepthMVP", depthMVP);
    shader->setUniformValue("uRefModel", (int)hair->refmodel);
    shader->setUniformValue("uDiffuseColor", hair->diffuse);
    shader->setUniformValue("uSpecularColor", hair->specular);

    unsigned int sum = 0;
    for (int i = 0; i < hair->numSegments.size(); i++) {
        glDrawElements(GL_LINE_STRIP, hair->numSegments[i] + 1, GL_UNSIGNED_INT, (void*)sum);
        sum += sizeof(unsigned int) * (hair->numSegments[i] + 1);
    }

    shader->release();
    vao->release();
}

void OGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
}

void OGLWidget::mousePressEvent(QMouseEvent *ev) {
    arcball->setOldPoint(ev->pos());
    arcball->setNewPoint(ev->pos());
    if (ev->button() == Qt::LeftButton) {
        arcball->setMode(ArcballMode::Rotate);
    } else if (ev->button() == Qt::RightButton) {
        arcball->setMode(ArcballMode::Translate);
    } else if (ev->button() == Qt::MiddleButton) {
        arcball->setMode(ArcballMode::Scale);
    }
}

void OGLWidget::mouseMoveEvent(QMouseEvent *ev) {
    arcball->setNewPoint(ev->pos());
    arcball->update();
    arcball->setOldPoint(ev->pos());
    update();
}

void OGLWidget::mouseReleaseEvent(QMouseEvent *ev) {
    arcball->setMode(ArcballMode::None);
}

void OGLWidget::wheelEvent(QWheelEvent *ev) {
    arcball->setScroll(arcball->scroll() + ev->delta() / 1000.0);
    arcball->update();
    update();
}
