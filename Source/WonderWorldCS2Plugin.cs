using System;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using BepInEx;
using BepInEx.Logging;
using Game;
using Unity.Entities;
using UnityEngine;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.UI;
using WonderWorld.Cities2Mod.Vehicles;

namespace WonderWorld.Cities2Mod
{
    /// <summary>
    /// Live BepInEx 5.4.23.6 Plugin for Cities: Skylines II (D:\Games\Cities - Skylines II\Cities2.exe)
    /// Integrates Mama's Wonder World // Project Purple City:
    /// - Dual-Currency HUD (Municipal $ + Wonder ⭐️W) via UIOverhaul.cs
    /// - Modular Hover-Drive Vehicle Physics ([V] Flight Toggle) via HoverDriveVehicle.cs
    /// - Dynamic Top-Hatter Cinema Portal Marquee via CinemaBuildingAI.cs
    /// - 12% Bond Yield -> Wonder ⭐️W Exchange via EconomyOverhaul.cs & WonderCurrencyConverter.cs
    /// </summary>
    [BepInPlugin(PluginGuid, PluginName, PluginVersion)]
    public class WonderWorldCS2Plugin : BaseUnityPlugin
    {
        public const string PluginGuid = "com.wonderworld.mamatower.cs2";
        public const string PluginName = "Mama's Wonder World // Purple City Dual-Currency, Hover-Drive & Cinema Portals";
        public const string PluginVersion = "3.2.0";

        internal static ManualLogSource ModLogger;
        private UIOverhaul _uiOverhaul;
        private HoverDriveVehicle _hoverDriveVehicle;
        private CinemaBuildingAI _cinemaBuildingAI;

        private void Awake()
        {
            ModLogger = Logger;
            ModLogger.LogInfo("====================================================================");
            ModLogger.LogInfo($"[WonderWorldCS2] {PluginName} v{PluginVersion} Loaded!");
            ModLogger.LogInfo("[WonderWorldCS2] Dual-Currency HUD: Municipal ($) + Wonder Super-Currency (⭐️W)");
            ModLogger.LogInfo("[WonderWorldCS2] Press [V] to toggle Modular Hover-Drive Flight | Press [F8] for Full HUD.");
            ModLogger.LogInfo("====================================================================");

            var go = new GameObject("WonderWorld_CS2_PurpleCity_Suite");
            DontDestroyOnLoad(go);
            go.hideFlags = HideFlags.HideAndDontSave;

            _hoverDriveVehicle = go.AddComponent<HoverDriveVehicle>();
            _cinemaBuildingAI = go.AddComponent<CinemaBuildingAI>();
            _uiOverhaul = go.AddComponent<UIOverhaul>();
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
            "Mama AI: Welcome to Purple City, baby! Press [V] for Hover-Drive or [F8] for the Dual-Currency & Cinema Portal HUD!";

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
                        "  \"schema_version\": \"3.2.0\",\n" +
                        "  \"tower_name\": \"Mama Wonder Tower\",\n" +
                        "  \"hardware_profile\": \"RTX_5060_16GB_RAM\",\n" +
                        "  \"evon_hub_permission_granted\": {0},\n" +
                        "  \"current_era_index\": {1},\n" +
                        "  \"universe\": \"Mama's Wonder World // Purple City Universe (Wonder Command v3.2)\",\n" +
                        "  \"dual_currency_hud\": {{\n" +
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
                        UnlockedTowerTier);

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
