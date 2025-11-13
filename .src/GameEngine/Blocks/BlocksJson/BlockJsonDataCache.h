#pragma once

#include <unordered_map>
#include <stdexcept>
#include <optional>
#include <filesystem>
#include <sstream>
#include <format>

#include "BlockModelConfig.h"
#include "BlockStateConfig.h"
#include "BlockDefinitionConfig.h"
#include "Options.h"
#include "BlockTypes.h"

#include "IPathProvider.h"
#include "ILogger.h"

#include "IDebugPrintable.h"
#include "DebugColors.h"

/**
 * @brief Holds all JSON-parsed data related to a single block type.
 *
 * This structure aggregates all information about a block loaded from
 * its configuration JSON files, including its state, definition, and
 * associated models.
 *
 * @struct BlockJsonData
 * @see BlockState
 * @see BlockDefinition
 * @see BlockModel
 */
struct BlockJsonData : public IDebugPrintable
{
    /** @brief Describes the possible or current states of the block. */
    BlockState state;

    /** @brief Defines physical and visual properties of the block. */
    BlockDefinition definition;

    /**
     * @brief Maps model names to their corresponding block models.
     *
     * A single block may have multiple models representing different
     * orientations, variants, or states.
     *
     * Each model name must start with the block name followed by an underscore.
     * Example: `log_waterfilled`.
     */
    std::unordered_map<std::string, BlockModel> models;

    /// @copydoc IDebugPrintable::ToShortString
    std::string ToShortString() const override
    {
        return std::format("BlockJsonData(models={}, state={}, def={})",
                           models.size(),
                           state.ToShortString(),
                           definition.ToShortString());
    }

    /// @copydoc IDebugPrintable::ToPrettyString
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Bold << "BlockJsonData" << Reset << " " << Dim << "{\n"
           << Reset;

        // --- Definition ---
        ss << "  " << BrightMagenta << "Definition:" << Reset << "\n";
        {
            std::string defStr = definition.ToPrettyString();
            std::istringstream defStream(defStr);
            std::string line;
            while (std::getline(defStream, line))
                ss << "    " << line << '\n';
        }

        // --- State ---
        ss << "  " << BrightMagenta << "State:" << Reset << "\n";
        {
            std::string stateStr = state.ToPrettyString();
            std::istringstream stateStream(stateStr);
            std::string line;
            while (std::getline(stateStream, line))
                ss << "    " << line << '\n';
        }

        // --- Models ---
        ss << "  " << BrightMagenta << "Models:" << Reset << "\n";
        if (models.empty())
        {
            ss << "    (none)\n";
        }
        else
        {
            for (const auto &[key, model] : models)
            {
                ss << std::format("    [{}{}{}]:\n", Magenta, key, Reset);
                std::string modelStr = model.ToPrettyString();
                std::istringstream modelStream(modelStr);
                std::string line;
                while (std::getline(modelStream, line))
                    ss << "      " << line << '\n';
            }
        }

        ss << Dim << "}" << Reset;
        return ss.str();
    }
};

/**
 * @brief Caches all block-related JSON data loaded from disk.
 *
 * The cache keeps all parsed block states, definitions, and models in memory
 * to avoid redundant file reads during runtime. It is typically initialized once
 * during startup and then used as a read-only resource.
 *
 * @note Intended for global use through @ref Instance().
 *
 * @see BlockJsonData
 * @see BlockState
 * @see BlockDefinition
 * @see BlockModel
 */
class BlockJsonDataCache
{
private:
    /**
     * @brief Maps block identifiers to their corresponding JSON-parsed data.
     *
     * The key is the block name (e.g., `"minecraft:stone"`),
     * and the value contains its state, definition, and models.
     */
    std::unordered_map<std::string, BlockJsonData> cache_;

    /** @brief Path to the folder containing block state JSON files. */
    std::filesystem::path blockStatesFolder;

    /** @brief Path to the folder containing block model JSON files. */
    std::filesystem::path blockModelsFolder;

    /** @brief Path to the folder containing block definition JSON files. */
    std::filesystem::path blockDefinitionsFolder;

    /** @brief Reference to the logger instance used for reporting messages and errors. */
    ILogger &logger_;

    /** @brief Reference to the path provider used to resolve resource directories. */
    const IPathProvider &paths_;

private:
    /**
     * @brief Loads and parses a JSON file into an object of the specified type.
     *
     * @tparam T The type representing the JSON structure.
     * @param path Path to the JSON file.
     * @return A fully initialized object of type @c T with @c wasLoaded set to @c true.
     *
     * @throws std::runtime_error If the file cannot be opened or parsed.
     *
     * @note The function assumes that @c Options<T> performs JSON parsing
     *       and that type @c T contains a boolean field @c wasLoaded.
     */
    template <typename T>
    T LoadJsonFile(const std::filesystem::path &path)
    {
        try
        {
            Options<T> opts(path.string());
            T obj = opts.Value();
            obj.wasLoaded = true;
            return obj;
        }
        catch (const std::exception &e)
        {
            logger_.Error("Failed to load JSON: " + path.string() + " (" + e.what() + ")");
            throw;
        }
    }

