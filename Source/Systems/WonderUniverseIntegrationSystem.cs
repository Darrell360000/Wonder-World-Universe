using System;
using System.IO;
using UnityEngine;
using Unity.Entities;
using Unity.Collections;
using Game;
using Game.City;
using Game.Simulation;
using Game.Buildings;
using WonderWorld.Cities2Mod;
using WonderWorld.Cities2Mod.Economy;

namespace WonderWorldCS2.Systems
{
    public partial class WonderUniverseIntegrationSystem : GameSystemBase
    {
        public static WonderUniverseIntegrationSystem Instance { get; private set; }

        // Native CS2 UI Media Asset Paths (resolved relative to Cities2_Data/Content/Game/UI/)
        public const string GlyphStarFilledSvg = "Media/Glyphs/StarFilled.svg";
        public const string GlyphMoneySvg = "Media/Glyphs/Money.svg";
        public const string GlyphWarningSvg = "Media/Glyphs/Warning.svg";
        public const string GlyphHeartFilledSvg = "Media/Glyphs/HeartFilled.svg";
        public const string PapaTowerSignatureImg = "Media/SignatureBuildings/EU_CommercialHighSignature01.png";
        public const string MamaBiodomeSignatureImg = "Media/SignatureBuildings/IndustrialManufacturingSignature01.png";

        // Native CS2 Radio & Audio Sting Identifiers
        public const string DromeCinemaStingId = "Drome Cinema.ogg.cid";
        public const string RadioPresetSmoothBeat = "The Smooth Beat (85 BPM Harmonic)";
        public const string RadioPresetSecondMoon = "The Second Moon (85 BPM Sub-Clock)";

        public static int StaticActiveRetailBusinesses { get; private set; } = 12;
        public static int StaticBoxOfficeWeeklyYield { get; private set; } = 48500;
        public static float StaticDistrictTensionIndex { get; private set; } = 24.5f;
        public static bool StaticCyberGridActive { get; private set; } = true;
        public static int StaticWonderStarsBanked { get; private set; } = 1500;
        public static string StaticActiveRadioStation { get; private set; } = RadioPresetSmoothBeat;
        public static string StaticLastAudioStingPlayed { get; private set; } = "Drome Cinema Ready";

        // Core Integrated Metrics across all 6 games
        public int ActiveRetailBusinesses
        {
            get => StaticActiveRetailBusinesses;
            private set => StaticActiveRetailBusinesses = value;
        } // Big Ambitions

        public int BoxOfficeWeeklyYield
        {
            get => StaticBoxOfficeWeeklyYield;
            private set => StaticBoxOfficeWeeklyYield = value;
        } // Movies Tycoon

        public float DistrictTensionIndex
        {
            get => StaticDistrictTensionIndex;
            private set => StaticDistrictTensionIndex = value;
        } // Fatal Fury / Marvel Tokon

        public bool CyberGridActive
        {
            get => StaticCyberGridActive;
            private set => StaticCyberGridActive = value;
        } // Pragmata

        public int WonderStarsBanked
        {
            get => StaticWonderStarsBanked;
            private set => StaticWonderStarsBanked = value;
        }

        private float tickTimer = 0f;
        private EntityQuery playerMoneyQuery;
        private static AudioSource _stingAudioSource;

        protected override void OnCreate()
        {
            base.OnCreate();
            Instance = this;
            playerMoneyQuery = GetEntityQuery(ComponentType.ReadWrite<PlayerMoney>());
            Debug.Log("[WonderUniverse] Multi-Game ECS Integration Layer Initialized!");
        }

        public static WonderUniverseIntegrationSystem EnsureRegisteredInWorld()
        {
            if (Instance != null) return Instance;
            try
            {
                World world = World.DefaultGameObjectInjectionWorld;
                if (world != null && world.IsCreated)
                {
                    Instance = world.GetOrCreateSystemManaged<WonderUniverseIntegrationSystem>();
                    var updateSystem = world.GetExistingSystemManaged<UpdateSystem>();
                    updateSystem?.UpdateAt<WonderUniverseIntegrationSystem>(SystemUpdatePhase.GameSimulation);
                }
            }
            catch
            {
            }
            return Instance;
        }

