#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <variant>
#include <vector>
#include <array>
#include <nlohmann/json.hpp>

#include "JsonLoadable.h"
#include "IDebugPrintable.h"
#include "DebugHelpers.h"

using nlohmann::json;

// ============================================================================
// Blockbench-specific structures
// ============================================================================

/**
 * @brief Represents one face of a cube element (north, east, south, west, up, down).
 */
struct BlockFace : public IDebugPrintable
{
    std::array<float, 4> uv{}; ///< UV coordinates of the face.
    std::string texture;       ///< Texture reference name.

    /// @copydoc IDebugPrintable::ToShortString
    std::string ToShortString() const override
    {
        using namespace DebugHelpers;
        return std::format("Face(uv={}, tex={})", Arr4ToStr(uv), texture);
    }

    /// @copydoc IDebugPrintable::ToPrettyString
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Title("BlockFace") << " " << Brace("{\n");
        ss << "  " << Key("uv") << " = " << Value(Arr4ToStr(uv)) << "\n";
        ss << "  " << Key("texture") << " = " << Value(texture) << "\n";
        ss << Brace("}");
        return ss.str();
    }
};

/**
 * @brief Represents a single 3D cube (element) in a Blockbench model.
 */
struct BlockElement : public IDebugPrintable
{
    std::array<float, 3> from{}; ///< Lower corner coordinates.
    std::array<float, 3> to{};   ///< Upper corner coordinates.

    /**
     * @brief Describes element rotation parameters.
     */
    struct Rotation
    {
        float angle = 0.0f;            ///< Rotation angle in degrees.
        std::string axis;              ///< Rotation axis ("x", "y" or "z").
        std::array<float, 3> origin{}; ///< Rotation origin point.
    } rotation;                        ///< Rotation settings for this element.

    std::unordered_map<std::string, BlockFace> faces; ///< Mapping of face names to their definitions.

    /// @copydoc IDebugPrintable::ToShortString
    std::string ToShortString() const override
    {
        using namespace DebugHelpers;
        return std::format("Elem({}, {}) faces={}",
                           Arr3ToStr(from), Arr3ToStr(to), faces.size());
    }

    /// @copydoc IDebugPrintable::ToPrettyString
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Title("BlockElement") << " " << Brace("{\n");
        ss << "  " << Key("from") << " = " << Value(Arr3ToStr(from)) << "\n";
        ss << "  " << Key("to") << " = " << Value(Arr3ToStr(to)) << "\n";

        ss << "  " << Section("rotation") << ": "
           << Key("angle") << "=" << Value(std::to_string(rotation.angle)) << ", "
           << Key("axis") << "=" << Value(rotation.axis) << ", "
           << Key("origin") << "=" << Value(Arr3ToStr(rotation.origin)) << "\n";

        ss << "  " << Section("faces") << ":\n";
        if (faces.empty())
        {
            ss << "    " << Dim << "(none)\n"
               << Reset;
        }
        else
        {
            for (auto &[name, face] : faces)
            {
                ss << "    " << Key(name) << ":\n";
                ss << IndentLines(face.ToPrettyString(), "      ");
            }
        }

        ss << Brace("}");
        return ss.str();
    }
};

/**
 * @brief Represents a complete Blockbench-generated model definition.
 */
struct BlockBenchModel : public IDebugPrintable
{
    std::string formatVersion;                             ///< Format version string.
    std::string credit;                                    ///< Author or credits.
    std::unordered_map<std::string, std::string> textures; ///< Texture mapping.
    std::vector<BlockElement> elements;                    ///< List of model elements.

    /// @copydoc IDebugPrintable::ToShortString
    std::string ToShortString() const override
    {
        return std::format("BlockbenchModel(elems={}, textures={})",
                           elements.size(), textures.size());
    }

    /// @copydoc IDebugPrintable::ToPrettyString
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Title("BlockBenchModel") << " " << Brace("{\n");
        ss << "  " << Key("formatVersion") << " = " << Value(formatVersion) << "\n";
        ss << "  " << Key("credit") << " = " << Value(credit) << "\n";

        ss << "  " << Section("textures") << ":\n"
           << MapToStr(textures, "    ");

        ss << "  " << Section("elements") << ":\n";
        if (elements.empty())
        {
            ss << "    " << Dim << "(none)\n"
               << Reset;
        }
        else
        {
            for (const auto &el : elements)
            {
                ss << "    - " << Section("Element") << ":\n";
                ss << IndentLines(el.ToPrettyString(), "      ");
            }
        }

        ss << Brace("}");
        return ss.str();
    }
};

// ============================================================================
// Vanilla-like model structures
// ============================================================================

/**
 * @brief Represents a vanilla-like block model structure.
 *
 * This type corresponds to standard Minecraft JSON model format.
 */
struct DefaultTemplateModel : public IDebugPrintable
{
    std::string parent;                                    ///< Name of the parent model (optional).
    std::unordered_map<std::string, std::string> textures; ///< Texture mappings.
    json elements;                                         ///< Raw JSON definition of model elements.
    std::string formatVersion;                             ///< Optional format version.

    /// @copydoc IDebugPrintable::ToShortString
    std::string ToShortString() const override
    {
        return std::format("VanillaModel(tex={}, elements={})",
                           textures.size(),
                           elements.is_null() ? 0 : elements.size());
    }

