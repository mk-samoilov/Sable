#pragma once

#include "sableEng/utils/defines.h"

#include <glm/glm.hpp>
#include <atomic>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace GameObjects
{
    struct Info
    {
        glm::vec3   Position = glm::vec3(0.0f);
        std::string Material;
        std::string Vertex;
        std::string Fragment;
        std::string Mesh;
        std::string ParsedID;

        bool IsLight = false;
        bool IsModel = false;
    };

    enum Type
    {
        CAMERA = 0,
        SPRITE,
        SIZE
    };

    class Object
    {
        public:
            Object() { UniqueID = IDCounter++; }
            virtual ~Object() {}

            virtual Type GetType() const { return SIZE; }

            virtual const std::string& GetMaterialName() const { return EMPTY_STRING; }
            virtual const std::string& GetMeshName() const { return EMPTY_STRING; }
            virtual const std::string& GetParsedID() const { return EMPTY_STRING; }
            virtual glm::vec3 GetPosition() const { return glm::vec3(0.0f); }

            virtual void Update() {}

            uint32_t GetID() const { return UniqueID; }
            void ResetCounterID() { IDCounter = 1; }

        private:
            std::string EMPTY_STRING = "";
            uint32_t UniqueID = 1;
            inline static std::atomic_uint32_t IDCounter = 1;
    };

    typedef std::map<int, std::vector<Object*>> GameObjectsMap;

    class Keeper
    {
        public:
            Keeper() {}
            ~Keeper();

            template<typename T>
            void Create(T* obj)
            {
                ASSERT(obj->GetType() == SIZE, "GameObjects::Keeper::Create(): child has no GetType() defined");
                ObjectsList[obj->GetType()].push_back(obj);
                Added.push_back(obj);
            }
            void Create(const std::vector<Info>& infos);
            void Erase(int id);

            bool Find(Type type) const;
            bool IsValid(Type type);
            std::vector<Object*>& Get(Type type);
            const GameObjectsMap& GetObjects() const { return ObjectsList; }
            const Object* GetObject(Type type, int id) const;
            Object* GetNotConstObject(Type type, int id);
            Object* GetLast(Type type);
            size_t GetObjectsSize() const;

            void SetSelectedID(uint32_t id) { SelectedID = id; }
            uint32_t GetSelectedID() const { return SelectedID; }
            bool EraseSelected();

            void Update();
            void CleanIfNot(Type type, bool resetID = false);

            const std::vector<Object*>& GetAdded() const { return Added; }
            const std::vector<int>& GetRemoved() const { return Removed; }
            void ClearPending() { Added.clear(); Removed.clear(); }

        private:
            GameObjectsMap ObjectsList;
            uint32_t SelectedID = 0;

            std::vector<Object*> Added;
            std::vector<int> Removed;
    };
}
