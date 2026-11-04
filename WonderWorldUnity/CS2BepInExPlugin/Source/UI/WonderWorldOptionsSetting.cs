using System;
using Colossal.Localization;
using Game.SceneFlow;
using Game.Settings;
using Game.UI.Menu;
using Unity.Entities;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.Vehicles;
using WonderWorldCS2.Systems;
using WonderWorldCS2.UI;

namespace WonderWorld.Cities2Mod.UI
{
    /// <summary>
    /// Native Cities: Skylines II Options Menu Section ("🎩 MAMA'S WONDER WORLD").
    /// Appears directly in the left sidebar of the CS2 OPTIONS screen alongside
    /// GENERAL, GRAPHICS, GAMEPLAY, INTERFACE, AUDIO, INPUT, and MODDING.
    /// </summary>
    [SettingsUIShowGroupName]
    public sealed class WonderWorldOptionsSetting : Setting
    {
        public const string SectionId = "WonderWorldPurpleCity";
        private static WonderWorldOptionsSetting _instance;
        private static bool _registeredInOptions = false;

        [SettingsUISection("DualCurrencyAndTreasury")]
        [SettingsUIDisplayName("WW_ShowDualCurrencyHud", "Show Purple City Dual-Currency HUD Overlay [F8]")]
        [SettingsUIDescription("WW_ShowDualCurrencyHud_Desc", "Toggles the top-screen Purple City Dual-Currency HUD (City $ + Municipal $ + Wonder ⭐️W) and F8 Command Deck.")]
        public bool ShowDualCurrencyHud
        {
            get => UIOverhaul.Instance == null || UIOverhaul.Instance.ShowHud;
            set
            {
                if (UIOverhaul.Instance != null)
                {
                    UIOverhaul.Instance.ShowHud = value;
                    UIOverhaul.Instance.ForceCohtmlRefresh();
                }
            }
        }

