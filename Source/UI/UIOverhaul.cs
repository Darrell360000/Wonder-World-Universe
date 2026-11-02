using System;
using System.Globalization;
using System.Text;
using Colossal.UI.Binding;
using cohtml.Net;
using Game.SceneFlow;
using UnityEngine;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.UI;
using WonderWorld.Cities2Mod.Vehicles;
using WonderWorldCS2.Portals;
using WonderWorldCS2.Systems;

namespace WonderWorldCS2.UI
{
    /// <summary>
    /// Cities: Skylines II Native Cohtml HTML5 UI Overhaul + IMGUI Fallback.
    /// Injects the Purple City Dual-Currency HUD (Municipal $ + City Treasury $ + Wonder ⭐️W)
    /// skinned with native CS2 vector glyphs (StarFilled.svg, Money.svg, Warning.svg, HeartFilled.svg),
    /// Signature Building thumbnails (EU_CommercialHighSignature01.png, IndustrialManufacturingSignature01.png),
    /// Drome Cinema audio stings, 85 BPM Radio Presets (The Smooth Beat / The Second Moon),
    /// Multi-Game Wonder Universe Integration, Modular Hover-Drive [V] Flight Cockpit,
    /// and the 5 Top Hatter CINEMA STORY MISSIONS (CinemaPortalMissionSystem).
    /// </summary>
    public sealed class UIOverhaul : MonoBehaviour
    {
        public static UIOverhaul Instance { get; private set; }

        public bool ShowHud = true;
        private int _lastTickedFrame = -1;
        private float _cohtmlSyncTimer = 0f;
        private Rect _fallbackWindowRect = new Rect(24f, 52f, 740f, 760f);
        private IBindingRegistry _commandBindingRegistry;
        private TriggerBinding _spawnStreetBrawlBinding;

        private void Awake()
        {
            Instance = this;
        }

        private void Start()
        {
            MamaTowerCities2System.SyncFromSharedJson();
        }

        private void Update()
        {
            TickFrame();
        }

        private void OnDestroy()
        {
            if (_commandBindingRegistry != null && _spawnStreetBrawlBinding != null)
            {
                _commandBindingRegistry.RemoveBinding(_spawnStreetBrawlBinding);
            }

            if (Instance == this)
            {
                Instance = null;
            }
        }

        public void ForceCohtmlRefresh()
        {
            PushStateToCohtmlDom(forceImmediate: true);
        }

        public void TickFrame()
        {
            if (_lastTickedFrame == Time.frameCount) return;
            _lastTickedFrame = Time.frameCount;

            MamaCityControlSystem.EnsureRegisteredInWorld();
            WonderUniverseIntegrationSystem.EnsureRegisteredInWorld();
            CinemaPortalMissionSystem.EnsureRegisteredInWorld();
            WonderDeepSimulationSystem.EnsureRegisteredInWorld();
            SyndicateEnforcerSpawnerSystem.EnsureRegisteredInWorld();
            WonderWorldOptionsSetting.EnsureRegisteredInOptionsUI();

            // [F8] Toggles the full Wonder Command Interactive Panel
            if (Input.GetKeyDown(KeyCode.F8))
            {
                ShowHud = !ShowHud;
                if (ShowHud)
                {
                    MamaTowerCities2System.SyncFromSharedJson();
                }
                PushStateToCohtmlDom(forceImmediate: true);
            }

            // [F6] Inject +$500,000 directly into Cities: Skylines II City Treasury
            if (Input.GetKeyDown(KeyCode.F6))
            {
                ExecuteWonderAction("inject_500k");
            }

            // [F7] Autonomous Bailout (+$250,000 City Treasury + Zero Column 2 Overhead)
            if (Input.GetKeyDown(KeyCode.F7))
            {
                ExecuteWonderAction("bailout_250k");
            }

            // [F10] (or [F9]) Trigger 12% Bond Yield -> Wonder ⭐️W Super-Currency Conversion
            if (Input.GetKeyDown(KeyCode.F10) || Input.GetKeyDown(KeyCode.F9))
            {
                ExecuteWonderAction("mint_wonder_w");
            }

            // [Ctrl+E] Enter or Complete Active Cinema Story Portal Flashback; E places the Magenta portal.
            if (Input.GetKeyDown(KeyCode.E) &&
                (Input.GetKey(KeyCode.LeftControl) || Input.GetKey(KeyCode.RightControl)))
            {
                ExecuteWonderAction("enter_cinema_portal");
            }

            // [1..5] Trigger the 5 Deep Simulation Pillars directly (when Alt/Shift are not held)
            bool modKey = Input.GetKey(KeyCode.LeftAlt) || Input.GetKey(KeyCode.RightAlt) ||
                          Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift);
            if (!modKey)
            {
                if (Input.GetKeyDown(KeyCode.Alpha1)) ExecuteWonderAction("airlift_cargo");
                if (Input.GetKeyDown(KeyCode.Alpha2)) ExecuteWonderAction("toggle_floodgates");
                if (Input.GetKeyDown(KeyCode.Alpha3)) ExecuteWonderAction("enact_hope_decree");
                if (Input.GetKeyDown(KeyCode.Alpha4)) ExecuteWonderAction("sync_specialist_grid");
                if (Input.GetKeyDown(KeyCode.Alpha5)) ExecuteWonderAction("boost_vip_commutes");
            }

            // Tick 12% Bond Yield Economy & Autonomous City Governor
            EconomyOverhaul.TickRealtimeEconomy(Time.unscaledDeltaTime);

            // Sync Cohtml HTML5 UI overlay every 0.25s (and immediately on key actions)
            _cohtmlSyncTimer += Time.unscaledDeltaTime;
            if (_cohtmlSyncTimer >= 0.25f)
            {
                _cohtmlSyncTimer = 0f;
                MamaCityControlSystem.RunStaticAutonomousCityManagement();
                PushStateToCohtmlDom(forceImmediate: false);
            }
        }

