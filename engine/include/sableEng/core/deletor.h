#pragma once

#include <deque>
#include <functional>
#include <utility>

#include "sableEng/utils/singleton.h"

namespace Core
{
    class Deletor : public Utils::Singleton<Deletor>
    {
        public:
            enum Tag {
                NONE = 0,
                SWAPCHAIN,
                PIPELINE,
                COMMAND,
                SYNC,
            };

            void Push(int tag, std::function<void()>&& function)
            {
                Deletors.emplace_back(tag, std::move(function));
            }

            void CleanIf(int tag)
            {
                for (auto it = Deletors.rbegin(); it != Deletors.rend(); ++it) {
                    if (it->first == tag) {
                        it->second();
                    }
                }

                std::deque<DeletorPair> remaining;
                for (auto& entry : Deletors) {
                    if (entry.first != tag) {
                        remaining.push_back(std::move(entry));
                    }
                }
                Deletors = std::move(remaining);
            }

            void CleanAll()
            {
                for (auto it = Deletors.rbegin(); it != Deletors.rend(); ++it) {
                    it->second();
                }
                Deletors.clear();
            }

        private:
            using DeletorPair = std::pair<int, std::function<void()>>;

            std::deque<DeletorPair> Deletors;
    };
}
