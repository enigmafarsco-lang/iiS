/**
 * @file camera3d.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef CAMERA3D_H
#define CAMERA3D_H
#include <QMatrix4x4>

/**
 * @class Camera3D
 * @brief High-level controller class for Camera3D.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Camera3D
{
public:
    /**
     * @brief Constructs a new Camera3D instance and initializes subsystem components and UI state.
     */
    Camera3D();

    /**
     * @brief Executes the to matrix procedure for Camera3D.
     * @return Output value of type const QMatrix4x4&.
     */
    const QMatrix4x4& toMatrix();
    /**
     * @brief Executes the rotate procedure for Camera3D.
     * @param dr Input parameter specifying dr (const QQuaternion).
     */
    void rotate(const QQuaternion &dr);
    /**
     * @brief Executes the rotate procedure for Camera3D.
     * @param angle Input parameter specifying angle (float).
     * @param axis Input parameter specifying axis (const QVector3D).
     */
    void rotate(float angle, const QVector3D &axis);
    /**
     * @brief Executes the rotate procedure for Camera3D.
     * @param angle Input parameter specifying angle (float).
     * @param ax Input parameter specifying ax (float).
     * @param ay Input parameter specifying ay (float).
     * @param az Input parameter specifying az (float).
     */
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
