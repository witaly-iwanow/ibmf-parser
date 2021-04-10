#ifndef IBMF_BOX_H
#define IBMF_BOX_H

#include <string>
#include <vector>

namespace IBMF
{

struct Box
{
    uint64_t offset = 0;
    uint64_t size = 0;
    uint32_t headerLen = 0; // at this point it's either 8 (regular box) or 16 (large size box)
    std::string type;

    std::vector<Box> children;

    std::string ToString(const std::string& linePrefix = "", const std::string& indent = "  ") const
    {
        std::string result = linePrefix + (type.empty() ? "<empty>" : type) + " at " + std::to_string(offset) + ", " + std::to_string(size) + " bytes";
        for (const auto& child: children)
            result += "\n" + child.ToString(linePrefix + indent, indent);
        return result;
    }
};

};  // namespace IBMF

#endif // IBMF_BOX_H
