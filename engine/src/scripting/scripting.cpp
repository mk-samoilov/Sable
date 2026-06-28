#include "sableEng/scripting/scripting.h"

namespace Scripting
{
    void ScriptManager::Init()
    {
        InitSol2();
        InitLuaErrorHandling();
    }

    void ScriptManager::HandleLuaError(const std::string& script_name, const std::string& message)
    {
        std::cerr << "Lua Error: '" << script_name << "' Has been crashed with error:" << "\n" << message << "\n";
        SetScriptState(script_name, ScriptState::CRASHED);
    }

    void ScriptManager::InitSol2()
    {
        Lua.open_libraries(sol::lib::base, sol::lib::package,
                           sol::lib::string, sol::lib::debug);

        std::string scripts_dir = "scripts/";
        std::string current_path = Lua["package"]["path"];
        Lua["package"]["path"] = scripts_dir + "?.lua;" 
                               + scripts_dir + "?/init.lua;" 
                               + current_path;
    }

    void ScriptManager::InitLuaErrorHandling()
    {
        sol::protected_function::set_default_handler(Lua["debug"]["traceback"]);

        Lua.set_panic(+[](lua_State* L) -> int {
            const char* msg = lua_tostring(L, -1);
            Scripting::ScriptManager::GetInstance()->HandleLuaError("unknown", msg ? msg : "unknown");
            return 0;
        });
    }

    void ScriptManager::RunScript(const std::string& name)
    {

    }

    void ScriptManager::LoadScript(const std::string& name)
    {

    }

    void ScriptManager::ReloadScript(const std::string& name)
    {

    }

    void ScriptManager::UnloadScript(const std::string& name)
    {

    }

    static std::string ExtractScriptName(const std::string& msg, const std::string& fallback)
    {
        auto colon = msg.find(":", 2);
        if (colon == std::string::npos) return fallback;
        return msg.substr(0, colon);
    }

    void ScriptManager::SetScriptState(const std::string& name, ScriptState state)
    {
        for (auto s : Scripts) {
            if (s.name == name) {
                s.state = state;
            }
        }
    }

    ScriptState ScriptManager::GetScriptState(const std::string& name)
    {
        for (const auto& s : Scripts) {
            if (s.name == name) {
                return s.state;
            }
        }
        return ScriptState::NOT_LOADED;
    }
}
