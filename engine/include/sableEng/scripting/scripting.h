#pragma once

#include "sableEng/utils/defines.h"

#include <sol/sol.hpp>
#include <string>

namespace Scripting
{
    enum ScriptState {
        NOT_LOADED = 0,
        CRASHED,
        LOADING,
        RUNNING,
        RELOADING,
    };

    struct script {
        std::string name;
        ScriptState state;
    };

    class ScriptManager : public Utils::Singleton<ScriptManager>
    {
        public:
            void Init();

            void RunScript(const std::string& name);
            void LoadScript(const std::string& name);
            void ReloadScript(const std::string& name);
            void UnloadScript(const std::string& name);

            ScriptState GetScriptState(const std::string& name);

        private:
            void InitSol2();
            void InitLuaErrorHandling();

            void HandleLuaError(const std::string& script_name, const std::string& message);
            static std::string ExtractScriptName(const std::string& msg, const std::string& fallback);
            void SetScriptState(const std::string& name, ScriptState state);

            sol::state Lua;
            std::vector<script> Scripts;
    };
}
