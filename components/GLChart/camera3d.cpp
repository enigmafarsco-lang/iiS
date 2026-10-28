/**
 * @file camera3d.cpp
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

/**
 * @brief Executes the rotate procedure for Camera3D.
 * @param dr Input parameter specifying dr (const QQuaternion).
 */
void Camera3D::rotate(const QQuaternion &dr)
{
  m_dirty = true;
  m_rotation = dr * m_rotation;
}
