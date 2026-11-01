using System;
using System.IO;
using UnityEngine;
using Unity.Entities;
using Unity.Collections;
using Game;
using Game.City;
using Game.Simulation;
using Game.Buildings;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;

namespace WonderWorldCS2.Systems
{
    public partial class WonderUniverseIntegrationSystem : GameSystemBase
    {
        public static WonderUniverseIntegrationSystem Instance { get; private set; }

        public static int StaticActiveRetailBusinesses { get; private set; } = 12;
        public static int StaticBoxOfficeWeeklyYield { get; private set; } = 48500;
        public static float StaticDistrictTensionIndex { get; private set; } = 24.5f;
        public static bool StaticCyberGridActive { get; private set; } = true;
        public static int StaticWonderStarsBanked { get; private set; } = 1500;

        // Core Integrated Metrics across all 6 games
        public int ActiveRetailBusinesses
        {
            get => StaticActiveRetailBusinesses;
            private set => StaticActiveRetailBusinesses = value;
        } // Big Ambitions

        public int BoxOfficeWeeklyYield
        {
            get => StaticBoxOfficeWeeklyYield;
            private set => StaticBoxOfficeWeeklyYield = value;
        } // Movies Tycoon

        public float DistrictTensionIndex
        {
            get => StaticDistrictTensionIndex;
            private set => StaticDistrictTensionIndex = value;
        } // Fatal Fury / Marvel Tokon

        public bool CyberGridActive
        {
            get => StaticCyberGridActive;
            private set => StaticCyberGridActive = value;
        } // Pragmata

        public int WonderStarsBanked
        {
            get => StaticWonderStarsBanked;
            private set => StaticWonderStarsBanked = value;
        }

        private float tickTimer = 0f;
        private EntityQuery playerMoneyQuery;

        protected override void OnCreate()
        {
            base.OnCreate();
            Instance = this;
            playerMoneyQuery = GetEntityQuery(ComponentType.ReadWrite<PlayerMoney>());
            Debug.Log("[WonderUniverse] Multi-Game ECS Integration Layer Initialized!");
        }

        public static WonderUniverseIntegrationSystem EnsureRegisteredInWorld()
        {
            if (Instance != null) return Instance;
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world != null && world.IsCreated)
                {
                    Instance = world.GetOrCreateSystemManaged<WonderUniverseIntegrationSystem>();
                    var updateSystem = world.GetExistingSystemManaged<UpdateSystem>();
                    updateSystem?.UpdateAt<WonderUniverseIntegrationSystem>(SystemUpdatePhase.GameSimulation);
                }
            }
            catch
            {
            }
            return Instance;
        }

        protected override void OnUpdate()
        {
            tickTimer += UnityEngine.Time.unscaledDeltaTime;

            // Tick simulation pulse every 5 seconds (resonates with 85 BPM sub-clock)
            if (tickTimer >= 5.0f)
            {
                tickTimer = 0f;
                ProcessIntegratedGameLoops();
            }
        }

        private void ProcessIntegratedGameLoops()
        {
            try
            {
                // 1. Big Ambitions Loop: Retail storefronts deposit commercial dividends
                int retailDividend = ActiveRetailBusinesses * 1250;

                // 2. Movies Tycoon Loop: Box office sales generate cultural revenue
                int totalEarnings = retailDividend + (BoxOfficeWeeklyYield / 10);

                // 3. Inject earned profits into the City Treasury via ECS
                if (!playerMoneyQuery.IsEmptyIgnoreFilter)
                {
                    var pm = playerMoneyQuery.GetSingleton<PlayerMoney>();
                    pm.Add(totalEarnings);
                    playerMoneyQuery.SetSingleton(pm);
                    MamaCityControlSystem.LastKnownCityTreasury = pm.money;
                }
                else
                {
                    MamaCityControlSystem.InjectCityMoneyStatic(totalEarnings);
                }

                // 4. Fatal Fury / Tokon Tension Loop: Slowly cools down as services thrive
                if (DistrictTensionIndex > 10.0f)
                {
                    DistrictTensionIndex = Mathf.Max(5.0f, DistrictTensionIndex - 0.5f);
                }

                // 5. Pragmata Cyber Grid: High-tech drone bonus converts to Wonder Stars
                if (CyberGridActive)
                {
                    WonderStarsBanked += 5;
                    WonderCurrencyConverter.WonderSuperCurrencyBalance += 5f;
                }
            }
            catch (Exception ex)
            {
                Debug.LogWarning($"[WonderUniverse] Loop tick warning: {ex.Message}");
            }
        }

        // Triggered via F8 Wonder Command HUD
        public void HostDistrictShowdown() => HostDistrictShowdownStatic();

        public static void HostDistrictShowdownStatic()
        {
            // Resolves district tension with a massive tournament event
            StaticDistrictTensionIndex = 0.0f;
            StaticWonderStarsBanked += 250;
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 250f;
            MamaTowerCities2System.LastVoiceBroadcast =
                "Mama AI: 🏆 TOKON STREET SHOWDOWN hosted! District tension neutralized, +250 ⭐️W minted!";
            MamaTowerCities2System.SaveToSharedJson();
            Debug.Log("[WonderUniverse] 🏆 TOKON STREET SHOWDOWN hosted! District tension neutralized, +250 ⭐️W minted!");
        }

        public void PremiereBlockbusterMovie() => PremiereBlockbusterMovieStatic();

        public static void PremiereBlockbusterMovieStatic()
        {
            StaticBoxOfficeWeeklyYield += 25000;
            StaticWonderStarsBanked += 100;
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 100f;
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: 🎬 Top Hatter Blockbuster Premiered! Box office surge active (${StaticBoxOfficeWeeklyYield:N0}/wk, +100 ⭐️W)!";
            MamaTowerCities2System.SaveToSharedJson();
            Debug.Log("[WonderUniverse] 🎬 Top Hatter Blockbuster Premiered! Box office surge active!");
        }
    }
}
