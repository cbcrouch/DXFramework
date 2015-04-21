//
// File:     CameraDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "CameraDXF.h"


namespace DXF {

    //
    // ViewVolume class
    //

    ViewVolume::ViewVolume() : ViewVolume(DXF_CONSTANTS::verticalFOV, DXF_CONSTANTS::aspectRatio,
        DXF_CONSTANTS::nearPlane, DXF_CONSTANTS::farPlane) { }

    ViewVolume::ViewVolume(real32_t verticalFOV, real32_t aspectRatio, real32_t nearPlane, real32_t farPlane) {
        setShape(verticalFOV, aspectRatio, nearPlane, farPlane);
    }

    ViewVolume::ViewVolume(const ViewVolume& viewVolume) {
        *this = viewVolume;
    }

    ViewVolume& ViewVolume::operator=(const ViewVolume& viewVolume) {
        assert(this != &viewVolume);
        m_verticalFOV = viewVolume.m_verticalFOV;
        m_horizontalFOV = viewVolume.m_horizontalFOV;
        m_aspectRatio = viewVolume.m_aspectRatio;
        m_nearPlane = viewVolume.m_nearPlane;
        m_farPlane = viewVolume.m_farPlane;
        m_projection = viewVolume.m_projection;
        return *this;
    }

    XMMATRIX ViewVolume::getProjection() const {
        return m_projection;
    }

    void ViewVolume::setShape(real32_t verticalFOV, real32_t aspectRatio, real32_t nearPlane, real32_t farPlane) {
        m_verticalFOV = verticalFOV;
        m_horizontalFOV = atanf(tanf(verticalFOV / 2.0f) * aspectRatio);
        m_aspectRatio = aspectRatio;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        m_projection = XMMatrixPerspectiveFovLH(m_verticalFOV, m_aspectRatio, m_nearPlane, m_farPlane);
    }

    //
    // Camera class
    //

    Camera::Camera() : Camera(DXF_CONSTANTS::position, DXF_CONSTANTS::look, DXF_CONSTANTS::up) { }

    Camera::Camera(XMVECTOR position, XMVECTOR look, XMVECTOR up) {
        setPose(position, look, up);
    }

    Camera::Camera(const Camera& cam) {
        *this = cam;
    }

    Camera& Camera::operator=(const Camera& cam) {
        assert(this != &cam);
        m_position = cam.m_position;
        m_look = cam.m_look;
        m_up = cam.m_up;

        m_yaw = cam.m_yaw;
        m_pitch = cam.m_pitch;
        //m_roll = cam.m_roll;

        //
        // TODO: translation speed should be in world units per second and scaled to the
        //       seconds elapse parameter
        //
        m_translationSpeed = { 0.025f, 0.0f, 0.025f };

        m_translationFlags = cam.m_translationFlags;
        m_rotationFlags = cam.m_rotationFlags;

        m_viewMatrix = cam.m_viewMatrix;
        m_viewVolume = cam.m_viewVolume;
        return *this;
    }

    //
    // TODO: consider inlining getters
    //

    XMVECTOR Camera::getPosition() const {
        return m_position;
    }

    XMVECTOR Camera::getLook() const {
        return m_look;
    }

    XMVECTOR Camera::getUp() const {
        return m_up;
    }

    real32_t Camera::getYaw() const {
        return m_yaw;
    }

    real32_t Camera::getPitch() const {
        return m_pitch;
    }

    XMMATRIX Camera::getViewMatrix() const {
        return m_viewMatrix;
    }

    ViewVolume Camera::getViewVolume() const {
        return m_viewVolume;
    }

    void Camera::setLook(real32_t yaw, real32_t pitch) {
        m_yaw = yaw;
        m_pitch = pitch;

        real32_t r = cosf(pitch);
        m_look = XMVectorSet(r*sinf(yaw), sinf(pitch), r*cosf(yaw), 0.0f);

        XMVECTOR right = XMVectorSet(sinf(yaw - XM_PIDIV2), 0.0f, cosf(yaw - XM_PIDIV2), 0.0f);
        m_up = XMVector2Cross(right, m_look);

        //
        // TODO: could try an alternate (potentially faster) implementation using a fixed up
        //       vector and rotating the flat yaw vector around cross(look, up) i.e.
        //
        // up = (0, 1, 0)
        // look = (cos(yaw), 0, sin(yaw))
        // pitchMatrix = rotate(matrix, cross(look, up), pitch)
        // look = pitchMatrix * look;

        updateViewMatrix();
    }

