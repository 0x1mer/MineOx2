#include "BlockJsonDataCache.h"

#include <filesystem>
#include <sstream>

// Implementation of non-template member functions

BlockJsonDataCache::BlockJsonDataCache(ILogger &logger, const IPathProvider &paths)
    : logger_(logger), paths_(paths)
{
    Init();
}

BlockJsonDataCache &BlockJsonDataCache::Instance(ILogger &logger, const IPathProvider &paths)
{
    static BlockJsonDataCache instance(logger, paths);
    return instance;
}

std::optional<std::reference_wrapper<const BlockJsonData>>
BlockJsonDataCache::Get(const std::string &name) const
{
    if (auto it = cache_.find(name); it != cache_.end())
        return std::cref(it->second);

    return std::nullopt;
}

std::optional<std::reference_wrapper<const BlockJsonData>>
BlockJsonDataCache::Get(BlockType type) const
{
    return Get(ToString(type));
}

size_t BlockJsonDataCache::GetLoadedCount() const noexcept
{
    return cache_.size();
}

void BlockJsonDataCache::LoadAllStates()
{
    namespace fs = std::filesystem;
    for (const auto &entry : fs::directory_iterator(blockStatesFolder))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".json")
            continue;

        std::string name = entry.path().stem().string();
        cache_[name].state = LoadJsonFile<BlockState>(entry.path());
    }
}

void BlockJsonDataCache::LoadAllDefinitions()
{
    namespace fs = std::filesystem;
    for (const auto &entry : fs::directory_iterator(blockDefinitionsFolder))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".json")
            continue;

        std::string name = entry.path().stem().string();
        cache_[name].definition = LoadJsonFile<BlockDefinition>(entry.path());
    }
}

void BlockJsonDataCache::LoadAllModels()
{
    namespace fs = std::filesystem;
    for (const auto &entry : fs::directory_iterator(blockModelsFolder))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".json")
            continue;

        std::string filename = entry.path().stem().string();
        auto underscorePos = filename.find('_');
        if (underscorePos == std::string::npos)
            continue;

        std::string blockName = filename.substr(0, underscorePos);
        cache_[blockName].models[filename] = LoadJsonFile<BlockModel>(entry.path());
    }
}

void BlockJsonDataCache::LoadAllBlocks()
{
    LoadAllStates();
    LoadAllDefinitions();
    LoadAllModels();
}

void BlockJsonDataCache::ValidateAllBlocks() const
{
    bool hasError = false;

    for (auto type : AllBlockTypes())
    {
        std::string blockName = ToString(type);
        if (blockName == "unknown" || blockName == "air")
            continue;

        auto it = cache_.find(blockName);
        if (it == cache_.end())
        {
            logger_.Error("Missing BlockJsonData for: " + blockName);
            hasError = true;
            continue;
        }

        const BlockJsonData &data = it->second;
        bool ok = true;

        // --- State validation ---
        if (!data.state.wasLoaded)
        {
            logger_.Error("Missing BlockState file for: " + blockName);
            ok = false;
        }
        else
        {
            try
            {
                Validate(data.state);
            }
            catch (const std::exception &e)
            {
                logger_.Error("Invalid BlockState for " + blockName + ": " + e.what());
                ok = false;
            }
        }

        // --- Definition validation ---
        if (!data.definition.wasLoaded)
        {
            logger_.Error("Missing BlockDefinition file for: " + blockName);
            ok = false;
        }
        else
        {
            try
            {
                Validate(data.definition);
            }
            catch (const std::exception &e)
            {
                logger_.Error("Invalid BlockDefinition for " + blockName + ": " + e.what());
                ok = false;
            }
        }

        // --- Models validation ---
        if (data.models.empty())
        {
            logger_.Error("Block " + blockName + " has no models");
            ok = false;
        }
        else
        {
            for (const auto &[modelName, model] : data.models)
            {
                if (!model.wasLoaded)
                {
                    logger_.Error("Missing model file for " + modelName);
                    ok = false;
                    continue;
                }
                try
                {
                    Validate(model);
                }
                catch (const std::exception &e)
                {
                    logger_.Error("Invalid BlockModel " + modelName + " for block " + blockName + ": " + e.what());
                    ok = false;
                }
            }
        }

        if (!ok)
            hasError = true;
    }

    if (hasError)
        throw std::runtime_error("BlockJsonDataCache validation failed: missing or invalid data detected.");
}

void BlockJsonDataCache::Init()
{
    blockStatesFolder = paths_.GetPath(Folders::BlockStates);
    blockModelsFolder = paths_.GetPath(Folders::BlockModels);
    blockDefinitionsFolder = paths_.GetPath(Folders::BlockDefinitions);

    LoadAllBlocks();
    ValidateAllBlocks();
}