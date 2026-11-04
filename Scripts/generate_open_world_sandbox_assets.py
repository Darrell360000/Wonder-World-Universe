import os
import sys
import argparse
import math

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


def make_mat(name, base_color, roughness=0.25, metallic=0.8, emission_color=None, emission_strength=0.0):
    if not HAS_BPY:
        return None
    mat = bpy.data.materials.new(name=name)
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
        if emission_color and emission_strength > 0:
            if "Emission Color" in bsdf.inputs:
                bsdf.inputs["Emission Color"].default_value = emission_color
            elif "Emission" in bsdf.inputs:
                bsdf.inputs["Emission"].default_value = emission_color
            if "Emission Strength" in bsdf.inputs:
                bsdf.inputs["Emission Strength"].default_value = emission_strength
    return mat


def create_street_cruiser(location=(-6.0, -6.0, 0.6)):
    if not HAS_BPY:
        return None
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=location)
    body = bpy.context.active_object
    body.name = "SM_SoundMagic_StreetCruiser"
    body.scale = (2.2, 4.6, 0.65)
    bevel = body.modifiers.new(name="CruiserBevel", type="BEVEL")
    bevel.width = 0.08
    bevel.segments = 3
    subsurf = body.modifiers.new(name="CruiserSubsurf", type="SUBSURF")
    subsurf.levels = 1
    for p in body.data.polygons:
        p.use_smooth = True
    mat = make_mat(
        "M_SoundMagic_Cruiser",
        base_color=(0.04, 0.25, 0.85, 1.0),
        roughness=0.12,
        metallic=0.92,
        emission_color=(0.0, 0.90, 1.0, 1.0),
        emission_strength=2.2,
    )
    body.data.materials.append(mat)
    return body


def create_flying_car(location=(6.0, -6.0, 6.0)):
    if not HAS_BPY:
        return None
    bpy.ops.mesh.primitive_uv_sphere_add(segments=32, ring_count=16, radius=1.0, location=location)
    hull = bpy.context.active_object
    hull.name = "SM_PurpleCity_FlyingCar"
    hull.scale = (2.1, 4.2, 0.75)
    subsurf = hull.modifiers.new(name="FlyingCarSubsurf", type="SUBSURF")
    subsurf.levels = 2
    for p in hull.data.polygons:
        p.use_smooth = True
    mat = make_mat(
        "M_PurpleCity_FlyingCar",
        base_color=(0.14, 0.05, 0.28, 1.0),
        roughness=0.10,
        metallic=0.95,
        emission_color=(0.75, 0.15, 1.0, 1.0),
        emission_strength=3.0,
    )
    hull.data.materials.append(mat)

    # 4 Hover Thruster Pods
    thruster_mat = make_mat(
        "M_Thruster_CyanGlow",
        base_color=(0.0, 0.9, 1.0, 1.0),
        roughness=0.1,
        metallic=0.9,
        emission_color=(0.0, 0.95, 1.0, 1.0),
        emission_strength=4.5,
    )
    offsets = [(-1.8, -1.6, -0.2), (1.8, -1.6, -0.2), (-1.8, 1.6, -0.2), (1.8, 1.6, -0.2)]
    for idx, (ox, oy, oz) in enumerate(offsets):
        bpy.ops.mesh.primitive_cylinder_add(
            vertices=24,
            radius=0.55,
            depth=0.35,
            location=(location[0] + ox, location[1] + oy, location[2] + oz),
        )
        pod = bpy.context.active_object
        pod.name = f"SM_FlyingCar_Thruster_{idx+1}"
        pod.data.materials.append(thruster_mat)
        pod.parent = hull
    return hull


