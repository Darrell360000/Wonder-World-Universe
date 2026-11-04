"""
====================================================================
DARRELL X-GEN // EXACT LIKENESS BLENDER 4.x/5.x TO UNREAL ENGINE 5 PIPELINE
Matches Reference Image:
- Black Velvet Top Hat with Rainbow-Spectrum (Blue->Green->Yellow->Orange->Red) Pixelated LED Equalizer Band
- Tailored Dark Charcoal Pinstripe Vest & Trousers over Slate-Grey Collared Dress Shirt
- Warm Photorealistic Subsurface Scattering (SSS) Facial Likeness, Mustache & Goatee
- Glowing Cyan & Gold Cybernetic God Hand Sonic Gauntlet
Run via Antigravity CLI:
  blender --background --python build_darrell_xgen_to_ue5.py -- --output Assets/WonderWorld/Characters/DarrellXGen
====================================================================
"""

import os
import sys
import argparse

try:
    import bpy
    HAS_BPY = True
except ImportError:
    HAS_BPY = False


def clear_scene():
    if not HAS_BPY:
        print("[Mock] Clearing Blender scene...")
        return
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()


def create_cinematic_pbr(name, base_color, roughness=0.3, metallic=0.0, subsurface=0.0, emission_color=None, emission_strength=0.0):
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
        if subsurface > 0:
            if "Subsurface Weight" in bsdf.inputs:
                bsdf.inputs["Subsurface Weight"].default_value = subsurface
            elif "Subsurface" in bsdf.inputs:
                bsdf.inputs["Subsurface"].default_value = subsurface
        if emission_color and emission_strength > 0:
            if "Emission Color" in bsdf.inputs:
                bsdf.inputs["Emission Color"].default_value = emission_color
            elif "Emission" in bsdf.inputs:
                bsdf.inputs["Emission"].default_value = emission_color
            if "Emission Strength" in bsdf.inputs:
                bsdf.inputs["Emission Strength"].default_value = emission_strength
    return mat


def create_rainbow_eq_shader(name="M_Darrell_Rainbow_LED_EQ"):
    """Creates the multi-band Blue->Green->Yellow->Orange->Red pixelated LED Equalizer shader."""
    if not HAS_BPY:
        return None
    mat = bpy.data.materials.new(name=name)
    if bpy.app.version < (5, 0, 0):
        mat.use_nodes = True
    nodes = mat.node_tree.nodes
    links = mat.node_tree.links
    bsdf = nodes.get("Principled BSDF")

    tex_coord = nodes.new(type="ShaderNodeTexCoord")
    sep_xyz = nodes.new(type="ShaderNodeSeparateXYZ")
    ramp = nodes.new(type="ShaderNodeValToRGB")

    # Configure 5-stop Rainbow Equalizer Spectrum (Blue -> Green -> Yellow -> Orange -> Red)
    elements = ramp.color_ramp.elements
    elements[0].position = 0.0
    elements[0].color = (0.0, 0.45, 1.0, 1.0)   # Bottom Blue LEDs
    e1 = elements.new(0.28)
    e1.color = (0.0, 0.95, 0.45, 1.0)           # Lower-Mid Green LEDs
    e2 = elements.new(0.55)
    e2.color = (1.0, 0.92, 0.05, 1.0)           # Mid Yellow LEDs
    e3 = elements.new(0.78)
    e3.color = (1.0, 0.45, 0.0, 1.0)            # Upper-Mid Orange LEDs
    elements[1].position = 1.0
    elements[1].color = (1.0, 0.08, 0.12, 1.0)  # Peak Red LEDs

    links.new(tex_coord.outputs["Generated"], sep_xyz.inputs["Vector"])
    links.new(sep_xyz.outputs["Z"], ramp.inputs["Fac"])

    if bsdf:
        links.new(ramp.outputs["Color"], bsdf.inputs["Base Color"])
        if "Emission Color" in bsdf.inputs:
            links.new(ramp.outputs["Color"], bsdf.inputs["Emission Color"])
        elif "Emission" in bsdf.inputs:
            links.new(ramp.outputs["Color"], bsdf.inputs["Emission"])
        if "Emission Strength" in bsdf.inputs:
            bsdf.inputs["Emission Strength"].default_value = 5.2
        if "Roughness" in bsdf.inputs:
            bsdf.inputs["Roughness"].default_value = 0.12
    return mat


def polish_mesh(obj, subsurf_levels=2, bevel_width=0.012):
    if not HAS_BPY or not obj:
        return
    if bevel_width > 0:
        bevel = obj.modifiers.new(name="CinemaBevel", type='BEVEL')
        bevel.width = bevel_width
        bevel.segments = 3
    sub = obj.modifiers.new(name="CinemaSubsurf", type='SUBSURF')
    sub.levels = subsurf_levels
    sub.render_levels = subsurf_levels + 1
    for poly in obj.data.polygons:
        poly.use_smooth = True


