//
// File:     CameraDXF.h
// Project:  DXFramework
//
// Copyright (c) 2017 Casey Crouch. All rights reserved.
//

#include "DXFCommon.h"
#include "UtilsDXF.h"

namespace DXF_CONSTANTS {
    // view voluem defaults
    static const float32_t verticalFOV = XM_PIDIV4;
    static const float32_t aspectRatio = 1920.0f / 1080.0f;
    static const float32_t nearPlane = 2.0f;
    static const float32_t farPlane = 1000.0f;

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
        ViewVolume(float32_t verticalFOV, float32_t aspectRatio, float32_t nearPlane, float32_t farPlane);
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
        void setNearPlane(float32_t nearPlane);
        void setFarPlane(float32_t farPlane);
        void setVerticalFOV(float32_t verticalFOV);
        void setAspectRatio(float32_t aspectRatio);
        void setShape(float32_t verticalFOV, float32_t aspectRatio, float32_t nearPlane, float32_t farPlane);

    protected:
        float32_t m_verticalFOV;
        float32_t m_aspectRatio;
        float32_t m_nearPlane;
        float32_t m_farPlane;
        XMMATRIX m_projection;
    };

    class Camera {
    public:
        enum class TranslationState : uint32_t {
            FOWARD   = 0x01u,
            BACKWARD = 0x01u << 1,
            RIGHT    = 0x01u << 2,
            LEFT     = 0x01u << 3,
            UP       = 0x01u << 4,
            DOWN     = 0x01u << 5
        };

        enum class RotationState : uint32_t {
            ROLL  = 0x01u,
            PITCH = 0x01u << 1,
            YAW   = 0x01u << 2
        };

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

        float32_t getYaw() const;
        float32_t getPitch() const;

        //
        // TODO: implement roll
        //
        //float32_t getRoll() const;

        XMMATRIX getViewMatrix() const;
        ViewVolume getViewVolume() const;

        // setters
        void setLook(float32_t yaw, float32_t pitch);
        void setPose(XMVECTOR position, XMVECTOR look, XMVECTOR up);

        void setTranslationState(TranslationState state);
        void clearTranslationState(TranslationState state);

        void setRotationState(RotationState state);
        void clearRotationState(RotationState state);

        // methods
        void step(float32_t secsElapsed);
        void resetLookDirection();
        void resetState();
        void saveState();

    protected:
        void updateViewMatrix();

        XMVECTOR m_position;
        XMVECTOR m_look;
        XMVECTOR m_up;

        float32_t m_yaw;
        float32_t m_pitch;

        //
        // TODO: implement roll
        //
        //float32_t m_roll;

        //
        // TODO: implement functions for setting the translation and rotation speed
        //
        XMFLOAT3 m_translationSpeed;
        XMFLOAT3 m_rotationSpeed;

        XMVECTOR m_cachedPosition;
        float32_t m_cachedYaw;
        float32_t m_cachedPitch;

        uint32_t m_translationFlags;
        uint32_t m_rotationFlags;

        XMMATRIX m_viewMatrix;
        ViewVolume m_viewVolume;
    };

};
