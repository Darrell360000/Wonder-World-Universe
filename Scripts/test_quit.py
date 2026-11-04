import unreal
import sys

unreal.log(">>> SCRIPT_RUNNING_OK <<<")
print(">>> SCRIPT_PRINT_OK <<<")

src = "/Game/ThirdPerson/Lvl_ThirdPerson"
dst = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"

tools = unreal.AssetToolsHelpers.get_asset_tools()
res = unreal.EditorAssetLibrary.duplicate_asset(src, dst)
unreal.log(f">>> DUPLICATE_RESULT: {res} <<<")

unreal.SystemLibrary.quit_editor()
sys.exit(0)
