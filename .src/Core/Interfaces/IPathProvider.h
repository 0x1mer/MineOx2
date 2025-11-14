#pragma once

#include <filesystem>

enum class Folders {
    Logs,
    Data,
    Config,
    Saves,
    Screenshots,
    BlockStates,
    BlockModels,
    BlockDefinitions,
    Textures,
    BlockTextures
};

class IPathProvider {
public:
    virtual ~IPathProvider() = default;

    virtual std::filesystem::path GetRootFolder() const = 0;
    virtual std::filesystem::path GetPath(Folders folder) const = 0;
};