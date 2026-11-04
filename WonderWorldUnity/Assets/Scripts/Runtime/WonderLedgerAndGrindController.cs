using UnityEngine;
using UnityEngine.Events;

namespace WonderWorld.EconomyAndMinigames
{
    public class WonderLedgerAndGrindController : MonoBehaviour
    {
        [Header("Column 1: The Grind (Gross Revenue)")]
        public float cnaAndWetVacMiniGames = 8500f;
        public float arcadeBrawlerPayouts = 14500f;
        public float commercialLeaseIncome = 48000f;
        public float citiesSkylinesTourismBonus = 9250f;

        [Header("Column 2: The Overhead (Survival Gate)")]
        public float structuralMaintenance = 11200f;
        public float securityAndBrawlerStaff = 8400f;
        public float utilityGridLoadCost = 5100f;

        [Header("Column 3: The Foundation (Wonder Capital)")]
        public float liquidReserve = 55550f;
        public int vitalityCreditsTier1 = 12400;
        public int industrialBondsTier2And3 = 85;
        public int unlockedTier = 3;
        public bool evonHubPermissionGranted = true;

        [Header("Era 1 Mini-Game: 59,000-Gallon Wet-Vac Extraction")]
        public float remainingFloodGallons = 59000f;
        public float currentWetVacTank = 0f;
        public float maxWetVacTank = 2500f;

        [Header("Era 1 Mini-Game: CNA Shift & Mama Stamina Pacing")]
        public float playerStamina = 100f;
        public float legEdemaPressure = 0f;
        public int completedCNATasks = 0;

        public UnityEvent<string, string> onMamaPapaDialogue;

        public float GetColumn1Gross() =>
            cnaAndWetVacMiniGames + arcadeBrawlerPayouts + commercialLeaseIncome + citiesSkylinesTourismBonus;

        public float GetColumn2Overhead() =>
            structuralMaintenance + securityAndBrawlerStaff + utilityGridLoadCost;

        public void PumpWetVac(bool hit85BpmBeat)
        {
            if (remainingFloodGallons <= 0f || currentWetVacTank >= maxWetVacTank) return;
            float pumped = Mathf.Min(hit85BpmBeat ? 750f : 450f, Mathf.Min(maxWetVacTank - currentWetVacTank, remainingFloodGallons));
            currentWetVacTank += pumped;
            remainingFloodGallons -= pumped;

            if (currentWetVacTank >= maxWetVacTank || remainingFloodGallons <= 0f)
            {
                DumpWetVacTank();
            }
        }

        public void DumpWetVacTank()
        {
            if (currentWetVacTank <= 0f) return;
            float earnedCash = (currentWetVacTank / 100f) * 45f;
            int earnedVitality = Mathf.RoundToInt((currentWetVacTank / 100f) * 18f);
            currentWetVacTank = 0f;

            cnaAndWetVacMiniGames += earnedCash;
            vitalityCreditsTier1 += earnedVitality;

            if (remainingFloodGallons <= 0f)
            {
                utilityGridLoadCost = Mathf.Max(500f, utilityGridLoadCost - 653.91f);
            }

            RecalculateLedger();
            onMamaPapaDialogue?.Invoke(
                $"Baby, you pumped that flood down! Earned ${earnedCash:F0} and {earnedVitality} Vitality Credits!",
                "Minime Tophatter just locked the foundation light-codes! Column 3 is ready to build!"
            );
        }

        public void PerformCNATask(bool isHeavyHoyerLift)
        {
            float cost = isHeavyHoyerLift ? 20f : 12f;
            if (playerStamina < cost || legEdemaPressure >= 85f)
            {
                onMamaPapaDialogue?.Invoke(
                    "Sugar, sit down and elevate your legs on the sofa pillows first! Protect your breathing and stamina.",
                    "Listen to Big Mama! Take a quick rest break so you keep your shift multiplier!"
                );
                return;
            }

            playerStamina -= cost;
            legEdemaPressure += isHeavyHoyerLift ? 16f : 9f;
            completedCNATasks++;
            cnaAndWetVacMiniGames += isHeavyHoyerLift ? 450f : 275f;
            vitalityCreditsTier1 += isHeavyHoyerLift ? 150 : 95;
            RecalculateLedger();
        }

        public void TakeMamaRestBreak()
        {
            playerStamina = Mathf.Clamp(playerStamina + 45f, 0f, 100f);
            legEdemaPressure = Mathf.Clamp(legEdemaPressure - 50f, 0f, 100f);
        }

        public void RecalculateLedger()
        {
            liquidReserve = GetColumn1Gross() - GetColumn2Overhead();
            evonHubPermissionGranted = liquidReserve >= 0f;
        }
    }
}
