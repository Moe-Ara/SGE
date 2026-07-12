#ifndef SGE_CORE_OBJECTPOOL_H
#define SGE_CORE_OBJECTPOOL_H

#include <cstddef>
#include <memory>
#include <vector>

namespace SGE::CORE {

    // High-watermark pool for frame/rebuild-scoped objects. Acquired addresses stay
    // stable, and reset() makes all objects reusable without destroying them.
    template<typename Type>
    class ObjectPool {
    public:
        explicit ObjectPool(std::size_t initialCapacity = 0) {
            reserve(initialCapacity);
        }

        ObjectPool(const ObjectPool&) = delete;
        ObjectPool& operator=(const ObjectPool&) = delete;
        ObjectPool(ObjectPool&&) noexcept = default;
        ObjectPool& operator=(ObjectPool&&) noexcept = default;

        Type& acquire() {
            if (activeCount == objects.size()) {
                objects.push_back(std::make_unique<Type>());
            }
            return *objects[activeCount++];
        }

        void reset() noexcept {
            activeCount = 0;
        }

        void reserve(std::size_t capacity) {
            objects.reserve(capacity);
            while (objects.size() < capacity) {
                objects.push_back(std::make_unique<Type>());
            }
        }

        [[nodiscard]] std::size_t active() const noexcept { return activeCount; }
        [[nodiscard]] std::size_t capacity() const noexcept { return objects.size(); }

    private:
        std::vector<std::unique_ptr<Type>> objects;
        std::size_t activeCount{0};
    };

} // namespace SGE::CORE

#endif // SGE_CORE_OBJECTPOOL_H
