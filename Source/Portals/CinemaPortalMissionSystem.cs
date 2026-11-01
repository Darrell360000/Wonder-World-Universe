using System;
using System.IO;
using UnityEngine;
using Unity.Entities;
using Game;
using Game.Simulation;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;
using WonderWorldCS2.Systems;

namespace WonderWorldCS2.Portals
{
    [System.Serializable]
    public struct CinemaMissionData
    {
        public string MissionId;
        public string HeroName;
        public string MovieTitle;
        public string District;
        public int WonderStarReward;
        public int TreasuryBonus;
        public bool IsCompleted;
    }

    public partial class CinemaPortalMissionSystem : GameSystemBase
    {
        public static CinemaPortalMissionSystem Instance { get; private set; }

        public static CinemaMissionData[] StaticMissions = new CinemaMissionData[]
        {
            new CinemaMissionData
            {
                MissionId = "PORTAL_01",
                HeroName = "Darrell X-Gen",
                MovieTitle = "X-Gen & The Cybernetic God Hand",
                District = "Financial Core",
                WonderStarReward = 500,
                TreasuryBonus = 250000,
                IsCompleted = false
            },
            new CinemaMissionData
            {
                MissionId = "PORTAL_02",
                HeroName = "The Bio-Weaver",
                MovieTitle = "Spore Symphony of the Dome",
                District = "Botanical Commons",
                WonderStarReward = 350,
                TreasuryBonus = 150000,
                IsCompleted = false
            },
            new CinemaMissionData
            {
                MissionId = "PORTAL_03",
                HeroName = "The Forge Maestro",
                MovieTitle = "Overhead Zero: Industrial Rhythms",
                District = "Manufacturing Belt",
                WonderStarReward = 400,
                TreasuryBonus = 200000,
                IsCompleted = false
            },
            new CinemaMissionData
            {
                MissionId = "PORTAL_04",
                HeroName = "The Skyway Navigator",
                MovieTitle = "Cargo Run at 18.5m",
                District = "Harbor & Logistics Bay",
                WonderStarReward = 450,
                TreasuryBonus = 225000,
                IsCompleted = false
            },
            new CinemaMissionData
            {
                MissionId = "PORTAL_05",
                HeroName = "Mama AI & Papa AI",
                MovieTitle = "Harmonic Convergence (Skybridge Apex)",
                District = "Skybridge 150th Floor",
                WonderStarReward = 1000,
                TreasuryBonus = 500000,
                IsCompleted = false
            }
        };

        public CinemaMissionData[] Missions
        {
            get => StaticMissions;
            set => StaticMissions = value;
        }

        public static CinemaMissionData StaticActiveMission { get; private set; } = StaticMissions[0];
        public static bool StaticInMissionPortal { get; private set; } = false;

        public CinemaMissionData ActiveMission
        {
            get => StaticActiveMission;
            private set => StaticActiveMission = value;
        }

        public bool InMissionPortal
        {
            get => StaticInMissionPortal;
            private set => StaticInMissionPortal = value;
        }

        protected override void OnCreate()
        {
            base.OnCreate();
            Instance = this;
            Debug.Log("[CinemaPortals] Narrative RPG Mission System initialized!");
        }

        public static CinemaPortalMissionSystem EnsureRegisteredInWorld()
        {
            if (Instance != null) return Instance;
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world != null && world.IsCreated)
                {
                    Instance = world.GetOrCreateSystemManaged<CinemaPortalMissionSystem>();
                    var updateSystem = world.GetExistingSystemManaged<UpdateSystem>();
                    updateSystem?.UpdateAt<CinemaPortalMissionSystem>(SystemUpdatePhase.GameSimulation);
                }
            }
            catch
            {
            }
            return Instance;
        }

        protected override void OnUpdate() { }

        // Called when the 3D player character presses [E] at a Cinema building
        public bool EnterCinemaPortal(string missionId) => EnterCinemaPortalStatic(missionId);

        public static bool EnterCinemaPortalStatic(string missionId)
        {
            for (int i = 0; i < StaticMissions.Length; i++)
            {
                if (StaticMissions[i].MissionId == missionId)
                {
                    StaticActiveMission = StaticMissions[i];
                    StaticInMissionPortal = true;
                    WonderUniverseIntegrationSystem.PlayDromeCinemaAudioSting();
                    MamaTowerCities2System.LastVoiceBroadcast =
                        $"Mama AI: 🎬 ENTERING REEL MEMORY PORTAL [{StaticActiveMission.MissionId}]: \"{StaticActiveMission.MovieTitle}\" ({StaticActiveMission.HeroName})!";
                    Debug.Log($"[CinemaPortals] 🎬 ENTERING PORTAL: {StaticActiveMission.MovieTitle} ({StaticActiveMission.HeroName})");
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Enters the next uncompleted mission portal (or completes the currently active mission if already inside a portal).
        /// </summary>
        public static void TriggerNextOrCompletePortalFlashbackStatic()
        {
            if (StaticInMissionPortal)
            {
                CompleteActiveMissionStatic();
                return;
            }

            for (int i = 0; i < StaticMissions.Length; i++)
            {
                if (!StaticMissions[i].IsCompleted)
                {
                    EnterCinemaPortalStatic(StaticMissions[i].MissionId);
                    return;
                }
            }

            // If all 5 completed, replay Portal 01
            EnterCinemaPortalStatic(StaticMissions[0].MissionId);
        }

        // Called when the story flashback objective is completed
        public void CompleteActiveMission() => CompleteActiveMissionStatic();

        public static void CompleteActiveMissionStatic()
        {
            if (!StaticInMissionPortal) return;

            // 1. Mark complete
            for (int i = 0; i < StaticMissions.Length; i++)
            {
                if (StaticMissions[i].MissionId == StaticActiveMission.MissionId)
                {
                    StaticMissions[i].IsCompleted = true;
                    StaticActiveMission = StaticMissions[i];
                    break;
                }
            }

            // 2. Inject City Treasury & Wonder Stars
            MamaCityControlSystem.InjectCityMoneyStatic(StaticActiveMission.TreasuryBonus);
            WonderCurrencyConverter.WonderSuperCurrencyBalance += StaticActiveMission.WonderStarReward;
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + StaticActiveMission.WonderStarReward);

            // 3. Play fanfare and exit portal
            WonderUniverseIntegrationSystem.PremiereBlockbusterMovieStatic();
            StaticInMissionPortal = false;

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: ⭐️ MISSION COMPLETE: \"{StaticActiveMission.MovieTitle}\"! +{StaticActiveMission.WonderStarReward} ⭐️W & +${StaticActiveMission.TreasuryBonus:N0} awarded!";
            MamaTowerCities2System.SaveToSharedJson();

            Debug.Log($"[CinemaPortals] ⭐️ MISSION COMPLETE: {StaticActiveMission.MovieTitle}! +{StaticActiveMission.WonderStarReward} ⭐️W and +${StaticActiveMission.TreasuryBonus:N0} awarded!");
        }
    }
}
