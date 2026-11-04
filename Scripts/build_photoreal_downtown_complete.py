import unreal
import os

def log(msg):
    unreal.log(f"[DowntownBuilder] {msg}")
    print(f"[DowntownBuilder] {msg}")

def main():
    log("Starting Photorealistic Downtown Level Assembly...")

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    editor_asset_lib = unreal.EditorAssetLibrary

    project_dir = unreal.Paths.project_dir()
    downtown_asset_dir = unreal.Paths.combine([project_dir, "Assets/WonderWorld/Downtown"])
    downtown_tex_dir = unreal.Paths.combine([downtown_asset_dir, "Textures"])

    # -------------------------------------------------------------
    # 1. IMPORT TEXTURES
    # -------------------------------------------------------------
    log("=== Step 1: Importing PBR Textures ===")
    textures_to_import = [
        ("T_Asphalt_Albedo.png", "/Game/WonderWorld/Textures/Downtown", "T_Asphalt_Albedo"),
        ("T_Asphalt_Normal.png", "/Game/WonderWorld/Textures/Downtown", "T_Asphalt_Normal"),
        ("T_Asphalt_ORM.png", "/Game/WonderWorld/Textures/Downtown", "T_Asphalt_ORM"),
        ("T_Sidewalk_Albedo.png", "/Game/WonderWorld/Textures/Downtown", "T_Sidewalk_Albedo"),
        ("T_Sidewalk_Normal.png", "/Game/WonderWorld/Textures/Downtown", "T_Sidewalk_Normal")
    ]

    for fname, dest_pkg, asset_name in textures_to_import:
        fpath = unreal.Paths.combine([downtown_tex_dir, fname])
        if os.path.exists(fpath):
            task = unreal.AssetImportTask()
            task.filename = fpath
            task.destination_path = dest_pkg
            task.destination_name = asset_name
            task.replace_existing = True
            task.automated = True
            task.save = True
            asset_tools.import_asset_tasks([task])
            log(f"Imported texture: {asset_name}")

    # Set texture compression & sRGB settings
    for t_norm in ["T_Asphalt_Normal", "T_Sidewalk_Normal"]:
        t_asset = editor_asset_lib.load_asset(f"/Game/WonderWorld/Textures/Downtown/{t_norm}")
        if t_asset:
            t_asset.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
            try:
                t_asset.set_editor_property("srgb", False)
            except Exception:
                pass
            editor_asset_lib.save_asset(f"/Game/WonderWorld/Textures/Downtown/{t_norm}")
            log(f"Configured NormalMap compression for {t_norm}")

    t_orm = editor_asset_lib.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_ORM")
    if t_orm:
        try:
            t_orm.set_editor_property("srgb", False)
        except Exception:
            pass
        editor_asset_lib.save_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_ORM")
        log("Configured Linear sRGB=False for T_Asphalt_ORM")

    # -------------------------------------------------------------
    # 2. IMPORT NANITE FBX MESHES
    # -------------------------------------------------------------
    log("=== Step 2: Importing Nanite Meshes ===")
    meshes_to_import = [
        ("SM_Downtown_Avenue_Nanite.fbx", "/Game/WonderWorld/Meshes/Downtown", "SM_Downtown_Avenue_Nanite"),
        ("SM_Downtown_Curb_Sidewalk_Nanite.fbx", "/Game/WonderWorld/Meshes/Downtown", "SM_Downtown_Curb_Sidewalk_Nanite"),
        ("SM_Downtown_Building_Facade_Nanite.fbx", "/Game/WonderWorld/Meshes/Downtown", "SM_Downtown_Building_Facade_Nanite"),
        ("SM_WonderHover_Chassis_Nanite.fbx", "/Game/WonderWorld/Meshes/Downtown", "SM_WonderHover_Chassis_Nanite")
    ]

    for fname, dest_pkg, asset_name in meshes_to_import:
        fpath = unreal.Paths.combine([downtown_asset_dir, fname])
        if os.path.exists(fpath):
            task = unreal.AssetImportTask()
            task.filename = fpath
            task.destination_path = dest_pkg
            task.destination_name = asset_name
            task.replace_existing = True
            task.automated = True
            task.save = True

            options = unreal.FbxImportUI()
            options.import_mesh = True
            options.import_textures = False
            options.import_materials = False
            options.import_as_skeletal = False
            options.static_mesh_import_data.combine_meshes = True
            options.static_mesh_import_data.generate_lightmap_u_vs = True
            task.options = options

            asset_tools.import_asset_tasks([task])
            log(f"Imported mesh: {asset_name}")

    # Enable Nanite on all imported meshes
    for _, dest_pkg, asset_name in meshes_to_import:
        mesh_asset = editor_asset_lib.load_asset(f"{dest_pkg}/{asset_name}")
        if mesh_asset and isinstance(mesh_asset, unreal.StaticMesh):
            nanite_settings = mesh_asset.get_editor_property('nanite_settings')
            nanite_settings.set_editor_property('enabled', True)
            mesh_asset.set_editor_property('nanite_settings', nanite_settings)
            editor_asset_lib.save_asset(f"{dest_pkg}/{asset_name}")
            log(f"Enabled Nanite on: {asset_name}")

    # -------------------------------------------------------------
    # 3. CREATE SCANNED PBR MATERIALS WITH LUMEN PUDDLE REFLECTIONS
    # -------------------------------------------------------------
    log("=== Step 3: Creating PBR Materials ===")
    mat_dest = "/Game/WonderWorld/Materials/Downtown"
    mat_editing_lib = unreal.MaterialEditingLibrary

    # Material 1: Wet Asphalt with Puddles
    mat_asphalt_path = f"{mat_dest}/M_Photoreal_WetAsphalt_Nanite"
    mat_asphalt = editor_asset_lib.load_asset(mat_asphalt_path)
    if not mat_asphalt:
        factory = unreal.MaterialFactoryNew()
        mat_asphalt = asset_tools.create_asset("M_Photoreal_WetAsphalt_Nanite", mat_dest, unreal.Material, factory)

    if mat_asphalt:
        mat_editing_lib.delete_all_material_expressions(mat_asphalt)

        t_alb = editor_asset_lib.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_Albedo")
        if t_alb:
            node_alb = mat_editing_lib.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, -250)
            node_alb.set_editor_property("texture", t_alb)
            mat_editing_lib.connect_material_property(node_alb, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)

        t_nrm = editor_asset_lib.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_Normal")
        if t_nrm:
            node_nrm = mat_editing_lib.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, 50)
            node_nrm.set_editor_property("texture", t_nrm)
            node_nrm.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            mat_editing_lib.connect_material_property(node_nrm, "RGB", unreal.MaterialProperty.MP_NORMAL)

        t_orm_tex = editor_asset_lib.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_ORM")
        if t_orm_tex:
            node_orm = mat_editing_lib.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, 300)
            node_orm.set_editor_property("texture", t_orm_tex)
            # G = Puddle Roughness (0.05 puddles, 0.7 dry asphalt)
            mat_editing_lib.connect_material_property(node_orm, "G", unreal.MaterialProperty.MP_ROUGHNESS)
            # R = AO
            mat_editing_lib.connect_material_property(node_orm, "R", unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)

        node_spec = mat_editing_lib.create_material_expression(mat_asphalt, unreal.MaterialExpressionScalarParameter, -400, 480)
        node_spec.set_editor_property("parameter_name", "PuddleSpecular")
        node_spec.set_editor_property("default_value", 0.95)
        mat_editing_lib.connect_material_property(node_spec, "", unreal.MaterialProperty.MP_SPECULAR)

        mat_editing_lib.recompile_material(mat_asphalt)
        editor_asset_lib.save_asset(mat_asphalt_path)
        log("Compiled M_Photoreal_WetAsphalt_Nanite")

    # Material 2: Sidewalk PBR
    mat_sw_path = f"{mat_dest}/M_Photoreal_Sidewalk_Nanite"
    mat_sw = editor_asset_lib.load_asset(mat_sw_path)
    if not mat_sw:
        factory = unreal.MaterialFactoryNew()
        mat_sw = asset_tools.create_asset("M_Photoreal_Sidewalk_Nanite", mat_dest, unreal.Material, factory)

    if mat_sw:
        mat_editing_lib.delete_all_material_expressions(mat_sw)

        t_salb = editor_asset_lib.load_asset("/Game/WonderWorld/Textures/Downtown/T_Sidewalk_Albedo")
        if t_salb:
            node_salb = mat_editing_lib.create_material_expression(mat_sw, unreal.MaterialExpressionTextureSample, -400, -200)
            node_salb.set_editor_property("texture", t_salb)
            mat_editing_lib.connect_material_property(node_salb, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)

        t_snrm = editor_asset_lib.load_asset("/Game/WonderWorld/Textures/Downtown/T_Sidewalk_Normal")
        if t_snrm:
            node_snrm = mat_editing_lib.create_material_expression(mat_sw, unreal.MaterialExpressionTextureSample, -400, 50)
            node_snrm.set_editor_property("texture", t_snrm)
            node_snrm.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            mat_editing_lib.connect_material_property(node_snrm, "RGB", unreal.MaterialProperty.MP_NORMAL)

        node_srough = mat_editing_lib.create_material_expression(mat_sw, unreal.MaterialExpressionScalarParameter, -400, 250)
        node_srough.set_editor_property("parameter_name", "Roughness")
        node_srough.set_editor_property("default_value", 0.65)
        mat_editing_lib.connect_material_property(node_srough, "", unreal.MaterialProperty.MP_ROUGHNESS)

        mat_editing_lib.recompile_material(mat_sw)
        editor_asset_lib.save_asset(mat_sw_path)
        log("Compiled M_Photoreal_Sidewalk_Nanite")

    # -------------------------------------------------------------
    # 4. LOAD & POPULATE DOWNTOWN SHOWCASE LEVEL
    # -------------------------------------------------------------
    log("=== Step 4: Building Showcase Level Map ===")
    level_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
    world = unreal.EditorLevelLibrary.load_level(level_path)

    # Clean existing downtown actors
    for a in unreal.EditorLevelLibrary.get_all_level_actors():
        lbl = a.get_actor_label()
        if lbl.startswith("Downtown_") and not lbl.startswith("Downtown_SunLight") and not lbl.startswith("Downtown_Sky") and not lbl.startswith("Downtown_HeightFog") and not lbl.startswith("Downtown_PostProcess"):
            unreal.EditorLevelLibrary.destroy_actor(a)

    mesh_avenue = editor_asset_lib.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_Downtown_Avenue_Nanite")
    mesh_curb_sw = editor_asset_lib.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_Downtown_Curb_Sidewalk_Nanite")
    mesh_facade = editor_asset_lib.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_Downtown_Building_Facade_Nanite")
    mesh_hover = editor_asset_lib.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_WonderHover_Chassis_Nanite")
    mat_facade = editor_asset_lib.load_asset("/Game/WonderWorld/Materials/M_Building_Facade_Dark")
    if not mat_facade:
        mat_facade = mat_sw

    def spawn_mesh(mesh, label, loc, rot=(0,0,0), scale=(1,1,1), mat=None):
        if not mesh: return None
        act = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor.static_class(),
            unreal.Vector(loc[0], loc[1], loc[2]),
            unreal.Rotator(rot[0], rot[1], rot[2])
        )
        if not act: return None
        act.set_actor_label(label)
        act.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
        comp = act.get_component_by_class(unreal.StaticMeshComponent.static_class())
        if comp:
            comp.set_static_mesh(mesh)
            comp.set_collision_profile_name("BlockAll")
            comp.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
            if mat:
                comp.set_material(0, mat)
        return act

    # 3 Nanite Avenue Segments (120-meter long urban boulevard)
    for i, y_pos in enumerate([-4000.0, 0.0, 4000.0]):
        spawn_mesh(mesh_avenue, f"Downtown_Nanite_Avenue_Seg_{i+1}", (0, y_pos, 0), scale=(1,1,1), mat=mat_asphalt)
        spawn_mesh(mesh_curb_sw, f"Downtown_Nanite_CurbSW_West_{i+1}", (-800, y_pos, 0), scale=(1,1,1), mat=mat_sw)
        spawn_mesh(mesh_curb_sw, f"Downtown_Nanite_CurbSW_East_{i+1}", (800, y_pos, 0), rot=(0, 180, 0), scale=(1,1,1), mat=mat_sw)

    # Nanite Storefront High-Rises
    bldg_y_coords = [-3600, -1800, 200, 2200, 4000]
    for idx, by in enumerate(bldg_y_coords):
        spawn_mesh(mesh_facade, f"Downtown_Nanite_Building_West_{idx+1}", (-2300, by, 0), rot=(0, 90, 0), scale=(1.2, 1.2, 1.4), mat=mat_facade)
        spawn_mesh(mesh_facade, f"Downtown_Nanite_Building_East_{idx+1}", (2300, by, 0), rot=(0, -90, 0), scale=(1.2, 1.2, 1.4), mat=mat_facade)

    # Streetlights with Soft Volumetric Falloff
    for y_light in range(-4500, 5000, 1500):
        for side, lx in [("West", -840), ("East", 840)]:
            pt = unreal.EditorLevelLibrary.spawn_actor_from_class(
                unreal.PointLight.static_class(),
                unreal.Vector(lx, y_light, 450),
                unreal.Rotator(0, 0, 0)
            )
            if pt:
                pt.set_actor_label(f"Downtown_VolumetricLight_{side}_{y_light}")
                comp = pt.get_component_by_class(unreal.PointLightComponent.static_class())
                if comp:
                    comp.set_intensity(18000.0)
                    comp.set_light_color(unreal.LinearColor(1.0, 0.88, 0.70, 1.0))
                    comp.set_attenuation_radius(1400.0)
                    comp.set_volumetric_scattering_intensity(3.0)

    # PlayerStart
    pstart = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart.static_class(),
        unreal.Vector(-200, -4200, 80),
        unreal.Rotator(0, 90, 0)
    )
    if pstart: pstart.set_actor_label("Downtown_PlayerStart")

    # Spawn Darrell X-Gen Hero
    darrell_cls = getattr(unreal, "DarrellXGenCharacter", None)
    if darrell_cls:
        hero = unreal.EditorLevelLibrary.spawn_actor_from_class(
            darrell_cls.static_class(),
            unreal.Vector(-200, -3800, 80),
            unreal.Rotator(0, 90, 0)
        )
        if hero: hero.set_actor_label("Downtown_DarrellXGenHero")
        log("Spawned Darrell X-Gen Hero Actor")

    # Spawn Wonder Hover Vehicle with WonderHoverPhysicsComponent attached
    vehicle = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor.static_class(),
        unreal.Vector(250, -3800, 80),
        unreal.Rotator(0, 90, 0)
    )
    if vehicle:
        vehicle.set_actor_label("Downtown_WonderHoverVehicle")
        v_comp = vehicle.get_component_by_class(unreal.StaticMeshComponent.static_class())
        if v_comp:
            v_comp.set_static_mesh(mesh_hover)
            v_comp.set_material(0, mat_facade)
            v_comp.set_collision_profile_name("BlockAll")
            v_comp.set_simulate_physics(True)

        hover_comp_cls = getattr(unreal, "WonderHoverPhysicsComponent", None)
        if hover_comp_cls:
            try:
                h_comp = vehicle.add_component_by_class(hover_comp_cls, False, unreal.Transform(), False)
                if h_comp:
                    log("Attached WonderHoverPhysicsComponent to WonderHoverVehicle")
            except Exception as e:
                log(f"Component attachment note: {e}")

        # 4 Cyan Thruster Glow Lights underneath vehicle
        for ox, oy in [(-80, -140), (80, -140), (-80, 140), (80, 140)]:
            glow = unreal.EditorLevelLibrary.spawn_actor_from_class(
                unreal.PointLight.static_class(),
                unreal.Vector(250 + ox, -3800 + oy, 40),
                unreal.Rotator(0, 0, 0)
            )
            if glow:
                glow.set_actor_label(f"Downtown_Vehicle_Glow_{ox}_{oy}")
                g_comp = glow.get_component_by_class(unreal.PointLightComponent.static_class())
                if g_comp:
                    g_comp.set_intensity(8000.0)
                    g_comp.set_light_color(unreal.LinearColor(0.05, 0.75, 1.0, 1.0))
                    g_comp.set_attenuation_radius(450.0)
                    g_comp.set_volumetric_scattering_intensity(3.5)

    # Lighting & Atmosphere actors
    found_sun = False
    found_sky = False
    found_fog = False
    found_pp = False

    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if isinstance(actor, unreal.DirectionalLight):
            found_sun = True
            actor.set_actor_label("Downtown_SunLight")
            actor.set_actor_rotation(unreal.Rotator(-24.0, 52.0, 0.0), False)
            comp = actor.get_component_by_class(unreal.DirectionalLightComponent.static_class())
            if comp:
                comp.set_intensity(10.0)
                comp.set_light_color(unreal.LinearColor(1.0, 0.90, 0.80, 1.0))
                comp.set_atmosphere_sun_light(True)
                comp.set_editor_property("cast_volumetric_shadow", True)
                comp.set_editor_property("dynamic_shadow_distance_movable_light", 30000.0)
        elif isinstance(actor, unreal.SkyLight):
            found_sky = True
            actor.set_actor_label("Downtown_SkyLight")
            comp = actor.get_component_by_class(unreal.SkyLightComponent.static_class())
            if comp:
                comp.set_real_time_capture(True)
                comp.set_intensity(1.8)
        elif isinstance(actor, unreal.ExponentialHeightFog):
            found_fog = True
            actor.set_actor_label("Downtown_HeightFog")
            comp = actor.get_component_by_class(unreal.ExponentialHeightFogComponent.static_class())
            if comp:
                comp.set_fog_density(0.012)
                comp.set_volumetric_fog(True)
                comp.set_volumetric_fog_scattering_distribution(0.65)
                comp.set_volumetric_fog_albedo(unreal.Color(210, 225, 255, 255))
        elif isinstance(actor, unreal.PostProcessVolume):
            found_pp = True
            actor.set_actor_label("Downtown_PostProcessVolume")
            actor.set_editor_property("unbound", True)
            settings = actor.get_editor_property("settings")
            settings.set_editor_property("bloom_intensity", 1.25)
            settings.set_editor_property("lumen_reflection_quality", 2.0)
            settings.set_editor_property("lumen_scene_lighting_quality", 2.0)
            actor.set_editor_property("settings", settings)

    # Spawn missing atmosphere actors if not in level
    if not found_sun:
        sun = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight.static_class(), unreal.Vector(0,0,1000), unreal.Rotator(-24.0, 52.0, 0.0))
        if sun:
            sun.set_actor_label("Downtown_SunLight")
            comp = sun.get_component_by_class(unreal.DirectionalLightComponent.static_class())
            if comp:
                comp.set_intensity(10.0)
                comp.set_light_color(unreal.LinearColor(1.0, 0.90, 0.80, 1.0))
                comp.set_atmosphere_sun_light(True)
                comp.set_editor_property("cast_volumetric_shadow", True)
    if not found_sky:
        sky = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight.static_class(), unreal.Vector(0,0,800), unreal.Rotator(0,0,0))
        if sky:
            sky.set_actor_label("Downtown_SkyLight")
            comp = sky.get_component_by_class(unreal.SkyLightComponent.static_class())
            if comp:
                comp.set_real_time_capture(True)
                comp.set_intensity(1.8)
    if not found_fog:
        fog = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.ExponentialHeightFog.static_class(), unreal.Vector(0,0,200), unreal.Rotator(0,0,0))
        if fog:
            fog.set_actor_label("Downtown_HeightFog")
            comp = fog.get_component_by_class(unreal.ExponentialHeightFogComponent.static_class())
            if comp:
                comp.set_fog_density(0.012)
                comp.set_volumetric_fog(True)
                comp.set_volumetric_fog_scattering_distribution(0.65)
    if not found_pp:
        pp = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PostProcessVolume.static_class(), unreal.Vector(0,0,0), unreal.Rotator(0,0,0))
        if pp:
            pp.set_actor_label("Downtown_PostProcessVolume")
            pp.set_editor_property("unbound", True)
            settings = pp.get_editor_property("settings")
            settings.set_editor_property("bloom_intensity", 1.25)
            settings.set_editor_property("lumen_reflection_quality", 2.0)
            settings.set_editor_property("lumen_scene_lighting_quality", 2.0)
            pp.set_editor_property("settings", settings)

    # Save level and assets
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(False, True)
    unreal.EditorLevelLibrary.save_current_level()
    log("=== PHOTOREALISTIC DOWNTOWN SHOWCASE BUILD COMPLETED SUCCESSFULLY ===")

if __name__ == "__main__":
    main()
