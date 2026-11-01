using System;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using BepInEx;
using BepInEx.Logging;
using Game;
using Game.Modding;
using Game.SceneFlow;
using HarmonyLib;
using Unity.Entities;
using UnityEngine;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.Vehicles;
using WonderWorldCS2.Portals;
using WonderWorldCS2.UI;
using WonderWorldCS2.Systems;

namespace WonderWorld.Cities2Mod
{
    /// <summary>
    /// Live BepInEx 5.4.23.6 Plugin + Native CS2 IMod + Harmony GameManager.Update Hook
    /// for Cities: Skylines II (D:\Games\Cities - Skylines II\Cities2.exe).
    /// </summary>
    [BepInPlugin(PluginGuid, PluginName, PluginVersion)]
    public class WonderWorldCS2Plugin : BaseUnityPlugin
    {
        public const string PluginGuid = "com.wonderworld.mamatower.cs2";
        public const string PluginName = "Mama's Wonder World // Purple City Dual-Currency, Hover-Drive & Cinema Portals";
        public const string PluginVersion = "3.4.0";

        internal static ManualLogSource ModLogger;
        private static GameObject _suiteObject;
        private static UIOverhaul _uiOverhaul;
        private static HoverDriveVehicle _hoverDriveVehicle;
        private static CinemaBuildingAI _cinemaBuildingAI;
        private static Harmony _harmony;

        private void Awake()
        {
            ModLogger = Logger;
            ModLogger.LogInfo("====================================================================");
            ModLogger.LogInfo($"[WonderWorldCS2] {PluginName} v{PluginVersion} Loaded!");
            ModLogger.LogInfo("[WonderWorldCS2] Native Cohtml HTML5 Dual-Currency HUD + CS2 Options Tab ('🎩 MAMA'S WONDER WORLD')");
            ModLogger.LogInfo("[WonderWorldCS2] Hotkeys: [F8] HUD | [V] Hover-Drive Flight | [F6] +$500k | [F7] +$250k Bailout | [F9] Mint ⭐️W");
            ModLogger.LogInfo("====================================================================");

            try
            {
                _harmony = new Harmony(PluginGuid);
                _harmony.PatchAll(typeof(WonderWorldCS2Plugin).Assembly);
                ModLogger.LogInfo("[WonderWorldCS2] Harmony patched Game.SceneFlow.GameManager.Update successfully.");
            }
            catch (Exception ex)
            {
                ModLogger.LogWarning($"[WonderWorldCS2] Harmony patch warning: {ex.Message}");
            }

            EnsureSuiteCreated();
        }

        public static void EnsureSuiteCreated()
        {
            if (_suiteObject == null)
            {
                _suiteObject = new GameObject("WonderWorld_CS2_PurpleCity_Suite");
                DontDestroyOnLoad(_suiteObject);
                _suiteObject.hideFlags = HideFlags.HideAndDontSave;
            }

            if (_hoverDriveVehicle == null)
            {
                _hoverDriveVehicle = _suiteObject.GetComponent<HoverDriveVehicle>() ?? _suiteObject.AddComponent<HoverDriveVehicle>();
            }

            if (_cinemaBuildingAI == null)
            {
                _cinemaBuildingAI = _suiteObject.GetComponent<CinemaBuildingAI>() ?? _suiteObject.AddComponent<CinemaBuildingAI>();
            }

            if (_uiOverhaul == null)
            {
                _uiOverhaul = _suiteObject.GetComponent<UIOverhaul>() ?? _suiteObject.AddComponent<UIOverhaul>();
            }
        }

        public static void EnsureSuiteAndTickFrame()
        {
            EnsureSuiteCreated();
            _hoverDriveVehicle?.TickFrame();
            _cinemaBuildingAI?.TickFrame();
            _uiOverhaul?.TickFrame();
        }
    }

    /// <summary>
    /// Harmony Postfix on Game.SceneFlow.GameManager.Update so our HUD, Options tab registration,
    /// Hover-Drive flight, and City Treasury controls execute every frame inside Cities: Skylines II's main loop.
    /// </summary>
    [HarmonyPatch(typeof(GameManager), "Update")]
    public static class GameManagerUpdateHarmonyPatch
    {
        [HarmonyPostfix]
        public static void Postfix()
        {
            try
            {
                WonderWorldCS2Plugin.EnsureSuiteAndTickFrame();
            }
            catch
            {
            }
        }
    }

