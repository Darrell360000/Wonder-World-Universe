using System;
using System.Collections.Generic;
using Game;
using Unity.Mathematics;
using UnityEngine;
using WonderWorldCS2.Systems;

namespace WonderWorld.Cities2Mod.Vehicles
{
    public enum ModularVehicleSlot
    {
        ChassisCore = 0,
        SubBassRepulsorThrusters = 1,
        SonicPhaserFrontGrille = 2,
        RainbowEqualizerAeroWing = 3,
        TopHatMagneticRoofTurret = 4
    }

    [Serializable]
    public sealed class ModularVehiclePart
    {
        public ModularVehicleSlot Slot;
        public string PartId;
        public string DisplayName;
        public float ThrustMultiplier;
        public float StabilityBonus;
        public float WonderCurrencyCost;
        public bool IsAttached;

        public ModularVehiclePart(
            ModularVehicleSlot slot,
            string partId,
            string displayName,
            float thrustMultiplier,
            float stabilityBonus,
            float wonderCurrencyCost,
            bool isAttached = true)
        {
            Slot = slot;
            PartId = partId;
            DisplayName = displayName;
            ThrustMultiplier = thrustMultiplier;
            StabilityBonus = stabilityBonus;
            WonderCurrencyCost = wonderCurrencyCost;
            IsAttached = isAttached;
        }
    }

    /// <summary>
    /// Modular Street-to-Sky Hover-Drive Vehicle Controller for Cities: Skylines II (Project Purple City).
    /// Press [V] in-game to toggle between Strategic City Camera and live 3D Street-to-Sky Hover-Drive Flight
    /// via Game.CameraController + a 3D modular Hover-Cruiser mesh.
    /// </summary>
    public sealed class HoverDriveVehicle : MonoBehaviour
    {
        public static HoverDriveVehicle ActiveInstance { get; private set; }

        [Header("Hover-Drive Flight State ([V] Toggle)")]
        public bool IsHoverFlightActive = false;
        public float TargetHoverAltitudeMeters = 24.0f;
        public float CurrentAltitudeMeters = 0.0f;
        public float BaseHoverLiftForce = 45.0f;
        public float Harmonic85BpmPulseHz = 85.0f / 60.0f; // 1.4167 Hz beat clock

        [Header("Physics & Propulsion")]
        public Vector3 CurrentVelocity = Vector3.zero;
        public float ForwardSpeedMps = 85.0f;
        public float CurrentSpeedKmh = 0.0f;
        public bool SonicBoostActive = false;

        private float _savedStrategicZoom = 180.0f;
        private float2 _savedStrategicAngle = new float2(0f, 45f);
        private bool _hasSavedCameraState = false;

        private GameObject _cruiserRoot;
        private readonly Dictionary<ModularVehicleSlot, GameObject> _partVisuals =
            new Dictionary<ModularVehicleSlot, GameObject>();
        private readonly List<Renderer> _allCruiserRenderers = new List<Renderer>();
        private readonly List<Renderer> _glowRenderers = new List<Renderer>();
        private readonly List<Material> _glowMaterials = new List<Material>();
        private readonly List<Color> _glowBaseColors = new List<Color>();

        [Header("Visibility / Cloak ([Alt+5], HUD, or Options)")]
        public bool IsCloaked = false;
        public float CruiserVisualScale = 2.0f;
        public string ActiveShaderName { get; private set; } = "(not spawned)";

        private readonly Dictionary<ModularVehicleSlot, ModularVehiclePart> _modularLoadout =
            new Dictionary<ModularVehicleSlot, ModularVehiclePart>();

        public IReadOnlyDictionary<ModularVehicleSlot, ModularVehiclePart> ModularLoadout => _modularLoadout;

        private void Awake()
        {
            ActiveInstance = this;
            InitializeDefaultModularLoadout();
        }

