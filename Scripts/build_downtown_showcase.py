import unreal

def setup_downtown_block():
    unreal.log("[DowntownShowcase] Setting up Downtown City Block Showcase Level...")

    def get_or_create_material(mat_name, base_color, roughness=0.5, metallic=0.0, specular=0.5, emissive=None):
        pkg_path = "/Game/WonderWorld/Materials"
        full_path = f"{pkg_path}/{mat_name}"
        
        asset = unreal.EditorAssetLibrary.load_asset(full_path)
        if asset:
            return asset

        tools = unreal.AssetToolsHelpers.get_asset_tools()
        factory = unreal.MaterialFactoryNew()
        mat = tools.create_asset(mat_name, pkg_path, unreal.Material, factory)
        if not mat:
            unreal.log(f"[DowntownShowcase] Failed to create material {mat_name}")
            return None

        # Base Color
        col_node = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -400, -200)
        col_node.set_editor_property("parameter_name", "BaseColor")
        col_node.set_editor_property("default_value", unreal.LinearColor(base_color[0], base_color[1], base_color[2], 1.0))
        unreal.MaterialEditingLibrary.connect_material_property(col_node, "", unreal.MaterialProperty.MP_BASE_COLOR)

        # Roughness (wet puddles when low)
        rough_node = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -400, 0)
        rough_node.set_editor_property("parameter_name", "Roughness_Wetness")
        rough_node.set_editor_property("default_value", roughness)
        unreal.MaterialEditingLibrary.connect_material_property(rough_node, "", unreal.MaterialProperty.MP_ROUGHNESS)

        # Metallic
        if metallic > 0.0:
            met_node = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -400, 150)
            met_node.set_editor_property("parameter_name", "Metallic")
            met_node.set_editor_property("default_value", metallic)
            unreal.MaterialEditingLibrary.connect_material_property(met_node, "", unreal.MaterialProperty.MP_METALLIC)

        # Specular
        spec_node = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -400, 300)
        spec_node.set_editor_property("parameter_name", "Specular")
        spec_node.set_editor_property("default_value", specular)
        unreal.MaterialEditingLibrary.connect_material_property(spec_node, "", unreal.MaterialProperty.MP_SPECULAR)

        # Emissive
        if emissive:
            em_node = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -400, 450)
            em_node.set_editor_property("parameter_name", "EmissiveColor")
            em_node.set_editor_property("default_value", unreal.LinearColor(emissive[0], emissive[1], emissive[2], 1.0))
            unreal.MaterialEditingLibrary.connect_material_property(em_node, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

        unreal.MaterialEditingLibrary.recompile_material(mat)
        unreal.EditorAssetLibrary.save_asset(full_path)
        return mat

    def spawn_mesh(shape_mesh, name, loc, scale, rot=(0,0,0), material=None):
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor.static_class(),
            unreal.Vector(loc[0], loc[1], loc[2]),
            unreal.Rotator(rot[0], rot[1], rot[2])
        )
        if not actor:
            return None
        actor.set_actor_label(name)
        actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
        
        comp = actor.get_component_by_class(unreal.StaticMeshComponent.static_class())
        if comp:
            comp.set_static_mesh(shape_mesh)
            comp.set_collision_profile_name("BlockAll")
            comp.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
            if material:
                comp.set_material(0, material)
        return actor

    # Load shapes
    cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    cylinder = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cylinder.Cylinder")

    # Materials
    mat_wet_asphalt = get_or_create_material("M_WetAsphalt_Puddles", (0.02, 0.02, 0.025), roughness=0.05, metallic=0.0, specular=0.98)
    mat_sidewalk = get_or_create_material("M_Sidewalk_Concrete", (0.38, 0.38, 0.40), roughness=0.6, metallic=0.0, specular=0.4)
    mat_curb = get_or_create_material("M_Curb_Stone", (0.16, 0.16, 0.18), roughness=0.55, metallic=0.0, specular=0.5)
    mat_road_lines = get_or_create_material("M_RoadLine_Yellow", (0.95, 0.75, 0.05), roughness=0.25, metallic=0.0, specular=0.6)
    mat_white_lines = get_or_create_material("M_RoadLine_White", (0.9, 0.9, 0.92), roughness=0.3, metallic=0.0, specular=0.6)
    mat_glass = get_or_create_material("M_Storefront_Glass", (0.02, 0.05, 0.08), roughness=0.04, metallic=0.2, specular=1.0)
    mat_facade_dark = get_or_create_material("M_Building_Facade_Dark", (0.07, 0.08, 0.10), roughness=0.25, metallic=0.85, specular=0.8)
    mat_facade_steel = get_or_create_material("M_Building_Facade_Steel", (0.22, 0.24, 0.26), roughness=0.3, metallic=0.9, specular=0.85)
    
    # Neon Emissive Signs
    mat_neon_cyan = get_or_create_material("M_Neon_Cyan", (0.05, 0.75, 1.0), roughness=0.1, emissive=(0.2, 3.5, 6.0))
    mat_neon_magenta = get_or_create_material("M_Neon_Magenta", (1.0, 0.05, 0.65), roughness=0.1, emissive=(6.0, 0.2, 3.5))
    mat_neon_gold = get_or_create_material("M_Neon_Gold", (1.0, 0.75, 0.08), roughness=0.1, emissive=(6.0, 4.2, 0.5))

    # Clean existing blockout actors (preserve essential lighting & environment)
    preserved_types = {
        "DirectionalLight", "SkyAtmosphere", "VolumetricCloud", "SkyLight",
        "ExponentialHeightFog", "PostProcessVolume", "WorldDataLayers", "WorldPartitionMiniMap"
    }
    
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        cls_name = actor.get_class().get_name()
        label = actor.get_actor_label()
        if cls_name not in preserved_types and not label.startswith("Downtown_"):
            unreal.EditorLevelLibrary.destroy_actor(actor)

    # 1. Lighting & Atmosphere Tuning
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if isinstance(actor, unreal.DirectionalLight):
            actor.set_actor_label("Downtown_SunLight")
            actor.set_actor_rotation(unreal.Rotator(-24.0, 52.0, 0.0), False)
            comp = actor.get_component_by_class(unreal.DirectionalLightComponent.static_class())
            if comp:
                comp.set_intensity(10.0)
                comp.set_light_color(unreal.LinearColor(1.0, 0.88, 0.76, 1.0))
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

    # 2. Multi-lane Wet Asphalt Avenue
    road_length = 120.0  # 12,000 units
    road_width = 16.0    # 1,600 units (4-lane urban avenue)
    
    spawn_mesh(cube, "Downtown_Boulevard_WetAsphalt", (0, 0, 0), (road_width, road_length, 0.4), material=mat_wet_asphalt)

    # Center Double-Yellow Median Striping
    spawn_mesh(cube, "Downtown_RoadStriping_Center_01", (-10, 0, 22), (0.15, road_length * 0.98, 0.05), material=mat_road_lines)
    spawn_mesh(cube, "Downtown_RoadStriping_Center_02", (10, 0, 22), (0.15, road_length * 0.98, 0.05), material=mat_road_lines)

    # White Lane Dashes
    for y in range(-5500, 5500, 600):
        spawn_mesh(cube, f"Downtown_LaneDash_Left_{y}", (-380, y, 22), (0.18, 2.5, 0.05), material=mat_white_lines)
        spawn_mesh(cube, f"Downtown_LaneDash_Right_{y}", (380, y, 22), (0.18, 2.5, 0.05), material=mat_white_lines)

    # Curbs (Elevated stone border)
    curb_x_left = - (road_width * 50.0 + 15)
    curb_x_right = (road_width * 50.0 + 15)
    spawn_mesh(cube, "Downtown_Curb_West", (curb_x_left, 0, 15), (0.35, road_length, 0.5), material=mat_curb)
    spawn_mesh(cube, "Downtown_Curb_East", (curb_x_right, 0, 15), (0.35, road_length, 0.5), material=mat_curb)

    # Raised Sidewalks (West & East Promenades)
    sw_width = 8.0 # 800 units wide
    sw_x_left = curb_x_left - (sw_width * 50.0)
    sw_x_right = curb_x_right + (sw_width * 50.0)
    spawn_mesh(cube, "Downtown_Sidewalk_West", (sw_x_left, 0, 20), (sw_width, road_length, 0.4), material=mat_sidewalk)
    spawn_mesh(cube, "Downtown_Sidewalk_East", (sw_x_right, 0, 20), (sw_width, road_length, 0.4), material=mat_sidewalk)

    # 3. High-Density Downtown Storefronts & High-Rises
    buildings = [
        # West Side
        ("MamaWonder_Retail_Plaza", (sw_x_left - 800, -3200, 450), (10, 12, 9), mat_facade_dark, mat_neon_cyan),
        ("SoundMagic_Acoustic_Lab", (sw_x_left - 800, -1600, 600), (10, 12, 12), mat_facade_steel, mat_neon_magenta),
        ("Syndicate_CyberTower", (sw_x_left - 900, 200, 1200), (12, 14, 24), mat_facade_dark, mat_neon_cyan),
        ("AeroDrive_SkyPort_Hangar", (sw_x_left - 800, 2400, 500), (10, 14, 10), mat_facade_steel, mat_neon_gold),

        # East Side
        ("Municipal_Ledger_Bank", (sw_x_right + 800, -3200, 550), (10, 12, 11), mat_facade_steel, mat_neon_gold),
        ("WonderWorld_CyberArcade", (sw_x_right + 800, -1600, 450), (10, 12, 9), mat_facade_dark, mat_neon_magenta),
        ("PapaWonder_Commerce_Tower", (sw_x_right + 900, 200, 1350), (12, 14, 27), mat_facade_dark, mat_neon_cyan),
        ("Metropolis_Transit_Station", (sw_x_right + 800, 2400, 400), (10, 14, 8), mat_facade_steel, mat_neon_magenta),
    ]

    for b_name, b_loc, b_scale, b_mat, neon_mat in buildings:
        spawn_mesh(cube, f"Downtown_{b_name}_Body", b_loc, b_scale, material=b_mat)
        glass_x = b_loc[0] + (250 if b_loc[0] < 0 else -250)
        spawn_mesh(cube, f"Downtown_{b_name}_StorefrontGlass", (glass_x, b_loc[1], 150), (1.5, b_scale[1] * 0.8, 2.5), material=mat_glass)
        spawn_mesh(cube, f"Downtown_{b_name}_NeonSign", (glass_x, b_loc[1], 340), (0.8, 6.0, 1.2), material=neon_mat)

    # 4. Streetlights with Soft Volumetric Falloff
    for y in range(-4500, 5000, 1500):
        for side, pole_x in [("West", curb_x_left - 40), ("East", curb_x_right + 40)]:
            spawn_mesh(cylinder, f"Downtown_LightPole_{side}_{y}", (pole_x, y, 220), (0.25, 0.25, 4.4), material=mat_curb)
            arm_x = pole_x + (60 if side == "West" else -60)
            spawn_mesh(cube, f"Downtown_LightArm_{side}_{y}", (arm_x, y, 440), (1.2, 0.18, 0.18), material=mat_curb)
            
            light_x = arm_x + (50 if side == "West" else -50)
            pt_light = unreal.EditorLevelLibrary.spawn_actor_from_class(
                unreal.PointLight.static_class(),
                unreal.Vector(light_x, y, 420),
                unreal.Rotator(0, 0, 0)
            )
            if pt_light:
                pt_light.set_actor_label(f"Downtown_StreetPointLight_{side}_{y}")
                lt_comp = pt_light.get_component_by_class(unreal.PointLightComponent.static_class())
                if lt_comp:
                    lt_comp.set_intensity(16000.0)
                    lt_comp.set_light_color(unreal.LinearColor(1.0, 0.85, 0.65, 1.0))
                    lt_comp.set_attenuation_radius(1200.0)
                    lt_comp.set_source_radius(25.0)
                    lt_comp.set_soft_source_radius(35.0)
                    lt_comp.set_volumetric_scattering_intensity(2.8)

    # 5. Spawn PlayerStart, Darrell X-Gen Hero, and Wonder Hover Vehicle
    start_loc = unreal.Vector(-200, -4200, 70)
    pstart = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart.static_class(),
        start_loc,
        unreal.Rotator(0, 90.0, 0)
    )
    if pstart:
        pstart.set_actor_label("Downtown_PlayerStart")

    # Spawn Darrell X-Gen Hero
    darrell_cls = getattr(unreal, "DarrellXGenCharacter", None)
    if darrell_cls:
        darrell = unreal.EditorLevelLibrary.spawn_actor_from_class(
            darrell_cls.static_class(),
            unreal.Vector(-200, -3800, 70),
            unreal.Rotator(0, 90.0, 0)
        )
        if darrell:
            darrell.set_actor_label("Downtown_DarrellXGenHero")

    # Spawn Wonder Hover Vehicle
    vehicle = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor.static_class(),
        unreal.Vector(250, -3800, 65),
        unreal.Rotator(0, 90.0, 0)
    )
    if vehicle:
        vehicle.set_actor_label("Downtown_WonderHoverVehicle")
        v_comp = vehicle.get_component_by_class(unreal.StaticMeshComponent.static_class())
        if v_comp:
            v_comp.set_static_mesh(cube)
            v_comp.set_material(0, mat_facade_dark)
            v_comp.set_collision_profile_name("BlockAll")
        vehicle.set_actor_scale3d(unreal.Vector(2.4, 4.8, 1.1))

        # Vehicle Thruster Glow Lights
        for ox, oy in [(-80, -120), (80, -120), (-80, 120), (80, 120)]:
            glow = unreal.EditorLevelLibrary.spawn_actor_from_class(
                unreal.PointLight.static_class(),
                unreal.Vector(250 + ox, -3800 + oy, 35),
                unreal.Rotator(0, 0, 0)
            )
            if glow:
                glow.set_actor_label(f"Downtown_ThrusterGlow_{ox}_{oy}")
                g_comp = glow.get_component_by_class(unreal.PointLightComponent.static_class())
                if g_comp:
                    g_comp.set_intensity(6000.0)
                    g_comp.set_light_color(unreal.LinearColor(0.1, 0.8, 1.0, 1.0)) # Neon Cyan
                    g_comp.set_attenuation_radius(400.0)
                    g_comp.set_volumetric_scattering_intensity(3.0)

    # Save level
    saved = unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"[DowntownShowcase] Downtown City Block Showcase setup complete. Save status: {saved}")
    return saved

result = setup_downtown_block()