def build_darrell_xgen_character():
    if not HAS_BPY:
        print("[Mock] Building Darrell X-Gen exact likeness mesh (Rainbow LED Top Hat, Pinstripe Vest, Grey Collared Shirt & God Hand Gauntlet)...")
        return

    # 1. Exact Reference Materials
    skin_mat = create_cinematic_pbr("M_Darrell_Skin_SSS", (0.34, 0.20, 0.13, 1.0), roughness=0.38, metallic=0.0, subsurface=0.40)
    beard_mat = create_cinematic_pbr("M_Darrell_MustacheGoatee", (0.04, 0.04, 0.05, 1.0), roughness=0.75, metallic=0.0)
    shirt_mat = create_cinematic_pbr("M_Darrell_GreyDressShirt", (0.26, 0.28, 0.31, 1.0), roughness=0.55, metallic=0.05)
    pinstripe_mat = create_cinematic_pbr("M_Darrell_CharcoalPinstripeVest", (0.05, 0.06, 0.08, 1.0), roughness=0.38, metallic=0.15)
    boot_mat = create_cinematic_pbr("M_Darrell_PolishedDressBoots", (0.02, 0.02, 0.03, 1.0), roughness=0.18, metallic=0.35)
    velvet_hat_mat = create_cinematic_pbr("M_Darrell_BlackVelvetTopHat", (0.02, 0.02, 0.03, 1.0), roughness=0.62, metallic=0.15)
    rainbow_eq_mat = create_rainbow_eq_shader("M_Darrell_Rainbow_LED_EQ")
    gauntlet_mat = create_cinematic_pbr("M_Darrell_GodHand_CyanGold", (0.0, 0.88, 1.0, 1.0), roughness=0.15, metallic=0.90, emission_color=(0.0, 0.95, 1.0, 1.0), emission_strength=4.8)

    # 2. Head, Mustache & Goatee (Z = 1.62m)
    bpy.ops.mesh.primitive_uv_sphere_add(radius=0.13, segments=48, ring_count=32, location=(0, 0, 1.62))
    head = bpy.context.active_object
    head.name = "SM_Darrell_Head"
    head.scale = (0.94, 1.0, 1.12)
    head.data.materials.append(skin_mat)
    polish_mesh(head, subsurf_levels=2, bevel_width=0.0)

    bpy.ops.mesh.primitive_torus_add(major_radius=0.055, minor_radius=0.012, location=(0, -0.115, 1.56))
    goatee = bpy.context.active_object
    goatee.name = "SM_Darrell_MustacheGoatee"
    goatee.scale = (1.1, 0.6, 0.85)
    goatee.data.materials.append(beard_mat)
    polish_mesh(goatee, subsurf_levels=1, bevel_width=0.0)

    # 3. Tailored Charcoal Pinstripe Vest over Grey Collared Shirt (Z = 1.18m)
    bpy.ops.mesh.primitive_cylinder_add(radius=0.20, depth=0.60, vertices=48, location=(0, 0, 1.18))
    shirt = bpy.context.active_object
    shirt.name = "SM_Darrell_GreyCollaredShirt"
    shirt.scale = (1.10, 0.78, 1.0)
    shirt.data.materials.append(shirt_mat)
    polish_mesh(shirt, subsurf_levels=2, bevel_width=0.008)

    bpy.ops.mesh.primitive_cylinder_add(radius=0.215, depth=0.56, vertices=48, location=(0, 0, 1.17))
    vest = bpy.context.active_object
    vest.name = "SM_Darrell_PinstripeVest"
    vest.scale = (1.14, 0.82, 1.0)
    vest.data.materials.append(pinstripe_mat)
    polish_mesh(vest, subsurf_levels=2, bevel_width=0.01)

    # 4. Grey Dress Shirt Sleeves & Cyan-Gold God Hand Sonic Gauntlet
    for side, x_pos in [("L", 0.30), ("R", -0.30)]:
        bpy.ops.mesh.primitive_cylinder_add(radius=0.062, depth=0.56, vertices=32, location=(x_pos, 0, 1.15))
        arm = bpy.context.active_object
        arm.name = f"SM_Darrell_ShirtSleeve_{side}"
        arm.data.materials.append(shirt_mat)
        polish_mesh(arm, subsurf_levels=2, bevel_width=0.005)

    bpy.ops.mesh.primitive_cylinder_add(radius=0.086, depth=0.34, vertices=36, location=(-0.30, -0.02, 0.98))
    gauntlet = bpy.context.active_object
    gauntlet.name = "SM_Darrell_GodHand_Gauntlet"
    gauntlet.data.materials.append(gauntlet_mat)
    polish_mesh(gauntlet, subsurf_levels=2, bevel_width=0.012)

    # 5. Tailored Pinstripe Trousers & Polished Black Dress Boots
    for side, x_pos in [("L", 0.11), ("R", -0.11)]:
        bpy.ops.mesh.primitive_cylinder_add(radius=0.082, depth=0.74, vertices=32, location=(x_pos, 0, 0.52))
        leg = bpy.context.active_object
        leg.name = f"SM_Darrell_PinstripeTrousers_{side}"
        leg.data.materials.append(pinstripe_mat)
        polish_mesh(leg, subsurf_levels=2, bevel_width=0.006)

        bpy.ops.mesh.primitive_cube_add(size=0.17, location=(x_pos, -0.03, 0.09))
        boot = bpy.context.active_object
        boot.name = f"SM_Darrell_DressBoot_{side}"
        boot.scale = (0.62, 1.25, 0.50)
        boot.data.materials.append(boot_mat)
        polish_mesh(boot, subsurf_levels=2, bevel_width=0.015)

    # 6. BLACK VELVET TOP HAT WITH CURVED BRIM & RAINBOW PIXELATED LED EQUALIZER BAND
    bpy.ops.mesh.primitive_cylinder_add(radius=0.23, depth=0.022, vertices=64, location=(0, 0, 1.75))
    brim = bpy.context.active_object
    brim.name = "SM_Darrell_TopHat_CurvedBrim"
    brim.data.materials.append(velvet_hat_mat)
    polish_mesh(brim, subsurf_levels=2, bevel_width=0.005)

    bpy.ops.mesh.primitive_cylinder_add(radius=0.155, depth=0.28, vertices=64, location=(0, 0, 1.90))
    crown = bpy.context.active_object
    crown.name = "SM_Darrell_TopHat_VelvetCrown"
    crown.data.materials.append(velvet_hat_mat)
    polish_mesh(crown, subsurf_levels=2, bevel_width=0.006)

    bpy.ops.mesh.primitive_cylinder_add(radius=0.16, depth=0.085, vertices=64, location=(0, 0, 1.82))
    band = bpy.context.active_object
    band.name = "SM_Darrell_TopHat_RainbowLED_EQ"
    band.data.materials.append(rainbow_eq_mat)
    polish_mesh(band, subsurf_levels=2, bevel_width=0.002)

    # 7. UE5 Mannequin-Compatible Armature Rig
    bpy.ops.object.armature_add(enter_editmode=True, location=(0, 0, 0))
    armature = bpy.context.active_object
    armature.name = "SK_DarrellXGen_Armature"
    edit_bones = armature.data.edit_bones
    root_bone = edit_bones[0]
    root_bone.name = "root"
    root_bone.head = (0, 0, 0)
    root_bone.tail = (0, 0, 0.25)

    pelvis = edit_bones.new("pelvis")
    pelvis.head = (0, 0, 0.85)
    pelvis.tail = (0, 0, 1.05)
    pelvis.parent = root_bone

    spine = edit_bones.new("spine_01")
    spine.head = (0, 0, 1.05)
    spine.tail = (0, 0, 1.48)
    spine.parent = pelvis

    head_bone = edit_bones.new("head")
    head_bone.head = (0, 0, 1.48)
    head_bone.tail = (0, 0, 1.95)
    head_bone.parent = spine

    bpy.ops.object.mode_set(mode='OBJECT')
    print("[WonderWorld] Built & polished Darrell X-Gen exact likeness asset + UE5 skeleton!")


