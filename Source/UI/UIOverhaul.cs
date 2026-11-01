using System;
using System.Globalization;
using System.Text;
using cohtml.Net;
using Game.SceneFlow;
using UnityEngine;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.Vehicles;
using WonderWorldCS2.Systems;

namespace WonderWorldCS2.UI
{
    /// <summary>
    /// Cities: Skylines II Native Cohtml HTML5 UI Overhaul + IMGUI Fallback.
    /// Injects the Purple City Dual-Currency HUD (Municipal $ + City Treasury $ + Wonder ⭐️W),
    /// Multi-Game Wonder Universe Integration (Big Ambitions, Movies Tycoon, Fatal Fury / Marvel Tokon, Pragmata),
    /// Modular Hover-Drive [V] Flight Cockpit, Cinema Portal Marquee, and Mama's City Treasury Controls
    /// directly into GameManager.instance.userInterface.view.View so it renders natively in CS2 HDRP.
    /// </summary>
    public sealed class UIOverhaul : MonoBehaviour
    {
        public static UIOverhaul Instance { get; private set; }

        public bool ShowHud = true;
        private int _lastTickedFrame = -1;
        private float _cohtmlSyncTimer = 0f;
        private View _boundView = null;
        private BoundEventHandle _boundCallHandle;
        private BoundEventHandle _boundEventHandle;
        private Rect _fallbackWindowRect = new Rect(24f, 52f, 680f, 680f);

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

            // [F9] Trigger 12% Bond Yield -> Wonder ⭐️W Super-Currency Conversion
            if (Input.GetKeyDown(KeyCode.F9))
            {
                ExecuteWonderAction("mint_wonder_w");
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
                case "toggle_hover":
                    vehicle?.ToggleHoverFlight();
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
            }

            PushStateToCohtmlDom(forceImmediate: true);
        }

