import unreal

def setup_brawler_spawn_points():
    unreal.log("==================================================")
    unreal.log("INITIALIZING BRAWLER MODE SPAWN POINTS")
    unreal.log("==================================================")

    # 1. Spawn Player Spawn Marker (Ground Floor Lobby / Street Entrance)
    player_spawn_loc = unreal.Vector(0.0, -200.0, 50.0)
    player_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor.static_class(), player_spawn_loc, unreal.Rotator(0, 0, 0)
    )
    if player_actor:
        mesh_comp = player_actor.get_component_by_class(unreal.StaticMeshComponent.static_class())
        if mesh_comp:
            asset = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cone.Cone")
            if asset:
                mesh_comp.set_static_mesh(asset)
        player_actor.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
        player_actor.set_actor_label("Player_Brawler_SpawnPoint")
        unreal.log("Spawned Player Brawler Entry Point.")

    # 2. Spawn Copycat Faction / Wannabe Enemy Spawn Points (Alleyway / Street Bottlenecks)
    enemy_spawns = [
        {"name": "Copycat_Thug_Spawn_Left", "pos": unreal.Vector(-250.0, 300.0, 50.0)},
        {"name": "Copycat_Thug_Spawn_Right", "pos": unreal.Vector(250.0, 300.0, 50.0)},
        {"name": "Copycat_Boss_Spawn", "pos": unreal.Vector(0.0, 600.0, 50.0)}
    ]

    for spawn in enemy_spawns:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor.static_class(), spawn["pos"], unreal.Rotator(0, 0, 0)
        )
        if actor:
            mesh_comp = actor.get_component_by_class(unreal.StaticMeshComponent.static_class())
            if mesh_comp:
                asset = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Sphere.Sphere")
                if asset:
                    mesh_comp.set_static_mesh(asset)
            actor.set_actor_scale3d(unreal.Vector(1.2, 1.2, 1.2))
            actor.set_actor_label(f"BrawlerEnemySpawn_{spawn['name']}")
            unreal.log(f"Spawned Enemy Node: {spawn['name']}")

    unreal.log("==================================================")
    unreal.log("BRAWLER SPAWN POINTS READY FOR ACTION")
    unreal.log("==================================================")

setup_brawler_spawn_points()