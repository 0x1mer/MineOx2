#pragma once

#include "IPathProvider.h"
#include <map>
#include <filesystem>

class PathProvider : public IPathProvider {
public:
    static PathProvider& Instance();

    std::filesystem::path GetRootFolder() const override;
    std::filesystem::path GetPath(Folders folder) const override;

private:
    PathProvider();
    void CreateDirectories();

    PathProvider(const PathProvider&) = delete;
    PathProvider& operator=(const PathProvider&) = delete;

private:
    std::filesystem::path m_root;
    std::map<Folders, std::filesystem::path> m_folders;
};