        private void PushStateToCohtmlDom(bool forceImmediate)
        {
            try
            {
                var gm = GameManager.instance;
                var uiView = gm?.userInterface?.view;
                View cohtmlView = uiView?.View;
                if (cohtmlView == null) return;

                if (!ReferenceEquals(_boundView, cohtmlView))
                {
                    _boundView = cohtmlView;
                    try
                    {
                        _boundCallHandle = _boundView.BindCall("wonder_cmd", new Action<string>(ExecuteWonderAction));
                        _boundEventHandle = _boundView.RegisterForEvent("wonder_cmd", new Action<string>(ExecuteWonderAction));
                        WonderWorldCS2Plugin.ModLogger?.LogInfo("[UIOverhaul] Bound native Cohtml HTML5 UI callbacks (wonder_cmd).");
                    }
                    catch (Exception bindEx)
                    {
                        WonderWorldCS2Plugin.ModLogger?.LogWarning($"[UIOverhaul] Cohtml bind notice: {bindEx.Message}");
                    }
                }

                int cityTreasury = MamaCityControlSystem.ReadLiveCityTreasury();
                var vehicle = HoverDriveVehicle.ActiveInstance;
                var cinema = CinemaBuildingAI.ActiveInstance;
                var universe = WonderUniverseIntegrationSystem.Instance;

                int activeRetail = universe != null ? universe.ActiveRetailBusinesses : WonderUniverseIntegrationSystem.StaticActiveRetailBusinesses;
                int boxOfficeYield = universe != null ? universe.BoxOfficeWeeklyYield : WonderUniverseIntegrationSystem.StaticBoxOfficeWeeklyYield;
                float tensionIndex = universe != null ? universe.DistrictTensionIndex : WonderUniverseIntegrationSystem.StaticDistrictTensionIndex;
                bool cyberGrid = universe != null ? universe.CyberGridActive : WonderUniverseIntegrationSystem.StaticCyberGridActive;
                int wonderStars = universe != null ? universe.WonderStarsBanked : WonderUniverseIntegrationSystem.StaticWonderStarsBanked;

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
                    hoverOn,
                    altitude,
                    speedKmh,
                    thrustMult,
                    stability,
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
            bool hoverOn,
            float altitude,
            float speedKmh,
            float thrustMult,
            float stability,
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

            var sb = new StringBuilder(6144);
            sb.Append("(function(){");
            sb.Append("if(!document.body)return;");
            sb.Append("window.wwSend=function(c){try{if(window.engine&&engine.call)engine.call('wonder_cmd',c);else if(window.engine&&engine.trigger)engine.trigger('wonder_cmd',c);}catch(e){}};");
            sb.Append("var root=document.getElementById('ww-cs2-hud-root');");
            sb.Append("if(!root){");
            sb.Append("root=document.createElement('div');root.id='ww-cs2-hud-root';");
            sb.Append("root.style.cssText='position:fixed;top:8px;left:50%;transform:translateX(-50%);z-index:999999;font-family:sans-serif;color:#fff;pointer-events:auto;width:940px;';");
            sb.Append("document.body.appendChild(root);");
            sb.Append("}");

            // Build clean innerHTML
            sb.Append("root.innerHTML='");
            // Top Dual-Currency Bar
            sb.Append("<div style=\"background:rgba(22,6,38,0.94);border:2px solid #b84dff;border-radius:8px;padding:7px 14px;display:flex;justify-content:space-between;align-items:center;box-shadow:0 4px 18px rgba(0,0,0,0.75);font-size:13px;font-weight:bold;\">");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:#00ffb4;\">🏛️ City: ${0:N0}</span>", cityTreasury);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:#4deeea;\">💵 Municipal: ${0:N2}</span>", col1Grind);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:#ffd700;font-size:14px;\">⭐️W Wonder: {0:N2} ⭐️W ({1:N0} Banked)</span>", wonderW, wonderStars);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<span style=\"color:{0};cursor:pointer;\" onclick=\"window.wwSend(\\x27toggle_hover\\x27)\">{1}</span>", hoverColor, hoverBadge);
            sb.Append("<span style=\"background:#8a2be2;color:#fff;padding:4px 10px;border-radius:5px;cursor:pointer;\" onclick=\"window.wwSend(\\x27toggle_hud\\x27)\">🎩 WONDER HUD [F8]</span>");
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

            // Section 2: 6-Game Wonder Universe ECS Integration
            sb.Append("<div style=\"color:#ff9900;font-weight:bold;font-size:13px;margin-bottom:6px;border-bottom:1px solid #552288;padding-bottom:4px;\">2. MULTI-GAME WONDER UNIVERSE INTEGRATION (BIG AMBITIONS | MOVIES TYCOON | FIGHTING SOULS | PRAGMATA)</div>");
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:4px;color:#e0d4ff;\">🏪 Retail Businesses Active (Big Ambitions): <b style=\"color:#00ffb4;\">{0}</b> &nbsp;|&nbsp; 🎬 Weekly Box Office Yield (Movies Tycoon): <b style=\"color:#ffd700;\">${1:N0}</b></div>",
                activeRetail, boxOfficeYield);
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "<div style=\"margin-bottom:8px;color:#e0d4ff;\">🥊 District Tension Meter (Fighting Souls): <b style=\"color:#ff5577;\">{0:F1}%</b> &nbsp;|&nbsp; 🤖 Pragmata Cyber Grid: <b style=\"color:#00f0ff;\">{1}</b> &nbsp;|&nbsp; ⭐️W Banked: <b>{2:N0}</b></div>",
                tensionIndex, cyberGrid ? "ONLINE (+5 ⭐️W/tick)" : "STANDBY", wonderStars);

            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:10px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27host_tokon_showdown\\x27)\" style=\"background:#d90429;color:#fff;padding:6px 12px;border-radius:5px;margin-right:10px;cursor:pointer;font-weight:bold;\">🏆 Host Tokon Showdown</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27premiere_wonder_movie\\x27)\" style=\"background:#f77f00;color:#111;padding:6px 12px;border-radius:5px;cursor:pointer;font-weight:bold;\">🎬 Premiere Wonder Movie</div>");
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

            // Section 4: Cinema Portal Marquee (5 Top Hatters)
            sb.Append("<div style=\"color:#ff77ff;font-weight:bold;font-size:13px;margin-bottom:5px;border-bottom:1px solid #552288;padding-bottom:4px;\">4. WONDER CINEMA PORTAL MARQUEE (NEAREST ACTIVE TOP HATTER HERO)</div>");
            sb.AppendFormat("<div style=\"margin-bottom:4px;color:#fff;font-weight:bold;\">{0}</div>", marquee);
            sb.AppendFormat("<div style=\"margin-bottom:6px;color:#aaddff;font-size:11px;\">🌀 Portal Destination: {0}</div>", portalStage);

            sb.Append("<div style=\"display:flex;flex-wrap:wrap;margin-bottom:8px;\">");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_0\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:6px;cursor:pointer;\">🎩 [Shift+1] Darrell X-Gen</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_1\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:6px;cursor:pointer;\">🕷️ [Shift+2] Spider-Pulse</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_2\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:6px;cursor:pointer;\">✨ [Shift+3] Divine Hands</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_3\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;margin-right:6px;cursor:pointer;\">🎸 [Shift+4] Jaxson Thorne</div>");
            sb.Append("<div onclick=\"window.wwSend(\\x27hero_4\\x27)\" style=\"background:#5a189a;padding:4px 8px;border-radius:4px;cursor:pointer;\">🌌 [Shift+5] Vesper Vance</div>");
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
            GUILayout.EndArea();
        }
    }
}