    /**
     * @brief Loads all block state configurations from the @c blockStatesFolder.
     *
     * Iterates over all `.json` files and loads each into the cache.
     * The file name (without extension) is used as the block identifier.
     *
     * @throws std::filesystem::filesystem_error If the directory cannot be accessed.
     * @throws std::runtime_error If any JSON file cannot be parsed.
     */
    void LoadAllStates();

    /**
     * @brief Loads all block definition configurations from the @c blockDefinitionsFolder.
     *
     * Each file name (without extension) is used as the block identifier.
     *
     * @throws std::filesystem::filesystem_error If the directory cannot be accessed.
     * @throws std::runtime_error If any JSON file cannot be parsed.
     */
    void LoadAllDefinitions();

    /**
     * @brief Loads all block models from the @c blockModelsFolder.
     *
     * Expects model filenames to follow the pattern: `blockName_modelName.json`.
     * The part before the first underscore is treated as the block name.
     *
     * @throws std::filesystem::filesystem_error If the directory cannot be accessed.
     * @throws std::runtime_error If any JSON file cannot be parsed.
     *
     * @note Files without an underscore ('_') are ignored.
     */
    void LoadAllModels();

    /**
     * @brief Validates all loaded block configurations.
     *
     * Ensures that each registered block (except "unknown" and "air")
     * has valid and fully loaded data for its state, definition, and models.
     *
     * Validation steps:
     * - Confirms presence of each component.
     * - Calls @c Validate() on each JSON structure.
     * - Reports all errors through @ref ILogger.
     *
     * @throws std::runtime_error If any block has missing or invalid data.
     *
     * @note Validation continues through all blocks before throwing an error.
     */
    void ValidateAllBlocks() const;

    /**
     * @brief Loads all block-related JSON data.
     *
     * Sequentially loads:
     * - Block states
     * - Block definitions
     * - Block models
     *
     * @throws std::filesystem::filesystem_error If directories are inaccessible.
     * @throws std::runtime_error If a JSON file cannot be parsed.
     */
    void LoadAllBlocks();

    /**
     * @brief Initializes the cache by loading and validating all data.
     *
     * Obtains resource directories from @c IPathProvider,
     * loads all block JSON files, and performs validation.
     * This method runs automatically during construction.
     *
     * @throws std::filesystem::filesystem_error If directories cannot be accessed.
     * @throws std::runtime_error If validation or parsing fails.
     */
    void Init();

    /**
     * @brief Constructs the @c BlockJsonDataCache and initializes all data.
     *
     * Automatically calls @c Init() to load and validate all JSON files.
     *
     * @param logger Reference to an @c ILogger instance for reporting messages and errors.
     * @param paths  Reference to an @c IPathProvider for resolving resource paths.
     *
     * @throws std::runtime_error If initialization or validation fails.
     */
    BlockJsonDataCache(ILogger &logger, const IPathProvider &paths);

public:
    BlockJsonDataCache(const BlockJsonDataCache &) = delete;
    BlockJsonDataCache &operator=(const BlockJsonDataCache &) = delete;
    BlockJsonDataCache(BlockJsonDataCache &&) = delete;
    BlockJsonDataCache &operator=(BlockJsonDataCache &&) = delete;

    /**
     * @brief Returns the global singleton instance of the cache.
     *
     * Initializes the instance on first call (thread-safe).
     *
     * @param logger Reference to a logger used for reporting messages.
     * @param paths  Reference to a path provider used to resolve directories.
     * @return Reference to the global @c BlockJsonDataCache instance.
     */
    static BlockJsonDataCache &Instance(ILogger &logger, const IPathProvider &paths);

    /**
     * @brief Retrieves block data by block name.
     *
     * @param name The block's unique name (without extension).
     * @return Optional constant reference to the cached block data,
     *         or @c std::nullopt if not found.
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<const BlockJsonData>>
    Get(const std::string &name) const;

    /**
     * @brief Retrieves block data by @c BlockType.
     *
     * Converts @p type to its string representation and calls @ref Get(const std::string&).
     *
     * @param type The block type.
     * @return Optional constant reference to the cached block data,
     *         or @c std::nullopt if not found.
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<const BlockJsonData>>
    Get(BlockType type) const;

    /**
     * @brief Returns the total number of cached block entries.
     *
     * @return Number of loaded block entries.
     */
    size_t GetLoadedCount() const noexcept;
};