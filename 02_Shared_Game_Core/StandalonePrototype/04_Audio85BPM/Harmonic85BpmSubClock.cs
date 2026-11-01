using System;
using UnityEngine;

namespace WonderWorld.Audio
{
    /// <summary>
    /// 85 BPM Harmonic Pulse Sub-Clock (1.4167 Hz / 0.7059s per beat).
    /// Drives the Tokon 1.5x Rev-Meter beat window, Exosuit Hover-Drive harmonic oscillation,
    /// and radio station synchronization (The Smooth Beat / The Second Moon).
    /// </summary>
    public class Harmonic85BpmSubClock : MonoBehaviour
    {
        public static Harmonic85BpmSubClock Instance { get; private set; }

        public const float TargetBpm = 85.0f;
        public const float BeatsPerSecond = TargetBpm / 60.0f; // 1.4166667 Hz
        public const float SecondsPerBeat = 60.0f / TargetBpm; // ~0.70588s
        public const float OnBeatToleranceSeconds = 0.11f;

        public int TotalBeatCount { get; private set; }
        public float CurrentBeatPhase01 { get; private set; }

        private float _beatTimer;

        private void Awake()
        {
            Instance = this;
        }

        private void Update()
        {
            _beatTimer += Time.unscaledDeltaTime;
            if (_beatTimer >= SecondsPerBeat)
            {
                _beatTimer -= SecondsPerBeat;
                TotalBeatCount++;
            }
            CurrentBeatPhase01 = _beatTimer / SecondsPerBeat;
        }

        /// <summary>
        /// Returns true if the player strikes within +/- 110ms of the 85 BPM sub-bass downbeat.
        /// </summary>
        public bool IsWithinOnBeatWindow()
        {
            return _beatTimer <= OnBeatToleranceSeconds ||
                   (SecondsPerBeat - _beatTimer) <= OnBeatToleranceSeconds;
        }

        public float GetHarmonicWaveOffset()
        {
            return Mathf.Sin(CurrentBeatPhase01 * Mathf.PI * 2.0f);
        }
    }
}
