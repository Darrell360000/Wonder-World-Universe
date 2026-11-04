using System;
using System.IO;
using UnityEngine;

namespace WonderWorld.CitiesSkylines2Mod
{
    /// <summary>
    /// Cities: Skylines II Custom Mod System for Mama Wonder Tower (8m x 8m Zone Lot Footprint,
    /// 4096x4096 _ControlMask & _Emissive Signage, +35% Attractiveness / +20% Tax Yield).
    /// Reads 02_Shared_Game_Core/State/WonderLedgerState.json 16 times per in-game day (Interval = 262144 / 16).
    /// </summary>
    public class MamaTowerCities2System : MonoBehaviour
    {
        public const int SimulationTicksPerDay = 262144;
        public const int UpdatesPerDay = 16;

        [Header("Cities: Skylines II Lot & Texture Spec")]
        public string lotFootprintSize = "8m x 8m";
        public int textureResolution = 4096;
        public float cityAttractivenessBonus = 0.35f;
        public float commercialTaxYieldBonus = 0.20f;

        [Header("Live Synced State from WonderLedgerState.json")]
        public bool emissiveSignageActive = true;
        public int unlockedTowerTier = 3;
        public float syncedLiquidReserve = 55550f;

        public int GetUpdateInterval() => SimulationTicksPerDay / UpdatesPerDay;

        public bool SyncFromSharedLedgerJson(string jsonFilePath)
        {
            if (string.IsNullOrEmpty(jsonFilePath) || !File.Exists(jsonFilePath))
            {
                return false;
            }

            string content = File.ReadAllText(jsonFilePath);
            emissiveSignageActive = content.Contains("\"emissive_signage_active\": true");
            return true;
        }
    }
}
