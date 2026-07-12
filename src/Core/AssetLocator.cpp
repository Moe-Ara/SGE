#include "AssetLocator.h"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <vector>

#if defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace SGE::CORE {

    namespace {
        std::filesystem::path resourceRoot;

        std::filesystem::path platformExecutablePath() {
#if defined(__linux__)
            std::vector<char> buffer(4096);
            const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1u);
            if (length > 0) {
                buffer[static_cast<std::size_t>(length)] = '\0';
                return std::filesystem::path{buffer.data()};
            }
#elif defined(_WIN32)
            std::vector<wchar_t> buffer(32768);
            const DWORD length = GetModuleFileNameW(nullptr, buffer.data(),
                                                    static_cast<DWORD>(buffer.size()));
            if (length > 0 && length < buffer.size()) {
                return std::filesystem::path{std::wstring{buffer.data(), length}};
            }
#elif defined(__APPLE__)
            uint32_t size = 0;
            _NSGetExecutablePath(nullptr, &size);
            std::vector<char> buffer(size);
            if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
                return std::filesystem::weakly_canonical(buffer.data());
            }
#endif
            return {};
        }

        bool containsParentTraversal(const std::filesystem::path& path) {
            for (const auto& part : path) {
                if (part == "..") return true;
            }
            return false;
        }
    }

    void AssetLocator::initialize(const std::filesystem::path& executableHint) {
        std::vector<std::filesystem::path> candidates;
        if (const char* configuredRoot = std::getenv("SGE_RESOURCE_ROOT");
            configuredRoot && configuredRoot[0] != '\0') {
            candidates.emplace_back(configuredRoot);
        }

        const auto addAncestorCandidates = [&candidates](std::filesystem::path path) {
            if (path.empty()) return;
            path = std::filesystem::absolute(path);
            if (!std::filesystem::is_directory(path)) path = path.parent_path();
            for (int depth = 0; depth < 8 && !path.empty(); ++depth) {
                candidates.push_back(path / "resources");
                const std::filesystem::path parent = path.parent_path();
                if (parent == path) break;
                path = parent;
            }
        };

        const std::filesystem::path platformPath = platformExecutablePath();
        addAncestorCandidates(platformPath);
        addAncestorCandidates(executableHint);
        addAncestorCandidates(std::filesystem::current_path());

        for (const auto& candidate : candidates) {
            if (std::filesystem::is_directory(candidate)) {
                initializeFromRoot(candidate);
                return;
            }
        }
        std::ostringstream message;
        message << "Unable to locate the SGE resources directory. Checked:";
        for (const auto& candidate : candidates) {
            message << "\n  - " << candidate.string();
        }
        message << "\nSet SGE_RESOURCE_ROOT to the resources directory if using a custom layout.";
        throw std::runtime_error(message.str());
    }

    void AssetLocator::initializeFromRoot(const std::filesystem::path& rootPath) {
        if (!std::filesystem::is_directory(rootPath)) {
            throw std::invalid_argument("Asset root is not a directory: " + rootPath.string());
        }
        resourceRoot = std::filesystem::weakly_canonical(rootPath);
    }

    std::filesystem::path AssetLocator::resolve(const std::filesystem::path& relativeAssetPath) {
        if (resourceRoot.empty()) {
            throw std::logic_error("AssetLocator must be initialized before resolving assets");
        }
        if (relativeAssetPath.empty() || relativeAssetPath.is_absolute() ||
            containsParentTraversal(relativeAssetPath)) {
            throw std::invalid_argument("Asset path must be a safe relative path");
        }

        const std::filesystem::path resolved =
            std::filesystem::weakly_canonical(resourceRoot / relativeAssetPath);
        const auto [rootEnd, resolvedPosition] = std::mismatch(
            resourceRoot.begin(), resourceRoot.end(), resolved.begin(), resolved.end()
        );
        (void)resolvedPosition;
        if (rootEnd != resourceRoot.end()) {
            throw std::invalid_argument("Resolved asset path escapes the asset root");
        }
        if (!std::filesystem::is_regular_file(resolved)) {
            throw std::runtime_error("Asset does not exist: " + resolved.string());
        }
        return resolved;
    }

    const std::filesystem::path& AssetLocator::root() {
        if (resourceRoot.empty()) {
            throw std::logic_error("AssetLocator has not been initialized");
        }
        return resourceRoot;
    }

} // namespace SGE::CORE
