#include "transform3d.h"



const QMatrix4x4 &Transform3D::toMatrix()
{
    if (m_dirty)
    {
      m_dirty = false;
      m_world.setToIdentity();
      m_world.translate(m_translation);
      m_world.rotate(m_rotation);
      m_world.scale(m_scale);
    }
    return m_world;
}
