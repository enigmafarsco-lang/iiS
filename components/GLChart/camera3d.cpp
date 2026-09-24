#include "camera3d.h"



const QMatrix4x4 &Camera3D::toMatrix()
{
    if (m_dirty)
      {
        m_dirty = false;
        m_world.setToIdentity();
        m_world.rotate(m_rotation.conjugate());
        m_world.translate(-m_translation);
      }
    return m_world;
}

void Camera3D::rotate(const QQuaternion &dr)
{
  m_dirty = true;
  m_rotation = dr * m_rotation;
}
