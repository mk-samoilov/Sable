#include <sableEng/engine.h>

void mainLoop(float dt) {

}

int main() {
    try {
        Engine::GetInstance()->Init();
        Engine::GetInstance()->Run(mainLoop);
    } catch (const std::exception& e) { std::cerr << e.what() << "\n"; return 1; }
}
