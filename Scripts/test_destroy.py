import unreal

level_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
unreal.EditorLevelLibrary.load_level(level_path)

all_acts = list(unreal.EditorLevelLibrary.get_all_level_actors())
unreal.log(f"BEFORE_PURGE_COUNT: {len(all_acts)}")

to_destroy = []
for a in all_acts:
    comp = a.get_component_by_class(unreal.StaticMeshComponent.static_class())
    if comp and comp.static_mesh:
        m_name = comp.static_mesh.get_name()
        if m_name in ["Cube", "Cylinder", "Cone", "Sphere"]:
            to_destroy.append(a)

unreal.log(f"TO_DESTROY_COUNT: {len(to_destroy)}")
destroyed = 0
for a in to_destroy:
    lbl = a.get_actor_label()
    res = unreal.EditorLevelLibrary.destroy_actor(a)
    if res:
        destroyed += 1
    else:
        # Fallback to World.editor_destroy_actor or a.destroy_actor()
        world = unreal.EditorLevelLibrary.get_editor_world()
        res2 = unreal.EditorActorSubsystem().destroy_actor(a) if hasattr(unreal, "EditorActorSubsystem") else False
        unreal.log(f"destroy_actor failed on {lbl}, subsystem res: {res2}")

unreal.log(f"ACTUALLY_DESTROYED: {destroyed}")

saved_dirty = unreal.EditorLoadingAndSavingUtils.save_dirty_packages(False, True)
saved_lvl = unreal.EditorLevelLibrary.save_current_level()
unreal.log(f"SAVED_DIRTY: {saved_dirty}, SAVED_LVL: {saved_lvl}")

remaining_acts = list(unreal.EditorLevelLibrary.get_all_level_actors())
unreal.log(f"AFTER_PURGE_COUNT: {len(remaining_acts)}")