    /// <summary>
    /// Native Cities: Skylines II IMod entrypoint so CS2's ModManager also registers our systems
    /// and Options UI tab ("🎩 MAMA'S WONDER WORLD").
    /// </summary>
    public sealed class WonderWorldNativeCS2Mod : IMod
    {
        public void OnLoad(UpdateSystem updateSystem)
        {
            WonderWorldCS2Plugin.ModLogger?.LogInfo("[WonderWorldNativeCS2Mod] IMod.OnLoad executed via CS2 ModManager!");
            WonderWorldCS2Plugin.EnsureSuiteCreated();
            WonderWorldOptionsSetting.EnsureRegisteredInOptionsUI();

            if (updateSystem != null)
            {
                updateSystem.UpdateAt<MamaTowerCities2System>(SystemUpdatePhase.GameSimulation);
                updateSystem.UpdateAt<EconomyOverhaul>(SystemUpdatePhase.GameSimulation);
                updateSystem.UpdateAt<MamaCityControlSystem>(SystemUpdatePhase.GameSimulation);
                updateSystem.UpdateAt<WonderUniverseIntegrationSystem>(SystemUpdatePhase.GameSimulation);
                updateSystem.UpdateAt<CinemaPortalMissionSystem>(SystemUpdatePhase.GameSimulation);
            }
        }

        public void OnDispose()
        {
        }
    }

    /// <summary>
    /// Cities: Skylines II ECS System (GameSystemBase) syncing the 3-Column Financial Ledger,
    /// Wonder Super-Currency (⭐️W), and 8m x 8m Mama & Papa Wonder Tower telemetry.
    /// </summary>
    public partial class MamaTowerCities2System : GameSystemBase
    {
        public static float Col1GrindBalance = 91782.82f;
        public static float Col2OverheadDebt = 0.00f;
        public static float Col3FoundationCapital = 52535.00f;
        public static float StakedIndustrialBonds = 15000.00f;
        public static int UnlockedTowerTier = 5;
        public static int CurrentEraIndex = 5; // Era 5: Purple City
        public static bool EvonHubPermissionGranted = true;
        public static float WetVacGallonsRemaining = 0.0f;
        public static float CnaPatientDignity = 100.0f;
        public static string LastVoiceBroadcast =
            "Mama AI: Welcome to Purple City, baby! Press [V] to fly the Hover-Cruiser, [F6] for +$500k City Treasury, or [F8] for the Command Deck!";

        public static readonly string[] SharedLedgerCandidatePaths = new[]
        {
            @"D:\Games\Cities - Skylines II\02_Shared_Game_Core\State\WonderLedgerState.json",
            @"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\02_Shared_Game_Core\State\WonderLedgerState.json",
            @"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\WonderPipeline\02_Shared_Game_Core\State\WonderLedgerState.json",
            @"C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe\02_Shared_Game_Core\State\WonderLedgerState.json"
        };

        protected override void OnCreate()
        {
            base.OnCreate();
            SyncFromSharedJson();
            WonderWorldCS2Plugin.ModLogger?.LogInfo("[MamaTowerCities2System] ECS System Created & Synced with WonderLedgerState.json.");
        }

        protected override void OnUpdate()
        {
            if (UnlockedTowerTier >= 3)
            {
                Col1GrindBalance += 12.50f;
                Col3FoundationCapital += 5.00f;
            }
        }

        public override int GetUpdateInterval(SystemUpdatePhase phase)
        {
            return 16384;
        }

        public static string ResolveLedgerPath()
        {
            foreach (var candidate in SharedLedgerCandidatePaths)
            {
                if (File.Exists(candidate))
                {
                    return candidate;
                }
            }
            return SharedLedgerCandidatePaths[0];
        }

