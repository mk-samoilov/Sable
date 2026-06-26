#pragma once

#include "sableEng/gameobjects/gameobjects.h"

namespace GameObjects
{
    // empty type for now, view/projection logic lands with the uniform-buffer step.
    class Camera : public Objects
    {
        public:
            Camera() {}
            Camera(const Info& info) { (void)info; }

            ~Camera() {}

            Type GetType() const override { return Type::CAMERA; }
    };
}
