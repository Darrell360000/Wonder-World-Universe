import unreal


GENERATED_PREFIX = "WT_Blockout_"
LEGACY_MARKER_LABELS = {
    "Player_Brawler_SpawnPoint",
    "BrawlerEnemySpawn_Copycat_Thug_Spawn_Left",
    "BrawlerEnemySpawn_Copycat_Thug_Spawn_Right",
    "BrawlerEnemySpawn_Copycat_Boss_Spawn",
}


def _load_shape_assets():
    asset_paths = {
        "cube": "/Engine/BasicShapes/Cube.Cube",
        "cone": "/Engine/BasicShapes/Cone.Cone",
        "cylinder": "/Engine/BasicShapes/Cylinder.Cylinder",
        "sphere": "/Engine/BasicShapes/Sphere.Sphere",
    }
    assets = {}
    for shape, path in asset_paths.items():
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            raise RuntimeError("Could not load Unreal basic shape: " + path)
        assets[shape] = asset
    return assets


def _remove_previous_blockout():
    labels_to_remove = []
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        label = actor.get_actor_label()
        if label.startswith(GENERATED_PREFIX) or label in LEGACY_MARKER_LABELS:
            labels_to_remove.append(actor)

    for actor in labels_to_remove:
        if not unreal.EditorLevelLibrary.destroy_actor(actor):
            raise RuntimeError(
                "Could not replace previously generated actor: "
                + actor.get_actor_label()
            )

    unreal.log("Removed {} previous blockout actors.".format(len(labels_to_remove)))


def _spawn_shape(shape_assets, shape, label, location, scale):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor.static_class(),
        unreal.Vector(location[0], location[1], location[2]),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    if not actor:
        raise RuntimeError("Could not spawn blockout actor: " + label)

    mesh_component = actor.get_component_by_class(
        unreal.StaticMeshComponent.static_class()
    )
    if not mesh_component:
        unreal.EditorLevelLibrary.destroy_actor(actor)
        raise RuntimeError("Spawned actor has no static mesh component: " + label)

    mesh_component.set_static_mesh(shape_assets[shape])
    mesh_component.set_collision_profile_name("BlockAll")
    mesh_component.set_collision_enabled(
        unreal.CollisionEnabled.QUERY_AND_PHYSICS
    )
    actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    actor.set_actor_label(GENERATED_PREFIX + label)
    return actor


