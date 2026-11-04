using System;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using UnityEngine;

namespace WonderWorld.Cities2Mod.Economy
{
    /// <summary>
    /// Converts surplus 12% Industrial Bond yields (once Column 2 Overhead reaches $0.00)
    /// into Wonder Super-Currency (⭐️W) at the 85 BPM harmonic exchange rate, persisting
    /// directly to 02_Shared_Game_Core/State/WonderLedgerState.json.
    /// </summary>
    public static class WonderCurrencyConverter
    {
        /// <summary>
        /// Base conversion rate: $100.00 USD surplus bond yield past $0 overhead = 1.00 ⭐️W
        /// modulated by the 85 BPM harmonic resonance multiplier (1.15x when skybridge is linked).
        /// </summary>
        public const float UsdPerWonderCoin = 100.0f;
        public const float Harmonic85BpmMultiplier = 1.15f;

        public static float WonderSuperCurrencyBalance { get; set; } = 85.00f;
        public static float LifetimeBondYieldConvertedUsd { get; set; } = 0.00f;
        public static string LastConversionReceipt { get; private set; } = "Awaiting $0.00 Col 2 Overhead + 12% Bond Yield tick.";

        public static readonly string[] LedgerPaths = new[]
        {
            @"D:\Games\Cities - Skylines II\02_Shared_Game_Core\State\WonderLedgerState.json",
            @"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\02_Shared_Game_Core\State\WonderLedgerState.json",
            @"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\WonderPipeline\02_Shared_Game_Core\State\WonderLedgerState.json",
            @"C:\Users\Admin\.gemini\antigravity\scratch\Wonder-World-Universe\02_Shared_Game_Core\State\WonderLedgerState.json"
        };

        /// <summary>
        /// Evaluates a 12% bond yield payout against current Column 2 Overhead Debt.
        /// Any yield accrued while Overhead is $0.00 (or any spillover after zeroing Overhead)
        /// is converted into Wonder Super-Currency (⭐️W) and written to WonderLedgerState.json.
        /// </summary>
        public static float ProcessBondYieldConversion(
            float stakedBondsUsd,
            float annualYieldRate,
            ref float col2OverheadDebtUsd,
            ref float col1MunicipalDollars,
            ref float col3FoundationReserveUsd)
        {
            if (stakedBondsUsd <= 0f)
            {
                return 0f;
            }

            // Periodic 12% bond coupon slice
            float grossBondYieldUsd = stakedBondsUsd * annualYieldRate;

            // If Column 2 Overhead Debt still exists, retire overhead first before minting ⭐️W
            if (col2OverheadDebtUsd > 0.01f)
            {
                float debtAbsorbed = Mathf.Min(col2OverheadDebtUsd, grossBondYieldUsd);
                col2OverheadDebtUsd -= debtAbsorbed;
                float remainingYield = grossBondYieldUsd - debtAbsorbed;

                if (remainingYield <= 0.001f)
                {
                    LastConversionReceipt = $"Bond yield (${grossBondYieldUsd:N2}) absorbed into Col 2 Overhead (${col2OverheadDebtUsd:N2} left).";
                    return 0f;
                }

                grossBondYieldUsd = remainingYield;
            }

            // Past $0.00 overhead: 60/20/20 Municipal allocation + automatic Wonder ⭐️W minting
            col1MunicipalDollars += grossBondYieldUsd * 0.60f;
            col3FoundationReserveUsd += grossBondYieldUsd * 0.40f;

            float mintedWonder = (grossBondYieldUsd / UsdPerWonderCoin) * Harmonic85BpmMultiplier;
            WonderSuperCurrencyBalance += mintedWonder;
            LifetimeBondYieldConvertedUsd += grossBondYieldUsd;

            LastConversionReceipt =
                $"12% Bond Yield (+${grossBondYieldUsd:N2} past $0 Overhead) -> Minted +{mintedWonder:F2} ⭐️W (Total: {WonderSuperCurrencyBalance:F2} ⭐️W)";

            WriteWonderCurrencyToSharedLedger();
            return mintedWonder;
        }

        public static void WriteWonderCurrencyToSharedLedger()
        {
            foreach (string path in LedgerPaths)
            {
                try
                {
                    string dir = Path.GetDirectoryName(path);
                    if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
                    {
                        Directory.CreateDirectory(dir);
                    }

                    if (File.Exists(path))
                    {
                        string existing = File.ReadAllText(path);
                        string updated = UpsertWonderCurrencyFields(existing);
                        File.WriteAllText(path, updated);
                    }
                    else
                    {
                        MamaTowerCities2System.SaveToSharedJson(path);
                    }
                }
                catch (Exception ex)
                {
                    WonderWorldCS2Plugin.ModLogger?.LogWarning($"[WonderCurrencyConverter] Failed writing {path}: {ex.Message}");
                }
            }
        }

        public static string UpsertWonderCurrencyFields(string json)
        {
            string wonderVal = WonderSuperCurrencyBalance.ToString("F2", CultureInfo.InvariantCulture);
            string convertedVal = LifetimeBondYieldConvertedUsd.ToString("F2", CultureInfo.InvariantCulture);

            if (Regex.IsMatch(json, "\"wonder_super_currency_w\"\\s*:\\s*[-0-9.]+"))
            {
                json = Regex.Replace(
                    json,
                    "\"wonder_super_currency_w\"\\s*:\\s*[-0-9.]+",
                    $"\"wonder_super_currency_w\": {wonderVal}");
            }
            else if (json.Contains("\"staked_industrial_bonds\""))
            {
                json = Regex.Replace(
                    json,
                    "(\"staked_industrial_bonds\"\\s*:\\s*[-0-9.]+,)",
                    $"$1\n      \"wonder_super_currency_w\": {wonderVal},\n      \"bond_yield_converted_usd\": {convertedVal},");
            }

            if (Regex.IsMatch(json, "\"bond_yield_converted_usd\"\\s*:\\s*[-0-9.]+"))
            {
                json = Regex.Replace(
                    json,
                    "\"bond_yield_converted_usd\"\\s*:\\s*[-0-9.]+",
                    $"\"bond_yield_converted_usd\": {convertedVal}");
            }

            return json;
        }
    }
}
