# Performance Architecture

## Allocation strategy

SGE avoids allocation in stable per-frame workloads through high-watermark reuse:

- `CORE::ObjectPool<T>` owns stable-address objects. `reset()` marks all objects
  reusable without destroying them; subsequent `acquire()` calls reuse the same
  instances and allocate only when a workload exceeds its previous peak.
- `PHYSICS::BVH` uses an `ObjectPool<BVHNode>`. It copies colliders once into one
  retained working array, partitions that array in place with longest-axis median
  splits, and stores only ranges in nodes. A stable workload therefore performs no
  BVH node allocation after warm-up and does not duplicate collider arrays per node.
- `CollisionSystem` owns and clears its spatial-item and collision-result vectors.
  Their capacities survive between fixed simulation steps.
- `ECS::SystemPipeline` reserves system ownership once during setup and updates a
  contiguous list without frame-local allocation.

## Object-pool contract

Pooled values retain their previous contents. Code acquiring an object must fully
initialize the state it will read. References and pointers remain valid while the
pool exists, but an object must not be treated as active after `reset()` unless it
has been acquired again.

The pool is intended for rebuild-scoped/transient engine objects. EnTT already pools
component storage, and long-lived assets continue to use explicit RAII ownership;
wrapping those in another pool would add complexity without reducing hot-path work.

## Verification

`SGE_tests` covers stable-address object reuse, system insertion order, BVH collision
correctness, and the BVH node high-watermark invariant. These are regression checks,
not performance benchmarks. Optimization decisions should still be based on Tracy,
RenderDoc, GPU timestamps, and representative stress scenes before expanding this
infrastructure.

## Asset path stability

`CORE::AssetLocator` resolves validated relative asset paths from the deployed
`resources` directory beside the executable, with a working-directory fallback for
development. Runtime loading no longer depends on the process launch directory.
