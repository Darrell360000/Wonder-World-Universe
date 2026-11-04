"""
Blender 5.2.3 CLI Mesh Generator for Nanite Downtown City Block
Generates high-poly, beveled, UV-unwrapped meshes:
- SM_Downtown_Avenue_Nanite
- SM_Downtown_Curb_Sidewalk_Nanite
- SM_Downtown_Building_Facade_Nanite
- SM_WonderHover_Chassis_Nanite
"""

import bpy
import os
import math

def clear_scene():
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()

def ensure_uv(obj):
    if not obj.data.uv_layers:
        obj.data.uv_layers.new(name="UVMap")
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=66.0, island_margin=0.02)
    bpy.ops.object.mode_set(mode='OBJECT')

def create_nanite_avenue(out_path):
    clear_scene()
    
    # 4-Lane Avenue Deck: 16m wide (X), 40m long (Y), 0.3m thick (Z)
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(0, 0, 0))
    road = bpy.context.active_object
    road.name = "SM_Downtown_Avenue_Nanite"
    road.scale = (16.0, 40.0, 0.3)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    # Subdivide along length for natural micro-undulations and vertex density
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.subdivide(number_cuts=8)
    bpy.ops.object.mode_set(mode='OBJECT')

    # Crown slope modifier (drainage toward curbs)
    for v in road.data.vertices:
        if v.co.z > 0.05:
            # Slight crown elevation in center
            dist_from_center = abs(v.co.x) / 8.0
            v.co.z += (1.0 - dist_from_center) * 0.06

    # Bevel on edges
    bevel = road.modifiers.new(name="RoadBevel", type='BEVEL')
    bevel.width = 0.05
    bevel.segments = 4
    bpy.ops.object.modifier_apply(modifier="RoadBevel")

    ensure_uv(road)
    bpy.ops.export_scene.fbx(filepath=out_path, use_selection=True, axis_forward='-Z', axis_up='Y')
    print(f"[Blender] Exported Avenue: {out_path}")

def create_nanite_curb_sidewalk(out_path):
    clear_scene()

    # Curb + Sidewalk Promenade: 8m wide (X), 40m long (Y)
    # Curb stone: 0.35m wide, 0.25m high
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(-0.175, 0, 0.125))
    curb = bpy.context.active_object
    curb.name = "Curb_Part"
    curb.scale = (0.35, 40.0, 0.25)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    # Smooth bevel on curb top edge
    bevel = curb.modifiers.new(name="CurbBevel", type='BEVEL')
    bevel.width = 0.04
    bevel.segments = 5
    bpy.ops.object.modifier_apply(modifier="CurbBevel")

    # Sidewalk slab: 7.65m wide, 40m long, 0.2m high
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(-4.175, 0, 0.1))
    sidewalk = bpy.context.active_object
    sidewalk.name = "Sidewalk_Part"
    sidewalk.scale = (7.65, 40.0, 0.2)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    # Join curb and sidewalk
    curb.select_set(True)
    sidewalk.select_set(True)
    bpy.context.view_layer.objects.active = sidewalk
    bpy.ops.object.join()
    combined = bpy.context.active_object
    combined.name = "SM_Downtown_Curb_Sidewalk_Nanite"

    # Subtle bevel on sidewalk outer edges
    bevel2 = combined.modifiers.new(name="SWBevel", type='BEVEL')
    bevel2.width = 0.02
    bevel2.segments = 3
    bpy.ops.object.modifier_apply(modifier="SWBevel")

    ensure_uv(combined)
    bpy.ops.export_scene.fbx(filepath=out_path, use_selection=True, axis_forward='-Z', axis_up='Y')
    print(f"[Blender] Exported Curb & Sidewalk: {out_path}")

