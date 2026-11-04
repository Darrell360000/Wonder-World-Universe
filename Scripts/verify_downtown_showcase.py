import unreal
import json

out_path = "C:/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon/Saved/downtown_verify_report.json"
report = {
    "actors": [],
    "meshes": {},
    "materials": {}
}

level_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
unreal.EditorLevelLibrary.load_level(level_path)

actors = unreal.EditorLevelLibrary.get_all_level_actors()
for a in actors:
    lbl = a.get_actor_label()
    cls_name = a.get_class().get_name()
    loc = a.get_actor_location()
    comp = a.get_component_by_class(unreal.StaticMeshComponent.static_class())
    mesh_name = comp.static_mesh.get_name() if comp and comp.static_mesh else "None"
    report["actors"].append({
        "label": lbl,
        "class": cls_name,
        "location": [loc.x, loc.y, loc.z],
        "mesh": mesh_name
    })

meshes = [
    'SM_Downtown_Avenue_Nanite',
    'SM_Downtown_Curb_Sidewalk_Nanite',
    'SM_Downtown_Building_Facade_Nanite',
    'SM_WonderHover_Chassis_Nanite'
]
for m_name in meshes:
    m = unreal.EditorAssetLibrary.load_asset(f"/Game/WonderWorld/Meshes/Downtown/{m_name}")
    if m:
        nanite = m.get_editor_property('nanite_settings')
        enabled = nanite.get_editor_property('enabled')
        report["meshes"][m_name] = {"found": True, "nanite_enabled": enabled}
    else:
        report["meshes"][m_name] = {"found": False, "nanite_enabled": False}

for mat_name in ['M_Photoreal_WetAsphalt_Nanite', 'M_Photoreal_Sidewalk_Nanite']:
    mat = unreal.EditorAssetLibrary.load_asset(f"/Game/WonderWorld/Materials/Downtown/{mat_name}")
    report["materials"][mat_name] = (mat is not None)

with open(out_path, "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)

unreal.log("VERIFY_REPORT_WRITTEN_TO_DISK")