def create_twin_towers_and_skypads():
    if not HAS_BPY:
        return
    # Mama Wonder Tower (Left) + Sky-Pad Helipad
    bpy.ops.mesh.primitive_cylinder_add(vertices=48, radius=5.0, depth=28.0, location=(-14.0, 14.0, 14.0))
    mama = bpy.context.active_object
    mama.name = "SM_MamaWonderTower_OpenWorld"
    for p in mama.data.polygons:
        p.use_smooth = True
    mama.data.materials.append(
        make_mat(
            "M_MamaTower_BioNeon",
            base_color=(0.06, 0.18, 0.28, 1.0),
            roughness=0.15,
            metallic=0.88,
            emission_color=(0.0, 0.92, 1.0, 1.0),
            emission_strength=2.0,
        )
    )

    bpy.ops.mesh.primitive_cylinder_add(vertices=48, radius=6.2, depth=0.6, location=(-14.0, 14.0, 28.4))
    mama_pad = bpy.context.active_object
    mama_pad.name = "SM_MamaTower_FlyingCar_SkyPad"
    mama_pad.data.materials.append(
        make_mat(
            "M_SkyPad_LandingRing",
            base_color=(0.02, 0.85, 0.65, 1.0),
            roughness=0.2,
            metallic=0.8,
            emission_color=(0.0, 1.0, 0.75, 1.0),
            emission_strength=3.2,
        )
    )

    # Papa Wonder Tower (Right) + Sky-Pad Helipad
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(14.0, 14.0, 15.0))
    papa = bpy.context.active_object
    papa.name = "SM_PapaWonderTower_OpenWorld"
    papa.scale = (4.8, 4.8, 15.0)
    bevel = papa.modifiers.new(name="PapaBevel", type="BEVEL")
    bevel.width = 0.15
    bevel.segments = 3
    for p in papa.data.polygons:
        p.use_smooth = True
    papa.data.materials.append(
        make_mat(
            "M_PapaTower_ObsidianGold",
            base_color=(0.12, 0.09, 0.03, 1.0),
            roughness=0.14,
            metallic=0.94,
            emission_color=(1.0, 0.78, 0.15, 1.0),
            emission_strength=2.4,
        )
    )

    bpy.ops.mesh.primitive_cylinder_add(vertices=48, radius=6.0, depth=0.6, location=(14.0, 14.0, 30.4))
    papa_pad = bpy.context.active_object
    papa_pad.name = "SM_PapaTower_FlyingCar_SkyPad"
    papa_pad.data.materials.append(
        make_mat(
            "M_PapaSkyPad_GoldRing",
            base_color=(1.0, 0.78, 0.15, 1.0),
            roughness=0.2,
            metallic=0.9,
            emission_color=(1.0, 0.82, 0.2, 1.0),
            emission_strength=3.2,
        )
    )


def main():
    argv = sys.argv
    if "--" in argv:
        argv = argv[argv.index("--") + 1 :]
    else:
        argv = []

    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default="Assets/WonderWorld/OpenWorld")
    args = parser.parse_known_args(argv)[0]

    os.makedirs(args.output, exist_ok=True)
    clear_scene()

    create_street_cruiser()
    create_flying_car()
    create_twin_towers_and_skypads()

    if HAS_BPY:
        blend_path = os.path.abspath(os.path.join(args.output, "WonderWorld_OpenWorld_Sandbox.blend"))
        fbx_path = os.path.abspath(os.path.join(args.output, "WonderWorld_OpenWorld_Sandbox.fbx"))
        glb_path = os.path.abspath(os.path.join(args.output, "WonderWorld_OpenWorld_Sandbox.glb"))

        bpy.ops.wm.save_as_mainfile(filepath=blend_path)
        bpy.ops.export_scene.fbx(filepath=fbx_path, use_selection=False, apply_scale_options="FBX_SCALE_ALL")
        bpy.ops.export_scene.gltf(filepath=glb_path, export_format="GLB")
        print(f"[WonderWorld] Open-World Vehicles, Flying Cars & Twin Tower Sky-Pads exported to {args.output}")


if __name__ == "__main__":
    main()
