import socket
import struct
import json
import os

def send_cmd(cmd_type, params):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 55557))
    msg = json.dumps({'type': cmd_type, 'params': params}).encode('utf-8')
    s.sendall(struct.pack('>I', len(msg)) + msg)
    raw_len = s.recv(4)
    resp_len = struct.unpack('>I', raw_len)[0]
    data = b''
    while len(data) < resp_len:
        chunk = s.recv(min(resp_len - len(data), 65536))
        if not chunk:
            break
        data += chunk
    s.close()
    return json.loads(data.decode('utf-8'))

def execute_python_in_unreal(code_str):
    return send_cmd('execute_python', {'code': code_str})

def main():
    print("=== Step 1: Importing PBR Textures via UnrealMCP ===")
    tex_dir = r"C:/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon/Assets/WonderWorld/Downtown/Textures"
    textures = [
        "T_Asphalt_Albedo.png",
        "T_Asphalt_Normal.png",
        "T_Asphalt_ORM.png",
        "T_Sidewalk_Albedo.png",
        "T_Sidewalk_Normal.png"
    ]
    for t in textures:
        res = send_cmd('import_asset', {
            'source_file': f"{tex_dir}/{t}",
            'destination_path': '/Game/WonderWorld/Textures/Downtown',
            'destination_name': t.replace('.png', '')
        })
        print(f"Imported Texture {t}: {res.get('status')}")

    print("\n=== Step 2: Importing Nanite Meshes via UnrealMCP ===")
    mesh_dir = r"C:/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon/Assets/WonderWorld/Downtown"
    meshes = [
        "SM_Downtown_Avenue_Nanite.fbx",
        "SM_Downtown_Curb_Sidewalk_Nanite.fbx",
        "SM_Downtown_Building_Facade_Nanite.fbx",
        "SM_WonderHover_Chassis_Nanite.fbx"
    ]
    for m in meshes:
        res = send_cmd('import_asset', {
            'source_file': f"{mesh_dir}/{m}",
            'destination_path': '/Game/WonderWorld/Meshes/Downtown',
            'destination_name': m.replace('.fbx', '')
        })
        print(f"Imported Mesh {m}: {res.get('status')}")

    print("\n=== Step 3: Configuring Nanite, Materials, and Level Layout ===")
    ue_code = """
import unreal

def log(m):
    unreal.log(f"[DowntownBuilder] {m}")

# 1. Enable Nanite on all imported meshes
mesh_names = [
    'SM_Downtown_Avenue_Nanite',
    'SM_Downtown_Curb_Sidewalk_Nanite',
    'SM_Downtown_Building_Facade_Nanite',
    'SM_WonderHover_Chassis_Nanite'
]
for m_name in mesh_names:
    asset_path = f"/Game/WonderWorld/Meshes/Downtown/{m_name}"
    m = unreal.EditorAssetLibrary.load_asset(asset_path)
    if m:
        nanite_settings = m.get_editor_property('nanite_settings')
        nanite_settings.set_editor_property('enabled', True)
        m.set_editor_property('nanite_settings', nanite_settings)
        unreal.EditorAssetLibrary.save_asset(asset_path)
        log(f"Nanite ENABLED on {m_name}")

# 2. Setup Normal Map Textures
for t_name in ['T_Asphalt_Normal', 'T_Sidewalk_Normal']:
    t_asset = unreal.EditorAssetLibrary.load_asset(f"/Game/WonderWorld/Textures/Downtown/{t_name}")
    if t_asset:
        t_asset.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
        t_asset.set_editor_property("s_rgb", False)
        unreal.EditorAssetLibrary.save_asset(f"/Game/WonderWorld/Textures/Downtown/{t_name}")

# 3. Create Scanned PBR Wet Asphalt Material
tools = unreal.AssetToolsHelpers.get_asset_tools()
mat_dest = "/Game/WonderWorld/Materials/Downtown"
mat_asphalt_path = f"{mat_dest}/M_Photoreal_WetAsphalt_Nanite"
mat_asphalt = unreal.EditorAssetLibrary.load_asset(mat_asphalt_path)
if not mat_asphalt:
    factory = unreal.MaterialFactoryNew()
    mat_asphalt = tools.create_asset("M_Photoreal_WetAsphalt_Nanite", mat_dest, unreal.Material, factory)

if mat_asphalt:
    unreal.MaterialEditingLibrary.delete_all_material_expressions(mat_asphalt)
    
    t_alb = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_Albedo")
    if t_alb:
        node_alb = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, -250)
        node_alb.set_editor_property("texture", t_alb)
        unreal.MaterialEditingLibrary.connect_material_property(node_alb, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)

    t_nrm = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_Normal")
    if t_nrm:
        node_nrm = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, 50)
        node_nrm.set_editor_property("texture", t_nrm)
        node_nrm.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        unreal.MaterialEditingLibrary.connect_material_property(node_nrm, "RGB", unreal.MaterialProperty.MP_NORMAL)

    t_orm = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Textures/Downtown/T_Asphalt_ORM")
    if t_orm:
        node_orm = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionTextureSample, -400, 300)
        node_orm.set_editor_property("texture", t_orm)
        unreal.MaterialEditingLibrary.connect_material_property(node_orm, "G", unreal.MaterialProperty.MP_ROUGHNESS)
        unreal.MaterialEditingLibrary.connect_material_property(node_orm, "R", unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)

    node_spec = unreal.MaterialEditingLibrary.create_material_expression(mat_asphalt, unreal.MaterialExpressionScalarParameter, -400, 480)
    node_spec.set_editor_property("parameter_name", "PuddleSpecular")
    node_spec.set_editor_property("default_value", 0.95)
    unreal.MaterialEditingLibrary.connect_material_property(node_spec, "", unreal.MaterialProperty.MP_SPECULAR)

    unreal.MaterialEditingLibrary.recompile_material(mat_asphalt)
    unreal.EditorAssetLibrary.save_asset(mat_asphalt_path)
    log("Recompiled M_Photoreal_WetAsphalt_Nanite")

# 4. Create Scanned PBR Sidewalk Material
mat_sw_path = f"{mat_dest}/M_Photoreal_Sidewalk_Nanite"
mat_sw = unreal.EditorAssetLibrary.load_asset(mat_sw_path)
if not mat_sw:
    factory = unreal.MaterialFactoryNew()
    mat_sw = tools.create_asset("M_Photoreal_Sidewalk_Nanite", mat_dest, unreal.Material, factory)
if mat_sw:
    unreal.MaterialEditingLibrary.delete_all_material_expressions(mat_sw)
    t_salb = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Textures/Downtown/T_Sidewalk_Albedo")
    if t_salb:
        node_salb = unreal.MaterialEditingLibrary.create_material_expression(mat_sw, unreal.MaterialExpressionTextureSample, -400, -200)
        node_salb.set_editor_property("texture", t_salb)
        unreal.MaterialEditingLibrary.connect_material_property(node_salb, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
    t_snrm = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Textures/Downtown/T_Sidewalk_Normal")
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
    log("Recompiled M_Photoreal_Sidewalk_Nanite")

# 5. Populate the Showcase Map with Nanite Meshes
map_path = "/Game/WonderWorld/Maps/WonderWorld_Downtown_Showcase"
unreal.EditorLevelLibrary.load_level(map_path)

# Clear existing downtown actors
for a in unreal.EditorLevelLibrary.get_all_level_actors():
    lbl = a.get_actor_label()
    if lbl.startswith("Downtown_") and not lbl.startswith("Downtown_SunLight") and not lbl.startswith("Downtown_Sky") and not lbl.startswith("Downtown_HeightFog") and not lbl.startswith("Downtown_PostProcess"):
        unreal.EditorLevelLibrary.destroy_actor(a)

mesh_avenue = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_Downtown_Avenue_Nanite")
mesh_curb_sw = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_Downtown_Curb_Sidewalk_Nanite")
mesh_facade = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_Downtown_Building_Facade_Nanite")
mesh_hover = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Meshes/Downtown/SM_WonderHover_Chassis_Nanite")
mat_facade = unreal.EditorAssetLibrary.load_asset("/Game/WonderWorld/Materials/M_Building_Facade_Dark")

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

# 3 Nanite Avenue Segments (120m long boulevard)
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

# Darrell X-Gen Hero
darrell_cls = getattr(unreal, "DarrellXGenCharacter", None)
if darrell_cls:
    hero = unreal.EditorLevelLibrary.spawn_actor_from_class(
        darrell_cls.static_class(),
        unreal.Vector(-200, -3800, 80),
        unreal.Rotator(0, 90, 0)
    )
    if hero: hero.set_actor_label("Downtown_DarrellXGenHero")

# Wonder Hover Vehicle with WonderHoverPhysicsComponent attached
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
                g_comp.set_light_color(unreal.LinearColor(0.05, 0.75, 1.0, 1.0))
                g_comp.set_attenuation_radius(450.0)
                g_comp.set_volumetric_scattering_intensity(3.5)

# Verify Lighting and Atmosphere
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
log("FINISHED_NANITE_ASSEMBLY_SUCCESS")
result = "FINISHED_NANITE_ASSEMBLY_SUCCESS"
"""
    res = execute_python_in_unreal(ue_code)
    print(f"Assembly Result: {res}")

if __name__ == "__main__":
    main()