def create_nanite_storefront(out_path):
    clear_scene()

    # High-rise Storefront Base: 14m wide (X), 18m deep (Y), 22m high (Z)
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(0, 0, 11.0))
    building = bpy.context.active_object
    building.name = "SM_Downtown_Building_Facade_Nanite"
    building.scale = (14.0, 18.0, 22.0)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    # Subdivide facade
    bevel = building.modifiers.new(name="FacadeBevel", type='BEVEL')
    bevel.width = 0.12
    bevel.segments = 4
    bpy.ops.object.modifier_apply(modifier="FacadeBevel")

    # Recessed Storefront Entrance Cavity
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(0, -8.6, 2.5))
    cavity = bpy.context.active_object
    cavity.scale = (10.0, 2.0, 4.5)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    # Boolean cut
    bool_mod = building.modifiers.new(name="StorefrontCut", type='BOOLEAN')
    bool_mod.object = cavity
    bool_mod.operation = 'DIFFERENCE'
    bpy.context.view_layer.objects.active = building
    bpy.ops.object.modifier_apply(modifier="StorefrontCut")
    bpy.data.objects.remove(cavity)

    # Cornice crown molding atop ground floor
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(0, -9.1, 5.0))
    cornice = bpy.context.active_object
    cornice.scale = (14.4, 0.4, 0.4)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    c_bev = cornice.modifiers.new(name="CBev", type='BEVEL')
    c_bev.width = 0.05
    c_bev.segments = 3
    bpy.ops.object.modifier_apply(modifier="CBev")

    # Join
    building.select_set(True)
    cornice.select_set(True)
    bpy.context.view_layer.objects.active = building
    bpy.ops.object.join()

    ensure_uv(building)
    bpy.ops.export_scene.fbx(filepath=out_path, use_selection=True, axis_forward='-Z', axis_up='Y')
    print(f"[Blender] Exported Storefront: {out_path}")

def create_nanite_hover_vehicle(out_path):
    clear_scene()

    # Sleek Cyber Hover Chassis: 2.4m wide (X), 5.2m long (Y), 0.9m high (Z)
    bpy.ops.mesh.primitive_cube_add(size=1.0, location=(0, 0, 0.5))
    body = bpy.context.active_object
    body.name = "SM_WonderHover_Chassis_Nanite"
    body.scale = (2.4, 5.2, 0.8)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    # Aerodynamic bevel & subdivision
    bev = body.modifiers.new(name="BodyBevel", type='BEVEL')
    bev.width = 0.18
    bev.segments = 5
    bpy.ops.object.modifier_apply(modifier="BodyBevel")

    # Canopy
    bpy.ops.mesh.primitive_cylinder_add(radius=0.7, depth=2.4, location=(0, -0.2, 0.95))
    canopy = bpy.context.active_object
    canopy.rotation_euler = (math.radians(90), 0, 0)
    canopy.scale = (1.1, 0.9, 0.6)
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)

    # 4 Thruster Nacelles
    nacelles = []
    pod_positions = [(-1.0, -1.8, 0.3), (1.0, -1.8, 0.3), (-1.0, 1.8, 0.3), (1.0, 1.8, 0.3)]
    for pos in pod_positions:
        bpy.ops.mesh.primitive_cylinder_add(radius=0.38, depth=0.8, location=pos)
        pod = bpy.context.active_object
        pod_bev = pod.modifiers.new(name="PodBevel", type='BEVEL')
        pod_bev.width = 0.05
        pod_bev.segments = 4
        bpy.ops.object.modifier_apply(modifier="PodBevel")
        nacelles.append(pod)

    # Join all vehicle parts
    for n in nacelles:
        n.select_set(True)
    canopy.select_set(True)
    body.select_set(True)
    bpy.context.view_layer.objects.active = body
    bpy.ops.object.join()

    # Smooth shading
    for p in body.data.polygons:
        p.use_smooth = True

    ensure_uv(body)
    bpy.ops.export_scene.fbx(filepath=out_path, use_selection=True, axis_forward='-Z', axis_up='Y')
    print(f"[Blender] Exported Hover Vehicle: {out_path}")

def main():
    out_dir = r"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Assets\WonderWorld\Downtown"
    os.makedirs(out_dir, exist_ok=True)

    create_nanite_avenue(os.path.join(out_dir, "SM_Downtown_Avenue_Nanite.fbx"))
    create_nanite_curb_sidewalk(os.path.join(out_dir, "SM_Downtown_Curb_Sidewalk_Nanite.fbx"))
    create_nanite_storefront(os.path.join(out_dir, "SM_Downtown_Building_Facade_Nanite.fbx"))
    create_nanite_hover_vehicle(os.path.join(out_dir, "SM_WonderHover_Chassis_Nanite.fbx"))
    print("[Blender] All Nanite Downtown meshes generated successfully.")

if __name__ == "__main__":
    main()