    /// @copydoc IDebugPrintable::ToPrettyString
    std::string ToPrettyString() const override
    {
        using namespace DebugHelpers;
        using namespace DebugColors;

        std::ostringstream ss;
        ss << Title("DefaultTemplateModel") << " " << Brace("{\n");
        ss << "  " << Key("parent") << " = " << Value(parent.empty() ? "(none)" : parent) << "\n";
        ss << "  " << Key("formatVersion") << " = " << Value(formatVersion.empty() ? "(none)" : formatVersion) << "\n";

        ss << "  " << Section("textures") << ":\n"
           << MapToStr(textures, "    ");

        ss << "  " << Section("elements (raw JSON)") << ":\n";
        if (elements.is_null())
        {
            ss << "    " << Dim << "(none)\n"
               << Reset;
        }
        else
        {
            ss << IndentLines(elements.dump(4), "    ");
        }

        ss << Brace("}");
        return ss.str();
    }
};

// ============================================================================
// Unified block model structure
// ============================================================================

/**
 * @brief Represents a unified model container that can hold either Blockbench or Vanilla-style models.
 */
struct BlockModel : public JsonLoadable, public IDebugPrintable
{
    bool isBlockbench = false;                                ///< True if model uses Blockbench format.
    std::variant<BlockBenchModel, DefaultTemplateModel> data; ///< Model data variant.

    /// @copydoc IDebugPrintable::ToShortString
    std::string ToShortString() const override
    {
        if (isBlockbench)
            return std::get<BlockBenchModel>(data).ToShortString();
        else
            return std::get<DefaultTemplateModel>(data).ToShortString();
    }

    /// @copydoc IDebugPrintable::ToPrettyString
    std::string ToPrettyString() const override
    {
        if (isBlockbench)
            return std::get<BlockBenchModel>(data).ToPrettyString();
        else
            return std::get<DefaultTemplateModel>(data).ToPrettyString();
    }
};

// ============================================================================
// JSON Deserialization
// ============================================================================

/**
 * @brief JSON deserialization for BlockFace.
 */
inline void from_json(const json &j, BlockFace &f)
{
    if (j.contains("uv") && j["uv"].is_array())
        j.at("uv").get_to(f.uv);

    if (j.contains("texture") && j["texture"].is_string())
        j.at("texture").get_to(f.texture);
}

/**
 * @brief JSON deserialization for BlockElement::Rotation.
 */
inline void from_json(const json &j, BlockElement::Rotation &r)
{
    if (j.contains("angle"))
        j.at("angle").get_to(r.angle);
    if (j.contains("axis"))
        j.at("axis").get_to(r.axis);
    if (j.contains("origin"))
        j.at("origin").get_to(r.origin);
}

/**
 * @brief JSON deserialization for BlockElement.
 */
inline void from_json(const json &j, BlockElement &e)
{
    if (j.contains("from"))
        j.at("from").get_to(e.from);
    if (j.contains("to"))
        j.at("to").get_to(e.to);
    if (j.contains("rotation"))
        j.at("rotation").get_to(e.rotation);
    if (j.contains("faces"))
        j.at("faces").get_to(e.faces);
}

/**
 * @brief JSON deserialization for BlockBenchModel.
 */
inline void from_json(const json &j, BlockBenchModel &bb)
{
    if (j.contains("format_version"))
        j.at("format_version").get_to(bb.formatVersion);
    if (j.contains("credit"))
        j.at("credit").get_to(bb.credit);
    if (j.contains("textures"))
        j.at("textures").get_to(bb.textures);
    if (j.contains("elements"))
        j.at("elements").get_to(bb.elements);
}

/**
 * @brief JSON deserialization for DefaultTemplateModel.
 */
inline void from_json(const json &j, DefaultTemplateModel &d)
{
    if (j.contains("parent"))
        j.at("parent").get_to(d.parent);
    if (j.contains("textures"))
        j.at("textures").get_to(d.textures);
    if (j.contains("elements"))
        d.elements = j.at("elements");
    if (j.contains("format_version"))
        j.at("format_version").get_to(d.formatVersion);
}

/**
 * @brief JSON deserialization for BlockModel.
 */
inline void from_json(const json &j, BlockModel &model)
{
    if (j.contains("credit") && j["credit"].is_string())
    {
        model.isBlockbench = true;
        model.data = j.get<BlockBenchModel>();
    }
    else
    {
        model.isBlockbench = false;
        model.data = j.get<DefaultTemplateModel>();
    }
    model.wasLoaded = true;
}

// ============================================================================
// Validation
// ============================================================================

/**
 * @brief Validates the logical correctness of a loaded BlockModel.
 *
 * @throws std::invalid_argument if the model is missing required fields.
 */
inline void Validate(const BlockModel &model)
{
    if (model.isBlockbench)
    {
        const auto &bb = std::get<BlockBenchModel>(model.data);

        if (bb.textures.empty())
            throw std::invalid_argument("Blockbench model must contain 'textures'");

        if (bb.formatVersion.empty())
            throw std::invalid_argument("Blockbench 'format_version' cannot be empty");

        if (bb.elements.empty())
            throw std::invalid_argument("Blockbench model must define at least one 'element'");
    }
    else
    {
        const auto &def = std::get<DefaultTemplateModel>(model.data);

        if (def.parent.empty() && def.textures.empty())
            throw std::invalid_argument("Vanilla model must define 'parent' or 'textures'");
    }
}