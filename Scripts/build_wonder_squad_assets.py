"""
Wonder World Universe: Top-Hatter Squad 3D Asset Builder
========================================================
Headless Blender CLI script to generate 3D meshes, materials, and exports for:
1. Darrell X-Gen (Equalizer X-Hat + Wand)
2. Minime Tophatter (Micro Topper)
3. Spider-Pulse (Acrobatic Pulse Hat + Gauntlets)
4. Divine Hands (God's Hands Gold Crown)
5. Jaxson Thorne (Anvil Crown + Hazard Lights + Gauntlets)
6. Tao X Zen (Wonder Hat + Cosmic Portal Sigils)
7. Mama Wonder (Empress Crown + Scepter)
"""

import sys
import bpy
from pathlib import Path

def reset_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)

def create_pbr_material(name, base_color, metallic=0.0, roughness=0.5, emission=None):
    mat = bpy.data.materials.new(name=name)
    nodes = mat.node_tree.nodes
    bsdf = nodes.get("Principled BSDF")
    if bsdf:
        bsdf.inputs["Base Color"].default_value = base_color
        bsdf.inputs["Metallic"].default_value = metallic
        bsdf.inputs["Roughness"].default_value = roughness
        if emission:
            bsdf.inputs["Emission Color"].default_value = emission
            bsdf.inputs["Emission Strength"].default_value = 5.0
    return mat

def build_tophat(name, height=0.4, radius=0.25, brim_radius=0.42, mat_body=None, mat_band=None):
    # Cylinder brim
    bpy.ops.mesh.primitive_cylinder_add(radius=brim_radius, depth=0.04, location=(0, 0, 0.02))
    brim = bpy.context.active_object
    brim.name = f"{name}_Brim"
    if mat_body:
        brim.data.materials.append(mat_body)

    # Cylinder crown
    bpy.ops.mesh.primitive_cylinder_add(radius=radius, depth=height, location=(0, 0, height/2 + 0.04))
    crown = bpy.context.active_object
    crown.name = f"{name}_Crown"
    if mat_body:
        crown.data.materials.append(mat_body)

    # Band
    bpy.ops.mesh.primitive_cylinder_add(radius=radius * 1.02, depth=0.08, location=(0, 0, 0.08))
    band = bpy.context.active_object
    band.name = f"{name}_Band"
    if mat_band:
        band.data.materials.append(mat_band)

    return [brim, crown, band]

def main():
    output_dir = Path("Assets/WonderWorld/Characters/SquadMeshes")
    output_dir.mkdir(parents=True, exist_ok=True)

    print("=== [BLENDER] Building Wonder World 7-Hero Top-Hatter Roster Assets ===")

    heroes_config = [
        ("Darrell_Equalizer_Hat", (0.02, 0.02, 0.02, 1.0), 0.1, 0.85, None, (0.0, 0.8, 1.0, 1.0), 0.42, 0.26, 0.45),
        ("Minime_Equalizer_Hat", (0.02, 0.02, 0.02, 1.0), 0.1, 0.85, None, (0.0, 0.8, 1.0, 1.0), 0.15, 0.09, 0.16),
        ("SpiderPulse_TopHat", (0.02, 0.02, 0.02, 1.0), 0.1, 0.85, None, (1.0, 0.2, 0.6, 1.0), 0.38, 0.24, 0.42),
        ("DivineHands_CrownHat", (1.0, 0.82, 0.2, 1.0), 0.9, 0.25, None, (0.2, 0.9, 1.0, 1.0), 0.45, 0.28, 0.48),
        ("Jaxson_AnvilCrown_Hat", (0.15, 0.15, 0.17, 1.0), 0.85, 0.4, None, (1.0, 0.6, 0.0, 1.0), 0.50, 0.30, 0.52),
        ("TaoXZen_WonderHat", (0.02, 0.02, 0.02, 1.0), 0.1, 0.85, None, (0.55, 0.0, 0.95, 1.0), 0.44, 0.25, 0.46),
        ("MamaWonder_EmpressHat", (1.0, 0.82, 0.2, 1.0), 0.9, 0.25, None, (0.0, 0.8, 1.0, 1.0), 0.46, 0.28, 0.48)
    ]

    for name, b_col, b_met, b_rough, b_emis, band_emis, h, r, br in heroes_config:
        reset_scene()
        m_body = create_pbr_material(f"M_{name}_Body", b_col, metallic=b_met, roughness=b_rough, emission=b_emis)
        m_band = create_pbr_material(f"M_{name}_Band", band_emis, metallic=0.0, roughness=0.5, emission=band_emis)

        parts = build_tophat(name, height=h, radius=r, brim_radius=br, mat_body=m_body, mat_band=m_band)
        
        # Select all and join
        for p in parts:
            p.select_set(True)
        bpy.context.view_layer.objects.active = parts[0]
        bpy.ops.object.join()
        obj = bpy.context.active_object
        obj.name = f"SM_{name}"

        # Export GLB
        glb_path = output_dir / f"SM_{name}.glb"
        bpy.ops.export_scene.gltf(filepath=str(glb_path), export_format='GLB')
        print(f"  * Exported: {glb_path}")

    print("=== [BLENDER] Top-Hatter Squad 3D Assets Generated Successfully ===")

if __name__ == "__main__":
    main()
