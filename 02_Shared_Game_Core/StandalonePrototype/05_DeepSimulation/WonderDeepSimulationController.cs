using System;
using UnityEngine;
using WonderWorld.Bridge;

namespace WonderWorld.DeepSimulation
{
    [Serializable]
    public struct StandaloneVipCitizen
    {
        public string citizenId;
        public string fullName;
        public string roleTitle;
        public string assignedFacility;
        public string commuteStatus;
        public float productivityBonusPercent;
    }

    /// <summary>
    /// Standalone 3D Prototype Controller for the 5 Non-Intrusive Deep Simulation Pillars:
    ///   [Key 1] Hover-Cruiser Cargo Airlift (4-Tier Supply Chain & Storefront Inventory)
    ///   [Key 2] 59,000-Gallon Botanical Bio-Dome Floodgate Purification (Timberborn Hydro-Engineering)
    ///   [Key 3] Mayor Hope Decree: CNA Dignity Shift & Mama Leg-Elevation Rest (Frostpunk Morale)
    ///   [Key 4] Awaken Crowned Specialists & 85 BPM Utility Grid Relay Sync (Workers & Resources)
    ///   [Key 5] Named VIP Citizen Hyper-Lane Commute Boost (RimWorld Persistent Citizens)
    /// </summary>
    public class WonderDeepSimulationController : MonoBehaviour
    {
        public static WonderDeepSimulationController Instance { get; private set; }

        [Header("Pillar 1: 4-Tier 85 BPM Supply Chain")]
        public int rawAcousticAlloyTons = 420;
        public int bioLuminescentSporeCrates = 310;
        public int forgedSubBassCoils = 185;
        public int storefrontInventoryCrates = 640;

        [Header("Pillar 2: 59,000-Gallon Hydro-Engineering & Bio-Dome")]
        public float canalRunoffGallons = 14250.0f;
        public float purifiedBioCoolantGallons = 44750.0f;
        public bool bioDomeFloodgatesOpen = true;

        [Header("Pillar 3: Frostpunk Hope & Mayor Decrees")]
        public float cityHopeIndex = 92.5f;
        public string activeMayorDecree = "CNA Dignity Shift & Mama Leg-Elevation Rest";

        [Header("Pillar 4: Specialist Workforce & 85 BPM Utility Grid")]
        public int crownedSpecialistsActive = 64;
        public float grid85BpmSyncPercent = 100.0f;

        [Header("Pillar 5: Persistent Named VIP Citizens")]
        public StandaloneVipCitizen[] vipRoster = new StandaloneVipCitizen[]
        {
            new StandaloneVipCitizen { citizenId = "VIP_01", fullName = "Darrell X-Gen", roleTitle = "Founder & Sonic Gauntlet Architect", assignedFacility = "Papa AI Financial Spire", commuteStatus = "Hover-Cruiser [V] Express", productivityBonusPercent = 250f },
            new StandaloneVipCitizen { citizenId = "VIP_02", fullName = "Nurse Ebony Vance", roleTitle = "Chief CNA Dignity Director", assignedFacility = "Mama Botanical Bio-Dome East", commuteStatus = "85 BPM Skybridge Monorail", productivityBonusPercent = 185f },
            new StandaloneVipCitizen { citizenId = "VIP_03", fullName = "Marcus 'Sub-Bass' Cole", roleTitle = "Chief 85 BPM Grid Engineer", assignedFacility = "Sub-Basement Hydro Relay", commuteStatus = "Pragmata Transit Pod", productivityBonusPercent = 175f },
            new StandaloneVipCitizen { citizenId = "VIP_04", fullName = "Elena 'Iron-Weld' Sato", roleTitle = "Master Repulsor Forge Lead", assignedFacility = "Acoustic Alloy Refinery", commuteStatus = "Heavy Cargo Convoy Escort", productivityBonusPercent = 165f },
            new StandaloneVipCitizen { citizenId = "VIP_05", fullName = "Dr. Kaelen Moss", roleTitle = "Botanical Hydro-Purification Lead", assignedFacility = "59,000-Gal Bio-Dome Control", commuteStatus = "Canal Hydro-Skiff", productivityBonusPercent = 190f }
        };

        void Awake()
        {
            if (Instance == null) Instance = this;
        }

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.Alpha1))
            {
                ExecuteHoverCargoAirlift();
            }
            else if (Input.GetKeyDown(KeyCode.Alpha2))
            {
                PurifyBioDomeReservoir();
            }
            else if (Input.GetKeyDown(KeyCode.Alpha3))
            {
                EnactMayorHopeDecree();
            }
            else if (Input.GetKeyDown(KeyCode.Alpha4))
            {
                AwakenSpecialistsAndSyncGrid();
            }
            else if (Input.GetKeyDown(KeyCode.Alpha5))
            {
                BoostNamedVipCommutes();
            }
        }

        public void ExecuteHoverCargoAirlift()
        {
            rawAcousticAlloyTons += 120;
            bioLuminescentSporeCrates += 95;
            forgedSubBassCoils += 60;
            storefrontInventoryCrates += 250;

            WonderWorldLedgerBridge.Instance?.SyncDeepSimulationPillars(
                addedCrates: 250,
                bioCoolantGallons: purifiedBioCoolantGallons,
                newHopeIndex: cityHopeIndex,
                addedSpecialists: 0,
                bonusTreasury: 45000,
                bonusWonderStars: 75);
        }

        public void PurifyBioDomeReservoir()
        {
            bioDomeFloodgatesOpen = true;
            canalRunoffGallons = 0.0f;
            purifiedBioCoolantGallons = 59000.0f;

            WonderWorldLedgerBridge.Instance?.SyncDeepSimulationPillars(
                addedCrates: 0,
                bioCoolantGallons: purifiedBioCoolantGallons,
                newHopeIndex: cityHopeIndex,
                addedSpecialists: 0,
                bonusTreasury: 35000,
                bonusWonderStars: 60);
        }

        public void EnactMayorHopeDecree()
        {
            cityHopeIndex = 100.0f;
            activeMayorDecree = "Mama Leg-Elevation Rest & CNA Dignity Double-Bonus";

            WonderWorldLedgerBridge.Instance?.SyncDeepSimulationPillars(
                addedCrates: 0,
                bioCoolantGallons: purifiedBioCoolantGallons,
                newHopeIndex: cityHopeIndex,
                addedSpecialists: 0,
                bonusTreasury: 30000,
                bonusWonderStars: 85);
        }

        public void AwakenSpecialistsAndSyncGrid()
        {
            crownedSpecialistsActive += 8;
            grid85BpmSyncPercent = 100.0f;

            WonderWorldLedgerBridge.Instance?.SyncDeepSimulationPillars(
                addedCrates: 0,
                bioCoolantGallons: purifiedBioCoolantGallons,
                newHopeIndex: cityHopeIndex,
                addedSpecialists: 8,
                bonusTreasury: 40000,
                bonusWonderStars: 90);
        }

        public void BoostNamedVipCommutes()
        {
            for (int i = 0; i < vipRoster.Length; i++)
            {
                vipRoster[i].commuteStatus = "85 BPM Hyper-Lane Express (0m Delay)";
                vipRoster[i].productivityBonusPercent += 15.0f;
            }

            WonderWorldLedgerBridge.Instance?.SyncDeepSimulationPillars(
                addedCrates: 0,
                bioCoolantGallons: purifiedBioCoolantGallons,
                newHopeIndex: cityHopeIndex,
                addedSpecialists: 0,
                bonusTreasury: 50000,
                bonusWonderStars: 100);
        }
    }
}