    void Camera::setPose(XMVECTOR position, XMVECTOR look, XMVECTOR up) {
        m_position = position;
        m_look = look;
        m_up = up;

        XMVECTOR hyp = XMVector4Normalize(XMVectorSubtract(position, look));
        m_yaw = -XM_PI + atan2f(XMVectorGetX(hyp), XMVectorGetY(hyp));
        m_pitch = -atan2f(XMVectorGetY(hyp), XMVectorGetY(hyp)) / 2.0f;

        //m_yaw = XM_PI;    // look to -Z
        //m_yaw = -XM_PI;   // look to -Z
        //m_yaw = 0.0f;     // look to +Z

        //m_pitch = XM_PIDIV2 - 0.01f;   // look straight up
        //m_pitch = -XM_PIDIV2 + 0.01f;  // look straight down
        //m_pitch = 0.0f;                // look at horizon

        updateViewMatrix();
    }

    inline void Camera::setTranslationState(CameraTranslationState state) {
        m_translationFlags |= static_cast<uint32_t>(state);
    }

    inline void Camera::clearTranslationState(CameraTranslationState state) {
        m_translationFlags &= ~(static_cast<uint32_t>(state));
    }

    inline void Camera::setRotationState(CameraRotationState state) {
        m_rotationFlags |= static_cast<uint32_t>(state);
    }

    inline void Camera::clearRotationState(CameraRotationState state) {
        m_rotationFlags &= ~(static_cast<uint32_t>(state));
    }

    void Camera::step(real32_t secsElapsed) {
        bool updated = false;

        if (m_translationFlags & static_cast<uint32_t>(CameraTranslationState::FOWARD)) {
            XMVECTOR translationVec = XMVector4Normalize(XMVectorSubtract(m_position, m_look));
            //
            // TODO: determine that operator* is overloaded for a scalar multiply and implement translation speed vector
            //
            m_position = XMVectorSubtract(m_position, m_translationSpeed.z * translationVec);
            updated = true;
        }

        if (m_translationFlags & static_cast<uint32_t>(CameraTranslationState::BACKWARD)) {
            XMVECTOR translationVec = XMVector4Normalize(XMVectorSubtract(m_position, m_look));
            m_position = XMVectorAdd(m_position, m_translationSpeed.z * translationVec);
            updated = true;
        }

        if (m_translationFlags & static_cast<uint32_t>(CameraTranslationState::LEFT)) {
            XMVECTOR side = XMVector2Cross(XMVectorSubtract(m_position, m_look), m_up);
            XMVECTOR translationVec = XMVector4Normalize(side);
            m_position = XMVectorAdd(m_position, m_translationSpeed.x * translationVec);
            updated = true;
        }

        if (m_translationFlags & static_cast<uint32_t>(CameraTranslationState::RIGHT)) {
            XMVECTOR side = XMVector2Cross(XMVectorSubtract(m_position, m_look), m_up);
            XMVECTOR translationVec = XMVector4Normalize(side);
            m_position = XMVectorSubtract(m_position, m_translationSpeed.x * translationVec);
            updated = true;
        }

        if (updated) {
            setLook(m_yaw, m_pitch);
        }
    }

    void Camera::resetLookDirection() {
        setLook(m_cachedYaw, m_cachedPitch);
    }

    void Camera::resetState() {
        m_position = m_cachedPosition;
        setLook(m_cachedYaw, m_cachedPitch);
    }

    void Camera::saveState() {
        m_cachedPosition = m_position;
        m_cachedYaw = m_yaw;
        m_cachedPitch = m_pitch;
    }

    void Camera::updateViewMatrix() {
        m_viewMatrix = XMMatrixLookAtLH(m_position, m_look, m_up);
    }
};
