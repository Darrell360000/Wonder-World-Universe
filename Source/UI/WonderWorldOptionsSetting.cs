using System;
using Colossal.Localization;
using Game.SceneFlow;
using Game.Settings;
using Game.UI.Menu;
using Unity.Entities;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.Vehicles;
using WonderWorldCS2.Systems;

namespace WonderWorldCS2.UI
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
        [SettingsUIDisplayName("WW_MintWonderCurrencyButton", "⚡ Trigger 12% Bond Yield -> Mint Wonder ⭐️W [F9]")]
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
        [SettingsUIDisplayName("WW_HoverFlightEnabled", "✈️ Engage 3D Street-to-Sky Hover-Drive Flight Mode [V]")]
        [SettingsUIDescription("WW_HoverFlightEnabled_Desc", "Toggles 85 BPM Anti-Gravity Hover-Drive Flight in your city (WASD to fly, Space/C altitude, Shift Sonic Boost).")]
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

        [SettingsUISection("HoverDriveAndCinema")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_EnterCinemaPortalButton", "🌀 Enter / Complete Active Cinema Story Portal Flashback [E]")]
        [SettingsUIDescription("WW_EnterCinemaPortalButton_Desc", "Enters or completes the active Top Hatter Cinema Story Portal mission (PORTAL_01 to PORTAL_05) for one-time Treasury & ⭐️W rewards.")]
        public bool EnterCinemaPortalButton
        {
            set
            {
                if (value)
                {
                    UIOverhaul.Instance?.ExecuteWonderAction("enter_cinema_portal");
                }
            }
        }

        [SettingsUISection("DeepSimulationPillars")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_AirliftCargoButton", "📦 Pillar 1: Hover-Cargo Airlift (+250 Storefront Crates & +75 ⭐️W) [Key 1]")]
        [SettingsUIDescription("WW_AirliftCargoButton_Desc", "Airlifts emergency 4-tier supply chain crates to Big Ambitions storefronts and deposits +$45,000 Treasury & +75 ⭐️W.")]
        public bool AirliftCargoButton
        {
            set { if (value) UIOverhaul.Instance?.ExecuteWonderAction("airlift_cargo"); }
        }

        [SettingsUISection("DeepSimulationPillars")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_PurifyBioDomeButton", "🌊 Pillar 2: Purify 59,000-Gal Botanical Bio-Dome Floodgates (+60 ⭐️W) [Key 2]")]
        [SettingsUIDescription("WW_PurifyBioDomeButton_Desc", "Flushes sub-basement canal runoff through Mama AI's Bio-Dome, fills 59,000-gal bio-coolant (1,000 MW), and zeroes Col 2 Debt.")]
        public bool PurifyBioDomeButton
        {
            set { if (value) UIOverhaul.Instance?.ExecuteWonderAction("toggle_floodgates"); }
        }

        [SettingsUISection("DeepSimulationPillars")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_EnactHopeDecreeButton", "📜 Pillar 3: Enact Mayor Hope Decree (CNA Dignity & Mama Rest, +85 ⭐️W) [Key 3]")]
        [SettingsUIDescription("WW_EnactHopeDecreeButton_Desc", "Enacts Mayor Hope Decree, restoring City Hope to 100%, cooling Tokon District Tension by -15%, and banking +85 ⭐️W.")]
        public bool EnactHopeDecreeButton
        {
            set { if (value) UIOverhaul.Instance?.ExecuteWonderAction("enact_hope_decree"); }
        }

        [SettingsUISection("DeepSimulationPillars")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_SyncSpecialistGridButton", "🎩 Pillar 4: Awaken +8 Crowned Specialists & Sync 85 BPM Grid (+90 ⭐️W) [Key 4]")]
        [SettingsUIDescription("WW_SyncSpecialistGridButton_Desc", "Awakens +8 Crowned Specialists (CNAs, Builders, DJs, Engineers) and synchronizes the 85 BPM utility relay to 100%.")]
        public bool SyncSpecialistGridButton
        {
            set { if (value) UIOverhaul.Instance?.ExecuteWonderAction("sync_specialist_grid"); }
        }

        [SettingsUISection("DeepSimulationPillars")]
        [SettingsUIButton]
        [SettingsUIDisplayName("WW_BoostVipCommutesButton", "🚆 Pillar 5: Boost Named VIP Citizen Hyper-Lane Commutes (+100 ⭐️W) [Key 5]")]
        [SettingsUIDescription("WW_BoostVipCommutesButton_Desc", "Upgrades all 5 persistent Named VIP Citizens onto the 85 BPM Hyper-Lane Express (+15% productivity, +$50,000 & +100 ⭐️W).")]
        public bool BoostVipCommutesButton
        {
            set { if (value) UIOverhaul.Instance?.ExecuteWonderAction("boost_vip_commutes"); }
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

                var gm = GameManager.instance;
                if (gm == null || gm.state < GameManager.State.UIReady) return;

                var dict = gm.localizationManager?.activeDictionary;
                if (dict != null)
                {
                    AddLocaleEntry(dict, $"Options.SECTION[{SectionId}]", "🎩 MAMA'S WONDER WORLD");
                    AddLocaleEntry(dict, $"Options.GROUP[{SectionId}.DualCurrencyAndTreasury]", "DUAL-CURRENCY LEDGER ($ + ⭐️W) & CITY TREASURY");
                    AddLocaleEntry(dict, $"Options.GROUP[{SectionId}.HoverDriveAndCinema]", "MODULAR HOVER-DRIVE [V] & CINEMA PORTALS");
                    AddLocaleEntry(dict, $"Options.GROUP[{SectionId}.DeepSimulationPillars]", "5 DEEP SIMULATION PILLARS (SUPPLY | HYDRO | HOPE | GRID | VIPs)");
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
