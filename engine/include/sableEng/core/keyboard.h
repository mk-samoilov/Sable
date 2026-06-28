#pragma once

#include "sableEng/utils/singleton.h"

#include <vector>
#include <string>
#include <functional>
#include <algorithm>

namespace Core
{
    struct Keybind {
        std::string Name;
        int Key;
        std::function<void()> Func;
    };

    class Keyboard : public Utils::Singleton<Keyboard>
    {
        public:
            void Update();

            void Bind(std::string bindName, int key, std::function<void()> func) {
                Keybind bind{};
                bind.Name = bindName;
                bind.Key = key;
                bind.Func = func;
                Binds.push_back(bind);
            }

            void CleanBind(const std::string& bindName) {
                std::erase_if(Binds, [&bindName](const Keybind& x) {
                    return x.Name == bindName;
                });
            }

            std::vector<Keybind> GetKeyBinds() { return Binds; }

        private:
            std::vector<Keybind> Binds;
    };
}
