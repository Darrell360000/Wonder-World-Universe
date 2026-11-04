using System;
using UnityEngine;
using Unity.Entities;
using Game;
using Game.City;
using Game.Simulation;
using WonderWorld.Cities2Mod;

namespace WonderWorldCS2.Systems
{
    /// <summary>
    /// Real-time Cities: Skylines II ECS Treasury & Autonomous City Governor System.
    /// Reads and modifies the live PlayerMoney component on CitySystem.City inside World.DefaultGameObjectInjectionWorld.
    /// </summary>
    public partial class MamaCityControlSystem : GameSystemBase
    {
        public static MamaCityControlSystem Instance { get; private set; }
        public static bool StaticAutonomousGovernorActive = true;
        public static int LastKnownCityTreasury = 500000;

        public bool AutonomousGovernorActive
        {
            get => StaticAutonomousGovernorActive;
            set => StaticAutonomousGovernorActive = value;
        }

        private EntityQuery _playerMoneyQuery;
        private float _updateTimer = 0f;

        protected override void OnCreate()
        {
            base.OnCreate();
            Instance = this;
            _playerMoneyQuery = GetEntityQuery(ComponentType.ReadWrite<PlayerMoney>());
            WonderWorldCS2Plugin.ModLogger?.LogInfo("[MamaCityControl] Mama's ECS Simulation Control System initialized!");
        }

        protected override void OnUpdate()
        {
            _updateTimer += UnityEngine.Time.unscaledDeltaTime;
            if (StaticAutonomousGovernorActive && _updateTimer >= 3.0f)
            {
                _updateTimer = 0f;
                RunAutonomousCityManagement();
            }
        }

        public static void EnsureRegisteredInWorld()
        {
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world == null || !world.IsCreated) return;

                if (Instance == null)
                {
                    Instance = world.GetOrCreateSystemManaged<MamaCityControlSystem>();
                    var updateSystem = world.GetExistingSystemManaged<UpdateSystem>();
                    updateSystem?.UpdateAt<MamaCityControlSystem>(SystemUpdatePhase.GameSimulation);
                }
            }
            catch
            {
                // World may still be initializing during splash/menu
            }
        }

        public static int ReadLiveCityTreasury()
        {
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world == null || !world.IsCreated) return LastKnownCityTreasury;

                EntityManager em = world.EntityManager;
                var citySystem = world.GetExistingSystemManaged<CitySystem>();
                if (citySystem != null && citySystem.City != Entity.Null && em.HasComponent<PlayerMoney>(citySystem.City))
                {
                    PlayerMoney pm = em.GetComponentData<PlayerMoney>(citySystem.City);
                    LastKnownCityTreasury = pm.money;
                    return pm.money;
                }
            }
            catch
            {
                // Ignore if no city save is currently loaded
            }
            return LastKnownCityTreasury;
        }

        public void RunAutonomousCityManagement() => RunStaticAutonomousCityManagement();

        public static void RunStaticAutonomousCityManagement()
        {
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world == null || !world.IsCreated) return;

                EntityManager em = world.EntityManager;
                var citySystem = world.GetExistingSystemManaged<CitySystem>();
                if (citySystem != null && citySystem.City != Entity.Null && em.HasComponent<PlayerMoney>(citySystem.City))
                {
                    PlayerMoney pm = em.GetComponentData<PlayerMoney>(citySystem.City);
                    LastKnownCityTreasury = pm.money;
                    if (StaticAutonomousGovernorActive && pm.money < 50000)
                    {
                        pm.Add(250000);
                        em.SetComponentData(citySystem.City, pm);
                        LastKnownCityTreasury = pm.money;
                        MamaTowerCities2System.LastVoiceBroadcast =
                            $"Mama AI: Autonomous Bailout triggered! Injected +$250,000 -> City Treasury: ${pm.money:N0}!";
                        WonderWorldCS2Plugin.ModLogger?.LogInfo(
                            $"[MamaCityControl] Autonomous Bailout: Injected $250,000! New City Balance: ${pm.money:N0}");
                    }
                }
            }
            catch
            {
            }
        }

        public bool InjectCityMoney(int amount) => InjectCityMoneyStatic(amount);

        public static bool InjectCityMoneyStatic(int amount)
        {
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world != null && world.IsCreated)
                {
                    EntityManager em = world.EntityManager;
                    var citySystem = world.GetExistingSystemManaged<CitySystem>();
                    if (citySystem != null && citySystem.City != Entity.Null && em.HasComponent<PlayerMoney>(citySystem.City))
                    {
                        PlayerMoney pm = em.GetComponentData<PlayerMoney>(citySystem.City);
                        pm.Add(amount);
                        em.SetComponentData(citySystem.City, pm);
                        LastKnownCityTreasury = pm.money;
                        MamaTowerCities2System.Col1GrindBalance += amount * 0.10f;
                        MamaTowerCities2System.LastVoiceBroadcast =
                            $"Mama AI: Injected +${amount:N0} into City Treasury! New Balance: ${pm.money:N0}!";
                        WonderWorldCS2Plugin.ModLogger?.LogInfo(
                            $"[MamaCityControl] Injected ${amount:N0} to City Treasury! New Total: ${pm.money:N0}");
                        MamaTowerCities2System.SaveToSharedJson();
                        return true;
                    }
                }
            }
            catch (Exception ex)
            {
                WonderWorldCS2Plugin.ModLogger?.LogWarning($"[MamaCityControl] Inject warning: {ex.Message}");
            }

            // Fallback if triggered on Main Menu before a city save is loaded
            LastKnownCityTreasury += amount;
            MamaTowerCities2System.Col1GrindBalance += amount;
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: Added +${amount:N0} to Purple City Reserve (${MamaTowerCities2System.Col1GrindBalance:N0})!";
            MamaTowerCities2System.SaveToSharedJson();
            return true;
        }
    }
}
