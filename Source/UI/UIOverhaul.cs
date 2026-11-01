using System;
using UnityEngine;
using WonderWorld.Cities2Mod.Economy;
using WonderWorld.Cities2Mod.Portals;
using WonderWorld.Cities2Mod.Vehicles;

namespace WonderWorld.Cities2Mod.UI
{
    /// <summary>
    /// Cities: Skylines II UI Overhaul (Dual-Currency HUD + Hover-Drive Vehicle + Cinema Portal Marquee).
    /// Displays Municipal Dollars ($) alongside Wonder Super-Currency (⭐️W) at the top bar and inside
    /// the interactive F8 Wonder Command v3.0 control deck.
    /// </summary>
    public sealed class UIOverhaul : MonoBehaviour
    {
        private bool _showHud = false;
        private Rect _windowRect = new Rect(24f, 62f, 640f, 650f);

        private void Start()
        {
            MamaTowerCities2System.SyncFromSharedJson();
        }

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.F8))
            {
                _showHud = !_showHud;
                if (_showHud)
                {
                    MamaTowerCities2System.SyncFromSharedJson();
                }
            }

            EconomyOverhaul.TickRealtimeEconomy(Time.unscaledDeltaTime);
        }

        private void OnGUI()
        {
            DrawPersistentDualCurrencyTopBar();

            if (!_showHud)
            {
                return;
            }

            _windowRect = GUI.Window(
                85085,
                _windowRect,
                DrawWonderOverhaulWindow,
                "🎩 MAMA'S WONDER WORLD // PURPLE CITY DUAL-CURRENCY, HOVER-DRIVE & CINEMA PORTAL HUD");
        }

        private void DrawPersistentDualCurrencyTopBar()
        {
            GUI.color = new Color(0.08f, 0.92f, 1.0f, 0.96f);
            string hoverStatus = (HoverDriveVehicle.ActiveInstance != null && HoverDriveVehicle.ActiveInstance.IsHoverFlightActive)
                ? $"✈️ HOVER ON ({HoverDriveVehicle.ActiveInstance.CurrentAltitudeMeters:F1}m)"
                : "🚗 STREET [V]";

            string topBarText =
                $"🎩 PURPLE CITY [F8]  |  💵 Municipal: ${MamaTowerCities2System.Col1GrindBalance:N2}  |  ⭐️W Wonder: {WonderCurrencyConverter.WonderSuperCurrencyBalance:N2} ⭐️W  |  {hoverStatus}";

            if (GUI.Button(new Rect(16f, 14f, 760f, 34f), topBarText))
            {
                _showHud = !_showHud;
            }
            GUI.color = Color.white;
        }

        private void DrawWonderOverhaulWindow(int windowId)
        {
            GUILayout.BeginVertical();

            // 1. DUAL-CURRENCY HUD HEADER
            GUILayout.Box("=== 1. DUAL-CURRENCY HUD (MUNICIPAL $  +  WONDER ⭐️W SUPER-CURRENCY) ===");
            GUILayout.Label(
                $"💵 Municipal Liquid ($): ${MamaTowerCities2System.Col1GrindBalance:N2}   |   " +
                $"🏦 Col 3 Reserve ($): ${MamaTowerCities2System.Col3FoundationCapital:N2}   |   " +
                $"⭐️W Wonder Super-Currency: {WonderCurrencyConverter.WonderSuperCurrencyBalance:N2} ⭐️W");
            GUILayout.Label(
                $"📉 Col 2 Overhead Debt: ${MamaTowerCities2System.Col2OverheadDebt:N2}   |   " +
                $"📈 12% Industrial Bonds Staked: ${MamaTowerCities2System.StakedIndustrialBonds:N2}");
            GUILayout.Label($"🧾 Last Bond->⭐️W Mint: {WonderCurrencyConverter.LastConversionReceipt}");

            GUILayout.BeginHorizontal();
            if (GUILayout.Button("💳 Zero Out Col 2 Overhead ($0.00)"))
            {
                MamaTowerCities2System.Col2OverheadDebt = 0.00f;
                MamaTowerCities2System.EvonHubPermissionGranted = true;
                MamaTowerCities2System.LastVoiceBroadcast =
                    "Mama AI: Column 2 Overhead is $0.00! 12% Bond Yields now mint Wonder ⭐️W directly!";
                MamaTowerCities2System.SaveToSharedJson();
            }

            if (GUILayout.Button("⚡ Trigger 12% Bond Yield -> ⭐️W Conversion Now"))
            {
                EconomyOverhaul.ExecuteBondYieldCycle(EconomyOverhaul.AnnualBondCouponRate);
            }
            GUILayout.EndHorizontal();

            GUILayout.Space(6f);

            // 2. MODULAR HOVER-DRIVE VEHICLE ([V] KEY)
            GUILayout.Box("=== 2. MODULAR HOVER-DRIVE VEHICLE ([V] HOVER FLIGHT & MODULAR PARTS) ===");
            var vehicle = HoverDriveVehicle.ActiveInstance;
            if (vehicle != null)
            {
                string flightState = vehicle.IsHoverFlightActive ? "ENGAGED (85 BPM Anti-Grav Flight)" : "GROUNDED (Street Cruiser)";
                GUILayout.Label(
                    $"🚀 Flight Mode [Press V]: {flightState} | Altitude: {vehicle.CurrentAltitudeMeters:F1}m | " +
                    $"Thrust Multiplier: {vehicle.GetTotalThrustMultiplier():F2}x | Stability: {vehicle.GetTotalStabilityScore():F0}");

                if (GUILayout.Button("✈️ Toggle [V] Hover Flight Mode"))
                {
                    vehicle.ToggleHoverFlight();
                }

                GUILayout.BeginHorizontal();
                if (GUILayout.Button("🔧 Sub-Bass Repulsors"))
                {
                    vehicle.AttachOrToggleModularPart(ModularVehicleSlot.SubBassRepulsorThrusters);
                }
                if (GUILayout.Button("🔧 God-Hand Sonic Grille"))
                {
                    vehicle.AttachOrToggleModularPart(ModularVehicleSlot.SonicPhaserFrontGrille);
                }
                if (GUILayout.Button("🔧 Rainbow LED Wing"))
                {
                    vehicle.AttachOrToggleModularPart(ModularVehicleSlot.RainbowEqualizerAeroWing);
                }
                if (GUILayout.Button("🔧 Top-Hat Turret"))
                {
                    vehicle.AttachOrToggleModularPart(ModularVehicleSlot.TopHatMagneticRoofTurret);
                }
                GUILayout.EndHorizontal();
            }

            GUILayout.Space(6f);

            // 3. CINEMA PORTAL BUILDING AI (NEAREST ACTIVE TOP HATTER MARQUEE)
            GUILayout.Box("=== 3. CINEMA PORTAL MARQUEE (DYNAMICALLY BRANDED BY NEAREST TOP HATTER) ===");
            var cinema = CinemaBuildingAI.ActiveInstance;
            if (cinema != null)
            {
                GUILayout.Label(cinema.ActiveMarqueeHeadline);
                GUILayout.Label($"🌀 Portal Warp Destination: {cinema.ActivePortalTargetStage}");

                GUILayout.BeginHorizontal();
                for (int i = 0; i < cinema.TopHatterRoster.Length; i++)
                {
                    string shortBtn = $"#{i + 1} {cinema.TopHatterRoster[i].HeroName.Split(' ')[0]}";
                    if (GUILayout.Button(shortBtn))
                    {
                        cinema.SetHeroClosestToCinema(i);
                    }
                }
                GUILayout.EndHorizontal();
            }

            GUILayout.Space(6f);

            // 4. WORKFORCE & SHARED LEDGER SYNC
            GUILayout.Box("=== 4. WORKFORCE SHIFT & SHARED LEDGER SYNC (02_Shared_Game_Core) ===");
            GUILayout.BeginHorizontal();
            if (GUILayout.Button("🌊 59,000-Gal Wet-Vac Shift (+$185 / -$653.91 Debt)"))
            {
                MamaTowerCities2System.Col1GrindBalance += 185.00f;
                MamaTowerCities2System.Col2OverheadDebt = Mathf.Max(0f, MamaTowerCities2System.Col2OverheadDebt - 653.91f);
                MamaTowerCities2System.Col3FoundationCapital += 37.00f;
                MamaTowerCities2System.SaveToSharedJson();
            }

            if (GUILayout.Button("🩺 CNA Dignity Shift (+$285 / 100% Dignity)"))
            {
                MamaTowerCities2System.Col1GrindBalance += 285.00f;
                MamaTowerCities2System.Col3FoundationCapital += 57.00f;
                MamaTowerCities2System.CnaPatientDignity = 100.0f;
                MamaTowerCities2System.SaveToSharedJson();
            }

            if (GUILayout.Button("🔄 Sync WonderLedgerState.json"))
            {
                MamaTowerCities2System.SaveToSharedJson();
                MamaTowerCities2System.SyncFromSharedJson();
            }
            GUILayout.EndHorizontal();

            GUILayout.Space(4f);
            GUILayout.Label($"🎙️ {MamaTowerCities2System.LastVoiceBroadcast}");

            GUILayout.EndVertical();
            GUI.DragWindow(new Rect(0, 0, 10000, 26));
        }
    }
}
