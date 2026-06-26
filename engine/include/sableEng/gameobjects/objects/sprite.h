#pragma once

#include "sableEng/gameobjects/gameobjects.h"

namespace GameObjects
{
    class Sprite : public Objects
    {
        public:
            Sprite() {}
            Sprite(const Info& info);

            ~Sprite() {}

            Type GetType() const override { return ObjectType; }

            const std::string& GetMaterialName() const override { return MaterialName; }
            const std::string& GetMeshName() const override { return MeshName; }
            const std::string& GetParsedID() const override { return ParsedID; }
            glm::vec3 GetPosition() const override { return Position; }

            void Update() override {}

        private:
            Type ObjectType = Type::SPRITE;

            glm::vec3   Position = glm::vec3(0.0f);
            std::string MaterialName;
            std::string MeshName;
            std::string ParsedID;
    };
}
