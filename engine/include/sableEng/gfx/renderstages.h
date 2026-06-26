#pragma once

#include "sableEng/gfx/renderhelpers.h"
#include "sableEng/gameobjects/gameobjects.h"

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace Stages
{
    enum QueueType { RQ_GENERAL };

    typedef std::vector<Gfx::RenderObject>      RenderQueueVec;
    typedef std::map<QueueType, RenderQueueVec> RenderQueueMap;

    class Stage
    {
        public:
            Stage() {}
            Stage(const std::string& tag) : Tag(tag) {}

            const std::string& GetTag() const { return Tag; }
            void SetTag(const std::string& tag) { Tag = tag; }

            void AddRQ(QueueType type, const Gfx::RenderObject& obj) { RenderQueue[type].push_back(obj); }
            RenderQueueVec& GetRenderQueue(QueueType type) { return RenderQueue[type]; }
            RenderQueueMap& GetRenderQueueMap() { return RenderQueue; }

        private:
            std::string    Tag;
            RenderQueueMap RenderQueue;
    };

    enum Update { RQ, MATERIALS };

    typedef std::unordered_map<std::string, Stage> StagesMap;

    class Stages
    {
        public:
            Stages() {}
            Stages(GameObjects::Keeper* keeper) : Keep(keeper) {}

            void Clear() { SceneStages.clear(); }

            void Populate(const std::string& key, std::function<bool(GameObjects::Type)> skip);
            Gfx::RenderObject LoadResources(const GameObjects::Objects* obj);

            void Sync();
            void UpdateRQ();
            void UpdateMaterials();
            void Update(uint32_t type);

            Stage& Get(const std::string& key) { return SceneStages[key]; }
            bool Find(const std::string& key) const { return SceneStages.find(key) != SceneStages.end(); }
            StagesMap& GetSceneStages() { return SceneStages; }

        private:
            StagesMap SceneStages;
            GameObjects::Keeper* Keep = nullptr;
    };
}
