#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "geometryengine.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "math/sensor_processing_lib.h"

class GeometryEngine;
class QOpenGLVertexArrayObject;


class RotationCube : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit RotationCube(QWidget *parent = 0);
    ~RotationCube();
public:
    void updateSensorData(float gyro_x, float gyro_y, float gyro_z, float acc_x, float acc_y, float acc_z, float mag_x, float mag_y, float mag_z);
    void resetToDefaultPosition();
protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent *e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();
    void initTextures();

    float _GetGLValue(GLenum name);
    const char * _IsGLEnabled(GLenum name);

private:
    vector_ijk fused_vector;
    double startTime;
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries;

    QOpenGLTexture *texture;
    QOpenGLVertexArrayObject *_vao;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    float angularSpeed;
    QQuaternion rotation;
};

#endif // MAINWIDGET_H