        public static void SyncFromSharedJson()
        {
            try
            {
                string path = ResolveLedgerPath();
                if (!File.Exists(path)) return;

                string json = File.ReadAllText(path);
                Col3FoundationCapital = ExtractJsonFloat(json, "liquid_reserve", Col3FoundationCapital);
                StakedIndustrialBonds = ExtractJsonFloat(json, "staked_industrial_bonds", StakedIndustrialBonds);
                UnlockedTowerTier = (int)ExtractJsonFloat(json, "unlocked_tier", UnlockedTowerTier);
                WonderCurrencyConverter.WonderSuperCurrencyBalance =
                    ExtractJsonFloat(json, "wonder_super_currency_w", WonderCurrencyConverter.WonderSuperCurrencyBalance);
                WonderCurrencyConverter.LifetimeBondYieldConvertedUsd =
                    ExtractJsonFloat(json, "bond_yield_converted_usd", WonderCurrencyConverter.LifetimeBondYieldConvertedUsd);

                int syncedBusinesses = (int)ExtractJsonFloat(json, "activeBusinesses", WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses);
                float syncedTension = ExtractJsonFloat(json, "districtTension", WonderUniverseIntegrationSystem.StaticDistrictTensionIndex);
                int syncedStars = (int)ExtractJsonFloat(json, "wonderStars", WonderUniverseIntegrationSystem.StaticWonderStarsBanked);
                WonderUniverseIntegrationSystem.ApplyExternalBridgeSync(syncedBusinesses, syncedTension, syncedStars);

                float brawler = ExtractJsonFloat(json, "arcade_brawler_payouts", 20820.71f);
                float lease = ExtractJsonFloat(json, "commercial_lease_income", 45805.55f);
                float tourism = ExtractJsonFloat(json, "cities_skylines_tourism_bonus", 16656.56f);
                float cna = ExtractJsonFloat(json, "cna_and_wetvac_minigames", 8500.00f);
                Col1GrindBalance = brawler + lease + tourism + cna;

                float maint = ExtractJsonFloat(json, "structural_maintenance", 0.00f);
                float staff = ExtractJsonFloat(json, "security_and_brawler_staff", 0.00f);
                float grid = ExtractJsonFloat(json, "utility_grid_load_cost", 0.00f);
                Col2OverheadDebt = maint + staff + grid;

                EvonHubPermissionGranted = Col3FoundationCapital >= 25000.0f || Col2OverheadDebt <= 0.01f;
            }
            catch (Exception ex)
            {
                WonderWorldCS2Plugin.ModLogger?.LogWarning($"[MamaTowerCities2System] JSON Read Warning: {ex.Message}");
            }
        }

