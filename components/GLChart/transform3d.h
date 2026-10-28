/**
 * @file transform3d.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H
#include <QMatrix4x4>

/**
 * @class Transform3D
 * @brief High-level controller class for Transform3D.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class Transform3D
{
public:
    /**
     * @brief Constructs a new Transform3D instance and initializes subsystem components and UI state.
     */
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
