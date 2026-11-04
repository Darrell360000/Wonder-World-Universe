import unreal

OPEN_WORLD_PREFIX = "WW_OpenWorld_"


def _load_shapes():
    paths = {
        "cube": "/Engine/BasicShapes/Cube.Cube",
        "cylinder": "/Engine/BasicShapes/Cylinder.Cylinder",
        "sphere": "/Engine/BasicShapes/Sphere.Sphere",
        "cone": "/Engine/BasicShapes/Cone.Cone",
    }
    loaded = {}
    for k, p in paths.items():
        asset = unreal.EditorAssetLibrary.load_asset(p)
        if not asset:
            raise RuntimeError("Missing shape asset: " + p)
        loaded[k] = asset
    return loaded


def _clear_previous_open_world():
    removed = 0
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        label = actor.get_actor_label()
        if label.startswith(OPEN_WORLD_PREFIX):
            unreal.EditorLevelLibrary.destroy_actor(actor)
            removed += 1
    unreal.log(f"[WonderWorld] Cleared {removed} previous Open-World Sandbox actors.")


def _spawn_piece(shapes, shape_key, name, loc, scale, rot=(0.0, 0.0, 0.0)):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor.static_class(),
        unreal.Vector(loc[0], loc[1], loc[2]),
        unreal.Rotator(rot[0], rot[1], rot[2]),
    )
    if not actor:
        return None
    comp = actor.get_component_by_class(unreal.StaticMeshComponent.static_class())
    if comp:
        comp.set_static_mesh(shapes[shape_key])
        comp.set_collision_profile_name("BlockAll")
        comp.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    actor.set_actor_label(OPEN_WORLD_PREFIX + name)
    return actor


def build_open_world_metropolis():
    unreal.log("====================================================================")
    unreal.log(" BUILDING MAMA'S WONDER WORLD: OPEN-WORLD SANDBOX, TOWERS & FLYING CARS")
    unreal.log("====================================================================")

    shapes = _load_shapes()
    _clear_previous_open_world()

    # 1. Open-World City Boulevard & Cross-Street Grid
    _spawn_piece(shapes, "cube", "CityBoulevard_Main", (0, 800, -40), (90, 90, 0.4))
    _spawn_piece(shapes, "cube", "Highway_NorthSouth", (0, 800, -18), (12, 85, 0.25))
    _spawn_piece(shapes, "cube", "Highway_EastWest", (0, 200, -16), (85, 12, 0.25))

    # 2. Mama Wonder Tower (Left) & Papa Wonder Tower (Right) + Rooftop Flying-Car Sky-Pads
    _spawn_piece(shapes, "cylinder", "MamaWonderTower_Core", (-1400, 2200, 900), (11, 11, 18))
    _spawn_piece(shapes, "sphere", "MamaWonderTower_BioDome", (-1400, 2200, 2750), (9, 9, 5))
    _spawn_piece(shapes, "cylinder", "MamaWonderTower_FlyingCar_SkyPad", (-1400, 2200, 2980), (13, 13, 0.4))

    _spawn_piece(shapes, "cube", "PapaWonderTower_Core", (1400, 2200, 950), (10, 10, 19))
    _spawn_piece(shapes, "cylinder", "PapaWonderTower_LedgerRing", (1400, 2200, 2600), (12, 12, 1.2))
    _spawn_piece(shapes, "cylinder", "PapaWonderTower_FlyingCar_SkyPad", (1400, 2200, 2950), (13, 13, 0.4))

    # Sky-Bridge connecting Mama Tower & Papa Tower
    _spawn_piece(shapes, "cube", "TwinTower_SkyBridge_Deck", (0, 2200, 1850), (28, 3.5, 0.5))

    # 3. City Builder Sandbox District Blocks (Surrounding the Avenue)
    sandbox_lots = [
        ("DistrictBlock_NW_Clinic", (-2200, 900, 350), (7, 7, 7)),
        ("DistrictBlock_NE_SoundLab", (2200, 900, 420), (7, 7, 8.4)),
        ("DistrictBlock_SW_ArcadeMall", (-2200, -600, 300), (8, 6, 6)),
        ("DistrictBlock_SE_SkyPortGarage", (2200, -600, 380), (8, 6, 7.6)),
    ]
    for name, loc, scale in sandbox_lots:
        _spawn_piece(shapes, "cube", name, loc, scale)

    # 4. Street-Driving Cyber Cruisers on the Boulevard
    street_cars = [
        ("StreetCruiser_PlayerReady", (-280, -350, 45), (2.2, 4.5, 1.1)),
        ("StreetCruiser_Patrol_01", (280, 150, 45), (2.2, 4.5, 1.1)),
        ("StreetCruiser_Patrol_02", (-280, 950, 45), (2.2, 4.5, 1.1)),
    ]
    for name, loc, scale in street_cars:
        _spawn_piece(shapes, "cube", name, loc, scale)

    # 5. Airborne Flying Cars on 3 Sky-Highway Altitudes
    flying_cars = [
        ("FlyingCar_SkyLaneLow_01", (-700, 400, 650), (2.4, 4.8, 0.9)),
        ("FlyingCar_SkyLaneLow_02", (700, 900, 720), (2.4, 4.8, 0.9)),
        ("FlyingCar_SkyLaneMid_01", (-900, 1500, 1350), (2.5, 5.0, 0.95)),
        ("FlyingCar_SkyLaneMid_02", (900, 1500, 1420), (2.5, 5.0, 0.95)),
        ("FlyingCar_SkyDeckApproach_01", (-1400, 1650, 3080), (2.6, 5.2, 1.0)),
        ("FlyingCar_SkyDeckApproach_02", (1400, 1650, 3080), (2.6, 5.2, 1.0)),
    ]
    for name, loc, scale in flying_cars:
        _spawn_piece(shapes, "sphere", name, loc, scale)

    unreal.log("[WonderWorld] Open-World Sandbox, Twin Towers, Street Cruisers & Flying Cars spawned!")


build_open_world_metropolis()