        [SettingsUISection("DualCurrencyAndTreasury")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_Inject500kButton", "💰 Inject +$500,000 to City Treasury [F6]")]
        [SettingsUIDescription("WW_Inject500kButton_Desc", "Immediately injects +$500,000 into your active Cities: Skylines II City Treasury (PlayerMoney).")]
        public bool Inject500kButton
        {
            set
            {
                if (value)
                {
                    MamaCityControlSystem.InjectCityMoneyStatic(500000);
                    UIOverhaul.Instance?.ForceCohtmlRefresh();
                }
            }
        }

        [SettingsUISection("DualCurrencyAndTreasury")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_AutonomousBailoutButton", "💜 Autonomous Bailout (+$250,000 & Zero Overhead Debt) [F7]")]
        [SettingsUIDescription("WW_AutonomousBailoutButton_Desc", "Injects +$250,000 into City Treasury and zeroes Column 2 Overhead Debt ($0.00) so 12% Bonds mint Wonder ⭐️W.")]
        public bool AutonomousBailoutButton
        {
            set
            {
                if (value)
                {
                    UIOverhaul.Instance?.ExecuteWonderAction("bailout_250k");
                }
            }
        }

        [SettingsUISection("DualCurrencyAndTreasury")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_MintWonderCurrencyButton", "⚡ Trigger 12% Bond Yield -> Mint Wonder ⭐️W [F10]")]
        [SettingsUIDescription("WW_MintWonderCurrencyButton_Desc", "Runs a 12% Industrial Bond coupon cycle past $0 overhead and writes Wonder Super-Currency (⭐️W) to WonderLedgerState.json.")]
        public bool MintWonderCurrencyButton
        {
            set
            {
                if (value)
                {
                    EconomyOverhaul.ExecuteBondYieldCycle(EconomyOverhaul.AnnualBondCouponRate);
                    UIOverhaul.Instance?.ForceCohtmlRefresh();
                }
            }
        }

        [SettingsUISection("HoverDriveAndCinema")]
        [SettingsUIDisplayName("WW_HoverFlightEnabled", "✈️ Engage 3D Street-to-Sky Hover-Drive Flight Mode [V / F4]")]
        [SettingsUIDescription("WW_HoverFlightEnabled_Desc", "Toggles 85 BPM Anti-Gravity Hover-Drive Flight in your city (WASD to fly, R/F or PgUp/PgDn altitude, Shift Sonic Boost).")]
        public bool HoverFlightEnabled
        {
            get => HoverDriveVehicle.ActiveInstance != null && HoverDriveVehicle.ActiveInstance.IsHoverFlightActive;
            set
            {
                var v = HoverDriveVehicle.ActiveInstance;
                if (v != null && v.IsHoverFlightActive != value)
                {
                    v.ToggleHoverFlight();
                    UIOverhaul.Instance?.ForceCohtmlRefresh();
                }
            }
        }

        [SettingsUISection("HoverDriveAndCinema")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_CycleCinemaHeroButton", "🎬 Cycle Nearest Top Hatter Hero on Cinema Portal Marquee [Shift+1..5]")]
        [SettingsUIDescription("WW_CycleCinemaHeroButton_Desc", "Cycles the active Top Hatter Hero (Darrell X-Gen, Spider-Pulse, Divine Hands, Jaxson Thorne, Vesper Vance) at the Cinema Portal.")]
        public bool CycleCinemaHeroButton
        {
            set
            {
                if (value && CinemaBuildingAI.ActiveInstance != null)
                {
                    int next = (CinemaBuildingAI.ActiveInstance.NearestActiveHeroIndex + 1) % 5;
                    CinemaBuildingAI.ActiveInstance.SetHeroClosestToCinema(next);
                    UIOverhaul.Instance?.ForceCohtmlRefresh();
                }
            }
        }

        public override void SetDefaults()
        {
        }

        public static void EnsureRegisteredInOptionsUI()
        {
            if (_registeredInOptions) return;

            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world == null || !world.IsCreated) return;

                var optionsUISystem = world.GetExistingSystemManaged<OptionsUISystem>();
                if (optionsUISystem == null) return;

                var dict = GameManager.instance?.localizationManager?.activeDictionary;
                if (dict != null)
                {
                    AddLocaleEntry(dict, $"Options.SECTION[{SectionId}]", "🎩 MAMA'S WONDER WORLD");
                    AddLocaleEntry(dict, $"Options.GROUP[{SectionId}.DualCurrencyAndTreasury]", "DUAL-CURRENCY LEDGER ($ + ⭐️W) & CITY TREASURY");
                    AddLocaleEntry(dict, $"Options.GROUP[{SectionId}.HoverDriveAndCinema]", "MODULAR HOVER-DRIVE [V / F4] & CINEMA PORTALS");

                    AddLocaleEntry(dict, "WW_ShowDualCurrencyHud", "Show Purple City Dual-Currency HUD Overlay [F8]");
                    AddLocaleEntry(dict, "WW_ShowDualCurrencyHud_Desc", "Toggles the top-screen Purple City Dual-Currency HUD (City $ + Municipal $ + Wonder ⭐️W) and F8 Command Deck.");
                    AddLocaleEntry(dict, "WW_Inject500kButton", "💰 Inject +$500,000 to City Treasury [F6]");
                    AddLocaleEntry(dict, "WW_Inject500kButton_Desc", "Immediately injects +$500,000 into your active Cities: Skylines II City Treasury (PlayerMoney).");
                    AddLocaleEntry(dict, "WW_AutonomousBailoutButton", "💜 Autonomous Bailout (+$250,000 & Zero Overhead Debt) [F7]");
                    AddLocaleEntry(dict, "WW_AutonomousBailoutButton_Desc", "Injects +$250,000 into City Treasury and zeroes Column 2 Overhead Debt ($0.00) so 12% Bonds mint Wonder ⭐️W.");
                    AddLocaleEntry(dict, "WW_MintWonderCurrencyButton", "⚡ Trigger 12% Bond Yield -> Mint Wonder ⭐️W [F10]");
                    AddLocaleEntry(dict, "WW_MintWonderCurrencyButton_Desc", "Runs a 12% Industrial Bond coupon cycle past $0 overhead and writes Wonder Super-Currency (⭐️W) to WonderLedgerState.json.");
                    AddLocaleEntry(dict, "WW_HoverFlightEnabled", "✈️ Engage 3D Street-to-Sky Hover-Drive Flight Mode [V / F4]");
                    AddLocaleEntry(dict, "WW_HoverFlightEnabled_Desc", "Toggles 85 BPM Anti-Gravity Hover-Drive Flight in your city (WASD to fly, R/F or PgUp/PgDn altitude, Shift Sonic Boost).");
                    AddLocaleEntry(dict, "WW_CycleCinemaHeroButton", "🎬 Cycle Nearest Top Hatter Hero on Cinema Portal Marquee [Shift+1..5]");
                    AddLocaleEntry(dict, "WW_CycleCinemaHeroButton_Desc", "Cycles the active Top Hatter Hero (Darrell X-Gen, Spider-Pulse, Divine Hands, Jaxson Thorne, Vesper Vance) at the Cinema Portal.");
                }

                if (_instance == null)
                {
                    _instance = new WonderWorldOptionsSetting();
                }

                optionsUISystem.RegisterSetting(_instance, SectionId, false);
                _registeredInOptions = true;
                WonderWorldCS2Plugin.ModLogger?.LogInfo("[WonderWorldOptionsSetting] Registered '🎩 MAMA'S WONDER WORLD' tab in Cities: Skylines II OPTIONS menu!");
            }
            catch (Exception ex)
            {
                WonderWorldCS2Plugin.ModLogger?.LogWarning($"[WonderWorldOptionsSetting] Options UI registration notice: {ex.Message}");
            }
        }

        private static void AddLocaleEntry(LocalizationDictionary dict, string key, string value)
        {
            try
            {
                dict.Add(key, value, true);
            }
            catch
            {
            }
        }
    }
}