        public void ExecuteWonderAction(string cmd)
        {
            if (string.IsNullOrEmpty(cmd)) return;

            var vehicle = HoverDriveVehicle.ActiveInstance;
            var cinema = CinemaBuildingAI.ActiveInstance;
            var universe = WonderUniverseIntegrationSystem.EnsureRegisteredInWorld();

            switch (cmd)
            {
                case "toggle_hud":
                    ShowHud = !ShowHud;
                    break;
                case "inject_500k":
                    MamaCityControlSystem.InjectCityMoneyStatic(500000);
                    break;
                case "bailout_250k":
                    MamaCityControlSystem.InjectCityMoneyStatic(250000);
                    MamaTowerCities2System.Col2OverheadDebt = 0.00f;
                    MamaTowerCities2System.EvonHubPermissionGranted = true;
                    MamaTowerCities2System.LastVoiceBroadcast =
                        "Mama AI: +$250,000 Bailout Injected & Col 2 Overhead cleared to $0.00! 12% Bonds now mint ⭐️W!";
                    MamaTowerCities2System.SaveToSharedJson();
                    break;
                case "zero_overhead":
                    MamaTowerCities2System.Col2OverheadDebt = 0.00f;
                    MamaTowerCities2System.EvonHubPermissionGranted = true;
                    MamaTowerCities2System.LastVoiceBroadcast =
                        "Mama AI: Column 2 Overhead is $0.00! Evon Hub GRANTED & 12% Bond Yields now mint ⭐️W!";
                    MamaTowerCities2System.SaveToSharedJson();
                    break;
                case "mint_wonder_w":
                    EconomyOverhaul.ExecuteBondYieldCycle(EconomyOverhaul.AnnualBondCouponRate);
                    break;
                case "host_tokon_showdown":
                    if (universe != null)
                    {
                        universe.HostDistrictShowdown();
                    }
                    else
                    {
                        WonderUniverseIntegrationSystem.HostDistrictShowdownStatic();
                    }
                    break;
                case "premiere_wonder_movie":
                    if (universe != null)
                    {
                        universe.PremiereBlockbusterMovie();
                    }
                    else
                    {
                        WonderUniverseIntegrationSystem.PremiereBlockbusterMovieStatic();
                    }
                    break;
                case "cycle_radio_preset":
                    WonderUniverseIntegrationSystem.CycleWonderRadioPresetStatic();
                    break;
                case "enter_cinema_portal":
                    CinemaPortalMissionSystem.TriggerNextOrCompletePortalFlashbackStatic();
                    break;
                case "complete_cinema_portal":
                    CinemaPortalMissionSystem.CompleteActiveMissionStatic();
                    break;
                case "portal_01":
                    CinemaPortalMissionSystem.EnterCinemaPortalStatic("PORTAL_01");
                    break;
                case "portal_02":
                    CinemaPortalMissionSystem.EnterCinemaPortalStatic("PORTAL_02");
                    break;
                case "portal_03":
                    CinemaPortalMissionSystem.EnterCinemaPortalStatic("PORTAL_03");
                    break;
                case "portal_04":
                    CinemaPortalMissionSystem.EnterCinemaPortalStatic("PORTAL_04");
                    break;
                case "portal_05":
                    CinemaPortalMissionSystem.EnterCinemaPortalStatic("PORTAL_05");
                    break;
                case "toggle_hover":
                    vehicle?.ToggleHoverFlight();
                    break;
                case "toggle_cloak":
                    vehicle?.ToggleCloak();
                    break;
                case "part_1":
                    vehicle?.AttachOrToggleModularPart(ModularVehicleSlot.SubBassRepulsorThrusters);
                    break;
                case "part_2":
                    vehicle?.AttachOrToggleModularPart(ModularVehicleSlot.SonicPhaserFrontGrille);
                    break;
                case "part_3":
                    vehicle?.AttachOrToggleModularPart(ModularVehicleSlot.RainbowEqualizerAeroWing);
                    break;
                case "part_4":
                    vehicle?.AttachOrToggleModularPart(ModularVehicleSlot.TopHatMagneticRoofTurret);
                    break;
                case "hero_0":
                    cinema?.SetHeroClosestToCinema(0);
                    break;
                case "hero_1":
                    cinema?.SetHeroClosestToCinema(1);
                    break;
                case "hero_2":
                    cinema?.SetHeroClosestToCinema(2);
                    break;
                case "hero_3":
                    cinema?.SetHeroClosestToCinema(3);
                    break;
                case "hero_4":
                    cinema?.SetHeroClosestToCinema(4);
                    break;
                case "wetvac_shift":
                    MamaTowerCities2System.Col1GrindBalance += 185.00f;
                    MamaTowerCities2System.Col2OverheadDebt = Mathf.Max(0f, MamaTowerCities2System.Col2OverheadDebt - 653.91f);
                    MamaTowerCities2System.Col3FoundationCapital += 37.00f;
                    MamaTowerCities2System.LastVoiceBroadcast =
                        "Mama AI: 59,000-Gal Wet-Vac shift complete! +$185 Grind & -$653.91 Col 2 Overhead!";
                    MamaTowerCities2System.SaveToSharedJson();
                    break;
                case "cna_shift":
                    MamaTowerCities2System.Col1GrindBalance += 285.00f;
                    MamaTowerCities2System.Col3FoundationCapital += 57.00f;
                    MamaTowerCities2System.CnaPatientDignity = 100.0f;
                    MamaTowerCities2System.LastVoiceBroadcast =
                        "Mama AI: CNA Dignity Shift 100%! +$285 Grind & +$57 Col 3 Reserve!";
                    MamaTowerCities2System.SaveToSharedJson();
                    break;
                case "airlift_cargo":
                    WonderDeepSimulationSystem.ExecuteHoverCargoAirliftStatic();
                    break;
                case "toggle_floodgates":
                    WonderDeepSimulationSystem.ToggleBioDomeFloodgatesAndPurifyStatic();
                    break;
                case "enact_hope_decree":
                    WonderDeepSimulationSystem.EnactHopeDecreeStatic();
                    break;
                case "sync_specialist_grid":
                    WonderDeepSimulationSystem.DispatchSpecialistAndSyncGridStatic();
                    break;
                case "boost_vip_commutes":
                    WonderDeepSimulationSystem.BoostVipCitizenCommutesStatic();
                    break;
                case "spawn_street_brawl":
                    SyndicateEnforcerSpawnerSystem.SpawnStreetBrawlWave();
                    break;
            }

            PushStateToCohtmlDom(forceImmediate: true);
        }

