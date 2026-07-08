#ifndef SGE_CORE_ENGINEMODE_H
#define SGE_CORE_ENGINEMODE_H

namespace SGE::CORE {

    // Inspection: the default, editor-only state - the scene can be viewed and
    // edited but nothing simulates (no input/physics/collision).
    // Play: simulation systems run too, as if the game were actually running.
    enum class EngineMode {
        Inspection,
        Play
    };

} // namespace SGE::CORE

#endif // SGE_CORE_ENGINEMODE_H
