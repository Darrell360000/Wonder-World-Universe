using System;
using System.Collections.Generic;
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
    /// Modular Street-to-Sky Hover-Drive Vehicle Controller for Purple City.
    /// Press [V] to toggle between Street Cruiser Mode and 85 BPM Anti-Gravity Hover Flight Mode.
    /// Supports runtime attachment/detachment of modular vehicle parts.
    /// </summary>
    public sealed class HoverDriveVehicle : MonoBehaviour
    {
        public static HoverDriveVehicle ActiveInstance { get; private set; }

        [Header("Hover-Drive Flight State ([V] Toggle)")]
        public bool IsHoverFlightActive = false;
        public float TargetHoverAltitudeMeters = 18.5f;
        public float CurrentAltitudeMeters = 0.0f;
        public float BaseHoverLiftForce = 28.0f;
        public float Harmonic85BpmPulseHz = 85.0f / 60.0f; // 1.4167 Hz beat clock

        [Header("Physics & Propulsion")]
        public Vector3 CurrentVelocity = Vector3.zero;
        public float ForwardSpeedMps = 42.0f;

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
                "5-Stop Rainbow LED Equalizer Stabilizer Wing",
                1.20f,
                1.35f,
                12.5f,
                true);

            _modularLoadout[ModularVehicleSlot.TopHatMagneticRoofTurret] = new ModularVehiclePart(
                ModularVehicleSlot.TopHatMagneticRoofTurret,
                "MOD_TURRET_TOPHAT_HOMING",
                "Magnetic Top-Hat Launcher Roof Hardpoint",
                1.05f,
                1.05f,
                20.0f,
                false);
        }

        private void Update()
        {
            // [V] Key toggles Hover Flight Mode
            if (Input.GetKeyDown(KeyCode.V))
            {
                ToggleHoverFlight();
            }

            SimulateHoverPhysics(Time.unscaledDeltaTime);
        }

        public bool ToggleHoverFlight()
        {
            IsHoverFlightActive = !IsHoverFlightActive;

            string modeLabel = IsHoverFlightActive
                ? $"ENGAGED (Target Altitude: {TargetHoverAltitudeMeters:F1}m | Thrust: {GetTotalThrustMultiplier():F2}x)"
                : "DISENGAGED (Grounded Street Cruiser Mode)";

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Darrell X-Gen: [V] Hover-Drive {modeLabel}!";
            WonderWorldCS2Plugin.ModLogger?.LogInfo($"[HoverDriveVehicle] [V] Hover Flight {modeLabel}");

            return IsHoverFlightActive;
        }

        public bool AttachOrToggleModularPart(ModularVehicleSlot slot)
        {
            if (!_modularLoadout.TryGetValue(slot, out ModularVehiclePart part))
            {
                return false;
            }

            part.IsAttached = !part.IsAttached;
            string state = part.IsAttached ? "ATTACHED" : "DETACHED";
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Papa AI: Modular Part [{part.DisplayName}] {state} (Thrust: {GetTotalThrustMultiplier():F2}x)!";
            return part.IsAttached;
        }

        public float GetTotalThrustMultiplier()
        {
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

        private void SimulateHoverPhysics(float dt)
        {
            if (IsHoverFlightActive)
            {
                float beatOscillation = Mathf.Sin(Time.unscaledTime * Harmonic85BpmPulseHz * Mathf.PI * 2.0f) * 0.35f;
                float desiredAltitude = (TargetHoverAltitudeMeters * GetTotalThrustMultiplier()) + beatOscillation;
                CurrentAltitudeMeters = Mathf.MoveTowards(CurrentAltitudeMeters, desiredAltitude, BaseHoverLiftForce * dt);
            }
            else
            {
                CurrentAltitudeMeters = Mathf.MoveTowards(CurrentAltitudeMeters, 0.0f, (BaseHoverLiftForce * 1.5f) * dt);
            }
        }
    }
}