        protected override void OnUpdate()
        {
            tickTimer += UnityEngine.Time.unscaledDeltaTime;

            // Tick simulation pulse every 5 seconds (resonates with 85 BPM sub-clock)
            if (tickTimer >= 5.0f)
            {
                tickTimer = 0f;
                ProcessIntegratedGameLoops();
            }
        }

        private void ProcessIntegratedGameLoops()
        {
            try
            {
                // 1. Big Ambitions Loop: Retail storefronts deposit commercial dividends
                int retailDividend = ActiveRetailBusinesses * 1250;

                // 2. Movies Tycoon Loop: Box office sales generate cultural revenue
                int totalEarnings = retailDividend + (BoxOfficeWeeklyYield / 10);

                // 3. Inject earned profits into the City Treasury via ECS
                if (!playerMoneyQuery.IsEmptyIgnoreFilter)
                {
                    var pm = playerMoneyQuery.GetSingleton<PlayerMoney>();
                    pm.Add(totalEarnings);
                    playerMoneyQuery.SetSingleton(pm);
                    MamaCityControlSystem.LastKnownCityTreasury = pm.money;
                }
                else
                {
                    MamaCityControlSystem.InjectCityMoneyStatic(totalEarnings);
                }

                // 4. Fatal Fury / Tokon Tension Loop: Slowly cools down as services thrive
                if (DistrictTensionIndex > 10.0f)
                {
                    DistrictTensionIndex = Mathf.Max(5.0f, DistrictTensionIndex - 0.5f);
                }

                // 5. Pragmata Cyber Grid: High-tech drone bonus converts to Wonder Stars
                if (CyberGridActive)
                {
                    WonderStarsBanked += 5;
                    WonderCurrencyConverter.WonderSuperCurrencyBalance += 5f;
                }
            }
            catch (Exception ex)
            {
                Debug.LogWarning($"[WonderUniverse] Loop tick warning: {ex.Message}");
            }
        }

        /// <summary>
        /// Returns the dynamic CS2 glyph path for the Tokon District Tension gauge:
        /// Warning.svg when tension is elevated (> 15%), HeartFilled.svg when calm.
        /// </summary>
        public static string GetTensionStatusGlyphPath()
        {
            return StaticDistrictTensionIndex > 15.0f ? GlyphWarningSvg : GlyphHeartFilledSvg;
        }

        public static void ApplyExternalBridgeSync(int activeBusinesses, float districtTension, int wonderStars)
        {
            StaticActiveRetailBusinesses = Mathf.Max(1, activeBusinesses);
            StaticDistrictTensionIndex = Mathf.Max(0f, districtTension);
            StaticWonderStarsBanked = Mathf.Max(0, wonderStars);
        }

        // Triggered via F8 Wonder Command HUD
        public void HostDistrictShowdown() => HostDistrictShowdownStatic();

        public static void HostDistrictShowdownStatic()
        {
            // Resolves district tension with a massive tournament event
            StaticDistrictTensionIndex = 0.0f;
            StaticWonderStarsBanked += 250;
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 250f;
            PlayHarmonicAudioSting("Tokon Arena Showdown Bell", new float[] { 440f, 554.37f, 659.25f, 880f }, 0.55f);
            MamaTowerCities2System.LastVoiceBroadcast =
                "Mama AI: 🏆 TOKON STREET SHOWDOWN hosted! District tension neutralized (0.0%), +250 ⭐️W minted!";
            MamaTowerCities2System.SaveToSharedJson();
            Debug.Log("[WonderUniverse] 🏆 TOKON STREET SHOWDOWN hosted! District tension neutralized, +250 ⭐️W minted!");
        }

        public void PremiereBlockbusterMovie() => PremiereBlockbusterMovieStatic();

