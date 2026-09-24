#ifndef CAMERA3D_H
#define CAMERA3D_H
#include <QMatrix4x4>

class Camera3D
{
public:
    Camera3D();

    const QMatrix4x4& toMatrix();
    void rotate(const QQuaternion &dr);
    void rotate(float angle, const QVector3D &axis);
    void rotate(float angle, float ax, float ay, float az);

private:
  bool               m_dirty;
   QMatrix4x4        m_world;
   QQuaternion       m_rotation;
   QVector3D         m_translation;
};
inline Camera3D::Camera3D() : m_dirty(true) {}
inline void Camera3D::rotate(float angle, const QVector3D &axis) { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Camera3D::rotate(float angle, float ax, float ay,float az) { rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }
#endif // CAMERA3D_H
