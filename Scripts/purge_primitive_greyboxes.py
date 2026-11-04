import unreal

level_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
unreal.EditorLevelLibrary.load_level(level_path)

destroyed_count = 0
for a in unreal.EditorLevelLibrary.get_all_level_actors():
    comp = a.get_component_by_class(unreal.StaticMeshComponent.static_class())
    if comp and comp.static_mesh:
        mesh_name = comp.static_mesh.get_name()
        # If it's using primitive Cube or Cylinder from the old blockout, remove it!
        if mesh_name in ["Cube", "Cylinder", "Cone", "Sphere"]:
            lbl = a.get_actor_label()
            unreal.EditorLevelLibrary.destroy_actor(a)
            destroyed_count += 1
            print(f"Removed legacy primitive actor: {lbl} (Mesh: {mesh_name})")

print(f"Total legacy primitive blockout actors purged: {destroyed_count}")
unreal.EditorLoadingAndSavingUtils.save_dirty_packages(False, True)
unreal.EditorLevelLibrary.save_current_level()
print("Saved clean downtown showcase level!")
