//
// File:     CameraDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "DXFCommon.h"
#include "UtilsDXF.h"

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
        void setShape(real32_t verticalFOV, real32_t aspectRatio, real32_t nearPlane, real32_t farPlane);

    protected:
        real32_t m_verticalFOV;
        real32_t m_horizontalFOV;
        real32_t m_aspectRatio;
        real32_t m_nearPlane;
        real32_t m_farPlane;
        XMMATRIX m_projection;
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
        XMVECTOR getPosition();
        XMVECTOR getLook();
        XMVECTOR getUp();
        real32_t getYaw();
        real32_t getPitch();
        XMMATRIX getViewMatrix();

        // setters
        void setLook(real32_t yaw, real32_t pitch);
        //void setTranslationState(CAMERA_STATE state);


        // behaviors
        // homomorphisms
        // morphisms

        void step(real32_t secsElapsed);

        void resetLookDirection();
        void resetState();
        void saveState();

    protected:
        XMVECTOR m_position;
        XMVECTOR m_look;
        XMVECTOR m_up;

        real32_t m_yaw;
        real32_t m_pitch;

        XMMATRIX m_viewMatrix;
        ViewVolume m_viewVolume;
    };

};