        private void PushStateToCohtmlDom(bool forceImmediate)
        {
            try
            {
                var gm = GameManager.instance;
                if (gm == null || gm.state < GameManager.State.UIReady) return;

                var ui = gm.userInterface;
                EnsureStreetBrawlBindingRegistered(ui);
                var uiView = ui?.view;
                View cohtmlView = uiView?.View;
                if (cohtmlView == null) return;

                int cityTreasury = MamaCityControlSystem.ReadLiveCityTreasury();
                var vehicle = HoverDriveVehicle.ActiveInstance;
                var heroActions = WonderHeroActionMechanics.Instance;
                var legendary = WonderLegendaryMechanicsSystem.Instance;
                var cinema = CinemaBuildingAI.ActiveInstance;
                var universe = WonderUniverseIntegrationSystem.Instance;

                int activeRetail = universe != null ? universe.ActiveRetailBusinesses : WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses;
                int boxOfficeYield = universe != null ? universe.BoxOfficeWeeklyYield : WonderUniverseIntegrationSystem.StaticBoxOfficeWeeklyYield;
                float tensionIndex = universe != null ? universe.DistrictTensionIndex : WonderUniverseIntegrationSystem.StaticDistrictTensionIndex;
                bool cyberGrid = universe != null ? universe.CyberGridActive : WonderUniverseIntegrationSystem.StaticCyberGridActive;
                int wonderStars = universe != null ? universe.WonderStarsBanked : WonderUniverseIntegrationSystem.StaticWonderStarsBanked;

                string tensionGlyph = WonderUniverseIntegrationSystem.GetTensionStatusGlyphPath();
                string radioStation = EscapeJs(WonderUniverseIntegrationSystem.StaticActiveRadioStation);
                string lastSting = EscapeJs(WonderUniverseIntegrationSystem.StaticLastAudioStingPlayed);

                bool hoverOn = vehicle != null && vehicle.IsHoverFlightActive;
                float altitude = vehicle != null ? vehicle.CurrentAltitudeMeters : 0f;
                float speedKmh = vehicle != null ? vehicle.CurrentSpeedKmh : 0f;
                float thrustMult = vehicle != null ? vehicle.GetTotalThrustMultiplier() : 1.80f;
                float stability = vehicle != null ? vehicle.GetTotalStabilityScore() : 196f;

                string marquee = cinema != null ? EscapeJs(cinema.ActiveMarqueeHeadline) : "NOW PLAYING: DARRELL X-GEN";
                string portalStage = cinema != null ? EscapeJs(cinema.ActivePortalTargetStage) : "UE5.8 Stage 2: Twin Tower Skybridge";
                string voiceText = EscapeJs(MamaTowerCities2System.LastVoiceBroadcast);
                string receiptText = EscapeJs(WonderCurrencyConverter.LastConversionReceipt);

                string js = BuildCohtmlInjectionScript(
                    ShowHud,
                    cityTreasury,
                    MamaTowerCities2System.Col1GrindBalance,
                    MamaTowerCities2System.Col2OverheadDebt,
                    MamaTowerCities2System.Col3FoundationCapital,
                    MamaTowerCities2System.StakedIndustrialBonds,
                    WonderCurrencyConverter.WonderSuperCurrencyBalance,
                    activeRetail,
                    boxOfficeYield,
                    tensionIndex,
                    cyberGrid,
                    wonderStars,
                    tensionGlyph,
                    radioStation,
                    lastSting,
                    hoverOn,
                    altitude,
                    speedKmh,
                    thrustMult,
                    stability,
                    heroActions != null ? heroActions.BulletTimeEnergyPercent : 100f,
                    heroActions != null && heroActions.IsBulletTimeActive,
                    heroActions != null ? heroActions.currentHealth : 100,
                    heroActions != null ? heroActions.maxHealth : 100,
                    heroActions != null ? heroActions.rallyPotentialHealth : 0,
                    heroActions != null ? heroActions.PlayerPosturePercent : 0f,
                    heroActions != null ? heroActions.ComboCounter : 0,
                    heroActions != null ? heroActions.SonicDiscStatus : "Equipped",
                    heroActions != null && heroActions.CounterWindowOpen,
                    heroActions != null ? heroActions.CounterWindowRemaining : 0f,
                    legendary != null ? legendary.AscendStatus : "UNAVAILABLE",
                    legendary != null ? legendary.GrindStatus : "UNAVAILABLE",
                    legendary != null ? legendary.PortalStatus : "Unavailable",
                    legendary != null ? legendary.EnemyPostureStrain : 0f,
                    legendary != null ? legendary.RallyBuffer : 0,
                    marquee,
                    portalStage,
                    voiceText,
                    receiptText);

                cohtmlView.ExecuteScript(js);
            }
            catch
            {
                // Ignore transient errors while Cohtml view reloads between scenes
            }
        }

        private void EnsureStreetBrawlBindingRegistered(Game.SceneFlow.UserInterface userInterface)
        {
            IBindingRegistry bindings = userInterface?.bindings;
            if (bindings == null || ReferenceEquals(bindings, _commandBindingRegistry)) return;

            if (_commandBindingRegistry != null && _spawnStreetBrawlBinding != null)
            {
                _commandBindingRegistry.RemoveBinding(_spawnStreetBrawlBinding);
            }

            _spawnStreetBrawlBinding = new TriggerBinding(
                "WonderWorldCS2Mod",
                "SpawnStreetBrawlWave",
                SyndicateEnforcerSpawnerSystem.SpawnStreetBrawlWave);
            bindings.AddBinding(_spawnStreetBrawlBinding);
            _commandBindingRegistry = bindings;
        }

        private static string EscapeJs(string input)
        {
            if (string.IsNullOrEmpty(input)) return "";
            return input
                .Replace("\\", "\\\\")
                .Replace("'", "\\'")
                .Replace("\"", "\\\"")
                .Replace("\r", "")
                .Replace("\n", " ");
        }