        public void InitializeDefaultModularLoadout()
        {
            _modularLoadout[ModularVehicleSlot.ChassisCore] = new ModularVehiclePart(
                ModularVehicleSlot.ChassisCore,
                "MOD_CHASSIS_PURPLE_MK5",
                "Purple City Obsidian-Gold Monocoque",
                1.00f,
                1.00f,
                0f,
                true);

            _modularLoadout[ModularVehicleSlot.SubBassRepulsorThrusters] = new ModularVehiclePart(
                ModularVehicleSlot.SubBassRepulsorThrusters,
                "MOD_THRUST_85BPM_SUB",
                "85 BPM Sub-Bass Repulsor Pods (x4)",
                1.45f,
                1.20f,
                15.0f,
                true);

            _modularLoadout[ModularVehicleSlot.SonicPhaserFrontGrille] = new ModularVehiclePart(
                ModularVehicleSlot.SonicPhaserFrontGrille,
                "MOD_GRILLE_GODHAND_CYAN",
                "God-Hand Cyan Sonic Wave Splitter",
                1.15f,
                1.10f,
                10.0f,
                true);

            _modularLoadout[ModularVehicleSlot.RainbowEqualizerAeroWing] = new ModularVehiclePart(
                ModularVehicleSlot.RainbowEqualizerAeroWing,
                "MOD_WING_5STOP_LED",
                "5-Stop Rainbow LED Equalizer Wing",
                1.20f,
                1.35f,
                12.5f,
                true);

            _modularLoadout[ModularVehicleSlot.TopHatMagneticRoofTurret] = new ModularVehiclePart(
                ModularVehicleSlot.TopHatMagneticRoofTurret,
                "MOD_TURRET_TOPHAT_HOMING",
                "Magnetic Top-Hat Roof Turret",
                1.10f,
                1.05f,
                20.0f,
                true);
        }

        private int _lastTickedFrame = -1;

        private void Update()
        {
            TickFrame();
        }

        public void TickFrame()
        {
            if (_lastTickedFrame == Time.frameCount) return;
            _lastTickedFrame = Time.frameCount;

            if (Input.GetKeyDown(KeyCode.V) || Input.GetKeyDown(KeyCode.F4))
            {
                ToggleHoverFlight();
            }

            // Alt + 1..4 hotkeys to toggle modular vehicle attachments in-flight
            if (Input.GetKey(KeyCode.LeftAlt) || Input.GetKey(KeyCode.RightAlt))
            {
                if (Input.GetKeyDown(KeyCode.Alpha1)) AttachOrToggleModularPart(ModularVehicleSlot.SubBassRepulsorThrusters);
                if (Input.GetKeyDown(KeyCode.Alpha2)) AttachOrToggleModularPart(ModularVehicleSlot.SonicPhaserFrontGrille);
                if (Input.GetKeyDown(KeyCode.Alpha3)) AttachOrToggleModularPart(ModularVehicleSlot.RainbowEqualizerAeroWing);
                if (Input.GetKeyDown(KeyCode.Alpha4)) AttachOrToggleModularPart(ModularVehicleSlot.TopHatMagneticRoofTurret);
                if (Input.GetKeyDown(KeyCode.Alpha5)) ToggleCloak();
            }

            UpdateCruiserVisualEffects();
            SimulateHoverPhysicsAndCamera(Time.unscaledDeltaTime);
        }

        /// <summary>
        /// Toggles 85 BPM Cloak Mode. When cloaked the Hover-Cruiser is invisible except for a faint
        /// cyan shimmer pulse on every beat so you can still find it. Toggle again to make it fully visible.
        /// </summary>
        public bool ToggleCloak()
        {
            IsCloaked = !IsCloaked;
            ApplyCloakState(true);
            string state = IsCloaked ? "CLOAK ENGAGED (Invisible + 85 BPM shimmer)" : "CLOAK OFF (Fully Visible)";
            MamaTowerCities2System.LastVoiceBroadcast = $"Darrell X-Gen: Hover-Cruiser {state}!";
            WonderWorldCS2Plugin.ModLogger?.LogInfo($"[HoverDriveVehicle] {state}");
            return IsCloaked;
        }

        public void SetCloak(bool cloaked)
        {
            if (IsCloaked != cloaked) ToggleCloak();
        }