def build_wonder_tower_blockout():
    unreal.log("Building Wonder Tower street and tower blockout.")
    shapes = _load_shape_assets()
    _remove_previous_blockout()

    pieces = [
        # Ground and street
        ("cube", "Ground", (0, 450, -55), (22, 32, 0.5)),
        ("cube", "Road", (0, 250, -25), (8, 24, 0.25)),
        ("cube", "Sidewalk_Left", (-550, 250, -7.5), (3, 24, 0.35)),
        ("cube", "Sidewalk_Right", (550, 250, -7.5), (3, 24, 0.35)),
        # Three simple masses make a recognizable tower silhouette.
        ("cube", "Tower_Base", (0, 1450, 200), (7, 7, 4)),
        ("cube", "Tower_Middle", (0, 1450, 700), (6, 6, 6)),
        ("cube", "Tower_Top", (0, 1450, 1300), (5, 5, 6)),
        # Street lamps and simple barriers
        ("cylinder", "Lamp_Left_01", (-550, -650, 150), (0.18, 0.18, 3)),
        ("cylinder", "Lamp_Right_01", (550, -650, 150), (0.18, 0.18, 3)),
        ("cylinder", "Lamp_Left_02", (-550, 50, 150), (0.18, 0.18, 3)),
        ("cylinder", "Lamp_Right_02", (550, 50, 150), (0.18, 0.18, 3)),
        ("cylinder", "Lamp_Left_03", (-550, 750, 150), (0.18, 0.18, 3)),
        ("cylinder", "Lamp_Right_03", (550, 750, 150), (0.18, 0.18, 3)),
        ("cube", "Barrier_Left", (-330, 100, 35), (1.4, 0.3, 0.7)),
        ("cube", "Barrier_Right", (330, 100, 35), (1.4, 0.3, 0.7)),
        # Existing brawler spawn locations, retained as visible markers.
        ("cone", "Player_Spawn", (0, -200, 50), (1, 1, 1)),
        ("sphere", "Thug_Spawn_Left", (-250, 300, 50), (1.2, 1.2, 1.2)),
        ("sphere", "Thug_Spawn_Right", (250, 300, 50), (1.2, 1.2, 1.2)),
        ("sphere", "Boss_Spawn", (0, 600, 50), (1.5, 1.5, 1.5)),
    ]

    facade_tiers = [
        ("Base", 750, 700, [-450, -150, 150, 450], [260, 440], 1.0, 1.25),
        ("Middle", 850, 600, [-360, 0, 360], [360, 660, 960], 0.9, 1.2),
        ("Top", 950, 500, [-300, 0, 300], [980, 1280, 1580], 0.8, 1.15),
    ]
    for tier_name, front_y, half_width, window_columns, window_rows, window_width, window_height in facade_tiers:
        for column_index, x in enumerate(window_columns):
            for row_index, z in enumerate(window_rows):
                pieces.append(
                    (
                        "cube",
                        "{}_FrontWindow_{}_{}".format(tier_name, column_index + 1, row_index + 1),
                        (x, front_y - 8, z),
                        (window_width, 0.08, window_height),
                    )
                )

        for side_name, side_sign in (("Left", -1), ("Right", 1)):
            for row_index, z in enumerate(window_rows):
                pieces.append(
                    (
                        "cube",
                        "{}_{}_SideWindow_{}".format(tier_name, side_name, row_index + 1),
                        (side_sign * (half_width + 8), 1450, z),
                        (0.08, 1.0, window_height),
                    )
                )

        pieces.extend(
            [
                (
                    "cube",
                    "{}_Front_Cornice".format(tier_name),
                    (0, front_y - 12, max(window_rows) + 180),
                    (half_width / 50.0, 0.18, 0.18),
                ),
                (
                    "cube",
                    "{}_Front_Left_Pilaster".format(tier_name),
                    (-half_width + 35, front_y - 12, (min(window_rows) + max(window_rows)) / 2.0),
                    (0.35, 0.18, (max(window_rows) - min(window_rows) + 220) / 100.0),
                ),
                (
                    "cube",
                    "{}_Front_Right_Pilaster".format(tier_name),
                    (half_width - 35, front_y - 12, (min(window_rows) + max(window_rows)) / 2.0),
                    (0.35, 0.18, (max(window_rows) - min(window_rows) + 220) / 100.0),
                ),
            ]
        )

    pieces.extend(
        [
            ("cube", "Lobby_Door_Left", (-70, 735, 105), (0.65, 0.12, 1.05)),
            ("cube", "Lobby_Door_Right", (70, 735, 105), (0.65, 0.12, 1.05)),
            ("cube", "Lobby_Entrance_Canopy", (0, 690, 205), (2.6, 0.9, 0.18)),
            ("cylinder", "Lobby_Column_Left", (-220, 700, 105), (0.24, 0.24, 1.05)),
            ("cylinder", "Lobby_Column_Right", (220, 700, 105), (0.24, 0.24, 1.05)),
            ("cube", "Tower_Rooftop_Sign", (0, 940, 1780), (2.1, 0.15, 0.45)),
            ("cylinder", "Tower_Antenna", (0, 1450, 2010), (0.08, 0.08, 0.9)),
        ]
    )

    for shape, label, location, scale in pieces:
        _spawn_shape(shapes, shape, label, location, scale)

    unreal.log(
        "Wonder Tower blockout ready: {} actors created. Save the open level "
        "to keep the changes.".format(len(pieces))
    )


build_wonder_tower_blockout()
