#ifndef SGE_CORE_ASSETLOCATOR_H
#define SGE_CORE_ASSETLOCATOR_H

#include <filesystem>

namespace SGE::CORE {

    class AssetLocator {
    public:
        static void initialize(const std::filesystem::path& executableHint = {});
        static void initializeFromRoot(const std::filesystem::path& resourceRoot);

        [[nodiscard]] static std::filesystem::path resolve(
            const std::filesystem::path& relativeAssetPath
        );
        [[nodiscard]] static const std::filesystem::path& root();
    };

} // namespace SGE::CORE

#endif // SGE_CORE_ASSETLOCATOR_H
