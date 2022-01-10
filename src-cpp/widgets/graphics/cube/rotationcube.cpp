#include "rotationcube.h"

#include <QMouseEvent>
#include <QQuaternion>
#include <iostream>

#include <QOpenGLVertexArrayObject>

#include "math/sensor_processing_lib.h"

RotationCube::RotationCube(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    texture(0),
    angularSpeed(0),
    startTime(( double ) std::chrono::duration_cast<std::chrono::nanoseconds> ( std::chrono::system_clock::now().time_since_epoch ( ) ).count())
{
    Q_INIT_RESOURCE(shaders);
    Q_INIT_RESOURCE(textures);

    _vao = new QOpenGLVertexArrayObject();

    setFixedWidth(300);
    setFixedHeight(300);
}

RotationCube::~RotationCube()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    delete geometries;
    delete _vao;
    doneCurrent();
}

void RotationCube::resetToDefaultPosition()
{
    rotation = {1,0,0,0 };
    angularSpeed = 0.0;
    rotationAxis = {0,0,0 };
    update();
}

void RotationCube::updateSensorData (float gyro_x, float gyro_y, float gyro_z, float acc_x, float acc_y, float acc_z, float mag_x, float mag_y, float mag_z)
{
    /*
    fused_vector - corrected accelerometer readings
    delta - delay or time taken to complete a loop
    wx,wy,wz - gyro values in rad/s
    AcX, AcY, AcZ - raw accelerometer values
    q_acc - quaternion representing orientation
    angles - euler angles
    */

    std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();

    const double ms = ( double ) std::chrono::duration_cast<std::chrono::nanoseconds> ( timestamp.time_since_epoch ( ) ).count();
    double delta = 0.00000001 * ( ( double ) ( ms - startTime ) );
    startTime = ms;
    // std::cout << "time: " << delta << std::endl;

    fused_vector = update_fused_vector ( fused_vector, acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, delta );

    // Quaternion q = quaternion_from_accelerometer( fused_vector.a, fused_vector.b, fused_vector.c);
    Quaternion q = quaternion_from_gyro ( gyro_x, gyro_y, gyro_z, delta );
    q = quaternion_normalize(q);
    rotation = QQuaternion(q.a, q.b, q.c, q.d);
    // std::cout << "My Q: " << rotation.scalar() << ", " << rotation.x() << " " << rotation.y() << " " << rotation.z() << std::endl;
    update();
}

//! [0]
void RotationCube::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void RotationCube::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    float acc = diff.length() / 100.0f;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}
//! [0]

//! [1]
void RotationCube::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
        std::cout << "Natural Q: " << rotation.scalar() << ", " << rotation.x() << " " << rotation.y() << " " << rotation.z() << std::endl;
        // Request an update
        update();
    }
}
//! [1]

void RotationCube::initializeGL()
{
    initializeOpenGLFunctions();
    qDebug() << "GL Version: " << _GetGLValue(GL_VERSION);
    qDebug() << "GLSL Version: " << _GetGLValue(GL_SHADING_LANGUAGE_VERSION);

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //    glEnable(GL_CULL_FACE);
//! [2]

    geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);

    _vao->create();
    _vao->bind();
}

//! [3]
void RotationCube::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}
//! [3]

//! [4]
void RotationCube::initTextures()
{
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}
//! [4]

//! [5]
void RotationCube::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void RotationCube::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);
    //qDebug() << projection * matrix * QVector4D(1.0f, 0.0f, 0.0f, 0.0f);
//! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("a_texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);
}

float
RotationCube::_GetGLValue(GLenum name)
{   
    const GLubyte * strVal = glGetString(name);
    float val;
    sscanf((char *)strVal, "%f", &val);

    return val;
}

const char *
RotationCube::_IsGLEnabled(GLenum name)
{
    return glIsEnabled(name) ? "Enabled" : "Disabled";
}
