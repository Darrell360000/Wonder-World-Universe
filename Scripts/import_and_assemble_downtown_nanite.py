import unreal

def run_import_and_assembly():
    def log(msg):
        unreal.log(f"[DowntownNanite] {msg}")

    log("Starting Nanite & PBR Downtown Assembly...")

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    # 1. Import Textures
    tex_dir = r"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Assets\WonderWorld\Downtown\Textures"
    tex_dest = "/Game/WonderWorld/Textures/Downtown"
    textures = [
        "T_Asphalt_Albedo.png",
        "T_Asphalt_Normal.png",
        "T_Asphalt_ORM.png",
        "T_Sidewalk_Albedo.png",
        "T_Sidewalk_Normal.png"
    ]
    
    imported_tex = {}
    for t_name in textures:
        src = f"{tex_dir}\\{t_name}"
        task = unreal.AssetImportTask()
        task.filename = src
        task.destination_path = tex_dest
        task.destination_name = t_name.replace(".png", "")
        task.replace_existing = True
        task.automated = True
        task.save = True
        asset_tools.import_asset_tasks([task])
        loaded = unreal.EditorAssetLibrary.load_asset(f"{tex_dest}/{task.destination_name}")
        if loaded:
            # If normal map, set compression to TC_Normalmap
            if "Normal" in t_name:
                loaded.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
                loaded.set_editor_property("s_rgb", False)
                unreal.EditorAssetLibrary.save_asset(f"{tex_dest}/{task.destination_name}")
            imported_tex[t_name] = loaded
            log(f"Imported texture: {task.destination_name}")

    # 2. Import Nanite FBX Meshes
    mesh_dir = r"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\Assets\WonderWorld\Downtown"
    mesh_dest = "/Game/WonderWorld/Meshes/Downtown"
    meshes = [
        "SM_Downtown_Avenue_Nanite.fbx",
        "SM_Downtown_Curb_Sidewalk_Nanite.fbx",
        "SM_Downtown_Building_Facade_Nanite.fbx",
        "SM_WonderHover_Chassis_Nanite.fbx"
    ]

    imported_meshes = {}
    for m_name in meshes:
        src = f"{mesh_dir}\\{m_name}"
        clean_name = m_name.replace(".fbx", "")
        task = unreal.AssetImportTask()
        task.filename = src
        task.destination_path = mesh_dest
        task.destination_name = clean_name
        task.replace_existing = True
        task.automated = True
        task.save = True
        
        fbx_ui = unreal.FbxImportUI()
        fbx_ui.set_editor_property("import_mesh", True)
        fbx_ui.set_editor_property("import_materials", False)
        fbx_ui.set_editor_property("import_textures", False)
        fbx_ui.static_mesh_import_data.set_editor_property("combine_meshes", True)
        fbx_ui.static_mesh_import_data.set_editor_property("build_nanite", True)
        task.options = fbx_ui
        
        asset_tools.import_asset_tasks([task])
        mesh_asset = unreal.EditorAssetLibrary.load_asset(f"{mesh_dest}/{clean_name}")
        if mesh_asset:
            # Explicitly ensure Nanite is enabled
            nanite_settings = mesh_asset.get_editor_property("nanite_settings")
            nanite_settings.set_editor_property("enabled", True)
            mesh_asset.set_editor_property("nanite_settings", nanite_settings)
            unreal.EditorAssetLibrary.save_asset(f"{mesh_dest}/{clean_name}")
            imported_meshes[clean_name] = mesh_asset
            log(f"Imported & Nanite-Enabled mesh: {clean_name}")

    # 3. Create PBR Materials
    mat_dest = "/Game/WonderWorld/Materials/Downtown"
    
    # Asphalt PBR Material
    mat_asphalt_path = f"{mat_dest}/M_Photoreal_WetAsphalt_Nanite"
    mat_asphalt = unreal.EditorAssetLibrary.load_asset(mat_asphalt_path)
    if not mat_asphalt:
        factory = unreal.MaterialFactoryNew()
        mat_asphalt = asset_tools.create_asset("M_Photoreal_WetAsphalt_Nanite", mat_dest, unreal.Material, factory)
    
    if mat_asphalt:
        unreal.MaterialEditingLibrary.delete_all_material_expressions(mat_asphalt)
        
        # Texture Samples
        t_alb = imported_tex.get("T_Asphalt_Albedo.png")
        if t_alb:
            node_alb = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, -250)
            node_alb.set_editor_property("texture", t_alb)
            unreal.MaterialEditingLibrary.connect_material_property(node_alb, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)

        t_nrm = imported_tex.get("T_Asphalt_Normal.png")
        if t_nrm:
            node_nrm = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, 50)
            node_nrm.set_editor_property("texture", t_nrm)
            node_nrm.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            unreal.MaterialEditingLibrary.connect_material_property(node_nrm, "RGB", unreal.MaterialProperty.MP_NORMAL)

        t_orm = imported_tex.get("T_Asphalt_ORM.png")
        if t_orm:
            node_orm = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, 300)
            node_orm.set_editor_property("texture", t_orm)
            # G = Roughness (low for puddles -> Lumen mirror, high for asphalt)
            unreal.MaterialEditingLibrary.connect_material_property(node_orm, "G", unreal.MaterialProperty.MP_ROUGHNESS)
            # R = AO
            unreal.MaterialEditingLibrary.connect_material_property(node_orm, "R", unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)

        # Specular
        node_spec = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionScalarParameter, -400, 480)
        node_spec.set_editor_property("parameter_name", "PuddleSpecular")
        node_spec.set_editor_property("default_value", 0.95)
        unreal.MaterialEditingLibrary.connect_material_property(node_spec, "", unreal.MaterialProperty.MP_SPECULAR)

        unreal.MaterialEditingLibrary.recompile_material(mat_asphalt)
        unreal.EditorAssetLibrary.save_asset(mat_asphalt_path)
        log("Compiled M_Photoreal_WetAsphalt_Nanite")

    # Sidewalk PBR Material
    mat_sw_path = f"{mat_dest}/M_Photoreal_Sidewalk_Nanite"
    mat_sw = unreal.EditorAssetLibrary.load_asset(mat_sw_path)
    if not mat_sw:
        factory = unreal.MaterialFactoryNew()
        mat_sw = asset_tools.create_asset("M_Photoreal_Sidewalk_Nanite", mat_dest, unreal.Material, factory)
    if mat_sw:
        unreal.MaterialEditingLibrary.delete_all_material_expressions(mat_sw)
        t_salb = imported_tex.get("T_Sidewalk_Albedo.png")
        if t_salb:
            node_salb = unreal.MaterialEditingLibrary.create_material_expression(mat_sw, unreal.MaterialExpressionTextureSample, -400, -200)
            node_salb.set_editor_property("texture", t_salb)
            unreal.MaterialEditingLibrary.connect_material_property(node_salb, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
        t_snrm = imported_tex.get("T_Sidewalk_Normal.png")
        if t_snrm:
            node_snrm = unreal.MaterialEditingLibrary.create_material_expression(mat_sw, unreal.MaterialExpressionTextureSample, -400, 50)
            node_snrm.set_editor_property("texture", t_snrm)
            node_snrm.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
            unreal.MaterialEditingLibrary.connect_material_property(node_snrm, "RGB", unreal.MaterialProperty.MP_NORMAL)
        node_srough = unreal.MaterialEditingLibrary.create_material_expression(mat_sw, unreal.MaterialExpressionScalarParameter, -400, 250)
        node_srough.set_editor_property("parameter_name", "Roughness")
        node_srough.set_editor_property("default_value", 0.65)
        unreal.MaterialEditingLibrary.connect_material_property(node_srough, "", unreal.MaterialProperty.MP_ROUGHNESS)
        unreal.MaterialEditingLibrary.recompile_material(mat_sw)
        unreal.EditorAssetLibrary.save_asset(mat_sw_path)
        log("Compiled M_Photoreal_Sidewalk_Nanite")

    # Facade & Neon Materials
    mat_facade = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Materials/M_Building_Facade_Dark")
    mat_neon_cyan = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Materials/M_Neon_Cyan")
    mat_neon_magenta = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Materials/M_Neon_Magenta")
    mat_neon_gold = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Materials/M_Neon_Gold")
    mat_glass = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Materials/M_Storefront_Glass")

    # 4. Assemble the Downtown Showcase Level
    map_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
    unreal.EditorLevelLibrary.load_level(map_path)

    # Delete all previously spawned Downtown blockout geometry
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        lbl = actor.get_actor_label()
        if lbl.startswith("Downtown_") and not lbl.startswith("Downtown_SunLight") and not lbl.startswith("Downtown_Sky") and not lbl.startswith("Downtown_HeightFog") and not lbl.startswith("Downtown_PostProcess"):
            unreal.EditorLevelLibrary.destroy_actor(actor)

    mesh_avenue = imported_meshes.get("SM_Downtown_Avenue_Nanite")
    mesh_curb_sw = imported_meshes.get("SM_Downtown_Curb_Sidewalk_Nanite")
    mesh_facade = imported_meshes.get("SM_Downtown_Building_Facade_Nanite")
    mesh_hover = imported_meshes.get("SM_WonderHover_Chassis_Nanite")

    # Helper spawn mesh
    def spawn_nanite_mesh(mesh, label, loc, rot=(0,0,0), scale=(1,1,1), mat=None):
        if not mesh:
            return None
        act = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor.static_class(),
            unreal.Vector(loc[0], loc[1], loc[2]),
            unreal.Rotator(rot[0], rot[1], rot[2])
        )
        if not act:
            return None
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

    # Spawn 3 consecutive Nanite Avenue segments (40m each -> 120m boulevard)
    for i, y_pos in enumerate([-4000.0, 0.0, 4000.0]):
        # Main Roadway with scanned PBR wet asphalt
        spawn_nanite_mesh(mesh_avenue, f"Downtown_Nanite_Avenue_Seg_{i+1}", (0, y_pos, 0), scale=(1,1,1), mat=mat_asphalt)
        
        # West Sidewalk & Beveled Curb
        spawn_nanite_mesh(mesh_curb_sw, f"Downtown_Nanite_CurbSW_West_{i+1}", (-800, y_pos, 0), scale=(1,1,1), mat=mat_sw)
        
        # East Sidewalk & Beveled Curb (Mirrored along X)
        spawn_nanite_mesh(mesh_curb_sw, f"Downtown_Nanite_CurbSW_East_{i+1}", (800, y_pos, 0), rot=(0, 180, 0), scale=(1,1,1), mat=mat_sw)

    # High-Poly Nanite High-Rise Storefront Buildings along both promenades
    bldg_y_coords = [-3600, -1800, 200, 2200, 4000]
    neon_cycle = [mat_neon_cyan, mat_neon_magenta, mat_neon_gold]
    for idx, by in enumerate(bldg_y_coords):
        n_mat = neon_cycle[idx % len(neon_cycle)]
        
        # West Facade
        w_bldg = spawn_nanite_mesh(mesh_facade, f"Downtown_Nanite_Building_West_{idx+1}", (-2300, by, 0), rot=(0, 90, 0), scale=(1.2, 1.2, 1.4), mat=mat_facade)
        # East Facade
        e_bldg = spawn_nanite_mesh(mesh_facade, f"Downtown_Nanite_Building_East_{idx+1}", (2300, by, 0), rot=(0, -90, 0), scale=(1.2, 1.2, 1.4), mat=mat_facade)

    # Streetlights with Volumetric Light Scattering onto Wet Asphalt
    for y_light in range(-4500, 5000, 1500):
        for side, lx in [("West", -840), ("East", 840)]:
            pt = unreal.EditorLevelLibrary.spawn_actor_from_class(
                unreal.PointLight.static_class(),
                unreal.Vector(lx, y_light, 450),
                unreal.Rotator(0, 0, 0)
            )
            if pt:
                pt.set_actor_label(f"Downtown_VolumetricStreetLight_{side}_{y_light}")
                comp = pt.get_component_by_class(unreal.PointLightComponent.static_class())
                if comp:
                    comp.set_intensity(18000.0)
                    comp.set_light_color(unreal.LinearColor(1.0, 0.88, 0.70, 1.0)) # Warm tungsten
                    comp.set_attenuation_radius(1400.0)
                    comp.set_source_radius(30.0)
                    comp.set_volumetric_scattering_intensity(3.0)

    # 5. Place PlayerStart, Darrell X-Gen Hero, and Wonder Hover Vehicle
    # PlayerStart
    pstart = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart.static_class(),
        unreal.Vector(-200, -4200, 80),
        unreal.Rotator(0, 90, 0)
    )
    if pstart:
        pstart.set_actor_label("Downtown_PlayerStart")

    # Darrell X-Gen Hero Actor
    darrell_cls = getattr(unreal, "DarrellXGenCharacter", None)
    if darrell_cls:
        hero = unreal.EditorLevelLibrary.spawn_actor_from_class(
            darrell_cls.static_class(),
            unreal.Vector(-200, -3800, 80),
            unreal.Rotator(0, 90, 0)
        )
        if hero:
            hero.set_actor_label("Downtown_DarrellXGenHero")

    # Wonder Hover Vehicle Actor with WonderHoverPhysicsComponent attached
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
                    g_comp.set_light_color(unreal.LinearColor(0.05, 0.75, 1.0, 1.0)) # Neon Cyan
                    g_comp.set_attenuation_radius(450.0)
                    g_comp.set_volumetric_scattering_intensity(3.5)

    # 6. Verify Lighting & Atmosphere Parameters
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if isinstance(actor, unreal.DirectionalLight):
            actor.set_actor_label("Downtown_SunLight")
            actor.set_actor_rotation(unreal.Rotator(-24.0, 52.0, 0.0), False)
            comp = actor.get_component_by_class(unreal.DirectionalLightComponent.static_class())
            if comp:
                comp.set_intensity(10.0)
                comp.set_light_color(unreal.LinearColor(1.0, 0.90, 0.80, 1.0))
                comp.set_atmosphere_sun_light(True)
                comp.set_editor_property("cast_volumetric_shadow", True)
                
        elif isinstance(actor, unreal.SkyLight):
            actor.set_actor_label("Downtown_SkyLight")
            comp = actor.get_component_by_class(unreal.SkyLightComponent.static_class())
            if comp:
                comp.set_real_time_capture(True)
                comp.set_intensity(1.8)

        elif isinstance(actor, unreal.ExponentialHeightFog):
            actor.set_actor_label("Downtown_HeightFog")
            comp = actor.get_component_by_class(unreal.ExponentialHeightFogComponent.static_class())
            if comp:
                comp.set_fog_density(0.014)
                comp.set_volumetric_fog(True)
                comp.set_volumetric_fog_scattering_distribution(0.65)
                comp.set_volumetric_fog_albedo(unreal.Color(210, 225, 255, 255))

        elif isinstance(actor, unreal.PostProcessVolume):
            actor.set_actor_label("Downtown_PostProcessVolume")
            actor.set_editor_property("unbound", True)
            settings = actor.get_editor_property("settings")
            settings.set_editor_property("bloom_intensity", 1.25)
            settings.set_editor_property("lumen_reflection_quality", 2.0)
            settings.set_editor_property("lumen_scene_lighting_quality", 2.0)
            actor.set_editor_property("settings", settings)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(False, True)
    unreal.EditorLevelLibrary.save_current_level()
    log("Nanite & PBR Downtown Showcase Level assembled and saved successfully!")
    return True

result = run_import_and_assembly()