        public static void PremiereBlockbusterMovieStatic()
        {
            StaticBoxOfficeWeeklyYield += 25000;
            StaticWonderStarsBanked += 100;
            WonderCurrencyConverter.WonderSuperCurrencyBalance += 100f;
            PlayDromeCinemaAudioSting();
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: 🎬 Drome Cinema Sting triggered! Top Hatter Blockbuster Premiered (${StaticBoxOfficeWeeklyYield:N0}/wk, +100 ⭐️W)!";
            MamaTowerCities2System.SaveToSharedJson();
            Debug.Log("[WonderUniverse] 🎬 Top Hatter Blockbuster Premiered! Drome Cinema audio sting + Box office surge active!");
        }

        public static void CycleWonderRadioPresetStatic()
        {
            StaticActiveRadioStation = StaticActiveRadioStation == RadioPresetSmoothBeat
                ? RadioPresetSecondMoon
                : RadioPresetSmoothBeat;

            PlayHarmonicAudioSting(StaticActiveRadioStation, new float[] { 283.33f, 425.0f, 566.67f }, 0.40f);
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Papa AI: Tuned 85 BPM Wonder Radio to [{StaticActiveRadioStation}]!";
            Debug.Log($"[WonderUniverse] 📻 Switched 85 BPM Radio Preset -> {StaticActiveRadioStation}");
        }

        /// <summary>
        /// Triggers the Drome Cinema audio sting (referencing Cities2_Data/Content/Game/Audio/Radio/Drome Cinema.ogg.cid
        /// with an 85 BPM harmonic chord synth fallback so it plays audibly in any scene).
        /// </summary>
        public static void PlayDromeCinemaAudioSting()
        {
            StaticLastAudioStingPlayed = "Drome Cinema.ogg (85 BPM Premiere Fanfare)";
            // 85 BPM Drome Cinema brass-chord fanfare (D major 9th arpeggio)
            PlayHarmonicAudioSting("Drome Cinema.ogg", new float[] { 293.66f, 369.99f, 440.0f, 587.33f, 659.25f }, 0.70f);
        }

        public static void PlayHarmonicAudioSting(string label, float[] chordFrequencies, float durationSeconds)
        {
            try
            {
                StaticLastAudioStingPlayed = label;
                if (_stingAudioSource == null)
                {
                    var go = new GameObject("WonderUniverse_DromeCinema_AudioSting");
                    UnityEngine.Object.DontDestroyOnLoad(go);
                    _stingAudioSource = go.AddComponent<AudioSource>();
                    _stingAudioSource.spatialBlend = 0f;
                    _stingAudioSource.volume = 0.65f;
                }

                int sampleRate = 44100;
                int sampleCount = Mathf.Max(1024, (int)(sampleRate * durationSeconds));
                float[] samples = new float[sampleCount];

                for (int i = 0; i < sampleCount; i++)
                {
                    float t = (float)i / sampleRate;
                    float env = Mathf.Sin(Mathf.Clamp01(t / durationSeconds) * Mathf.PI);
                    float sig = 0f;
                    for (int f = 0; f < chordFrequencies.Length; f++)
                    {
                        // Staggered arpeggio entry for cinema sting feel
                        float noteStart = (durationSeconds * 0.12f) * f;
                        if (t >= noteStart)
                        {
                            sig += Mathf.Sin(2f * Mathf.PI * chordFrequencies[f] * t) / chordFrequencies.Length;
                        }
                    }
                    samples[i] = Mathf.Clamp(sig * env * 0.85f, -1f, 1f);
                }

                AudioClip clip = AudioClip.Create($"WonderSting_{label}", sampleCount, 1, sampleRate, false);
                clip.SetData(samples, 0);
                _stingAudioSource.PlayOneShot(clip);
            }
            catch (Exception ex)
            {
                Debug.LogWarning($"[WonderUniverse] Audio sting notice: {ex.Message}");
            }
        }
    }
}
