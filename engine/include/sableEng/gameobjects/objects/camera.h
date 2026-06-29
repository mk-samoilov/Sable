#pragma once

#include "sableEng/gameobjects/gameobjects.h"

namespace GameObjects
{
    class Camera : public Object
    {
        public:
            Camera() {}
            Camera(const Info& info) { (void)info; }

            ~Camera() {}

            Type GetType() const override { return Type::CAMERA; }
    };
}