        public static void SaveToSharedJson(string explicitTargetPath = null)
        {
            string[] targets = explicitTargetPath != null ? new[] { explicitTargetPath } : SharedLedgerCandidatePaths;

            foreach (string path in targets)
            {
                try
                {
                    string dir = Path.GetDirectoryName(path);
                    if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
                    {
                        Directory.CreateDirectory(dir);
                    }

                    string activeHero = CinemaBuildingAI.ActiveInstance != null
                        ? CinemaBuildingAI.ActiveInstance.TopHatterRoster[CinemaBuildingAI.ActiveInstance.NearestActiveHeroIndex].HeroName
                        : "Darrell X-Gen";
                    string marqueeText = CinemaBuildingAI.ActiveInstance != null
                        ? CinemaBuildingAI.ActiveInstance.ActiveMarqueeHeadline
                        : "NOW PLAYING: DARRELL X-GEN // 85 BPM GOD-HAND SONIC ODYSSEY";

                    string json = string.Format(
                        CultureInfo.InvariantCulture,
                        "{{\n" +
                        "  \"schema_version\": \"3.4.0\",\n" +
                        "  \"municipalTreasury\": {17},\n" +
                        "  \"wonderStars\": {18},\n" +
                        "  \"activeBusinesses\": {19},\n" +
                        "  \"districtTension\": {20:F1},\n" +
                        "  \"activeRadioStation\": \"{21}\",\n" +
                        "  \"lastSyncTimestamp\": {22},\n" +
                        "  \"tower_name\": \"Mama Wonder Tower\",\n" +
                        "  \"hardware_profile\": \"RTX_5060_16GB_RAM\",\n" +
                        "  \"evon_hub_permission_granted\": {0},\n" +
                        "  \"current_era_index\": {1},\n" +
                        "  \"universe\": \"Mama's Wonder World // Purple City Universe (Wonder Command v3.4)\",\n" +
                        "  \"dual_currency_hud\": {{\n" +
                        "    \"city_treasury_usd\": {17},\n" +
                        "    \"municipal_dollars_usd\": {2:F2},\n" +
                        "    \"wonder_super_currency_w\": {3:F2},\n" +
                        "    \"bond_yield_rate_annual\": 0.12,\n" +
                        "    \"bond_yield_converted_usd\": {4:F2}\n" +
                        "  }},\n" +
                        "  \"cinema_portal_state\": {{\n" +
                        "    \"nearest_active_top_hatter\": \"{5}\",\n" +
                        "    \"marquee_headline\": \"{6}\"\n" +
                        "  }},\n" +
                        "  \"ledger_columns\": {{\n" +
                        "    \"column_1_gross_revenue\": {{\n" +
                        "      \"cna_and_wetvac_minigames\": {7:F2},\n" +
                        "      \"arcade_brawler_payouts\": {8:F2},\n" +
                        "      \"commercial_lease_income\": {9:F2},\n" +
                        "      \"cities_skylines_tourism_bonus\": {10:F2}\n" +
                        "    }},\n" +
                        "    \"column_2_operating_expenses\": {{\n" +
                        "      \"structural_maintenance\": {11:F2},\n" +
                        "      \"security_and_brawler_staff\": {12:F2},\n" +
                        "      \"utility_grid_load_cost\": {13:F2}\n" +
                        "    }},\n" +
                        "    \"column_3_net_reserve_and_upgrades\": {{\n" +
                        "      \"liquid_reserve\": {14:F2},\n" +
                        "      \"staked_industrial_bonds\": {15:F2},\n" +
                        "      \"wonder_super_currency_w\": {3:F2},\n" +
                        "      \"bond_yield_converted_usd\": {4:F2},\n" +
                        "      \"unlocked_tier\": {16},\n" +
                        "      \"cities_skylines_lot_size\": \"8m x 8m\",\n" +
                        "      \"emissive_signage_active\": true\n" +
                        "    }}\n" +
                        "  }}\n" +
                        "}}\n",
                        EvonHubPermissionGranted ? "true" : "false",
                        CurrentEraIndex,
                        Col1GrindBalance,
                        WonderCurrencyConverter.WonderSuperCurrencyBalance,
                        WonderCurrencyConverter.LifetimeBondYieldConvertedUsd,
                        activeHero.Replace("\"", "\\\""),
                        marqueeText.Replace("\"", "\\\""),
                        Col1GrindBalance * 0.10f,
                        Col1GrindBalance * 0.25f,
                        Col1GrindBalance * 0.45f,
                        Col1GrindBalance * 0.20f,
                        Col2OverheadDebt * 0.45f,
                        Col2OverheadDebt * 0.35f,
                        Col2OverheadDebt * 0.20f,
                        Col3FoundationCapital,
                        StakedIndustrialBonds,
                        UnlockedTowerTier,
                        MamaCityControlSystem.LastKnownCityTreasury,
                        WonderUniverseIntegrationSystem.StaticWonderStarsBanked,
                        WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses,
                        WonderUniverseIntegrationSystem.StaticDistrictTensionIndex,
                        WonderUniverseIntegrationSystem.StaticActiveRadioStation.Replace("\"", "\\\""),
                        DateTimeOffset.UtcNow.ToUnixTimeSeconds());

                    File.WriteAllText(path, json);
                }
                catch (Exception ex)
                {
                    WonderWorldCS2Plugin.ModLogger?.LogWarning($"[MamaTowerCities2System] JSON Write Warning ({path}): {ex.Message}");
                }
            }
        }

        private static float ExtractJsonFloat(string json, string key, float fallback)
        {
            var match = Regex.Match(json, $"\"{Regex.Escape(key)}\"\\s*:\\s*([-0-9.]+)");
            if (match.Success && float.TryParse(
                match.Groups[1].Value,
                NumberStyles.Float,
                CultureInfo.InvariantCulture,
                out float val))
            {
                return val;
            }
            return fallback;
        }
    }
}