        public bool ToggleHoverFlight()
        {
            if (_modularLoadout.Count == 0)
            {
                InitializeDefaultModularLoadout();
            }

            IsHoverFlightActive = !IsHoverFlightActive;

            try
            {
                if (CameraController.TryGet(out CameraController cam) && cam != null)
                {
                    if (IsHoverFlightActive)
                    {
                        _savedStrategicZoom = cam.zoom;
                        _savedStrategicAngle = cam.angle;
                        _hasSavedCameraState = true;

                        // Drop camera into low-altitude street-to-sky hover cruise perspective
                        TargetHoverAltitudeMeters = 24.0f;
                        CurrentAltitudeMeters = TargetHoverAltitudeMeters * GetTotalThrustMultiplier();
                        cam.zoom = Mathf.Clamp(CurrentAltitudeMeters, 12.0f, 300.0f);
                        cam.angle = new float2(cam.angle.x, 16.0f);

                        EnsureCruiserVisualSpawned();
                        if (_cruiserRoot != null) _cruiserRoot.SetActive(true);
                    }
                    else
                    {
                        if (_hasSavedCameraState)
                        {
                            cam.zoom = Mathf.Max(65.0f, _savedStrategicZoom);
                            cam.angle = new float2(cam.angle.x, Mathf.Clamp(_savedStrategicAngle.y, 30.0f, 65.0f));
                        }
                        if (_cruiserRoot != null) _cruiserRoot.SetActive(false);
                        CurrentSpeedKmh = 0f;
                    }
                }
            }
            catch (Exception ex)
            {
                WonderWorldCS2Plugin.ModLogger?.LogWarning($"[HoverDriveVehicle] Camera hook notice: {ex.Message}");
            }

            string modeLabel = IsHoverFlightActive
                ? $"ENGAGED (Altitude: {TargetHoverAltitudeMeters:F1}m | Thrust: {GetTotalThrustMultiplier():F2}x)"
                : "DISENGAGED (Strategic City View)";

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Darrell X-Gen: [V] Hover-Drive {modeLabel}!";
            WonderWorldCS2Plugin.ModLogger?.LogInfo($"[HoverDriveVehicle] [V] Hover Flight {modeLabel}");

            return IsHoverFlightActive;
        }

        public bool AttachOrToggleModularPart(ModularVehicleSlot slot)
        {
            if (_modularLoadout.Count == 0)
            {
                InitializeDefaultModularLoadout();
            }

            if (!_modularLoadout.TryGetValue(slot, out ModularVehiclePart part))
            {
                return false;
            }

            part.IsAttached = !part.IsAttached;
            if (_partVisuals.TryGetValue(slot, out GameObject visualGo) && visualGo != null)
            {
                visualGo.SetActive(part.IsAttached);
            }

            string state = part.IsAttached ? "ATTACHED" : "DETACHED";
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Papa AI: Modular Part [{part.DisplayName}] {state} (Thrust: {GetTotalThrustMultiplier():F2}x)!";
            WonderWorldCS2Plugin.ModLogger?.LogInfo(
                $"[HoverDriveVehicle] Modular Part {part.DisplayName} -> {state} (Thrust: {GetTotalThrustMultiplier():F2}x)");
            return part.IsAttached;
        }

        public float GetTotalThrustMultiplier()
        {
            if (_modularLoadout.Count == 0)
            {
                InitializeDefaultModularLoadout();
            }

            float mult = 1.0f;
            foreach (var kvp in _modularLoadout)
            {
                if (kvp.Value.IsAttached && kvp.Key != ModularVehicleSlot.ChassisCore)
                {
                    mult += (kvp.Value.ThrustMultiplier - 1.0f);
                }
            }
            return mult;
        }

        public float GetTotalStabilityScore()
        {
            if (_modularLoadout.Count == 0)
            {
                InitializeDefaultModularLoadout();
            }

            float score = 100.0f;
            foreach (var kvp in _modularLoadout)
            {
                if (kvp.Value.IsAttached)
                {
                    score *= kvp.Value.StabilityBonus;
                }
            }
            return score;
        }

