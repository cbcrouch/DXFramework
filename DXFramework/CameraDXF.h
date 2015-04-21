//
// File:     CameraDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "DXFCommon.h"
#include "UtilsDXF.h"

namespace DXF_CONSTANTS {
    // view voluem defaults
    static const real32_t verticalFOV = XM_PIDIV4;
    static const real32_t aspectRatio = 1920.0f / 1080.0f;
    static const real32_t nearPlane = 2.0f;
    static const real32_t farPlane = 1000.0f;

    // camera defaults
    static const XMVECTOR position = XMVectorSet(4.0f, 2.0f, 4.0f, 0.0f);
    static const XMVECTOR look = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    static const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    //
    // TODO: defaults for translation and rotation speed
    //
};

namespace DXF {

    class ViewVolume {
    public:
        ViewVolume();
        ViewVolume(real32_t verticalFOV, real32_t aspectRatio, real32_t nearPlane, real32_t farPlane);
        ~ViewVolume() = default;

        // copy ctor and assignment
        ViewVolume(const ViewVolume& viewVolume);
        ViewVolume& operator=(const ViewVolume& viewVolume);

        // move ctor and assignment
        ViewVolume(ViewVolume&&) = delete;
        ViewVolume& operator=(ViewVolume&&) = delete;

        // getters
        XMMATRIX getProjection() const;

        // setters

        //
        // TODO: functions for setting near plane, far plane, and aspect ratio
        //

        //void setNearPlane(real32_t nearPlane);
        //void setFarPlane(real32_t farPlane);
        //void setAspectRatio(real32_t aspectRatio);

        void setShape(real32_t verticalFOV, real32_t aspectRatio, real32_t nearPlane, real32_t farPlane);

    protected:
        real32_t m_verticalFOV;
        real32_t m_horizontalFOV;
        real32_t m_aspectRatio;
        real32_t m_nearPlane;
        real32_t m_farPlane;
        XMMATRIX m_projection;
    };

    
    enum class CameraTranslationState : uint32_t {
        FOWARD   = 0x01u,
        BACKWARD = 0x01u << 1,
        RIGHT    = 0x01u << 2,
        LEFT     = 0x01u << 3,
        UP       = 0x01u << 4,
        DOWN     = 0x01u << 5
    };

    enum class CameraRotationState : uint32_t {
        ROLL  = 0x01u,
        PITCH = 0x01u << 1,
        YAW   = 0x01u << 2
    };

    class Camera {
    public:
        Camera();
        Camera(XMVECTOR position, XMVECTOR look, XMVECTOR up);
        ~Camera() = default;

        // copy ctor and assignment
        Camera(const Camera& cam);
        Camera& operator=(const Camera& cam);

        // move ctor and assignment
        Camera(Camera&&) = delete;
        Camera& operator=(Camera&&) = delete;

        // getters
        XMVECTOR getPosition() const;
        XMVECTOR getLook() const;
        XMVECTOR getUp() const;

        real32_t getYaw() const;
        real32_t getPitch() const;

        //
        // TODO: implement roll
        //
        //real32_t getRoll() const;

        XMMATRIX getViewMatrix() const;
        ViewVolume getViewVolume() const;

        // setters
        void setLook(real32_t yaw, real32_t pitch);
        void setPose(XMVECTOR position, XMVECTOR look, XMVECTOR up);

        void setTranslationState(CameraTranslationState state);
        void clearTranslationState(CameraTranslationState state);

        void setRotationState(CameraRotationState state);
        void clearRotationState(CameraRotationState state);

        // methods
        void step(real32_t secsElapsed);
        void resetLookDirection();
        void resetState();
        void saveState();

    protected:
        void updateViewMatrix();

        XMVECTOR m_position;
        XMVECTOR m_look;
        XMVECTOR m_up;

        real32_t m_yaw;
        real32_t m_pitch;

        //
        // TODO: implement roll
        //
        //real32_t m_roll;

        //
        // TODO: implement functions for setting the translation and rotation speed
        //
        XMFLOAT3 m_translationSpeed;
        XMFLOAT3 m_rotationSpeed;

        XMVECTOR m_cachedPosition;
        real32_t m_cachedYaw;
        real32_t m_cachedPitch;

        uint32_t m_translationFlags;
        uint32_t m_rotationFlags;

        XMMATRIX m_viewMatrix;
        ViewVolume m_viewVolume;
    };

};
