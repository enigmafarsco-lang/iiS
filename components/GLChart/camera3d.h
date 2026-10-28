// ============================================================================
// File: camera3d.h
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for camera3d.h.
// ============================================================================

#ifndef CAMERA3D_H
#define CAMERA3D_H
#include <QMatrix4x4>

class Camera3D
{
public:
    // Constructor: Initializes Camera3D subsystem components, default parameters, and state.
    Camera3D();

    // Executes to matrix routine.
    const QMatrix4x4& toMatrix();
    // Executes rotate routine.
    void rotate(const QQuaternion &dr);
    // Executes rotate routine.
    void rotate(float angle, const QVector3D &axis);
    // Executes rotate routine.
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
