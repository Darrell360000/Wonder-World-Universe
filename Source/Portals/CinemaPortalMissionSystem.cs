using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
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
            if (StaticMissions == null || StaticInMissionPortal)
            {
                return false;
            }

            for (int i = 0; i < StaticMissions.Length; i++)
            {
                if (StaticMissions[i].MissionId == missionId && !StaticMissions[i].IsCompleted)
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
        }

        // Called when the story flashback objective is completed
        public void CompleteActiveMission() => CompleteActiveMissionStatic();

        public static void CompleteActiveMissionStatic()
        {
            if (!StaticInMissionPortal || StaticMissions == null) return;

            // Mark the mission complete before granting anything so repeated calls cannot pay twice.
            int activeMissionIndex = -1;
            for (int i = 0; i < StaticMissions.Length; i++)
            {
                if (StaticMissions[i].MissionId == StaticActiveMission.MissionId)
                {
                    activeMissionIndex = i;
                    break;
                }
            }
            if (activeMissionIndex < 0 || StaticMissions[activeMissionIndex].IsCompleted)
            {
                StaticInMissionPortal = false;
                return;
            }

            StaticMissions[activeMissionIndex].IsCompleted = true;
            StaticActiveMission = StaticMissions[activeMissionIndex];
            StaticInMissionPortal = false;

            // 2. Inject City Treasury & Wonder Stars
            MamaCityControlSystem.InjectCityMoneyStatic(StaticActiveMission.TreasuryBonus);
            WonderCurrencyConverter.WonderSuperCurrencyBalance += StaticActiveMission.WonderStarReward;
            WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(
                WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                WonderUniverseIntegrationSystem.StaticWonderStarsBanked + StaticActiveMission.WonderStarReward);

            // 3. Play fanfare and exit portal
            WonderUniverseIntegrationSystem.PremiereBlockbusterMovieStatic();

            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: ⭐️ MISSION COMPLETE: \"{StaticActiveMission.MovieTitle}\"! +{StaticActiveMission.WonderStarReward} ⭐️W & +${StaticActiveMission.TreasuryBonus:N0} awarded!";
            MamaTowerCities2System.SaveToSharedJson();

            Debug.Log($"[CinemaPortals] ⭐️ MISSION COMPLETE: {StaticActiveMission.MovieTitle}! +{StaticActiveMission.WonderStarReward} ⭐️W and +${StaticActiveMission.TreasuryBonus:N0} awarded!");
        }

        public static string SerializeCompletedMissionIds()
        {
            var completedIds = new List<string>();
            if (StaticMissions != null)
            {
                foreach (CinemaMissionData mission in StaticMissions)
                {
                    if (mission.IsCompleted && !string.IsNullOrEmpty(mission.MissionId))
                    {
                        completedIds.Add($"\"{mission.MissionId.Replace("\\", "\\\\").Replace("\"", "\\\"")}\"");
                    }
                }
            }

            return $"[{string.Join(", ", completedIds)}]";
        }

        public static void RestoreCompletedMissions(string ledgerJson)
        {
            if (string.IsNullOrEmpty(ledgerJson) || StaticMissions == null)
            {
                return;
            }

            try
            {
                Match idsMatch = Regex.Match(
                    ledgerJson,
                    "\"completed_cinema_mission_ids\"\\s*:\\s*\\[(?<ids>[^\\]]*)\\]");
                if (!idsMatch.Success)
                {
                    return;
                }

                var completedIds = new HashSet<string>();
                foreach (Match idMatch in Regex.Matches(idsMatch.Groups["ids"].Value, "\"(?<id>[^\"\\\\]*)\""))
                {
                    completedIds.Add(idMatch.Groups["id"].Value);
                }

                for (int i = 0; i < StaticMissions.Length; i++)
                {
                    StaticMissions[i].IsCompleted = completedIds.Contains(StaticMissions[i].MissionId);
                }
            }
            catch (Exception ex)
            {
                WonderWorldCS2Plugin.ModLogger?.LogWarning(
                    $"[CinemaPortals] Failed restoring completed missions from shared ledger: {ex.Message}");
            }
        }
    }
}
