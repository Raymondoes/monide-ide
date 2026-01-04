#include <cstdint>
#include <cstddef>
namespace Monide
{
    struct ModuleInfo
    {
        const char *name;
        const char *version;
        const char *author;
        const char *description;
        std::uint32_t apiVersion;
        size_t moduleSize;
        const char *entryPoint;
    };
}