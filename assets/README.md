# Base assets

Blender sources and source textures used to author the content.

These are the base assets, not the assets the game loads. The runtime assets in
`data/` are derived from these by hand: meshes are exported to OBJ, textures are
resized and repacked to PNG, and materials are written as ini files.

Both are kept because there is no content pipeline that derives one from the
other. Nothing here is read at runtime, and this folder is not copied into the
build.

## Contents

| File | Derives |
|---|---|
| `cave.blend` | `data/mesh/cave.obj`, `data/mesh/new_cave.obj` |
| `gear.blend` | `data/mesh/gear.obj` |
| `skybox.blend` | `data/mesh/skybox.obj` |
| `sphere_with_missing_faces.blend` | test mesh with intentionally missing faces |
| `Cliffs0149/` | textures for the cave interior material |

## Requirements

Editing these needs Blender. Producing the runtime assets is manual.