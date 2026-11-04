import unreal

src = "/Game/ThirdPerson/Lvl_ThirdPerson"
dst = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"

if unreal.EditorAssetLibrary.does_asset_exist(dst):
    unreal.EditorAssetLibrary.delete_asset(dst)

success = unreal.EditorAssetLibrary.duplicate_asset(src, dst)
print(f"DUPLICATE_RESULT: {success}")
