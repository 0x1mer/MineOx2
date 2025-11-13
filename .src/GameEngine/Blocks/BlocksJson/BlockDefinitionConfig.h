#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "JsonLoadable.h"
#include "IDebugPrintable.h"
#include "DebugHelpers.h"

using nlohmann::json;

/**
 * @brief Defines the physical and rendering properties of a block.
 *
 * This structure determines how a block behaves in the world,
 * including its transparency, solidity, and visibility to light.
 *
 * Example JSON:
 * @code{.json}
 * {
 *   "isTransparent": false,
 *   "isSolid": true,
 *   "isOpaque": true,
 *   "isFullCube": true
 * }
 * @endcode
 */
struct BlockDefinition : public JsonLoadable, public IDebugPrintable
{
    /// @brief Whether the block allows light to pass through.
    bool isTransparent = false;

    /// @brief Whether the block has a solid physical collision box.
    bool isSolid = true;

    /// @brief Whether the block fully blocks light.
    bool isOpaque = true;

    /// @brief Whether the block occupies a full cube space.
    bool isFullCube = true;

    /**
     * @brief Returns a compact one-line description of the block definition.
     * @return A formatted string with abbreviated property names.
     */
    std::string ToShortString() const override
    {
        using namespace DebugHelpers;
        return std::format("BlockDefinition(T={}, S={}, O={}, F={})",
                           isTransparent, isSolid, isOpaque, isFullCube);
    }

    /**
     * @brief Returns a human-readable, colorized multi-line string representation.
     * @return A detailed formatted string with syntax highlighting for console output.
     */
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Title("BlockDefinition") << " " << Brace("{\n");

        ss << std::format("  {} = {}\n", Key("isTransparent"), BoolToColored(isTransparent));
        ss << std::format("  {} = {}\n", Key("isSolid"), BoolToColored(isSolid));
        ss << std::format("  {} = {}\n", Key("isOpaque"), BoolToColored(isOpaque));
        ss << std::format("  {} = {}\n", Key("isFullCube"), BoolToColored(isFullCube));

        ss << Brace("}");
        return ss.str();
    }
};

/**
 * @brief Deserializes a BlockDefinition object from JSON.
 *
 * Safely reads optional properties and sets `wasLoaded = true`
 * if at least one field was found and parsed successfully.
 *
 * @param j JSON object containing block definition fields.
 * @param def Reference to the BlockDefinition to populate.
 */
inline void from_json(const json &j, BlockDefinition &def)
{
    if (j.contains("isTransparent") && j["isTransparent"].is_boolean())
        j.at("isTransparent").get_to(def.isTransparent);

    if (j.contains("isSolid") && j["isSolid"].is_boolean())
        j.at("isSolid").get_to(def.isSolid);

    if (j.contains("isOpaque") && j["isOpaque"].is_boolean())
        j.at("isOpaque").get_to(def.isOpaque);

    if (j.contains("isFullCube") && j["isFullCube"].is_boolean())
        j.at("isFullCube").get_to(def.isFullCube);

    def.wasLoaded = true;
}

/**
 * @brief Validates logical consistency of a block definition.
 *
 * Throws exceptions for physically impossible or contradictory states.
 *
 * @param def The block definition to validate.
 * @throws std::invalid_argument If the block configuration is invalid.
 *
 * Example invalid states:
 * - Transparent and opaque at the same time.
 * - Full cube that is not solid.
 */
inline void Validate(const BlockDefinition &def)
{
    if (def.isTransparent && def.isOpaque)
        throw std::invalid_argument("Block cannot be both transparent and opaque");

    if (def.isFullCube && !def.isSolid)
        throw std::invalid_argument("Full cubes must be solid blocks");
}