        private static string BuildCohtmlInjectionScript(
            bool showDeck,
            int cityTreasury,
            float col1Grind,
            float col2Debt,
            float col3Reserve,
            float stakedBonds,
            float wonderW,
            int activeRetail,
            int boxOfficeYield,
            float tensionIndex,
            bool cyberGrid,
            int wonderStars,
            string tensionGlyph,
            string radioStation,
            string lastSting,
            bool hoverOn,
            float altitude,
            float speedKmh,
            float thrustMult,
            float stability,
            float bulletTimeEnergy,
            bool bulletTimeActive,
            int currentHealth,
            int maxHealth,
            int rallyPotentialHealth,
            float playerPosturePercent,
            int comboCounter,
            string sonicDiscStatus,
            bool counterWindowOpen,
            float counterWindowRemaining,
            string ascendStatus,
            string grindStatus,
            string portalStatus,
            float enemyPostureStrain,
            int rallyBuffer,
            string marquee,
            string portalStage,
            string voiceText,
            string receiptText)
        {
            string hoverBadge = hoverOn
                ? string.Format(CultureInfo.InvariantCulture, "✈️ HOVER FLIGHT ON ({0:F1}m | {1:F0} km/h | {2:F2}x)", altitude, speedKmh, thrustMult)
                : "🚗 STREET CRUISER [Press V to Fly]";

            string hoverColor = hoverOn ? "#00ffcc" : "#ffd700";
            string deckDisplay = showDeck ? "block" : "none";
            string tensionColor = tensionIndex > 15.0f ? "#ff5577" : "#00ffb4";
            float healthCapacity = Mathf.Max(1, maxHealth);
            float currentHealthPercent = Mathf.Clamp(currentHealth / healthCapacity * 100f, 0f, 100f);
            float rallyPotentialPercent = Mathf.Clamp(rallyPotentialHealth / healthCapacity * 100f, 0f, 100f - currentHealthPercent);
            float depletedHealthPercent = Mathf.Max(0f, 100f - currentHealthPercent - rallyPotentialPercent);

            var activeMission = CinemaPortalMissionSystem.StaticActiveMission;
            bool inPortal = CinemaPortalMissionSystem.StaticInMissionPortal;
            var missions = CinemaPortalMissionSystem.StaticMissions;

            var sb = new StringBuilder(10240);
            sb.Append("(function(){");
            sb.Append("if(!document.body)return;");
            sb.Append("window.wwSend=function(c){try{if(c==='spawn_street_brawl')engine.trigger('WonderWorldCS2Mod','SpawnStreetBrawlWave');var r=document.getElementById('ww-cs2-hud-root');if(r)r.style.borderColor='#ffd700';}catch(e){}};");
            sb.Append("var root=document.getElementById('ww-cs2-hud-root');");
            sb.Append("if(!root){");
            sb.Append("root=document.createElement('div');root.id='ww-cs2-hud-root';");
            sb.Append("root.style.cssText='position:fixed;top:8px;left:50%;transform:translateX(-50%);z-index:999999;font-family:sans-serif;color:#fff;pointer-events:auto;width:980px;';");
            sb.Append("document.body.appendChild(root);");
            sb.Append("}");

            // Build clean innerHTML using native CS2 SVG/PNG assets
            sb.Append("root.innerHTML='");
            // Top Dual-Currency Bar with Native CS2 Vector Glyphs
            sb.Append("<div style=\"background:rgba(22,6,38,0.94);border:2px solid #b84dff;border-radius:8px;padding:7px 14px;display:flex;justify-content:space-between;align-items:center;box-shadow:0 4px 18px rgba(0,0,0,0.75);font-size:13px;font-weight:bold;\">");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:#00ffb4;display:flex;align-items:center;\"><img src=\"{0}\" style=\"width:18px;height:18px;margin-right:5px;\" />🏛️ City: ${1:N0}</span>",
                WonderUniverseIntegrationSystem.GlyphMoneySvg, cityTreasury);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:#4deeea;display:flex;align-items:center;\"><img src=\"{0}\" style=\"width:18px;height:18px;margin-right:5px;\" />Municipal: ${1:N2}</span>",
                WonderUniverseIntegrationSystem.GlyphMoneySvg, col1Grind);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:#ffd700;font-size:14px;display:flex;align-items:center;\"><img src=\"{0}\" style=\"width:18px;height:18px;margin-right:5px;\" />⭐️W Wonder: {1:N2} ⭐️W ({2:N0})</span>",
                WonderUniverseIntegrationSystem.GlyphStarFilledSvg, wonderW, wonderStars);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:{0};cursor:pointer;\" onclick=\"window.wwSend(\\x27toggle_hover\\x27)\">{1}</span>", hoverColor, hoverBadge);
            sb.Append("<span style=\"background:#8a2be2;color:#fff;padding:4px 10px;border-radius:5px;cursor:pointer;\" onclick=\"window.wwSend(\\x27toggle_hud\\x27)\">⭐️ WONDER COMMAND [F8]</span>");
            sb.Append("</div>");

            // Expandable F8 Command Deck
            sb.AppendFormat("<div style=\"display:{0};margin-top:6px;background:rgba(18,5,32,0.95);border:2px solid #00f0ff;border-radius:10px;padding:12px 16px;box-shadow:0 8px 28px rgba(0,0,0,0.85);font-size:12px;\">", deckDisplay);

            // Section 1: Dual-Currency & Treasury
            sb.Append("<div style=\"color:#ffd700;font-weight:bold;font-size:13px;margin-bottom:6px;border-bottom:1px solid #552288;padding-bottom:4px;\">1. DUAL-CURRENCY LEDGER & MAMA CITY TREASURY CONTROLS (12% BONDS -> ⭐️W)</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:6px;color:#e0d4ff;\">Col 1 Grind: <b>${0:N2}</b> &nbsp;|&nbsp; Col 2 Overhead Debt: <b style=\"color:#ff77aa;\">${1:N2}</b> &nbsp;|&nbsp; Col 3 Reserve: <b>${2:N2}</b> &nbsp;|&nbsp; 12% Bonds Staked: <b>${3:N2}</b></div>",
                col1Grind, col2Debt, col3Reserve, stakedBonds);
            sb.AppendFormat("<div style=\"margin-bottom:8px;color:#88ffea;font-size:11px;\">🧾 {0}</div>", receiptText);

            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:10px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27inject_500k\\x27)\" style=\"background:#009966;padding:6px 10px;border-radius:5px;margin-right:8px;cursor:pointer;font-weight:bold;\">💰 Inject +$500,000 City Treasury [F6]</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27bailout_250k\\x27)\" style=\"background:#7b2cbf;padding:6px 10px;border-radius:5px;margin-right:8px;cursor:pointer;font-weight:bold;\">💜 Bailout +$250k & Zero Debt [F7]</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27mint_wonder_w\\x27)\" style=\"background:#d49b00;color:#111;padding:6px 10px;border-radius:5px;margin-right:8px;cursor:pointer;font-weight:bold;\">⚡ 12% Bond Yield -> Mint ⭐️W [F9]</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27wetvac_shift\\x27)\" style=\"background:#1b6ca8;padding:6px 10px;border-radius:5px;cursor:pointer;font-weight:bold;\">🌊 Wet-Vac Shift (+$185)</div>");
            sb.Append("</div>");

            // Section 2: 6-Game Wonder Universe ECS Integration + Native Glyphs, Signature Buildings & Drome Cinema Audio Sting
            sb.Append("<div style=\"color:#ff9900;font-weight:bold;font-size:13px;margin-bottom:6px;border-bottom:1px solid #552288;padding-bottom:4px;\">2. MULTI-GAME WONDER UNIVERSE INTEGRATION & NATIVE CS2 ASSET ANCHORS</div>");

            sb.Append("<div style=\"display:flex;justify-content:space-between;align-items:center;margin-bottom:8px;\">");
            sb.Append("<div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:4px;color:#e0d4ff;display:flex;align-items:center;\"><img src=\"{0}\" style=\"width:16px;height:16px;margin-right:5px;\" />Retail Businesses Active (Big Ambitions): &nbsp;<b style=\"color:#00ffb4;\">{1}</b> &nbsp;|&nbsp; 🎬 Weekly Box Office Yield (Movies Tycoon): &nbsp;<b style=\"color:#ffd700;\">${2:N0}</b></div>",
                WonderUniverseIntegrationSystem.GlyphMoneySvg, activeRetail, boxOfficeYield);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:4px;color:#e0d4ff;display:flex;align-items:center;\"><img src=\"{0}\" style=\"width:16px;height:16px;margin-right:5px;\" />District Tension Meter (Fighting Souls): &nbsp;<b style=\"color:{1};\">{2:F1}%</b> &nbsp;|&nbsp; 🤖 Pragmata Cyber Grid: &nbsp;<b style=\"color:#00f0ff;\">{3}</b> &nbsp;|&nbsp; <img src=\"{4}\" style=\"width:15px;height:15px;margin-right:3px;\" /><b>{5:N0} ⭐️W</b></div>",
                tensionGlyph, tensionColor, tensionIndex, cyberGrid ? "ONLINE (+5 ⭐️W)" : "STANDBY", WonderUniverseIntegrationSystem.GlyphStarFilledSvg, wonderStars);
            sb.AppendFormat("<div style=\"color:#b8c0ff;font-size:11px;\">📻 85 BPM Radio: <b style=\"color:#00ffcc;\">{0}</b> &nbsp;|&nbsp; 🔊 Audio Sting: <b style=\"color:#ffd700;\">{1}</b></div>",
                radioStation, lastSting);
            sb.Append("</div>");

            // Signature Building Anchors (Papa AI Tower + Mama AI Biodome)
            sb.Append("<div style=\"display:flex;align-items:center;\">");
            sb.AppendFormat("<div style=\"text-align:center;margin-right:10px;background:rgba(255,255,255,0.06);padding:4px 6px;border-radius:6px;border:1px solid #ffd700;\"><img src=\"{0}\" style=\"width:42px;height:42px;display:block;margin:0 auto;\" /><span style=\"font-size:9px;color:#ffd700;\">Papa Tower HQ</span></div>",
                WonderUniverseIntegrationSystem.PapaTowerSignatureImg);
            sb.AppendFormat("<div style=\"text-align:center;background:rgba(255,255,255,0.06);padding:4px 6px;border-radius:6px;border:1px solid #00ffb4;\"><img src=\"{0}\" style=\"width:42px;height:42px;display:block;margin:0 auto;\" /><span style=\"font-size:9px;color:#00ffb4;\">Mama Biodome HQ</span></div>",
                WonderUniverseIntegrationSystem.MamaBiodomeSignatureImg);
            sb.Append("</div>");
            sb.Append("</div>");

            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:10px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27host_tokon_showdown\\x27)\" style=\"background:#d90429;color:#fff;padding:6px 12px;border-radius:5px;margin-right:10px;cursor:pointer;font-weight:bold;\">🏆 Host Tokon Showdown</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27premiere_wonder_movie\\x27)\" style=\"background:#f77f00;color:#111;padding:6px 12px;border-radius:5px;margin-right:10px;cursor:pointer;font-weight:bold;\">🎬 Premiere Wonder Movie (Drome Cinema Sting)</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27cycle_radio_preset\\x27)\" style=\"background:#4361ee;color:#fff;padding:6px 12px;border-radius:5px;cursor:pointer;font-weight:bold;\">📻 Switch 85 BPM Radio (Smooth Beat / Second Moon)</div>");
            sb.Append("</div>");

            // Section 3: Modular Hover-Drive Vehicle ([V] Key)
            sb.Append("<div style=\"color:#00ffcc;font-weight:bold;font-size:13px;margin-bottom:6px;border-bottom:1px solid #552288;padding-bottom:4px;\">3. MODULAR HOVER-DRIVE VEHICLE ([V] FLIGHT TOGGLE | WASD + SPACE/C + SHIFT BOOST)</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:6px;color:#e0d4ff;\">Mode: <b style=\"color:{0};\">{1}</b> &nbsp;|&nbsp; Altitude: <b>{2:F1}m</b> &nbsp;|&nbsp; Speed: <b>{3:F0} km/h</b> &nbsp;|&nbsp; Thrust: <b>{4:F2}x</b> &nbsp;|&nbsp; Stability: <b>{5:F0}</b></div>",
                hoverColor, hoverOn ? "ENGAGED (85 BPM ANTI-GRAV)" : "GROUNDED CRUISER", altitude, speedKmh, thrustMult, stability);

            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:10px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27toggle_hover\\x27)\" style=\"background:#008899;padding:5px 10px;border-radius:5px;margin-right:6px;cursor:pointer;font-weight:bold;\">✈️ Toggle Hover Flight [V]</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27part_1\\x27)\" style=\"background:#3c1361;border:1px solid #b84dff;padding:5px 8px;border-radius:5px;margin-right:6px;cursor:pointer;\">🔧 [Alt+1] Sub-Bass Pods</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27part_2\\x27)\" style=\"background:#3c1361;border:1px solid #b84dff;padding:5px 8px;border-radius:5px;margin-right:6px;cursor:pointer;\">🔧 [Alt+2] Sonic Grille</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27part_3\\x27)\" style=\"background:#3c1361;border:1px solid #b84dff;padding:5px 8px;border-radius:5px;margin-right:6px;cursor:pointer;\">🔧 [Alt+3] Rainbow Wing</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27part_4\\x27)\" style=\"background:#3c1361;border:1px solid #b84dff;padding:5px 8px;border-radius:5px;cursor:pointer;\">🔧 [Alt+4] Top-Hat Turret</div>");
            sb.Append("</div>");

            // Section 4: Hero action mechanics
            sb.Append("<div style=\"color:#00f5d4;font-weight:bold;font-size:13px;margin:8px 0 5px;border-bottom:1px solid #552288;padding-bottom:4px;\">4. HERO COMBAT ABILITIES</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"display:flex;align-items:center;gap:8px;margin-bottom:5px;color:#e0d4ff;\">Bullet Time {0}: <div style=\"width:180px;height:12px;background:#241b2f;border:1px solid #00f5d4;border-radius:6px;overflow:hidden;\"><div style=\"width:{1:F0}%;height:100%;background:#00d9ff;\"></div></div><b>{1:F0}%</b></div>",
                bulletTimeActive ? "ACTIVE" : "[Left Shift]",
                Mathf.Clamp(bulletTimeEnergy, 0f, 100f));
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:3px;color:#e0d4ff;\">Bio-Rally Health: <b style=\"color:#35e58a;\">{0}/{1} HP</b> &nbsp;|&nbsp; Rally Potential: <b style=\"color:#ff9f43;\">{2} HP</b></div>",
                Mathf.Clamp(currentHealth, 0, maxHealth), maxHealth, Mathf.Clamp(rallyPotentialHealth, 0, maxHealth));
            sb.Append("<div style=\"display:flex;width:250px;height:12px;margin-bottom:4px;background:#e5484d;border:1px solid #5b2630;border-radius:5px;overflow:hidden;\">");
            sb.AppendFormat(CultureInfo.InvariantCulture, "<div style=\"width:{0:F1}%;height:100%;background:#35d07f;\"></div>", currentHealthPercent);
            sb.AppendFormat(CultureInfo.InvariantCulture, "<div style=\"width:{0:F1}%;height:100%;background:#ff9f43;\"></div>", rallyPotentialPercent);
            sb.AppendFormat(CultureInfo.InvariantCulture, "<div style=\"width:{0:F1}%;height:100%;background:#e5484d;\"></div>", depletedHealthPercent);
            sb.Append("</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:5px;color:#b7a8c9;font-size:10px;\"><span style=\"color:#35d07f;\">■ Current HP</span> &nbsp; <span style=\"color:#ff9f43;\">■ Rally</span> &nbsp; <span style=\"color:#e5484d;\">■ Depleted</span> &nbsp;|&nbsp; Harmonic Strain / Posture: <b style=\"color:#00f5d4;\">{0:F0}%</b></div>",
                Mathf.Clamp(playerPosturePercent, 0f, 100f));
            sb.Append("<div style=\"color:#ffd700;font-weight:bold;font-size:12px;margin:7px 0 4px;border-bottom:1px solid #552288;padding-bottom:3px;\">LEGENDARY 5-PILLAR MECHANICS</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:5px;color:#e0d4ff;\">Ascend: <b style=\"color:#00f5d4;\">{0}</b> &nbsp;|&nbsp; Neon Rail Grind: <b style=\"color:#ff66d9;\">{1}</b></div>",
                ascendStatus, grindStatus);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:5px;color:#e0d4ff;\">Active Portals: <b style=\"color:#00d9ff;\">{0}</b> &nbsp;|&nbsp; Enemy Posture Strain: <b style=\"color:#ff9f43;\">{1:F0}%</b> &nbsp;|&nbsp; Rally Buffer: <b style=\"color:#ff9f43;\">{2} HP</b></div>",
                portalStatus, Mathf.Clamp(enemyPostureStrain, 0f, 100f), Mathf.Max(0, rallyBuffer));
            sb.Append("<div style=\"margin-bottom:6px;color:#b7a8c9;font-size:10px;\">C: Ascend | G: Ultra-Fuse | Q/E: Place portals | J: Frequency Codex | Space: Rail Surge</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:5px;color:#e0d4ff;\">Freeflow: <b style=\"color:#ffd700;\">Combo ×{0}</b> &nbsp;|&nbsp; Sonic Disc: <b style=\"color:#00f5d4;\">{1}</b> &nbsp;|&nbsp; Counter: <b style=\"color:{2};\">{3}</b></div>",
                comboCounter,
                sonicDiscStatus,
                counterWindowOpen ? "#00ffb4" : "#b7a8c9",
                counterWindowOpen ? string.Format(CultureInfo.InvariantCulture, "OPEN ({0:F2}s) — Right Click", counterWindowRemaining) : "Waiting for attack cue");
            int activeEnforcerCount = SyndicateEnforcerSpawnerSystem.Instance != null
                ? SyndicateEnforcerSpawnerSystem.Instance.ActiveEnforcerCount
                : 0;
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:6px;color:#ff8c9b;\">Active Syndicate Thugs: <b>{0}</b></div>",
                activeEnforcerCount);
            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:8px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27spawn_street_brawl\\x27)\" style=\"background:#b91c3c;color:#fff;padding:5px 9px;border-radius:5px;margin-right:6px;cursor:pointer;font-weight:bold;\">🚨 Spawn Street Brawl Wave</div>");
            sb.Append("</div>");
            sb.Append("<div style=\"margin-bottom:8px;color:#b7a8c9;\">Shift: Bullet Time | Right-click: Counter during an attack cue | Left+Right-click: Throw Disc | R: Recall</div>");

            // Section 5: Cinema Story Missions
            sb.Append("<div style=\"color:#ff77ff;font-weight:bold;font-size:13px;margin-bottom:5px;border-bottom:1px solid #552288;padding-bottom:4px;\">5. CINEMA STORY MISSIONS (REEL MEMORY PORTALS & TOP HATTER BACKSTORIES)</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:4px;color:#fff;font-weight:bold;\">Current Cinema Marquee: <span style=\"color:#ffd700;\">\"{0}\"</span> — Featured Top Hatter: <span style=\"color:#00ffcc;\">{1}</span> ({2})</div>",
                EscapeJs(activeMission.MovieTitle), EscapeJs(activeMission.HeroName), EscapeJs(activeMission.District));
            sb.AppendFormat("<div style=\"margin-bottom:6px;color:#aaddff;font-size:11px;\">{0} &nbsp;|&nbsp; 🌀 Portal Stage: {1}</div>",
                marquee, portalStage);

            // Mission Status List (Completed or Ready to Enter)
            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:8px;\">");
            for (int i = 0; i < missions.Length; i++)
            {
                var m = missions[i];
                string statusLabel = m.IsCompleted ? "✅ Completed" : "🟢 Ready to Enter";
                string bg = m.IsCompleted ? "#1b4332" : "#3c096c";
                string border = (activeMission.MissionId == m.MissionId && inPortal) ? "2px solid #ffd700" : "1px solid #9d4edd";
                string cmdId = string.Format(CultureInfo.InvariantCulture, "portal_0{0}", i + 1);
                sb.AppendFormat(CultureInfo.InvariantCulture,
                    "<div onclick=\"window.wwSend(\\x27{0}\\x27)\" style=\"background:{1};border:{2};padding:4px 8px;border-radius:4px;margin-right:6px;margin-bottom:4px;cursor:pointer;font-size:11px;\"><b>{3}</b>: {4} ({5}) — <span style=\"color:#ffd700;\">{6}</span></div>",
                    cmdId, bg, border, EscapeJs(m.MissionId), EscapeJs(m.MovieTitle), EscapeJs(m.HeroName), statusLabel);
            }
            sb.Append("</div>");

            // Interactive Cinema Portal Flashback [E] + Top Hatter Marquee Selector Buttons
            sb.Append("<div style=\"display:flex;flex-wrap:wrap;align-items:center;margin-bottom:8px;\">");
            string portalBtnText = inPortal
                ? string.Format(CultureInfo.InvariantCulture, "⭐️ Complete Flashback: {0} (+{1} ⭐️W & +${2:N0}) [Ctrl+E]", EscapeJs(activeMission.MovieTitle), activeMission.WonderStarReward, activeMission.TreasuryBonus)
                : "🎬 Enter Cinema Portal Flashback [Ctrl+E]";
            string portalBtnBg = inPortal ? "#00b4d8" : "#e63946";
            sb.AppendFormat("<div onclick=\"window.wwSend(\\x27enter_cinema_portal\\x27)\" style=\"background:{0};color:#fff;padding:6px 14px;border-radius:5px;margin-right:10px;cursor:pointer;font-weight:bold;\">{1}</div>",
                portalBtnBg, portalBtnText);
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_0\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:5px;cursor:pointer;\">🎩 Darrell X-Gen</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_1\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:5px;cursor:pointer;\">🕷️ Spider-Pulse</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_2\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:5px;cursor:pointer;\">✨ Divine Hands</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_3\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:5px;cursor:pointer;\">🎸 Jaxson Thorne</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_4\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;cursor:pointer;\">🌌 Vesper Vance</div>");
            sb.Append("</div>");

            // Section 6: 5 Deep Simulation Pillars
            sb.Append("<div style=\"color:#00f5d4;font-weight:bold;font-size:13px;margin-bottom:5px;border-bottom:1px solid #552288;padding-bottom:4px;\">6. DEEP SIMULATION PILLARS (SUPPLY CHAIN | 59K-GAL HYDRO DOME | HOPE DECREES | SPECIALIST GRID | NAMED VIPs)</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:4px;color:#e0d4ff;font-size:11px;\">📦 <b>4-Tier Supply Chain:</b> Alloy: <b>{0}t</b> &rarr; Spores: <b>{1}</b> &rarr; Sub-Bass Coils: <b>{2}</b> &rarr; Storefront Stock: <b style=\"color:#00ffb4;\">{3} Crates</b> ({4} Drones/Trucks)</div>",
                WonderDeepSimulationSystem.RawAcousticAlloyTons,
                WonderDeepSimulationSystem.BioLuminescentSporeCrates,
                WonderDeepSimulationSystem.ForgedSubBassCoils,
                WonderDeepSimulationSystem.StorefrontInventoryCrates,
                WonderDeepSimulationSystem.ActiveCargoDronesAndTrucks);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:4px;color:#e0d4ff;font-size:11px;\">🌊 <b>59,000-Gal Hydro Bio-Dome:</b> Purified Coolant: <b style=\"color:#00f0ff;\">{0:N0} / 59,000 gal</b> (Runoff: {1:N0} gal) &nbsp;|&nbsp; ⚡ Grid: <b style=\"color:#ffd700;\">{2:F0}% 85-BPM Sync ({3} MW)</b> &nbsp;|&nbsp; 🎩 Specialists: <b>{4} Crowned</b></div>",
                WonderDeepSimulationSystem.PurifiedBioCoolantGallons,
                WonderDeepSimulationSystem.CanalRunoffGallons,
                WonderDeepSimulationSystem.Grid85BpmSyncPercent,
                WonderDeepSimulationSystem.MegawattsGeneratedByBioCoolant,
                WonderDeepSimulationSystem.CrownedSpecialistsActive);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:6px;color:#e0d4ff;font-size:11px;\">📜 <b>Frostpunk Hope & Decrees:</b> City Hope: <b style=\"color:#00ffb4;\">{0:F1}%</b> &nbsp;|&nbsp; Status: <b style=\"color:#ffd700;\">{1}</b> &nbsp;|&nbsp; Active Decree: <i>{2}</i></div>",
                WonderDeepSimulationSystem.CityHopeIndex,
                EscapeJs(WonderDeepSimulationSystem.MoraleCrisisState),
                EscapeJs(WonderDeepSimulationSystem.ActiveMayorDecree));

            // Named VIP Citizens Strip
            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:6px;\">");
            var vips = WonderDeepSimulationSystem.StaticVipRoster;
            for (int i = 0; i < vips.Length; i++)
            {
                var v = vips[i];
                sb.AppendFormat(CultureInfo.InvariantCulture,
                    "<div style=\"background:rgba(255,255,255,0.06);border:1px solid #00f5d4;padding:3px 7px;border-radius:4px;margin-right:5px;margin-bottom:4px;font-size:10px;\">👑 <b>{0}</b> ({1}) &bull; <span style=\"color:#00ffcc;\">{2}</span> &bull; <span style=\"color:#ffd700;\">+{3:F0}%</span></div>",
                    EscapeJs(v.FullName), EscapeJs(v.AssignedFacility), EscapeJs(v.CommuteStatus), v.ProductivityBonusPercent);
            }
            sb.Append("</div>");

            // Interactive Deep Simulation Action Buttons
            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:8px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27airlift_cargo\\x27)\" style=\"background:#2a9d8f;color:#fff;padding:5px 9px;border-radius:5px;margin-right:6px;cursor:pointer;font-weight:bold;font-size:11px;\">📦 Hover-Cargo Airlift (+250 Crates & +75 ⭐️W)</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27toggle_floodgates\\x27)\" style=\"background:#0077b6;color:#fff;padding:5px 9px;border-radius:5px;margin-right:6px;cursor:pointer;font-weight:bold;font-size:11px;\">🌊 Purify 59k-Gal Bio-Dome (+60 ⭐️W)</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27enact_hope_decree\\x27)\" style=\"background:#e76f51;color:#fff;padding:5px 9px;border-radius:5px;margin-right:6px;cursor:pointer;font-weight:bold;font-size:11px;\">📜 Enact Mayor Hope Decree (+85 ⭐️W)</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27sync_specialist_grid\\x27)\" style=\"background:#8338ec;color:#fff;padding:5px 9px;border-radius:5px;margin-right:6px;cursor:pointer;font-weight:bold;font-size:11px;\">🎩 Awaken +8 Specialists & Sync Grid (+90 ⭐️W)</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27boost_vip_commutes\\x27)\" style=\"background:#fb8500;color:#111;padding:5px 9px;border-radius:5px;cursor:pointer;font-weight:bold;font-size:11px;\">🚆 Boost Named VIP Commutes (+100 ⭐️W)</div>");
            sb.Append("</div>");

            // Voice Broadcast Footer
            sb.AppendFormat("<div style=\"background:rgba(0,0,0,0.55);padding:6px 10px;border-radius:5px;color:#ffd700;font-size:12px;\">🎙️ {0}</div>", voiceText);

            sb.Append("</div>';");
            sb.Append("})();");
            return sb.ToString();
        }

        private void OnGUI()
        {
            // Fallback IMGUI for non-Cohtml environments
            GUI.color = new Color(0.08f, 0.92f, 1.0f, 0.96f);
            if (GUI.Button(new Rect(16f, 14f, 520f, 32f),
                $"🎩 PURPLE CITY [F8] | Municipal: ${MamaTowerCities2System.Col1GrindBalance:N0} | ⭐️W: {WonderCurrencyConverter.WonderSuperCurrencyBalance:F2}"))
            {
                ShowHud = !ShowHud;
            }
            GUI.color = Color.white;

            if (!ShowHud) return;

            var universe = WonderUniverseIntegrationSystem.Instance;
            int activeRetail = universe != null ? universe.ActiveRetailBusinesses : WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses;
            int boxOfficeYield = universe != null ? universe.BoxOfficeWeeklyYield : WonderUniverseIntegrationSystem.StaticBoxOfficeWeeklyYield;
            float tensionIndex = universe != null ? universe.DistrictTensionIndex : WonderUniverseIntegrationSystem.StaticDistrictTensionIndex;
            var activeMission = CinemaPortalMissionSystem.StaticActiveMission;
            var heroActions = WonderHeroActionMechanics.Instance;
            var legendary = WonderLegendaryMechanicsSystem.Instance;
            var vehicle = HoverDriveVehicle.ActiveInstance;

            GUILayout.BeginArea(_fallbackWindowRect, GUI.skin.box);
            GUILayout.Label("🎩 MAMA'S WONDER WORLD // F8 WONDER COMMAND HUD");
            GUILayout.Space(4f);
            GUILayout.Label($"Retail Businesses Active (Big Ambitions): {activeRetail}");
            GUILayout.Label($"Weekly Box Office Yield (Movies Tycoon): ${boxOfficeYield:N0}");
            GUILayout.Label($"District Tension Meter (Fighting Souls): {tensionIndex:F1}%");
            GUILayout.BeginHorizontal();
            if (GUILayout.Button("Host Tokon Showdown", GUILayout.Height(28f)))
            {
                ExecuteWonderAction("host_tokon_showdown");
            }
            if (GUILayout.Button("Premiere Wonder Movie", GUILayout.Height(28f)))
            {
                ExecuteWonderAction("premiere_wonder_movie");
            }
            GUILayout.EndHorizontal();

            GUILayout.Space(6f);
            GUILayout.Label("🎬 CINEMA STORY MISSIONS");
            GUILayout.Label($"Current Cinema Marquee: \"{activeMission.MovieTitle}\" — Featured Top Hatter: {activeMission.HeroName}");
            foreach (var m in CinemaPortalMissionSystem.StaticMissions)
            {
                string status = m.IsCompleted ? "Completed" : "Ready to Enter";
                GUILayout.Label($"• [{m.MissionId}] {m.MovieTitle} ({m.HeroName}) — {status}");
            }
            GUILayout.Space(6f);
            GUILayout.Label("HERO COMBAT ABILITIES");
            float bulletEnergy = heroActions != null ? heroActions.BulletTimeEnergyPercent : 100f;
            GUILayout.Label($"Bullet Time Energy: {bulletEnergy:F0}% {(heroActions != null && heroActions.IsBulletTimeActive ? "(ACTIVE)" : "(Left Shift)")}");
            Rect energyBar = GUILayoutUtility.GetRect(250f, 14f);
            GUI.color = new Color(0.14f, 0.1f, 0.18f, 1f);
            GUI.DrawTexture(energyBar, Texture2D.whiteTexture);
            energyBar.width *= bulletEnergy / 100f;
            GUI.color = new Color(0f, 0.85f, 1f, 1f);
            GUI.DrawTexture(energyBar, Texture2D.whiteTexture);
            GUI.color = Color.white;
            GUILayout.Label($"Freeflow: Combo ×{(heroActions != null ? heroActions.ComboCounter : 0)} | Sonic Disc: {(heroActions != null ? heroActions.SonicDiscStatus : "Equipped")}");
            int maxHealth = heroActions != null ? Mathf.Max(1, heroActions.maxHealth) : 100;
            int currentHealth = heroActions != null ? Mathf.Clamp(heroActions.currentHealth, 0, maxHealth) : maxHealth;
            int rallyPotentialHealth = heroActions != null
                ? Mathf.Clamp(heroActions.rallyPotentialHealth, 0, maxHealth - currentHealth)
                : 0;
            GUILayout.Label($"Bio-Rally Health: {currentHealth}/{maxHealth} HP | Rally Potential: {rallyPotentialHealth} HP");
            Rect healthBar = GUILayoutUtility.GetRect(250f, 14f);
            float currentHealthWidth = healthBar.width * currentHealth / maxHealth;
            float rallyPotentialWidth = healthBar.width * rallyPotentialHealth / maxHealth;
            GUI.color = new Color(0.9f, 0.28f, 0.3f, 1f);
            GUI.DrawTexture(healthBar, Texture2D.whiteTexture);
            GUI.color = new Color(0.2f, 0.82f, 0.5f, 1f);
            GUI.DrawTexture(new Rect(healthBar.x, healthBar.y, currentHealthWidth, healthBar.height), Texture2D.whiteTexture);
            GUI.color = new Color(1f, 0.62f, 0.26f, 1f);
            GUI.DrawTexture(new Rect(healthBar.x + currentHealthWidth, healthBar.y, rallyPotentialWidth, healthBar.height), Texture2D.whiteTexture);
            GUI.color = Color.white;
            GUILayout.Label($"Harmonic Strain / Posture: {(heroActions != null ? heroActions.PlayerPosturePercent : 0f):F0}%");
            GUILayout.Label(heroActions != null && heroActions.CounterWindowOpen
                ? $"Counter window OPEN ({heroActions.CounterWindowRemaining:F2}s) — Right Click"
                : "Counter window: Waiting for attack cue");
            var enforcerSpawner = SyndicateEnforcerSpawnerSystem.Instance;
            GUILayout.Label($"Active Syndicate Thugs: {(enforcerSpawner != null ? enforcerSpawner.ActiveEnforcerCount : 0)}");
            if (GUILayout.Button("Spawn Street Brawl Wave", GUILayout.Height(28f)))
            {
                SyndicateEnforcerSpawnerSystem.SpawnStreetBrawlWave();
            }
            GUILayout.Space(4f);
            GUILayout.Label("LEGENDARY 5-PILLAR MECHANICS");
            GUILayout.Label($"Ascend: {(legendary != null ? legendary.AscendStatus : "UNAVAILABLE")} | Neon Rail Grind: {(legendary != null ? legendary.GrindStatus : "UNAVAILABLE")}");
            GUILayout.Label($"Active Portals: {(legendary != null ? legendary.PortalStatus : "Unavailable")}");
            GUILayout.Label($"Enemy Posture Strain: {(legendary != null ? legendary.EnemyPostureStrain : 0f):F0}% | Rally Buffer: {(legendary != null ? legendary.RallyBuffer : 0)} HP");
            GUILayout.Label("C: Ascend | G: Ultra-Fuse | Q/E: Portals | J: Frequency Codex | Space: Rail Surge");
            if (GUILayout.Button(
                vehicle != null && vehicle.IsCloaked ? "Uncloak Hover-Cruiser [Alt+5]" : "Cloak Hover-Cruiser [Alt+5]",
                GUILayout.Height(26f)))
            {
                ExecuteWonderAction("toggle_cloak");
            }
            if (GUILayout.Button("Enter Cinema Portal Flashback [Ctrl+E]", GUILayout.Height(30f)))
            {
                ExecuteWonderAction("enter_cinema_portal");
            }

            GUILayout.Space(6f);
            GUILayout.Label($"⚙️ 5-PILLAR DEEP SIM: Stock {WonderDeepSimulationSystem.StorefrontInventoryCrates} Crates | Bio-Dome {WonderDeepSimulationSystem.PurifiedBioCoolantGallons:N0}/59k gal | Hope {WonderDeepSimulationSystem.CityHopeIndex:F1}% | Specialists {WonderDeepSimulationSystem.CrownedSpecialistsActive}");
            GUILayout.BeginHorizontal();
            if (GUILayout.Button("Cargo Airlift", GUILayout.Height(26f))) ExecuteWonderAction("airlift_cargo");
            if (GUILayout.Button("Purify 59k Bio-Dome", GUILayout.Height(26f))) ExecuteWonderAction("toggle_floodgates");
            if (GUILayout.Button("Hope Decree", GUILayout.Height(26f))) ExecuteWonderAction("enact_hope_decree");
            if (GUILayout.Button("Sync Grid", GUILayout.Height(26f))) ExecuteWonderAction("sync_specialist_grid");
            if (GUILayout.Button("Boost VIPs", GUILayout.Height(26f))) ExecuteWonderAction("boost_vip_commutes");
            GUILayout.EndHorizontal();
            GUILayout.EndArea();
        }
    }
}
