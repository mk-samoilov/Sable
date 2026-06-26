#include "sableEng/gameobjects/objects/sprite.h"

namespace GameObjects
{
    Sprite::Sprite(const Info& info)
    {
        Position = info.Position;
        MaterialName = info.Material;
        MeshName = info.Mesh;
        ParsedID = info.ParsedID;
    }
}
