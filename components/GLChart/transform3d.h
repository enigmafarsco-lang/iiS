#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H
#include <QMatrix4x4>

class Transform3D
{
public:
    Transform3D();

    // Accessors
    const QMatrix4x4& toMatrix();

private:
    bool              m_dirty;
    QMatrix4x4        m_world;
    QVector3D         m_translation;
    QQuaternion       m_rotation;
    QVector3D         m_scale;
};
inline Transform3D::Transform3D() : m_dirty(true), m_scale(1.0f, 1.0f, 1.0f) {}
#endif // TRANSFORM3D_H