        private void SimulateHoverPhysicsAndCamera(float dt)
        {
            if (!IsHoverFlightActive)
            {
                CurrentAltitudeMeters = Mathf.MoveTowards(CurrentAltitudeMeters, 0.0f, (BaseHoverLiftForce * 2.0f) * dt);
                CurrentSpeedKmh = 0f;
                return;
            }

            // Altitude climb/descent controls while in Hover-Drive mode:
            // [R] / [PageUp] climbs; R is reserved for disc recall while the disc is out.
            bool discRecallUsesR = WonderHeroActionMechanics.Instance != null &&
                WonderHeroActionMechanics.Instance.IsDiscThrown;
            if (Input.GetKey(KeyCode.PageUp) || (Input.GetKey(KeyCode.R) && !discRecallUsesR))
            {
                TargetHoverAltitudeMeters = Mathf.Clamp(TargetHoverAltitudeMeters + 35.0f * dt, 8.0f, 260.0f);
            }
            if (Input.GetKey(KeyCode.F) || Input.GetKey(KeyCode.PageDown))
            {
                TargetHoverAltitudeMeters = Mathf.Clamp(TargetHoverAltitudeMeters - 35.0f * dt, 8.0f, 260.0f);
            }

            SonicBoostActive = Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift);
            float thrustMult = GetTotalThrustMultiplier();
            float beatOscillation = Mathf.Sin(Time.unscaledTime * Harmonic85BpmPulseHz * Mathf.PI * 2.0f) * 0.85f;
            float desiredAltitude = TargetHoverAltitudeMeters + beatOscillation;
            CurrentAltitudeMeters = Mathf.MoveTowards(CurrentAltitudeMeters, desiredAltitude, BaseHoverLiftForce * dt);

