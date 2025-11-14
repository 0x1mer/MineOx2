#include "PathProvider.h"
#include <iostream>

PathProvider &PathProvider::Instance()
{
    static PathProvider instance;
    return instance;
}

PathProvider::PathProvider()
{
    // Root folder: %APPDATA%/.mineox
    m_root = std::filesystem::path(std::getenv("APPDATA")) / ".mineox";

    m_folders = {
        {Folders::Logs, m_root / "logs"},
        {Folders::Data, m_root / "data"},
        {Folders::Config, m_root / "config"},
        {Folders::Saves, m_root / "saves"},
        {Folders::Screenshots, m_root / "screenshots"}};

    m_folders[Folders::BlockStates] = m_folders[Folders::Data] / "blocks" / "block_states";
    m_folders[Folders::BlockModels] = m_folders[Folders::Data] / "blocks" / "block_models";
    m_folders[Folders::BlockDefinitions] = m_folders[Folders::Data] / "blocks" / "block_definitions";
    m_folders[Folders::Textures] = m_folders[Folders::Data] / "textures";
    m_folders[Folders::BlockTextures] = m_folders[Folders::Data] / "textures" / "blocks";

    CreateDirectories();
}

std::filesystem::path PathProvider::GetRootFolder() const
{
    return m_root;
}

std::filesystem::path PathProvider::GetPath(Folders folder) const
{
    return m_folders.at(folder);
}

void PathProvider::CreateDirectories()
{
    try
    {
        if (!std::filesystem::exists(m_root))
            std::filesystem::create_directories(m_root);

        for (auto &[_, path] : m_folders)
        {
            if (!std::filesystem::exists(path))
                std::filesystem::create_directories(path);
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Ошибка при создании директорий: " << e.what() << '\n';
    }
}