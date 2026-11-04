"""
Wonder Command Blender 4.x / 5.x AI Add-on (wonder_blender_ai_addon.py)
- 1. Build LOD0 (100%) / LOD1 (50%) / LOD2 (25%) + UCX_ Convex Collision Hull
- 2. Dual Export FBX:
  * 02_Shared_Game_Core/Exports_UE58_FBX (-Y Forward, Z Up, 1 unit = 1 cm)
  * 02_Shared_Game_Core/Exports_Unity_FBX (-Z Forward, Y Up, 1 unit = 1 m)
"""

bl_info = {
    "name": "Wonder Command AI Pipeline Add-on",
    "author": "Darrell X-Gen // Wonder Command v3.0",
    "version": (3, 0, 0),
    "blender": (4, 0, 0),
    "location": "View3D > Sidebar (N) > Wonder Command AI",
    "description": "Auto-LOD0/1/2 + UCX_ Collision + Dual UE5.8 & Unity/CS2 FBX Exporter",
    "category": "Import-Export",
}

import os

try:
    import bpy
    HAS_BPY = True
except ImportError:
    HAS_BPY = False


def build_lod_chain_and_ucx(obj):
    if not HAS_BPY or obj is None or obj.type != 'MESH':
        return []
    base_name = obj.name
    created = [obj]
    for lod_idx, ratio in ((1, 0.5), (2, 0.25)):
        dup = obj.copy()
        dup.data = obj.data.copy()
        dup.name = f"{base_name}_LOD{lod_idx}"
        bpy.context.collection.objects.link(dup)
        dec = dup.modifiers.new(name=f"LOD{lod_idx}_Decimate", type='DECIMATE')
        dec.ratio = ratio
        created.append(dup)

    ucx = obj.copy()
    ucx.data = obj.data.copy()
    ucx.name = f"UCX_{base_name}"
    bpy.context.collection.objects.link(ucx)
    dec_ucx = ucx.modifiers.new(name="UCX_Hull", type='DECIMATE')
    dec_ucx.ratio = 0.12
    created.append(ucx)
    return created


def dual_export_fbx(pipeline_root):
    if not HAS_BPY:
        return
    ue_dir = os.path.join(pipeline_root, "02_Shared_Game_Core", "Exports_UE58_FBX")
    unity_dir = os.path.join(pipeline_root, "02_Shared_Game_Core", "Exports_Unity_FBX")
    os.makedirs(ue_dir, exist_ok=True)
    os.makedirs(unity_dir, exist_ok=True)

    bpy.ops.export_scene.fbx(
        filepath=os.path.join(ue_dir, "WonderWorld_UE58.fbx"),
        axis_forward='-Y',
        axis_up='Z',
        apply_scale_options='FBX_SCALE_NONE',
    )
    bpy.ops.export_scene.fbx(
        filepath=os.path.join(unity_dir, "WonderWorld_UnityCS2.fbx"),
        axis_forward='-Z',
        axis_up='Y',
        apply_scale_options='FBX_SCALE_UNITS',
    )


if __name__ == "__main__":
    print("[Wonder Command AI Add-on] Loaded and ready.")