            try
            {
                if (CameraController.TryGet(out CameraController cam) && cam != null)
                {
                    // Yaw steering with Q / E
                    float yawDelta = 0f;
                    if (Input.GetKey(KeyCode.Q)) yawDelta -= 75.0f * dt;
                    if (Input.GetKey(KeyCode.E)) yawDelta += 75.0f * dt;

                    float2 angle = cam.angle;
                    angle.x += yawDelta;
                    // Keep a sleek low-angle hover pitch between 10 and 28 degrees
                    angle.y = Mathf.Clamp(angle.y, 10.0f, 32.0f);
                    cam.angle = angle;

                    // Lock zoom to Hover-Drive altitude with 85 BPM harmonic bobbing
                    cam.zoom = Mathf.Clamp(CurrentAltitudeMeters, 10.0f, 300.0f);

                    // Extra Hover-Drive propulsion boost when WASD/Arrows are held
                    float moveFwd = 0f;
                    float moveRight = 0f;
                    if (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow)) moveFwd += 1f;
                    if (Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow)) moveFwd -= 1f;
                    if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow)) moveRight += 1f;
                    if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow)) moveLeftOrRight(ref moveRight, -1f);

                    float boostFactor = SonicBoostActive ? 2.4f : 1.0f;
                    float speedMps = ForwardSpeedMps * thrustMult * boostFactor;

                    if (Mathf.Abs(moveFwd) > 0.01f || Mathf.Abs(moveRight) > 0.01f)
                    {
                        float yawRad = angle.x * Mathf.Deg2Rad;
                        Vector3 forwardDir = new Vector3(-Mathf.Sin(yawRad), 0f, -Mathf.Cos(yawRad));
                        Vector3 rightDir = new Vector3(-forwardDir.z, 0f, forwardDir.x);

                        Vector3 deltaPos = (forwardDir * moveFwd + rightDir * moveRight).normalized * (speedMps * dt);
                        cam.pivot += deltaPos;
                        CurrentVelocity = deltaPos / Mathf.Max(0.0001f, dt);
                        CurrentSpeedKmh = speedMps * 3.6f;
                    }
                    else
                    {
                        CurrentVelocity = Vector3.zero;
                        CurrentSpeedKmh = 0f;
                    }

                    UpdateCruiserVisualTransform(cam, beatOscillation);
                }
            }
            catch
            {
            }
        }

        private static void moveLeftOrRight(ref float val, float delta)
        {
            val += delta;
        }

        private void EnsureCruiserVisualSpawned()
        {
            if (_cruiserRoot != null) return;

            try
            {
                _cruiserRoot = new GameObject("PurpleCity_HoverDrive_Cruiser3D");
                DontDestroyOnLoad(_cruiserRoot);
                _cruiserRoot.transform.localScale = Vector3.one * CruiserVisualScale;

                Color purple = new Color(0.42f, 0.10f, 0.78f);
                Color gold = new Color(1.00f, 0.78f, 0.10f);
                Color cyan = new Color(0.00f, 0.95f, 1.00f);
                Color obsidian = new Color(0.06f, 0.05f, 0.09f);
                Color glass = new Color(0.20f, 0.55f, 0.95f);

                // 0. Chassis: purple monocoque + gold trim rails + glass canopy + headlights/taillights
                GameObject chassis = new GameObject("Chassis_Monocoque");
                chassis.transform.SetParent(_cruiserRoot.transform, false);
                CreatePart(PrimitiveType.Cube, "Body", chassis.transform, new Vector3(0f, 0f, 0f), new Vector3(2.4f, 0.65f, 5.2f), purple, 0f);
                CreatePart(PrimitiveType.Cube, "Nose", chassis.transform, new Vector3(0f, -0.08f, 2.95f), new Vector3(2.0f, 0.45f, 0.9f), purple, 0f);
                CreatePart(PrimitiveType.Cube, "GoldRail_L", chassis.transform, new Vector3(-1.23f, 0.05f, 0f), new Vector3(0.08f, 0.18f, 5.3f), gold, 0.4f);
                CreatePart(PrimitiveType.Cube, "GoldRail_R", chassis.transform, new Vector3(1.23f, 0.05f, 0f), new Vector3(0.08f, 0.18f, 5.3f), gold, 0.4f);
                CreatePart(PrimitiveType.Sphere, "Canopy", chassis.transform, new Vector3(0f, 0.45f, 0.6f), new Vector3(1.6f, 0.7f, 2.4f), glass, 0.25f);
                CreatePart(PrimitiveType.Cube, "Headlight_L", chassis.transform, new Vector3(-0.7f, 0f, 3.42f), new Vector3(0.45f, 0.16f, 0.06f), Color.white, 3.0f);
                CreatePart(PrimitiveType.Cube, "Headlight_R", chassis.transform, new Vector3(0.7f, 0f, 3.42f), new Vector3(0.45f, 0.16f, 0.06f), Color.white, 3.0f);
                CreatePart(PrimitiveType.Cube, "Taillight", chassis.transform, new Vector3(0f, 0.05f, -2.62f), new Vector3(2.0f, 0.12f, 0.06f), new Color(1f, 0.1f, 0.35f), 2.5f);
                CreatePart(PrimitiveType.Cube, "Underglow", chassis.transform, new Vector3(0f, -0.36f, 0f), new Vector3(2.0f, 0.04f, 4.6f), cyan, 2.5f);
                _partVisuals[ModularVehicleSlot.ChassisCore] = chassis;

                // 1. 85 BPM Sub-Bass Repulsor Pods (x4) with glowing cyan cores
                GameObject repulsors = new GameObject("SubBass_Repulsor_Pods");
                repulsors.transform.SetParent(_cruiserRoot.transform, false);
                Vector3[] podOffsets =
                {
                    new Vector3(-1.45f, -0.3f, 1.8f),
                    new Vector3(1.45f, -0.3f, 1.8f),
                    new Vector3(-1.45f, -0.3f, -1.8f),
                    new Vector3(1.45f, -0.3f, -1.8f)
                };
                foreach (var offset in podOffsets)
                {
                    CreatePart(PrimitiveType.Cylinder, "RepulsorPod", repulsors.transform, offset, new Vector3(0.85f, 0.22f, 0.85f), obsidian, 0f);
                    CreatePart(PrimitiveType.Cylinder, "RepulsorCore", repulsors.transform, offset + new Vector3(0f, -0.2f, 0f), new Vector3(0.6f, 0.04f, 0.6f), cyan, 4.0f);
                }
                _partVisuals[ModularVehicleSlot.SubBassRepulsorThrusters] = repulsors;

                // 2. God-Hand Cyan Sonic Front Grille (3 glowing slats)
                GameObject grille = new GameObject("Sonic_Front_Grille");
                grille.transform.SetParent(_cruiserRoot.transform, false);
                for (int i = 0; i < 3; i++)
                {
                    CreatePart(PrimitiveType.Cube, "GrilleSlat", grille.transform, new Vector3(0f, -0.2f + i * 0.12f, 3.43f), new Vector3(1.2f, 0.05f, 0.05f), cyan, 3.0f);
                }
                _partVisuals[ModularVehicleSlot.SonicPhaserFrontGrille] = grille;

                // 3. 5-Stop Rainbow LED Equalizer Aero Wing (Blue -> Green -> Yellow -> Orange -> Red)
                GameObject wing = new GameObject("Rainbow_Equalizer_Wing");
                wing.transform.SetParent(_cruiserRoot.transform, false);
                CreatePart(PrimitiveType.Cube, "WingStrut_L", wing.transform, new Vector3(-0.9f, 0.55f, -2.3f), new Vector3(0.1f, 0.5f, 0.2f), obsidian, 0f);
                CreatePart(PrimitiveType.Cube, "WingStrut_R", wing.transform, new Vector3(0.9f, 0.55f, -2.3f), new Vector3(0.1f, 0.5f, 0.2f), obsidian, 0f);
                Color[] rainbow =
                {
                    new Color(0.1f, 0.35f, 1f), new Color(0.1f, 1f, 0.3f), new Color(1f, 0.95f, 0.1f),
                    new Color(1f, 0.5f, 0.05f), new Color(1f, 0.1f, 0.1f)
                };
                for (int i = 0; i < rainbow.Length; i++)
                {
                    float x = -1.24f + i * 0.62f;
                    CreatePart(PrimitiveType.Cube, "EqualizerBar", wing.transform, new Vector3(x, 0.85f, -2.35f), new Vector3(0.6f, 0.14f, 0.75f), rainbow[i], 2.0f);
                }
                _partVisuals[ModularVehicleSlot.RainbowEqualizerAeroWing] = wing;

                // 4. Magnetic Top-Hat Roof Turret (black velvet top hat + rainbow band)
                GameObject turret = new GameObject("TopHat_Roof_Turret");
                turret.transform.SetParent(_cruiserRoot.transform, false);
                CreatePart(PrimitiveType.Cylinder, "HatBrim", turret.transform, new Vector3(0f, 0.95f, -0.9f), new Vector3(1.1f, 0.04f, 1.1f), obsidian, 0f);
                CreatePart(PrimitiveType.Cylinder, "HatCrown", turret.transform, new Vector3(0f, 1.35f, -0.9f), new Vector3(0.7f, 0.4f, 0.7f), obsidian, 0f);
                CreatePart(PrimitiveType.Cylinder, "HatBand", turret.transform, new Vector3(0f, 1.05f, -0.9f), new Vector3(0.72f, 0.06f, 0.72f), gold, 2.5f);
                _partVisuals[ModularVehicleSlot.TopHatMagneticRoofTurret] = turret;

                // Respect current modular loadout attach states
                foreach (var kvp in _modularLoadout)
                {
                    if (_partVisuals.TryGetValue(kvp.Key, out GameObject go) && go != null)
                    {
                        go.SetActive(kvp.Value.IsAttached);
                    }
                }

                ApplyCameraLayer(_cruiserRoot);
                ApplyCloakState(false);

                WonderWorldCS2Plugin.ModLogger?.LogInfo(
                    $"[HoverDriveVehicle] Hover-Cruiser spawned with {_allCruiserRenderers.Count} visible parts using shader '{ActiveShaderName}' (layer {_cruiserRoot.layer}).");
            }
            catch (Exception ex)
            {
                WonderWorldCS2Plugin.ModLogger?.LogWarning($"[HoverDriveVehicle] Cruiser spawn warning: {ex.Message}");
            }
        }

        private static Shader _cachedShader;
        private static bool _shaderSearched;

        /// <summary>
        /// Resolves a shader that the Cities: Skylines II HDRP pipeline can draw on plain GameObject meshes.
        /// (City building shaders only render through CS2's ECS batch renderer, which made the old cruiser invisible.)
        /// </summary>
        private static Shader ResolveVisibleShader()
        {
            if (_shaderSearched) return _cachedShader;
            _shaderSearched = true;

            string[] candidates = { "HDRP/Lit", "HDRP/LitTessellation", "HDRP/Unlit", "Universal Render Pipeline/Lit", "Standard", "Unlit/Color" };
            foreach (string name in candidates)
            {
                try
                {
                    Shader s = Shader.Find(name);
                    if (s != null && s.isSupported)
                    {
                        _cachedShader = s;
                        return s;
                    }
                }
                catch
                {
                }
            }

            // Fallback: borrow any loaded HDRP Lit shader instance from memory
            try
            {
                foreach (Shader s in Resources.FindObjectsOfTypeAll<Shader>())
                {
                    if (s != null && s.isSupported && s.name.IndexOf("HDRP/Lit", StringComparison.OrdinalIgnoreCase) >= 0)
                    {
                        _cachedShader = s;
                        return s;
                    }
                }
            }
            catch
            {
            }

            return null;
        }

        private GameObject CreatePart(
            PrimitiveType type,
            string name,
            Transform parent,
            Vector3 localPos,
            Vector3 localScale,
            Color tint,
            float glowStrength)
        {
            GameObject go = GameObject.CreatePrimitive(type);
            go.name = name;
            go.transform.SetParent(parent, false);
            go.transform.localPosition = localPos;
            go.transform.localScale = localScale;

            var col = go.GetComponent<Collider>();
            if (col != null) Destroy(col);

            var mr = go.GetComponent<MeshRenderer>();
            if (mr == null) return go;

            mr.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.On;
            mr.receiveShadows = true;
            mr.allowOcclusionWhenDynamic = false;

            Shader shader = ResolveVisibleShader();
            Material mat = shader != null ? new Material(shader) : (mr.sharedMaterial != null ? new Material(mr.sharedMaterial) : null);
            if (mat == null) return go;

            ActiveShaderName = mat.shader != null ? mat.shader.name : "(unknown)";
            mat.name = "WW_HoverCruiser_" + name;

            SetColorIfPresent(mat, "_BaseColor", tint);
            SetColorIfPresent(mat, "_Color", tint);
            SetColorIfPresent(mat, "_UnlitColor", tint);
            if (mat.HasProperty("_Metallic")) mat.SetFloat("_Metallic", glowStrength > 0f ? 0.1f : 0.65f);
            if (mat.HasProperty("_Smoothness")) mat.SetFloat("_Smoothness", 0.8f);

            if (glowStrength > 0f)
            {
                // HDRP emissive is physically based (nits) so it needs a strong value to read under daylight exposure
                Color emissive = tint * (glowStrength * 6000f);
                mat.EnableKeyword("_EMISSIVE_COLOR_MAP");
                SetColorIfPresent(mat, "_EmissiveColor", emissive);
                SetColorIfPresent(mat, "_EmissionColor", tint * glowStrength);
                mat.EnableKeyword("_EMISSION");
                if (mat.HasProperty("_EmissiveIntensity")) mat.SetFloat("_EmissiveIntensity", glowStrength * 6000f);

                _glowRenderers.Add(mr);
                _glowMaterials.Add(mat);
                _glowBaseColors.Add(tint);
            }

            mr.sharedMaterial = mat;
            _allCruiserRenderers.Add(mr);
            return go;
        }

        private static void SetColorIfPresent(Material mat, string prop, Color c)
        {
            if (mat.HasProperty(prop)) mat.SetColor(prop, c);
        }

        /// <summary>
        /// Put the cruiser on a layer the gameplay camera actually renders.
        /// </summary>
        private static void ApplyCameraLayer(GameObject root)
        {
            int layer = 0;
            try
            {
                Camera cam = Camera.main;
                if (cam == null)
                {
                    foreach (Camera c in Camera.allCameras)
                    {
                        if (c != null && c.enabled) { cam = c; break; }
                    }
                }

                if (cam != null)
                {
                    int mask = cam.cullingMask;
                    if ((mask & 1) == 0)
                    {
                        for (int i = 0; i < 32; i++)
                        {
                            if ((mask & (1 << i)) != 0) { layer = i; break; }
                        }
                    }
                }
            }
            catch
            {
            }

            foreach (Transform t in root.GetComponentsInChildren<Transform>(true))
            {
                t.gameObject.layer = layer;
            }
        }

        private void ApplyCloakState(bool announce)
        {
            if (_cruiserRoot == null) return;
            foreach (Renderer r in _allCruiserRenderers)
            {
                if (r != null) r.enabled = !IsCloaked;
            }
        }

        /// <summary>
        /// 85 BPM visual effects: pulsing glow on emissive parts, and the cloak shimmer when invisible.
        /// </summary>
        private void UpdateCruiserVisualEffects()
        {
            if (_cruiserRoot == null || !_cruiserRoot.activeSelf) return;

            float beatPhase = (Time.unscaledTime * Harmonic85BpmPulseHz) % 1.0f;
            float pulse = 0.65f + 0.35f * Mathf.Cos(beatPhase * Mathf.PI * 2.0f);

            if (IsCloaked)
            {
                // Brief, dim cyan shimmer on each beat.
                bool shimmer = beatPhase < 0.08f;
                Color shimmerColor = new Color(0f, 0.18f, 0.2f, 1f);
                for (int i = 0; i < _glowMaterials.Count; i++)
                {
                    Material material = _glowMaterials[i];
                    if (material == null) continue;

                    SetColorIfPresent(material, "_BaseColor", shimmerColor);
                    SetColorIfPresent(material, "_Color", shimmerColor);
                    SetColorIfPresent(material, "_UnlitColor", shimmerColor);
                    if (material.HasProperty("_EmissiveColor"))
                    {
                        material.SetColor("_EmissiveColor", shimmer ? shimmerColor * 180f : Color.black);
                    }
                    if (material.HasProperty("_EmissionColor"))
                    {
                        material.SetColor("_EmissionColor", shimmer ? shimmerColor * 0.18f : Color.black);
                    }
                }

                foreach (Renderer renderer in _allCruiserRenderers)
                {
                    if (renderer != null) renderer.enabled = false;
                }
                foreach (Renderer renderer in _glowRenderers)
                {
                    if (renderer != null) renderer.enabled = shimmer;
                }
                return;
            }

            float boost = SonicBoostActive ? 1.8f : 1.0f;
            for (int i = 0; i < _glowMaterials.Count; i++)
            {
                Material m = _glowMaterials[i];
                if (m == null) continue;
                Color baseC = _glowBaseColors[i];
                SetColorIfPresent(m, "_BaseColor", baseC);
                SetColorIfPresent(m, "_Color", baseC);
                SetColorIfPresent(m, "_UnlitColor", baseC);
                if (m.HasProperty("_EmissiveColor")) m.SetColor("_EmissiveColor", baseC * (pulse * boost * 12000f));
                if (m.HasProperty("_EmissionColor")) m.SetColor("_EmissionColor", baseC * (pulse * boost * 2f));
            }
        }

        private void UpdateCruiserVisualTransform(CameraController cam, float beatOscillation)
        {
            if (_cruiserRoot == null) return;

            Vector3 pivot = cam.pivot;
            float yawDeg = cam.angle.x;
            // Float the cruiser above the street at the camera focus point, bobbing on the 85 BPM beat
            _cruiserRoot.transform.position = pivot + new Vector3(0f, 3.0f + beatOscillation * 0.6f, 0f);

            // Bank into turns when strafing / rotating
            float bank = 0f;
            if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.Q)) bank = 12f;
            if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow) || Input.GetKey(KeyCode.E)) bank = -12f;
            float pitch = (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow)) ? 4f : 0f;

            Quaternion target = Quaternion.Euler(pitch, yawDeg + 180f, bank);
            _cruiserRoot.transform.rotation = Quaternion.Slerp(_cruiserRoot.transform.rotation, target, 0.15f);
        }
    }
}
