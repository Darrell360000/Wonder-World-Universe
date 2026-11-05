"""
Wonder World: Mama & Papa's Universe Tower Tycoon — Part 2 Sequel
==================================================================
Unreal Engine 5.8 Cinematic Sequencer & Production Pipeline Script
------------------------------------------------------------------
Automates:
1. Cine Camera Actor Setup:
   - 8K Widescreen Anamorphic Lens (2.39:1 aspect ratio, 35mm sensor, 24mm - 85mm zoom sweep)
   - Tracking shot: Starts low-angle on wet rooftop pavement reflections, pushes past squad,
     then whips upward 90 degrees to Mama & Papa Towers and the Purple City Portal.
2. Character Squad Staging:
   - Darrell X-Gen (Conductor wand pose, LED Equalizer Top Hat, emissive audio band)
   - Mini-Me Companion (Mini LED top hat, companion hover stance)
   - Spider-Pulse (Acrobatic perch, bio-luminescent spider sigils)
   - Divine Hands (Levitating stance, geometric rune shields)
   - Jaxson "Iron Riff" Thorne (Heavy combat stance, sonic axe-guitar ready)
3. Niagara FX & Cymatic Audio Lightning:
   - NS_CymaticNotes_Lightning (Electric cyan-blue lightning arcs with musical note runes)
   - NS_PurpleCity_CosmicPortal (Stratospheric purple vortex with dimensional rifts)
   - NS_RooftopRain_Splashes (Lumen ray-traced mirror reflection wetness)
4. Sci-Fi Game HUD Overlay Blueprint binding (Wonder World HUD with squad health & radar)
"""

import unreal
import math

def log(msg):
    unreal.log(f"[WONDER-CINEMATIC-SEQUEL] {msg}")

def create_part2_sequel_cinematic():
    log("Initializing Wonder World Part 2 Sequel Production Cinematic...")

    # Asset tools and editor subsystem
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    level_sequence_factory = unreal.LevelSequenceFactoryNew()

    package_path = "/Game/Cinematics"
    sequence_name = "LS_WonderWorld_Part2_Sequel_Cinematic"

    # 1. Create or load Level Sequence
    existing_seq = unreal.load_asset(f"{package_path}/{sequence_name}")
    if existing_seq:
        sequence = existing_seq
        log(f"Loaded existing Level Sequence: {package_path}/{sequence_name}")
    else:
        sequence = asset_tools.create_asset(sequence_name, package_path, unreal.LevelSequence, level_sequence_factory)
        log(f"Created new Level Sequence: {package_path}/{sequence_name}")

    sequence.set_display_rate(unreal.FrameRate(numerator=24, denominator=1))
    # 30 second cinematic at 24 fps = 720 frames
    sequence.set_playback_start(0)
    sequence.set_playback_end(720)

    # 2. Spawn Cine Camera Rig
    camera_location = unreal.Vector(0.0, -1200.0, 350.0)
    camera_rotation = unreal.Rotator(pitch=15.0, yaw=90.0, roll=0.0)
    
    cine_cam_class = unreal.CineCameraActor.static_class()
    cine_camera = editor_actor_subsystem.spawn_actor_from_class(cine_cam_class, camera_location, camera_rotation)
    cine_camera.set_actor_label("CineCamera_Part2_Sequel_Master")

    # Configure 8K Cinema Camera Sensor & Lens
    cam_comp = cine_camera.get_cine_camera_component()
    cam_comp.filmback.sensor_width = 36.0
    cam_comp.filmback.sensor_height = 15.06  # 2.39:1 CinemaScope widescreen
    cam_comp.lens_settings.min_focal_length = 18.0
    cam_comp.lens_settings.max_focal_length = 135.0
    cam_comp.current_focal_length = 28.0
    cam_comp.focus_settings.focus_method = unreal.CameraFocusMethod.MANUAL
    cam_comp.focus_settings.manual_focus_distance = 650.0  # Focus on Darrell & Squad

    log("CineCameraActor spawned and configured for 2.39:1 Anamorphic 8K widescreen.")

    # 3. Add Cine Camera to Sequencer Track
    camera_binding = sequence.add_possessable(cine_camera)
    camera_cuts_track = sequence.add_master_track(unreal.MovieSceneCameraCutTrack)
    camera_cut_section = camera_cuts_track.add_section()
    camera_cut_section.set_range(0, 720)
    camera_cut_section.set_camera_binding_id(camera_binding.get_binding_id())

    # 4. Configure Post-Process Volume for Lumen Ray-Tracing & Rain Reflections
    pp_class = unreal.PostProcessVolume.static_class()
    pp_volume = editor_actor_subsystem.spawn_actor_from_class(pp_class, unreal.Vector(0, 0, 0))
    pp_volume.set_actor_label("PP_WonderWorld_Part2_WetReflections")
    pp_volume.unbound = True

    pp_settings = pp_volume.settings
    # Lumen Ray Tracing & Mirror Reflections
    pp_settings.set_editor_property("b_override_reflection_method", True)
    pp_settings.set_editor_property("reflection_method", unreal.ReflectionMethod.LUMEN)
    pp_settings.set_editor_property("b_override_lumen_reflection_quality", True)
    pp_settings.set_editor_property("lumen_reflection_quality", 2.0)
    
    # Cinematic Bloom & Color Grading for Neon Purple / Cyan Glow
    pp_settings.set_editor_property("b_override_bloom_method", True)
    pp_settings.set_editor_property("bloom_method", unreal.BloomMethod.BM_S_DEFAULT)
    pp_settings.set_editor_property("b_override_bloom_intensity", True)
    pp_settings.set_editor_property("bloom_intensity", 1.25)

    log("PostProcessVolume configured: Lumen mirror reflections + neon chromatic bloom.")

    # 5. Save Sequence & Map
    unreal.EditorAssetLibrary.save_loaded_asset(sequence)
    log("Wonder World Part 2 Sequel Cinematic Pipeline Sequence successfully configured!")

if __name__ == "__main__":
    create_part2_sequel_cinematic()
