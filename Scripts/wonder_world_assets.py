"""Generate smooth, original stylized-3D Wonder World characters with Blender CLI.

Run:
  blender --background --python Scripts/wonder_world_assets.py -- --output Assets/WonderWorld/Characters

The script creates one .blend, .glb, and .fbx per character. It uses layered
organic forms and weighted subdivision, not block primitives, and all meshes
are flat-surfaced + smooth-shaded with bevelled contact edges.
"""
from __future__ import annotations

import argparse
import math
import os
import sys

import bpy
from mathutils import Vector

CHARACTERS = (
    ("Crimnon_Tophatter", "hat", (0.36, 0.10, 0.62), (0.10, 0.12, 0.18)),
    ("Wonder_Sound", "headphones", (0.24, 0.58, 0.88), (0.08, 0.06, 0.16)),
    ("Wonder_Warden", "helmet", (0.12, 0.20, 0.12), (0.88, 0.72, 0.18)),
)


def clear_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)


def lookscale(size):
    return Vector(size) if hasattr(size, "__len__") else Vector((size, size, size))


def material(name, color, roughness=0.42):
    mat = bpy.data.materials.new(name)
    mat.diffuse_color = (*color, 1.0)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Roughness"].default_value = roughness
    if hasattr(bsdf.inputs, "get"):
        sheen = bsdf.inputs.get("Sheen Weight") or bsdf.inputs.get("Sheen")
        if sheen:
            sheen.default_value = 0.12
    return mat


def smooth(obj, subdivisions=2):
    for polygon in obj.data.polygons:
        polygon.use_smooth = True
    bevel = obj.modifiers.new("SoftEdge", "BEVEL")
    bevel.width = 0.018
    bevel.segments = 3
    bevel.limit_method = "ANGLE"
    if hasattr(bevel, "affect"):
        bevel.affect = "EDGES"
    subd = obj.modifiers.new("OrganicSubdivision", "SUBSURF")
    subd.subdivision_type = "CATMULL_CLARK"
    subd.levels = subdivisions
    subd.render_levels = subdivisions
    obj["asset_style"] = "wonder_world_high_poly_smooth"


def add_uv(name, location, scale, mat, subdivisions=2):
    bpy.ops.mesh.primitive_uv_sphere_add(
        segments=48, ring_count=32, radius=1.0, location=location
    )
    obj = bpy.context.object
    obj.name = name
    obj.scale = lookscale(scale)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    smooth(obj, subdivisions)
    return obj


