#pragma once

#include "IBlock.h"

/**
 * @brief Extended block interface for blocks with mutable properties.
 *
 * This interface is used for blocks whose internal state or attributes
 * can change during gameplay (e.g. moisture, growth stage, or energy level).
 *
 * @note A block cannot implement both @ref IPropertyBlock and @ref IStaticBlock
 *       at the same time.
 * @note Property blocks are typically used for blocks that store dynamic state
 *       during runtime (e.g. crops, furnaces, energy cells)
 * 
 */
class IPropertyBlock : public IBlock
{
protected:
    IPropertyBlock() = default;

public:
    virtual ~IPropertyBlock() = default;

    /**
     * @brief Returns the current block properties in serialized string form.
     *
     * The format should follow the pattern:
     * ```
     * {property}:{value},{property2}:{value2},...
     * ```
     * where multiple key–value pairs are separated by commas.
     *
     * @return A string representation of the block's properties.
     */
    virtual std::string GetBlockProperties() const = 0;

    /**
     * @brief Sets the block's properties using a serialized string.
     *
     * The input string must follow the same format as returned by
     * @ref GetBlockProperties(), for example:
     * ```
     * {hardness}:{5},{flammable}:{true}
     * ```
     *
     * @param[in] properties  – A comma-separated list of property–value pairs.
     * 
     * @return `true` if the properties were successfully parsed and applied,  
     *         `false` if the input format was invalid or one or more properties could not be set.
     * 
     */
    virtual bool SetBlockProperties(const std::string& properties) = 0;
};