#pragma once

#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>

#include "IStaticBlock.h"
#include "Logger.h"

/**
 * @brief Custom hash function for @ref BlockType enumeration.
 */
struct BlockIDHash
{
    /**
     * @brief Hashes a @ref BlockType by converting it to std::size_t.
     */
    std::size_t operator()(const BlockType &blockType) const noexcept
    {
        return static_cast<std::size_t>(blockType);
    }
};

/**
 * @class BlockFactory
 * @brief Thread-safe factory for creating and managing block instances.
 *
 * This class maintains two internal registries:
 * - **Static blocks** — blocks that exist globally and never change, e.g. Air, Bedrock, Grass.
 * - **Dynamic blocks** — blocks that are created anew each time, e.g. Dirt, Chests, Furnaces.
 *
 * Static blocks are stored as persistent `std::shared_ptr<IStaticBlock>` instances,
 * while dynamic blocks are registered via constructor functions that return a fresh
 * `std::shared_ptr<IBlock>` upon each request.
 *
 * @note The class is purely static and cannot be copied or moved.
 * @note All public methods are thread-safe.
 */
class BlockFactory
{
private:
    /**
     * @brief Stores singleton instances of all static blocks.
     */
    inline static std::unordered_map<BlockType, std::shared_ptr<IStaticBlock>, BlockIDHash> staticBlocks_;

    /**
     * @brief Stores constructor functions for dynamic blocks.
     *
     * Each registered function returns a new @ref std::shared_ptr<IBlock>
     * when invoked via @ref GetBlock.
     */
    inline static std::unordered_map<BlockType, std::function<std::shared_ptr<IBlock>()>, BlockIDHash> dynamicBlockConstructors_;

    /**
     * @brief Mutex used to ensure thread-safe access to block registries.
     */
    inline static std::mutex factoryMutex_;

private:
    /**
     * @brief Private default constructor — prevents instantiation.
     */
    BlockFactory() = default;

    /**
     * @brief Private destructor.
     */
    ~BlockFactory() = default;

    // Copy and move are disabled
    BlockFactory(const BlockFactory &) = delete;
    BlockFactory &operator=(const BlockFactory &) = delete;
    BlockFactory(BlockFactory &&) = delete;
    BlockFactory &operator=(BlockFactory &&) = delete;

public:
    /**
     * @brief Registers a static block type.
     *
     * The factory will hold a single global shared instance of the block.
     *
     * @tparam T Class derived from @ref IStaticBlock.
     * @param type The block type to associate with this instance.
     */
    template <typename T>
    static void RegisterStatic(BlockType type)
    {
        std::lock_guard<std::mutex> lock(factoryMutex_);
        staticBlocks_[type] = std::make_shared<T>();
    }

    /**
     * @brief Registers a dynamic block type.
     *
     * Each call to @ref GetBlock for this type will create a new instance.
     *
     * @tparam T Class derived from @ref IBlock.
     * @param type The block type to associate with this block class.
     */
    template <typename T>
    static void RegisterDynamic(BlockType type)
    {
        std::lock_guard<std::mutex> lock(factoryMutex_);
        dynamicBlockConstructors_[type] = []()
        {
            return std::make_shared<T>();
        };
    }

    /**
     * @brief Retrieves a block instance by its @ref BlockType.
     *
     * - If the type corresponds to a static block, returns the existing shared instance.
     * - If the type corresponds to a dynamic block, constructs and returns a new instance.
     * - If the type is not registered, returns `nullptr`.
     *
     * @param type The block type to retrieve.
     * @return Shared pointer to the corresponding @ref IBlock instance, or `nullptr` if not found.
     */
    static std::shared_ptr<IBlock> GetBlock(BlockType type) noexcept
    {
        std::function<std::shared_ptr<IBlock>()> ctor;

        {
            std::scoped_lock lock(factoryMutex_);

            if (auto it = staticBlocks_.find(type); it != staticBlocks_.end())
                return it->second;

            if (auto it = dynamicBlockConstructors_.find(type); it != dynamicBlockConstructors_.end())
                ctor = it->second;
        }

        if (ctor)
            return ctor();

        return nullptr;
    }

    /**
     * @brief Returns the number of registered static blocks.
     *
     * Static blocks are single-instance blocks (similar to singletons)
     * that are stored and reused instead of being constructed dynamically.
     *
     * @return Number of registered static blocks.
     */
    static size_t GetStaticCount()
    {
        return staticBlocks_.size();
    }

    /**
     * @brief Returns the number of registered dynamic blocks.
     *
     * Dynamic blocks are created on demand using factory functions
     * each time they are requested (e.g., DirtBlock, Furnace, etc.).
     *
     * @return Number of registered dynamic blocks.
     */
    static size_t GetDynamicCount()
    {
        return dynamicBlockConstructors_.size();
    }

    /**
     * @brief Logs a summary of all registered blocks.
     *
     * Outputs a concise registration summary to the global @ref Logger instance
     * in the following format:
     * ```
     * [BlockFactory] 📦 Registered blocks: X static, Y dynamic
     * ```
     *
     * This method should be called after all block types are registered,
     * typically after including @ref BlockRegistration.h in `main()`.
     */
    static void LogRegistrationSummary()
    {
        Logger::Instance().Info(
            "[BlockFactory] 📦 Registered blocks: " + std::to_string(GetStaticCount()) + " static, " + std::to_string(GetDynamicCount()) + " dynamic");
    }
};