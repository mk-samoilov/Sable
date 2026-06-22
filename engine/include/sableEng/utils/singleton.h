#pragma once

namespace Utils
{
    template <typename T>
    class Singleton
    {
        protected:
            Singleton() {}

        public:
            Singleton(const Singleton&) = delete;
            Singleton& operator=(const Singleton&) = delete;

            static T* GetInstance() { static T Instance; return &Instance; }
    };
}
