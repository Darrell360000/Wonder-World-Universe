import unreal

map_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
world = unreal.EditorLoadingAndSavingUtils.load_map(map_path)
unreal.log(f"Loaded world: {world}")

# Delete all primitive greybox actors
destroyed = 0
for a in unreal.EditorLevelLibrary.get_all_level_actors():
    comp = a.get_component_by_class(unreal.StaticMeshComponent.static_class())
    if comp and comp.static_mesh:
        m_name = comp.static_mesh.get_name()
        if m_name in ["Cube", "Cylinder", "Cone", "Sphere"]:
            if unreal.EditorLevelLibrary.destroy_actor(a):
                destroyed += 1

unreal.log(f"Purged {destroyed} legacy primitive actors.")

# Ensure vehicle is Movable and physics simulated
for a in unreal.EditorLevelLibrary.get_all_level_actors():
    if a.get_actor_label() == "Downtown_WonderHoverVehicle":
        comp = a.get_component_by_class(unreal.StaticMeshComponent.static_class())
        if comp:
            comp.set_mobility(unreal.ComponentMobility.MOVABLE)
            comp.set_simulate_physics(True)
            unreal.log("Set vehicle mobility to MOVABLE and simulate_physics to True")

# Save the map properly using save_map!
res = unreal.EditorLoadingAndSavingUtils.save_map(world, map_path)
unreal.log(f"SAVE_MAP_RESULT: {res}")
