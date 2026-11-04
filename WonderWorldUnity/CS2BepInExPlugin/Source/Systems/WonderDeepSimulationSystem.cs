using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;
using Game;
using Game.Simulation;
using Unity.Entities;
using UnityEngine;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;

namespace WonderWorldCS2.Systems
{
    /// <summary>
    /// Persistent Named Crowned Citizen / VIP Record (RimWorld / Dwarf Fortress Pillar 5).
    /// Tracked across Cities: Skylines II and the Standalone 3D Prototype via WonderLedgerState.json.
    /// </summary>
    [Serializable]
    public struct CrownedVipCitizenRecord
    {
        public string CitizenId;
        public string FullName;
        public string RoleTitle;
        public string HomeDistrict;
        public string AssignedFacility;
        public string CommuteStatus;
        public float ProductivityBonusPercent;
        public int PersonalSavingsUsd;
    }

    /// <summary>
    /// Non-Intrusive 5-Pillar Deep Simulation ECS Layer for Mama's Wonder World // Purple City Universe:
    ///   1. Interdependent 85 BPM 4-Tier Supply Chains & Cargo Logistics (Anno 1800 / Factorio)
    ///   2. 59,000-Gallon Hydro-Engineering & Botanical Bio-Dome Floodgates (Timberborn)
    ///   3. District Hope vs. Tension Morale & Mayor Emergency Decrees (Frostpunk)
    ///   4. Physical Utility Grids & Crowned Specialist Workforces (Workers & Resources)
    ///   5. Persistent Named Citizen & VIP Routines (RimWorld / Dwarf Fortress)
    /// </summary>
    public partial class WonderDeepSimulationSystem : GameSystemBase
    {
        public static WonderDeepSimulationSystem Instance { get; private set; }

        // ====================================================================
        // PILLAR 1: Interdependent 85 BPM Supply Chains & Cargo Logistics
        // ====================================================================
        public static int RawAcousticAlloyTons { get; private set; } = 420;
        public static int BioLuminescentSporeCrates { get; private set; } = 310;
        public static int ForgedSubBassCoils { get; private set; } = 185;
        public static int StorefrontInventoryCrates { get; private set; } = 640;
        public static int ActiveCargoDronesAndTrucks { get; private set; } = 28;

        // ====================================================================
        // PILLAR 2: 59,000-Gallon Hydro-Engineering & Bio-Dome Floodgates
        // ====================================================================
        public const float MaxBioDomeReservoirCapacityGallons = 59000.0f;
        public static float CanalRunoffGallons { get; private set; } = 14250.0f;
        public static float PurifiedBioCoolantGallons { get; private set; } = 44750.0f;
        public static bool BioDomeFloodgatesOpen { get; private set; } = true;

        // ====================================================================
        // PILLAR 3: Frostpunk Hope, Discontent & Crisis Decrees
        // ====================================================================
        public static float CityHopeIndex { get; private set; } = 92.5f;
        public static string ActiveMayorDecree { get; private set; } = "CNA Dignity Shift & Mama Leg-Elevation Rest";
        public static string MoraleCrisisState { get; private set; } = "HARMONIC PEACE (0 Strikes)";

        // ====================================================================
        // PILLAR 4: Physical Utility Grids & Specialist Workforces
        // ====================================================================
        public static int CrownedSpecialistsActive { get; private set; } = 64;
        public static float Grid85BpmSyncPercent { get; private set; } = 100.0f;
        public static int MegawattsGeneratedByBioCoolant { get; private set; } = 850;

