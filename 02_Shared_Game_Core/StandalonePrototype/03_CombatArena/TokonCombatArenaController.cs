using System;
using UnityEngine;
using WonderWorld.Audio;
using WonderWorld.Bridge;

namespace WonderWorld.Combat
{
    /// <summary>
    /// Fatal Fury / Marvel Tokon 2.5D Fighting Souls Arena & 85 BPM Rev-Meter System.
    /// Features Darrell X-Gen and the 5 Top Hatters. Striking on the 85 BPM sub-bass beat
    /// builds Rev energy 1.5x faster. Winning a showdown resets CS2 DistrictTensionIndex to 0.0%
    /// and mints +250 ⭐️W across both games.
    /// </summary>
    public class TokonCombatArenaController : MonoBehaviour
    {
        public static TokonCombatArenaController Instance { get; private set; }

        public string ActiveFighterName = "Darrell X-Gen";
        public string AssistHeroName = "Spider-Pulse Heroine";
        public float PlayerHealth = 1000f;
        public float OpponentHealth = 1000f;
        public float RevMeterPercent = 0f;
        public bool IsArenaMatchActive = false;

        public const float BaseStrikeDamage = 65f;
        public const float BaseRevGain = 12f;
        public const float OnBeatRevMultiplier = 1.5f;

        private void Awake()
        {
            Instance = this;
        }

        private void Update()
        {
            // Automatically unlock or start arena showdown with [F] when tension is elevated
            if (Input.GetKeyDown(KeyCode.F) && !IsArenaMatchActive)
            {
                StartTokonArenaMatch();
            }

            if (!IsArenaMatchActive) return;

            // [J] Light/Heavy Strike (85 BPM Rev-Meter sync)
            if (Input.GetKeyDown(KeyCode.J) || Input.GetMouseButtonDown(0))
            {
                ExecuteFighterStrike();
            }

            // [K] Sonic Gauntlet Special (Requires >= 50% Rev Meter)
            if (Input.GetKeyDown(KeyCode.K) && RevMeterPercent >= 50f)
            {
                ExecuteSonicGauntletSpecial();
            }
        }

        public void StartTokonArenaMatch()
        {
            IsArenaMatchActive = true;
            PlayerHealth = 1000f;
            OpponentHealth = 1000f;
            RevMeterPercent = 25f;
            Debug.Log($"[TokonArena] 🥊 FIGHTING SOULS SHOWDOWN STARTED: {ActiveFighterName} (Assist: {AssistHeroName})!");
        }

        public void ExecuteFighterStrike()
        {
            bool onBeat = Harmonic85BpmSubClock.Instance != null && Harmonic85BpmSubClock.Instance.IsWithinOnBeatWindow();
            float revMult = onBeat ? OnBeatRevMultiplier : 1.0f;
            float dmgMult = onBeat ? 1.25f : 1.0f;

            RevMeterPercent = Mathf.Min(100f, RevMeterPercent + BaseRevGain * revMult);
            OpponentHealth = Mathf.Max(0f, OpponentHealth - BaseStrikeDamage * dmgMult);

            if (onBeat)
            {
                Debug.Log($"[TokonArena] ⚡ 85 BPM PERFECT BEAT STRIKE! (1.5x Rev Gain -> {RevMeterPercent:F0}% | Opponent HP: {OpponentHealth:F0})");
            }

            CheckVictoryCondition();
        }

        public void ExecuteSonicGauntletSpecial()
        {
            RevMeterPercent = Mathf.Max(0f, RevMeterPercent - 50f);
            OpponentHealth = Mathf.Max(0f, OpponentHealth - 320f);
            Debug.Log($"[TokonArena] 🌀 GOD-HAND SONIC GAUNTLET SPECIAL + {AssistHeroName} TAG ASSIST! (-320 HP)");
            CheckVictoryCondition();
        }

        private void CheckVictoryCondition()
        {
            if (OpponentHealth <= 0.01f && IsArenaMatchActive)
            {
                IsArenaMatchActive = false;
                Debug.Log("[TokonArena] 🏆 K.O.! TOKON SHOWDOWN VICTORY! Syncing 0.0% District Tension & +250 ⭐️W to Cities: Skylines II!");
                WonderWorldLedgerBridge.Instance?.SyncArenaShowdownVictory(250);
            }
        }
    }
}
