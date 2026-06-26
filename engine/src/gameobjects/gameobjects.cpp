#include "sableEng/gameobjects/gameobjects.h"
#include "sableEng/gameobjects/objects/sprite.h"

#include <algorithm>

namespace GameObjects
{
    Keeper::~Keeper()
    {
        for (auto& it : ObjectsList) {
            for (auto* obj : it.second) {
                delete obj;
            }
        }
        ObjectsList.clear();
    }

    void Keeper::Create(const std::vector<Info>& infos)
    {
        for (const Info& info : infos) {
            Create<Sprite>(new Sprite(info));
        }
    }

    void Keeper::Erase(int id)
    {
        for (auto& it : ObjectsList) {
            auto& vec = it.second;
            for (auto vit = vec.begin(); vit != vec.end(); ) {
                if ((*vit)->GetID() == (uint32_t)id) {
                    delete *vit;
                    vit = vec.erase(vit);
                    Removed.push_back(id);
                } else {
                    ++vit;
                }
            }
        }
    }

    bool Keeper::Find(Type type) const
    {
        return ObjectsList.find(type) != ObjectsList.end();
    }

    bool Keeper::IsValid(Type type)
    {
        return !ObjectsList[type].empty();
    }

    std::vector<Objects*>& Keeper::Get(Type type)
    {
        return ObjectsList[type];
    }

    const Objects* Keeper::GetObject(Type type, int id) const
    {
        const auto& vec = ObjectsList.at(type);
        auto it = std::find_if(vec.begin(), vec.end(), [id](const Objects* o) { return o->GetID() == (uint32_t)id; });
        return it == vec.end() ? nullptr : *it;
    }

    Objects* Keeper::GetNotConstObject(Type type, int id)
    {
        auto& vec = ObjectsList[type];
        auto it = std::find_if(vec.begin(), vec.end(), [id](const Objects* o) { return o->GetID() == (uint32_t)id; });
        return it == vec.end() ? nullptr : *it;
    }

    Objects* Keeper::GetLast(Type type)
    {
        auto& vec = ObjectsList[type];
        return vec.empty() ? nullptr : vec.back();
    }

    size_t Keeper::GetObjectsSize() const
    {
        size_t size = 0;
        for (const auto& it : ObjectsList) {
            size += it.second.size();
        }
        return size;
    }

    bool Keeper::EraseSelected()
    {
        int id = SelectedID;
        bool erased = false;
        for (auto& it : ObjectsList) {
            auto& vec = it.second;
            for (auto vit = vec.begin(); vit != vec.end(); ) {
                if ((*vit)->GetID() == (uint32_t)id) {
                    delete *vit;
                    vit = vec.erase(vit);
                    Removed.push_back(id);
                    erased = true;
                } else {
                    ++vit;
                }
            }
        }
        return erased;
    }

    void Keeper::Update()
    {
        for (auto& it : ObjectsList) {
            for (Objects* obj : it.second) {
                obj->Update();
            }
        }
    }

    void Keeper::CleanIfNot(Type type, bool resetID)
    {
        for (auto it = ObjectsList.begin(); it != ObjectsList.end(); ) {
            if (it->first != type) {
                for (auto* obj : it->second) {
                    if (resetID) {
                        obj->ResetCounterID();
                    }
                    delete obj;
                }
                it = ObjectsList.erase(it);
            } else {
                ++it;
            }
        }
    }
}
