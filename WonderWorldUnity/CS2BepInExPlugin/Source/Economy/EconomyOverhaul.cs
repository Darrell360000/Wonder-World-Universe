using System;
using Game;
using Unity.Entities;
using UnityEngine;

namespace WonderWorld.Cities2Mod.Economy
{
    /// <summary>
    /// Cities: Skylines II Economy Overhaul System (12% Industrial Bonds + 60/20/20 Split).
    /// Hooks WonderCurrencyConverter so that 12% bond yields past $0 overhead automatically
    /// mint Wonder Super-Currency (⭐️W) and persist to 02_Shared_Game_Core/State/WonderLedgerState.json.
    /// </summary>
    public partial class EconomyOverhaul : GameSystemBase
    {
        public const float AnnualBondCouponRate = 0.12f;
        private static float _realtimeBondTickAccumulator = 0f;

        protected override void OnCreate()
        {
            base.OnCreate();
            WonderWorldCS2Plugin.ModLogger?.LogInfo("[EconomyOverhaul] Initialized with 12% Bond Yield -> WonderCurrencyConverter hook.");
        }

        protected override void OnUpdate()
        {
            ExecuteBondYieldCycle(AnnualBondCouponRate);
        }

        public override int GetUpdateInterval(SystemUpdatePhase phase)
        {
            return 16384;
        }

        /// <summary>
        /// Runs the 12% Industrial Bond yield cycle and invokes WonderCurrencyConverter when
        /// Column 2 Overhead is at $0.00 (or absorbs overhead first and converts the surplus).
        /// </summary>
        public static float ExecuteBondYieldCycle(float yieldRate = AnnualBondCouponRate)
        {
            float mintedWonder = WonderCurrencyConverter.ProcessBondYieldConversion(
                MamaTowerCities2System.StakedIndustrialBonds,
                yieldRate,
                ref MamaTowerCities2System.Col2OverheadDebt,
                ref MamaTowerCities2System.Col1GrindBalance,
                ref MamaTowerCities2System.Col3FoundationCapital);

            if (MamaTowerCities2System.Col2OverheadDebt <= 0.01f)
            {
                MamaTowerCities2System.Col2OverheadDebt = 0.00f;
                MamaTowerCities2System.EvonHubPermissionGranted = true;
            }

            if (mintedWonder > 0f)
            {
                MamaTowerCities2System.LastVoiceBroadcast =
                    $"Papa AI: 12% Bond Coupon hit past $0 Overhead! Minted +{mintedWonder:F2} ⭐️W to WonderLedgerState.json!";
            }

            MamaTowerCities2System.SaveToSharedJson();
            return mintedWonder;
        }

        /// <summary>
        /// Real-time ticker invoked from the HUD/Plugin loop so bond yields tick smoothly even when simulation is paused.
        /// </summary>
        public static void TickRealtimeEconomy(float unscaledDeltaTime)
        {
            _realtimeBondTickAccumulator += unscaledDeltaTime;
            if (_realtimeBondTickAccumulator >= 7.0f)
            {
                _realtimeBondTickAccumulator = 0f;
                // 1/10th of the 12% annual coupon per real-time cycle
                ExecuteBondYieldCycle(AnnualBondCouponRate * 0.10f);
            }
        }
    }
}