        // ====================================================================
        // PILLAR 5: Persistent Named Citizen & VIP Roster
        // ====================================================================
        public static readonly CrownedVipCitizenRecord[] StaticVipRoster = new CrownedVipCitizenRecord[]
        {
            new CrownedVipCitizenRecord
            {
                CitizenId = "VIP_01",
                FullName = "Darrell X-Gen",
                RoleTitle = "Founder & Sonic Gauntlet Architect",
                HomeDistrict = "Twin Wonder Tower Penthouse",
                AssignedFacility = "Papa AI Financial Spire & 85 BPM Core",
                CommuteStatus = "Hover-Cruiser [V] Express (0m Delay)",
                ProductivityBonusPercent = 250.0f,
                PersonalSavingsUsd = 1250000
            },
            new CrownedVipCitizenRecord
            {
                CitizenId = "VIP_02",
                FullName = "Nurse Ebony Vance",
                RoleTitle = "Chief CNA Dignity Director",
                HomeDistrict = "Mama Botanical Bio-Dome East",
                AssignedFacility = "Wonder Rehabilitation & Care Wing",
                CommuteStatus = "85 BPM Skybridge Monorail (On-Time)",
                ProductivityBonusPercent = 185.0f,
                PersonalSavingsUsd = 420000
            },
            new CrownedVipCitizenRecord
            {
                CitizenId = "VIP_03",
                FullName = "Marcus 'Sub-Bass' Cole",
                RoleTitle = "Chief 85 BPM Harmonic Grid Engineer",
                HomeDistrict = "Neon Drome District",
                AssignedFacility = "Sub-Basement Hydro & Acoustic Relay",
                CommuteStatus = "Pragmata Transit Pod (On-Time)",
                ProductivityBonusPercent = 175.0f,
                PersonalSavingsUsd = 380000
            },
            new CrownedVipCitizenRecord
            {
                CitizenId = "VIP_04",
                FullName = "Elena 'Iron-Weld' Sato",
                RoleTitle = "Master Smelter & Repulsor Forge Lead",
                HomeDistrict = "Foundry Sector 4",
                AssignedFacility = "Acoustic Alloy & Coil Refinery",
                CommuteStatus = "Heavy Cargo Convoy Escort (On-Time)",
                ProductivityBonusPercent = 165.0f,
                PersonalSavingsUsd = 345000
            },
            new CrownedVipCitizenRecord
            {
                CitizenId = "VIP_05",
                FullName = "Dr. Kaelen Moss",
                RoleTitle = "Botanical Hydro-Purification Lead",
                HomeDistrict = "Emerald Canal Terrace",
                AssignedFacility = "59,000-Gal Bio-Dome Floodgate Control",
                CommuteStatus = "Canal Hydro-Skiff (On-Time)",
                ProductivityBonusPercent = 190.0f,
                PersonalSavingsUsd = 410000
            }
        };

        private float _simPulseTimer = 0f;

        protected override void OnCreate()
        {
            base.OnCreate();
            Instance = this;
            WonderWorldCS2Plugin.ModLogger?.LogInfo("[WonderDeepSimulationSystem] 5-Pillar Deep Simulation ECS System Initialized.");
        }

