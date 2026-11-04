import unreal

print("--- Testing FBX import via commandlet ---")
source_path = "C:/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon/Assets/WonderWorld/Downtown/SM_Downtown_Curb_Sidewalk_Nanite.fbx"
dest_path = "/Game/WonderWorld/Meshes/Downtown"
dest_name = "SM_Downtown_Curb_Sidewalk_Nanite"

task = unreal.AssetImportTask()
task.filename = source_path
task.destination_path = dest_path
task.destination_name = dest_name
task.replace_existing = True
task.automated = True
task.save = True

options = unreal.FbxImportUI()
options.import_mesh = True
options.import_textures = False
options.import_materials = False
options.import_as_skeletal = False
options.static_mesh_import_data.combine_meshes = True
task.options = options

unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
asset = unreal.EditorAssetLibrary.load_asset(f"{dest_path}/{dest_name}")
print(f"Loaded asset: {asset}")
if asset:
    nanite = asset.get_editor_property('nanite_settings')
    nanite.set_editor_property('enabled', True)
    asset.set_editor_property('nanite_settings', nanite)
    unreal.EditorAssetLibrary.save_asset(f"{dest_path}/{dest_name}")
    print("Nanite ENABLED and SAVED successfully!")
