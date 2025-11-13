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
 * @brief Represents the state configuration of a block.
 *
 * Describes how a block's property combinations (e.g. `"facing=north"`, `"wet=true"`)
 * correspond to specific model variants for rendering.
 *
 * Example JSON:
 * @code{.json}
 * {
 *   "properties": {
 *     "wet": "false"
 *   },
 *   "variants": {
 *     "wet=false": "block/dirt",
 *     "wet=true":  "block/mud"
 *   }
 * }
 * @endcode
 */
struct BlockState : public JsonLoadable, public IDebugPrintable
{
    /// @brief Key-value pairs describing possible block properties.
    /// Example: `{"facing": "north", "powered": "true"}`
    std::unordered_map<std::string, std::string> properties;

    /// @brief Maps a combination of property values to a specific model name.
    /// Example: `"facing=north,waterlogged=false" → "block/dirt_side"`
    std::unordered_map<std::string, std::string> variants;

    /**
     * @brief Returns a compact one-line summary of this block state.
     * @return A short string showing the number of properties and variants.
     */
    std::string ToShortString() const override
    {
        return std::format("BlockState(props={}, variants={})",
                           properties.size(), variants.size());
    }

    /**
     * @brief Returns a human-readable, colorized multi-line string representation.
     * @return A formatted string with color highlighting for properties and variants.
     */
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Title("BlockState") << " " << Brace("{\n");

        // --- Properties ---
        ss << "  " << Section("Properties") << ":\n";
        if (properties.empty())
            ss << "    " << Dim << "(none)\n" << Reset;
        else
            ss << MapToStr(properties, "    ");

        // --- Variants ---
        ss << "  " << Section("Variants") << ":\n";
        if (variants.empty())
            ss << "    " << Dim << "(none)\n" << Reset;
        else
            ss << MapToStr(variants, "    ");

        ss << Brace("}");
        return ss.str();
    }
};

/**
 * @brief Deserializes a BlockState object from JSON.
 *
 * Loads the "properties" and "variants" fields if they are present.
 *
 * @param j JSON object containing the block state definition.
 * @param state Reference to the BlockState object to populate.
 */
inline void from_json(const json &j, BlockState &state)
{
    if (j.contains("properties") && j["properties"].is_object())
        j.at("properties").get_to(state.properties);

    if (j.contains("variants") && j["variants"].is_object())
        j.at("variants").get_to(state.variants);

    state.wasLoaded = true;
}

/**
 * @brief Validates logical consistency of a block state.
 *
 * Ensures the state contains at least one model variant.
 *
 * @param state The BlockState to validate.
 * @throws std::invalid_argument If no variants are defined.
 */
inline void Validate(const BlockState &state)
{
    if (state.variants.empty())
        throw std::invalid_argument("BlockState must define at least one variant");
}