        public static WonderDeepSimulationSystem EnsureRegisteredInWorld()
        {
            if (Instance != null) return Instance;
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world != null && world.IsCreated)
                {
                    Instance = world.GetOrCreateSystemManaged<WonderDeepSimulationSystem>();
                    var updateSystem = world.GetExistingSystemManaged<UpdateSystem>();
                    updateSystem?.UpdateAt<WonderDeepSimulationSystem>(SystemUpdatePhase.GameSimulation);
                }
            }
            catch
            {
            }
            return Instance;
        }

        protected override void OnUpdate()
        {
            _simPulseTimer += UnityEngine.Time.unscaledDeltaTime;
            if (_simPulseTimer >= 6.0f)
            {
                _simPulseTimer = 0f;
                TickDeepSimulationCycle();
            }
        }

        public static void TickDeepSimulationCycle()
        {
            // 1. Supply Chain 4-Tier Manufacturing Loop (Raw -> Coils -> Storefront Stock -> Revenue)
            RawAcousticAlloyTons += 18;
            BioLuminescentSporeCrates += 14;
            int newlyForged = Mathf.Min(RawAcousticAlloyTons / 12, BioLuminescentSporeCrates / 10);
            if (newlyForged > 0)
            {
                RawAcousticAlloyTons -= newlyForged * 4;
                BioLuminescentSporeCrates -= newlyForged * 3;
                ForgedSubBassCoils += newlyForged * 6;
                StorefrontInventoryCrates += newlyForged * 15;
            }

            // Storefronts consume inventory crates to generate commercial supply-chain bonus
            int soldCrates = Mathf.Min(StorefrontInventoryCrates, WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses * 3);
            StorefrontInventoryCrates = Mathf.Max(50, StorefrontInventoryCrates - soldCrates + 24);
            MamaTowerCities2System.Col1GrindBalance += soldCrates * 22.5f;

            // 2. Hydro-Engineering Loop: Bio-Dome Floodgates purify canal runoff into bio-coolant
            if (BioDomeFloodgatesOpen)
            {
                float purified = Mathf.Min(CanalRunoffGallons, 2500.0f);
                CanalRunoffGallons = Mathf.Max(1200.0f, CanalRunoffGallons - purified + 950.0f);
                PurifiedBioCoolantGallons = Mathf.Min(MaxBioDomeReservoirCapacityGallons, PurifiedBioCoolantGallons + purified);
                MamaTowerCities2System.Col2OverheadDebt = 0.00f;
            }
            else
            {
                CanalRunoffGallons = Mathf.Min(MaxBioDomeReservoirCapacityGallons, CanalRunoffGallons + 1800.0f);
            }

            // 3. Hope vs. Tension Crisis Evaluation (Frostpunk style)
            float tension = WonderUniverseIntegrationSystem.StaticDistrictTensionIndex;
            if (tension > 80.0f)
            {
                MoraleCrisisState = "SYNDICATE LOCKDOWN (Host Tokon Showdown!)";
                CityHopeIndex = Mathf.Max(25.0f, CityHopeIndex - 2.0f);
            }
            else if (tension > 50.0f)
            {
                MoraleCrisisState = "STREET STRIKE WARNING (Enact Decree!)";
                CityHopeIndex = Mathf.Max(45.0f, CityHopeIndex - 1.0f);
            }
            else
            {
                MoraleCrisisState = "HARMONIC PEACE (100% Workforce Output)";
                CityHopeIndex = Mathf.Min(100.0f, CityHopeIndex + 0.8f);
            }

            // 4. Specialist Workforce & 85 BPM Grid Sync
            Grid85BpmSyncPercent = Mathf.Clamp(85.0f + (CrownedSpecialistsActive * 0.25f), 85.0f, 100.0f);
            MegawattsGeneratedByBioCoolant = Mathf.RoundToInt((PurifiedBioCoolantGallons / 59000.0f) * 850.0f * (Grid85BpmSyncPercent / 100.0f));

            // 5. Named VIP Citizen Wealth Accumulation
            for (int i = 0; i < StaticVipRoster.Length; i++)
            {
                StaticVipRoster[i].PersonalSavingsUsd += 450;
            }
        }

        /// <summary>
        /// Pillar 1 Action: Hover-Cruiser Emergency Cargo Airlift.
        /// Restocks all Tier-4 Retail Storefronts and deposits +$45,000 & +75 ⭐️W.
        /// </summary>
        public static void ExecuteHoverCargoAirliftStatic()
        {
            RawAcousticAlloyTons += 120;
            BioLuminescentSporeCrates += 95;
            ForgedSubBassCoils += 60;
            StorefrontInventoryCrates += 250;
            ActiveCargoDronesAndTrucks += 4;

            MamaCityControlSystem.InjectCityMoneyStatic(45000);
            MamaTowerCities2System.Col1GrindBalance += 4500.0f;
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 75.0f;
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses + 1,
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + 75);

            WonderUniverseIntegrationSystem.PlayHarmonicAudioSting(
                "85 BPM Cargo Airlift Drop",
                new float[] { 293.66f, 369.99f, 440.0f, 587.33f },
                0.45f);

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: 📦 Hover-Cargo Airlift delivered! +250 Storefront Crates ({StorefrontInventoryCrates} stocked), +$45,000 Treasury & +75 ⭐️W!";
            MamaTowerCities2System.SaveToSharedJson();
        }

        /// <summary>
        /// Pillar 2 Action: Toggle 59,000-Gallon Botanical Bio-Dome Floodgates & Purify Sludge.
        /// Converts Canal Runoff into Purified Bio-Coolant, zeroes Col 2 Debt, and banks +$35,000 & +60 ⭐️W.
        /// </summary>
        public static void ToggleBioDomeFloodgatesAndPurifyStatic()
        {
            BioDomeFloodgatesOpen = true;
            PurifiedBioCoolantGallons = MaxBioDomeReservoirCapacityGallons;
            CanalRunoffGallons = 0.0f;
            MegawattsGeneratedByBioCoolant = 1000;

            MamaTowerCities2System.Col2OverheadDebt = 0.00f;
            MamaTowerCities2System.WetVacGallonsRemaining = 0.0f;
            MamaCityControlSystem.InjectCityMoneyStatic(35000);
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 60.0f;
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + 60);

            WonderUniverseIntegrationSystem.PlayHarmonicAudioSting(
                "59,000-Gal Bio-Dome Hydro Flush",
                new float[] { 220.0f, 329.63f, 440.0f, 659.25f },
                0.50f);

            MamaTowerCities2System.LastVoiceBroadcast =
                "Mama AI: 🌊 59,000-Gal Bio-Dome Floodgates PURIFIED! Canal Runoff -> 0 gal, 1,000 MW Bio-Coolant Online, Col 2 Debt $0.00 & +60 ⭐️W!";
            MamaTowerCities2System.SaveToSharedJson();
        }

        /// <summary>
        /// Pillar 3 Action: Enact Mayor Hope Decree (CNA Dignity Shift & Mama Leg-Elevation Rest).
        /// Boosts City Hope to 100%, cools District Tension, and grants +$30,000 & +85 ⭐️W.
        /// </summary>
        public static void EnactHopeDecreeStatic()
        {
            CityHopeIndex = 100.0f;
            MoraleCrisisState = "GOLDEN AGE DECREE ACTIVE (0% Unrest)";
            ActiveMayorDecree = "Mama Leg-Elevation Rest & CNA Dignity Double-Bonus";
            MamaTowerCities2System.CnaPatientDignity = 100.0f;

            float cooledTension = Mathf.Max(0.0f, WonderUniverseIntegrationSystem.StaticDistrictTensionIndex - 15.0f);
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                cooledTension,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + 85);

            MamaCityControlSystem.InjectCityMoneyStatic(30000);
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 85.0f;

            WonderUniverseIntegrationSystem.PlayHarmonicAudioSting(
                "Mayor Hope Decree Fanfare",
                new float[] { 261.63f, 329.63f, 392.00f, 523.25f },
                0.50f);

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: 📜 Mayor Decree Enacted ('{ActiveMayorDecree}')! City Hope 100%, Tension cooled to {cooledTension:F1}%, +85 ⭐️W!";
            MamaTowerCities2System.SaveToSharedJson();
        }

        /// <summary>
        /// Pillar 4 Action: Awaken Crowned Specialists via Street Hat Cannon & Synchronize 85 BPM Utility Grid.
        /// </summary>
        public static void DispatchSpecialistAndSyncGridStatic()
        {
            CrownedSpecialistsActive += 8;
            Grid85BpmSyncPercent = 100.0f;
            MegawattsGeneratedByBioCoolant = 1050;

            MamaCityControlSystem.InjectCityMoneyStatic(40000);
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 90.0f;
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + 90);

            WonderUniverseIntegrationSystem.PlayHarmonicAudioSting(
                "85 BPM Utility Grid Overcharge",
                new float[] { 196.00f, 293.66f, 392.00f, 587.33f },
                0.48f);

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: 🎩 +8 Crowned Specialists Awakened ({CrownedSpecialistsActive} Active)! 85 BPM Utility Grid 100% Synced, +$40,000 & +90 ⭐️W!";
            MamaTowerCities2System.SaveToSharedJson();
        }

        /// <summary>
        /// Pillar 5 Action: Optimize Named VIP Citizen Express Commutes across all districts.
        /// </summary>
        public static void BoostVipCitizenCommutesStatic()
        {
            for (int i = 0; i < StaticVipRoster.Length; i++)
            {
                StaticVipRoster[i].CommuteStatus = "85 BPM Hyper-Lane Express (0m Delay)";
                StaticVipRoster[i].ProductivityBonusPercent += 15.0f;
                StaticVipRoster[i].PersonalSavingsUsd += 25000;
            }

            MamaCityControlSystem.InjectCityMoneyStatic(50000);
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 100.0f;
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + 100);

            WonderUniverseIntegrationSystem.PlayHarmonicAudioSting(
                "VIP Express Transit Chime",
                new float[] { 349.23f, 440.0f, 523.25f, 698.46f },
                0.45f);

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: 🚆 All {StaticVipRoster.Length} Named Crowned VIPs boosted on 85 BPM Hyper-Lane! +15% Productivity, +$50,000 & +100 ⭐️W!";
            MamaTowerCities2System.SaveToSharedJson();
        }

        public static void ApplyDeepSimulationJsonSync(
            int storefrontCrates,
            float bioCoolantGallons,
            float hopeIndex,
            int specialistsActive)
        {
            if (storefrontCrates > 0) StorefrontInventoryCrates = storefrontCrates;
            if (bioCoolantGallons >= 0f) PurifiedBioCoolantGallons = Mathf.Min(MaxBioDomeReservoirCapacityGallons, bioCoolantGallons);
            if (hopeIndex > 0f) CityHopeIndex = Mathf.Clamp(hopeIndex, 0f, 100f);
            if (specialistsActive > 0) CrownedSpecialistsActive = specialistsActive;
        }

        public static string SerializeVipRosterJsonArray()
        {
            var sb = new StringBuilder(1024);
            sb.Append("[");
            for (int i = 0; i < StaticVipRoster.Length; i++)
            {
                if (i > 0) sb.Append(", ");
                var v = StaticVipRoster[i];
                sb.AppendFormat(
                    CultureInfo.InvariantCulture,
                    "{{\"id\":\"{0}\",\"name\":\"{1}\",\"role\":\"{2}\",\"district\":\"{3}\",\"facility\":\"{4}\",\"commute\":\"{5}\",\"productivity_pct\":{6:F1},\"savings_usd\":{7}}}",
                    v.CitizenId,
                    v.FullName.Replace("\"", "\\\""),
                    v.RoleTitle.Replace("\"", "\\\""),
                    v.HomeDistrict.Replace("\"", "\\\""),
                    v.AssignedFacility.Replace("\"", "\\\""),
                    v.CommuteStatus.Replace("\"", "\\\""),
                    v.ProductivityBonusPercent,
                    v.PersonalSavingsUsd);
            }
            sb.Append("]");
            return sb.ToString();
        }
    }
}