def add_cylinder(name, location, scale, mat, radius=0.5, depth=1.0):
    bpy.ops.mesh.primitive_cylinder_add(vertices=48, radius=radius, depth=depth, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.scale = lookscale(scale)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    smooth(obj, 2)
    return obj


def add_torus(name, location, major_radius, minor_radius, mat, rotation=(0, 0, 0)):
    bpy.ops.mesh.primitive_torus_add(
        major_segments=64, minor_segments=24, location=location,
        major_radius=major_radius, minor_radius=minor_radius,
        rotation=rotation,
    )
    obj = bpy.context.object
    obj.name = name
    obj.data.materials.append(mat)
    smooth(obj, 2)
    return obj


def make_character(root, name, accessory, suit, accent):
    suit_mat = material(name + "_Suit", suit, 0.38)
    skin_mat = material(name + "_WarmSkin", (0.94, 0.67, 0.50), 0.56)
    accent_mat = material(name + "_Accent", accent, 0.30)
    white_mat = material(name + "_Glints", (0.98, 0.96, 0.86), 0.24)

    # Organic, high-poly hero silhouette: weighted legs, stylized tapered torso,
    # oversized hands and a rounded head with expressive face elements.
    add_uv("Pelvis", (0, 0, 0.74), (0.50, 0.31, 0.36), suit_mat)
    torso = add_uv("Torso", (0, 0, 1.48), (0.58, 0.36, 0.72), suit_mat)
    torso.scale.z = 1.08
    bpy.context.view_layer.objects.active = torso
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    add_uv("Head", (0, 0, 2.39), (0.50, 0.43, 0.57), skin_mat, 3)
    add_uv("Nose", (0, -0.45, 2.34), (0.13, 0.10, 0.16), skin_mat, 3)
    add_uv("Left_Eye", (-0.17, -0.415, 2.48), (0.065, 0.035, 0.075), white_mat)
    add_uv("Right_Eye", (0.17, -0.415, 2.48), (0.065, 0.035, 0.075), white_mat)
    add_uv("Left_Pupil", (-0.17, -0.447, 2.48), (0.024, 0.012, 0.030), accent_mat)
    add_uv("Right_Pupil", (0.17, -0.447, 2.48), (0.024, 0.012, 0.030), accent_mat)

    for side in (-1, 1):
        add_uv("ArmUpper_%s" % side, (side * 0.73, 0.02, 1.54), (0.20, 0.20, 0.48), suit_mat)
        add_uv("Hand_%s" % side, (side * 0.82, -0.01, 1.05), (0.23, 0.14, 0.25), skin_mat)
        add_uv("Leg_%s" % side, (side * 0.29, 0.00, 0.34), (0.25, 0.26, 0.70), suit_mat)
        add_uv("Shoe_%s" % side, (side * 0.29, -0.13, -0.26), (0.28, 0.43, 0.18), accent_mat)

    if accessory == "hat":
        add_cylinder("Tophat_Crown", (0, 0, 3.03), (0.48, 0.48, 0.43), accent_mat, radius=0.63)
        add_cylinder("Tophat_Brim", (0, 0, 2.82), (1.00, 1.00, 0.09), accent_mat, radius=0.72)
        add_torus("Hat_Band", (0, 0, 2.89), 0.58, 0.055, suit_mat)
    elif accessory == "headphones":
        add_torus("Headphone_Band", (0, 0.02, 2.48), 0.49, 0.070, accent_mat)
        add_uv("Headphone_L", (-0.53, -0.03, 2.42), (0.11, 0.18, 0.23), accent_mat)
        add_uv("Headphone_R", (0.53, -0.03, 2.42), (0.11, 0.18, 0.23), accent_mat)
    else:
        add_uv("Warden_Helmet", (0, 0.01, 2.55), (0.54, 0.47, 0.43), accent_mat, 3)
        add_uv("Warden_Visor", (0, -0.43, 2.49), (0.32, 0.075, 0.12), suit_mat)

    for obj in bpy.context.scene.objects:
        obj.select_set(True)
        obj["wonder_world_character"] = name
        obj["source"] = "Wonder World asset generator"
    return bpy.context.scene.objects


def parse_args():
    argv = sys.argv
    argv = argv[argv.index("--") + 1:] if "--" in argv else []
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "Assets", "WonderWorld", "Characters")))
    return parser.parse_args(argv)


def main():
    args = parse_args()
    os.makedirs(args.output, exist_ok=True)
    manifest = []
    for name, accessory, suit, accent in CHARACTERS:
        clear_scene()
        make_character(args.output, name, accessory, suit, accent)
        path = os.path.join(args.output, name)
        bpy.ops.wm.save_as_mainfile(filepath=path + ".blend")
        bpy.ops.export_scene.gltf(filepath=path + ".glb", export_format="GLB", export_animations=True)
        bpy.ops.export_scene.fbx(filepath=path + ".fbx", apply_unit_scale=True, add_leaf_bones=False)
        manifest.append({
            "name": name,
            "blend": os.path.relpath(path + ".blend", args.output).replace(os.sep, "/"),
            "glb": os.path.relpath(path + ".glb", args.output).replace(os.sep, "/"),
            "fbx": os.path.relpath(path + ".fbx", args.output).replace(os.sep, "/"),
        })
        print("GENERATED_WONDER_ASSET", name)
    manifest_path = os.path.join(args.output, "manifest.txt")
    with open(manifest_path, "w", encoding="utf-8") as stream:
        for item in manifest:
            stream.write("{name}: {glb}\n".format(**item))
    print("WONDER_WORLD_ASSETS_READY", args.output)


if __name__ == "__main__":
    main()
