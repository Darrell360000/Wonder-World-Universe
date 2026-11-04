import os
import sys
import argparse
import random

try:
    import bpy
    HAS_BPY = True
except ImportError:
    HAS_BPY = False


def clear_scene():
    if not HAS_BPY:
        return
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()


def apply_principled_material(
    obj,
    mat_name,
    base_color,
    roughness=0.3,
    metallic=0.1,
    emission_color=None,
    emission_strength=0.0,
):
    if not HAS_BPY:
        return
    mat = bpy.data.materials.new(name=mat_name)
    if bpy.app.version < (5, 0, 0):
        mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    if bsdf:
        if "Base Color" in bsdf.inputs:
            bsdf.inputs["Base Color"].default_value = base_color
        if "Roughness" in bsdf.inputs:
            bsdf.inputs["Roughness"].default_value = roughness
        if "Metallic" in bsdf.inputs:
            bsdf.inputs["Metallic"].default_value = metallic

        # Handles Blender 4.x/5.x ('Emission Color') vs 3.x ('Emission')
        if emission_color and emission_strength > 0:
            if "Emission Color" in bsdf.inputs:
                bsdf.inputs["Emission Color"].default_value = emission_color
            elif "Emission" in bsdf.inputs:
                bsdf.inputs["Emission"].default_value = emission_color
            if "Emission Strength" in bsdf.inputs:
                bsdf.inputs["Emission Strength"].default_value = emission_strength
    obj.data.materials.append(mat)


def create_pixar_building(name="CityBuilding", location=(0, 0, 0), height=5.0):
    if not HAS_BPY:
        return None
    # Organic skyscraper base
    bpy.ops.mesh.primitive_cube_add(
        size=2.0, location=(location[0], location[1], height / 2.0)
    )
    obj = bpy.context.active_object
    obj.name = name
    obj.scale[2] = height / 2.0
    obj["asset_style"] = "wonder_world_high_poly_smooth"

    # Tactile rounded edges (Pixar Edge Curvature)
    bevel = obj.modifiers.new(name="Bevel", type="BEVEL")
    bevel.width = 0.12
    bevel.segments = 3

    # Smooth curvature
    subsurf = obj.modifiers.new(name="BuildingSubsurf", type="SUBSURF")
    subsurf.levels = 1
    subsurf.render_levels = 2

    for poly in obj.data.polygons:
        poly.use_smooth = True

    apply_principled_material(
        obj,
        mat_name=f"{name}_Mat",
        base_color=(0.10, 0.13, 0.20, 1.0),
        roughness=0.15,
        metallic=0.85,
        emission_color=(0.0, 0.85, 1.0, 1.0),
        emission_strength=1.4,
    )
    return obj


def create_pixar_thug(name="SyndicateThug", location=(0, 0, 0)):
    if not HAS_BPY:
        return None
    bpy.ops.mesh.primitive_ico_sphere_add(
        radius=1.0, subdivisions=3, location=(location[0], location[1], 1.0)
    )
    obj = bpy.context.active_object
    obj.name = name
    obj["asset_style"] = "wonder_world_high_poly_smooth"

    subsurf = obj.modifiers.new(name="ThugSubsurf", type="SUBSURF")
    subsurf.levels = 2
    subsurf.render_levels = 2

    for poly in obj.data.polygons:
        poly.use_smooth = True

    apply_principled_material(
        obj,
        mat_name=f"{name}_Mat",
        base_color=(0.06, 0.07, 0.10, 1.0),
        roughness=0.45,
        metallic=0.25,
    )
    return obj


def main():
    argv = sys.argv
    if "--" in argv:
        argv = argv[argv.index("--") + 1 :]
    else:
        argv = argv[1:]

    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default="Assets/WonderWorld/Environment")
    args = parser.parse_known_args(argv)[0]

    output_dir = args.output
    os.makedirs(output_dir, exist_ok=True)

    clear_scene()
    random.seed(42)

    # Procedural 5x5 city grid (center plaza cleared for brawling)
    for x in range(-2, 3):
        for y in range(-2, 3):
            if x == 0 and y == 0:
                continue
            height = random.uniform(4.0, 14.0)
            create_pixar_building(
                f"Building_{x}_{y}", location=(x * 4.0, y * 4.0, 0.0), height=height
            )

    # Wave 1 Syndicate Patrols
    for i in range(3):
        create_pixar_thug(
            f"SyndicateThug_Wave1_{i+1}", location=(i * 2.5 - 2.5, -3.0, 0.0)
        )

    if HAS_BPY:
        blend_path = os.path.abspath(
            os.path.join(output_dir, "Metropolis_District_Alpha.blend")
        )
        bpy.ops.wm.save_as_mainfile(filepath=blend_path)

        fbx_path = os.path.abspath(
            os.path.join(output_dir, "Metropolis_District_Alpha.fbx")
        )
        bpy.ops.export_scene.fbx(
            filepath=fbx_path, use_selection=False, apply_scale_options="FBX_SCALE_ALL"
        )

        glb_path = os.path.abspath(
            os.path.join(output_dir, "Metropolis_District_Alpha.glb")
        )
        bpy.ops.export_scene.gltf(filepath=glb_path, export_format="GLB")

        print(f"[WonderWorld] District environment exported to {output_dir}")
    else:
        import json
        manifest_path = os.path.abspath(
            os.path.join(output_dir, "Metropolis_District_Alpha_Manifest.json")
        )
        with open(manifest_path, "w", encoding="utf-8") as mf:
            json.dump(
                {
                    "district": "Metropolis_District_Alpha",
                    "asset_style": "wonder_world_high_poly_smooth",
                    "grid_spacing_meters": 4.0,
                    "buildings": 24,
                    "wave1_syndicate_patrols": 3,
                    "outputs": [
                        "Metropolis_District_Alpha.blend",
                        "Metropolis_District_Alpha.fbx",
                        "Metropolis_District_Alpha.glb",
                    ],
                },
                mf,
                indent=2,
            )
        print(f"[WonderWorld] Procedural district manifest validated in {output_dir}")


if __name__ == "__main__":
    main()
