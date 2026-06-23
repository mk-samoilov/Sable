#include "sableEng/core/assetshelper.h"
#include "sableEng/utils/defines.h"

#include <vector>
#include <fstream>

namespace AssetsHelper {
    std::vector<char> ReadFile(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        ASSERT(!file.is_open(), "failed to open file!");

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
}