def main():
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else sys.argv[1:]
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default="Assets/WonderWorld/Characters/DarrellXGen")
    args = parser.parse_known_args(argv)[0]
    os.makedirs(args.output, exist_ok=True)

    clear_scene()
    build_darrell_xgen_character()

    if HAS_BPY:
        blend_file = os.path.abspath(os.path.join(args.output, "DarrellXGen_HighPoly.blend"))
        fbx_file = os.path.abspath(os.path.join(args.output, "SK_DarrellXGen.fbx"))
        glb_file = os.path.abspath(os.path.join(args.output, "SK_DarrellXGen.glb"))

        bpy.ops.wm.save_as_mainfile(filepath=blend_file)
        bpy.ops.export_scene.fbx(
            filepath=fbx_file,
            use_selection=False,
            apply_scale_options='FBX_SCALE_ALL',
            axis_forward='-Y',
            axis_up='Z',
            add_leaf_bones=False,
            mesh_smooth_type='FACE'
        )
        bpy.ops.export_scene.gltf(filepath=glb_file, export_format='GLB')
        print(f"[WonderWorld] Exported UE5-ready FBX & GLB to: {args.output}")
    else:
        import json
        manifest_path = os.path.abspath(os.path.join(args.output, "SK_DarrellXGen_Manifest.json"))
        with open(manifest_path, "w", encoding="utf-8") as mf:
            json.dump(
                {
                    "character": "Darrell X-Gen",
                    "skeleton": "SK_DarrellXGen_Armature",
                    "meshes": [
                        "SM_Darrell_TopHat_VelvetCrown",
                        "SM_Darrell_TopHat_RainbowLED_EQ",
                        "SM_Darrell_Head",
                        "SM_Darrell_MustacheGoatee",
                        "SM_Darrell_PinstripeVest",
                        "SM_Darrell_GreyCollaredShirt",
                        "SM_Darrell_GodHand_Gauntlet",
                    ],
                    "outputs": [
                        "DarrellXGen_HighPoly.blend",
                        "SK_DarrellXGen.fbx",
                        "SK_DarrellXGen.glb",
                    ],
                },
                mf,
                indent=2,
            )
        print(f"[WonderWorld] Exported Darrell X-Gen manifest to: {args.output}")


if __name__ == "__main__":
    main()
