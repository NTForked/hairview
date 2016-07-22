#ifndef _OGL_WIDGET_H_
#define _OGL_WIDGET_H_

#include <memory>
#include <vector>

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglvertexarrayobject.h>
#include <QtGui/qopenglbuffer.h>
#include <QtGui/qopengltexture.h>
#include <QtGui/qopenglframebufferobject.h>

class Hair;
enum class RefModel;
class ArcballController;

class OGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    OGLWidget(QWidget* parent = nullptr);
    ~OGLWidget();

    void setHairModel(const QString& filename);
    void saveHairMesh(const QString& filename) const;
    void setMethod(RefModel model);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;

private:
    std::unique_ptr<QOpenGLShaderProgram> shader = nullptr;
    std::unique_ptr<QOpenGLShaderProgram> depthShader = nullptr;
    std::unique_ptr<QOpenGLShaderProgram> opacShader = nullptr;

    std::unique_ptr<QOpenGLVertexArrayObject> vao = nullptr;
    std::unique_ptr<QOpenGLBuffer> vBuffer = nullptr;
    std::unique_ptr<QOpenGLBuffer> iBuffer = nullptr;

    std::unique_ptr<QOpenGLFramebufferObject> depthFbo = nullptr;
    std::unique_ptr<QOpenGLFramebufferObject> opacFbo = nullptr;

    std::unique_ptr<Hair> hair;
    std::unique_ptr<ArcballController> arcball;
};

#endif  // _OGL_WIDGET_H_
