using System;
using System.Collections.Generic;
using Game;
using Unity.Mathematics;
using UnityEngine;

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

            if (Input.GetKeyDown(KeyCode.V))
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
            }

            SimulateHoverPhysicsAndCamera(Time.unscaledDeltaTime);
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
            // [Space] / [PageUp] climbs; [C] / [LeftControl] / [PageDown] descends
            if (Input.GetKey(KeyCode.Space) || Input.GetKey(KeyCode.PageUp))
            {
                TargetHoverAltitudeMeters = Mathf.Clamp(TargetHoverAltitudeMeters + 35.0f * dt, 8.0f, 260.0f);
            }
            if (Input.GetKey(KeyCode.C) || Input.GetKey(KeyCode.LeftControl) || Input.GetKey(KeyCode.PageDown))
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

                // Find an existing HDRP material in the scene so our cruiser renders cleanly in HDRP
                Material baseMat = FindExistingHdrpMaterial();

                // 0. Main Purple-Gold Monocoque Body
                GameObject chassis = CreatePartPrimitive(
                    PrimitiveType.Cube,
                    "Chassis_Monocoque",
                    _cruiserRoot.transform,
                    new Vector3(0f, 0f, 0f),
                    new Vector3(2.4f, 0.65f, 5.2f),
                    baseMat,
                    new Color(0.45f, 0.12f, 0.78f));
                _partVisuals[ModularVehicleSlot.ChassisCore] = chassis;

                // 1. 85 BPM Sub-Bass Repulsor Pods (x4)
                GameObject repulsors = new GameObject("SubBass_Repulsor_Pods");
                repulsors.transform.SetParent(_cruiserRoot.transform, false);
                Vector3[] podOffsets = new[]
                {
                    new Vector3(-1.35f, -0.25f, 1.8f),
                    new Vector3(1.35f, -0.25f, 1.8f),
                    new Vector3(-1.35f, -0.25f, -1.8f),
                    new Vector3(1.35f, -0.25f, -1.8f)
                };
                foreach (var offset in podOffsets)
                {
                    CreatePartPrimitive(
                        PrimitiveType.Cylinder,
                        "RepulsorPod",
                        repulsors.transform,
                        offset,
                        new Vector3(0.75f, 0.22f, 0.75f),
                        baseMat,
                        new Color(0.0f, 0.95f, 1.0f));
                }
                _partVisuals[ModularVehicleSlot.SubBassRepulsorThrusters] = repulsors;

                // 2. God-Hand Cyan Sonic Front Grille
                GameObject grille = CreatePartPrimitive(
                    PrimitiveType.Cube,
                    "Sonic_Front_Grille",
                    _cruiserRoot.transform,
                    new Vector3(0f, -0.05f, 2.75f),
                    new Vector3(2.1f, 0.35f, 0.45f),
                    baseMat,
                    new Color(0.05f, 1.0f, 0.92f));
                _partVisuals[ModularVehicleSlot.SonicPhaserFrontGrille] = grille;

                // 3. 5-Stop Rainbow LED Equalizer Aero Wing
                GameObject wing = CreatePartPrimitive(
                    PrimitiveType.Cube,
                    "Rainbow_Equalizer_Wing",
                    _cruiserRoot.transform,
                    new Vector3(0f, 0.65f, -2.35f),
                    new Vector3(2.9f, 0.14f, 0.75f),
                    baseMat,
                    new Color(1.0f, 0.84f, 0.0f));
                _partVisuals[ModularVehicleSlot.RainbowEqualizerAeroWing] = wing;

                // 4. Magnetic Top-Hat Roof Turret
                GameObject turret = CreatePartPrimitive(
                    PrimitiveType.Cylinder,
                    "TopHat_Roof_Turret",
                    _cruiserRoot.transform,
                    new Vector3(0f, 0.65f, 0.2f),
                    new Vector3(0.85f, 0.45f, 0.85f),
                    baseMat,
                    new Color(0.12f, 0.12f, 0.15f));
                _partVisuals[ModularVehicleSlot.TopHatMagneticRoofTurret] = turret;
            }
            catch
            {
            }
        }

        private static Material FindExistingHdrpMaterial()
        {
            try
            {
                var renderers = FindObjectsOfType<MeshRenderer>();
                foreach (var r in renderers)
                {
                    if (r != null && r.sharedMaterial != null)
                    {
                        return new Material(r.sharedMaterial);
                    }
                }
            }
            catch
            {
            }
            return null;
        }

        private static GameObject CreatePartPrimitive(
            PrimitiveType type,
            string name,
            Transform parent,
            Vector3 localPos,
            Vector3 localScale,
            Material templateMat,
            Color tint)
        {
            GameObject go = GameObject.CreatePrimitive(type);
            go.name = name;
            go.transform.SetParent(parent, false);
            go.transform.localPosition = localPos;
            go.transform.localScale = localScale;

            var col = go.GetComponent<Collider>();
            if (col != null) Destroy(col);

            var mr = go.GetComponent<MeshRenderer>();
            if (mr != null)
            {
                if (templateMat != null)
                {
                    Material mat = new Material(templateMat);
                    if (mat.HasProperty("_BaseColor")) mat.SetColor("_BaseColor", tint);
                    if (mat.HasProperty("_Color")) mat.SetColor("_Color", tint);
                    if (mat.HasProperty("_EmissiveColor")) mat.SetColor("_EmissiveColor", tint * 2.5f);
                    mr.sharedMaterial = mat;
                }
            }
            return go;
        }

        private void UpdateCruiserVisualTransform(CameraController cam, float beatOscillation)
        {
            if (_cruiserRoot == null) return;

            Vector3 pivot = cam.pivot;
            float yawDeg = cam.angle.x;
            _cruiserRoot.transform.position = pivot + new Vector3(0f, 2.2f + beatOscillation * 0.5f, 0f);
            _cruiserRoot.transform.rotation = Quaternion.Euler(0f, yawDeg + 180f, 0f);
        }
    }
}
