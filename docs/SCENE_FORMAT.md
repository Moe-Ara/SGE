# SGE Scene Format

SGE scenes are UTF-8 JSON documents with a format marker and explicit version:

```json
{
  "format": "SGE.Scene",
  "version": 1,
  "entities": [
    {
      "id": 101,
      "components": {
        "Tag": { "name": "Player" },
        "Transform": {
          "translation": [0.0, 0.0, 0.0],
          "rotation": [1.0, 0.0, 0.0, 0.0],
          "scale": [1.0, 1.0, 1.0]
        }
      }
    }
  ]
}
```

## Identity and references

`SceneIdentityComponent` stores a non-zero 64-bit ID. IDs are stable serialized
identity; EnTT entity values remain process-local handles and are never persisted.
The serializer assigns IDs to entities that do not have one, rejects duplicates,
and remaps `ThirdPersonFollowComponent::target` in a two-pass load.

## Components

Version 1 supports every registered scene component: identity, tag, transform, mesh,
material, rigid body, sphere collider, point light, camera, free camera, third-person
follow, and player controller. Unknown component data and unregistered runtime pools
are rejected instead of being silently discarded.

Derived camera matrices and mouse-capture state are rebuilt/reset on load. Authored
camera/controller parameters, physics state, materials, and entity references are
preserved.

## Assets

Mesh and albedo-texture components serialize durable asset IDs rather than GPU
pointers. `SceneAssetResolver` maps those IDs to runtime `Model` and `Texture`
instances while loading. `cloneRuntime()` builds a resolver from the source world,
which lets Play/Stop snapshots preserve shared and generated resources without
serializing OpenGL objects.

Disk scene loading may omit a resolver; components and IDs still load, but their
runtime resource pointers remain empty until an asset manager resolves them.

## Safety and versioning

- `save()` writes a temporary file and replaces the destination only after a
  successful flush.
- Loads reject malformed JSON, unsupported versions, duplicate/zero IDs, unresolved
  entity references, unknown components, oversized files, invalid camera projection
  values, invalid dynamic masses, and invalid collider/material ranges.
- Version changes require an explicit migration path. Version 1 documents are not
  guessed into future layouts.

## Current limitations

The format API is operational, and Play/Stop uses its in-memory round trip. The
editor does not yet expose Save/Open commands, scene migrations, prefabs, overrides,
autosave, or an asset database. Those remain separate authoring-system milestones.

