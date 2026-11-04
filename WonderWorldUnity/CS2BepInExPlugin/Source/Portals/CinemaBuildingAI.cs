using System;
using UnityEngine;

namespace WonderWorld.Cities2Mod.Portals
{
    public enum TopHatterHeroId
    {
        DarrellXGen = 0,
        SpiderPulseHeroine = 1,
        DivineHandsTopHatter = 2,
        JaxsonIronRiffThorne = 3,
        VesperVanceDimensionMaestro = 4
    }

    [Serializable]
    public struct TopHatterHeroNode
    {
        public TopHatterHeroId HeroId;
        public string HeroName;
        public string SignatureShowTitle;
        public string PortalDestinationLabel;
        public Vector3 WorldPosition;
        public bool IsActive;
    }

    /// <summary>
    /// Cinema Portal Building AI for Purple City.
    /// Continuously scans the 5 Top Hatter Heroes for proximity & active status, dynamically
    /// re-branding the neon marquee text and inter-engine Cinema Portal destination (CS2 <-> UE5.8 <-> Unity).
    /// </summary>
    public sealed class CinemaBuildingAI : MonoBehaviour
    {
        public static CinemaBuildingAI ActiveInstance { get; private set; }

        public Vector3 CinemaLotWorldPosition = new Vector3(64.0f, 0.0f, 64.0f);
        public int NearestActiveHeroIndex = 0;
        public float NearestHeroDistanceMeters = 12.4f;
        public string ActiveMarqueeHeadline { get; private set; } =
            "🎬 NOW PLAYING AT WONDER CINEMA PORTAL: DARRELL X-GEN // 85 BPM GOD-HAND SONIC ODYSSEY";
        public string ActivePortalTargetStage { get; private set; } =
            "UE5.8 Stage 2: Twin Tower Skybridge & 85 BPM Harmonic Arena";

        public readonly TopHatterHeroNode[] TopHatterRoster = new TopHatterHeroNode[5]
        {
            new TopHatterHeroNode
            {
                HeroId = TopHatterHeroId.DarrellXGen,
                HeroName = "Darrell X-Gen",
                SignatureShowTitle = "85 BPM GOD-HAND SONIC ODYSSEY",
                PortalDestinationLabel = "UE5.8 Stage 2: Twin Tower Skybridge & 85 BPM Harmonic Arena",
                WorldPosition = new Vector3(58.0f, 0.0f, 60.0f),
                IsActive = true
            },
            new TopHatterHeroNode
            {
                HeroId = TopHatterHeroId.SpiderPulseHeroine,
                HeroName = "Spider-Pulse Heroine",
                SignatureShowTitle = "NEON SKYLINE WEB-PULSE CHRONICLES",
                PortalDestinationLabel = "Unity Brawler Stage: High-Line Monorail & Rooftop Chase",
                WorldPosition = new Vector3(110.0f, 24.0f, 85.0f),
                IsActive = true
            },
            new TopHatterHeroNode
            {
                HeroId = TopHatterHeroId.DivineHandsTopHatter,
                HeroName = "The Divine Hands Top-Hatter",
                SignatureShowTitle = "REALITY SHAPER & GOLDEN LEDGER MIRACLE",
                PortalDestinationLabel = "UE5.8 Stage 3: Evon Hub Sanctuary & Bio-Dome Atrium",
                WorldPosition = new Vector3(95.0f, 0.0f, 120.0f),
                IsActive = true
            },
            new TopHatterHeroNode
            {
                HeroId = TopHatterHeroId.JaxsonIronRiffThorne,
                HeroName = "Jaxson 'Iron Riff' Thorne",
                SignatureShowTitle = "HEAVY INDUSTRIAL BASS-DROP BRAWL",
                PortalDestinationLabel = "Unity Brawler Stage: Sub-Basement Wet-Vac & Foundry Pit",
                WorldPosition = new Vector3(140.0f, 0.0f, 45.0f),
                IsActive = true
            },
            new TopHatterHeroNode
            {
                HeroId = TopHatterHeroId.VesperVanceDimensionMaestro,
                HeroName = "Vesper Vance (Dimension-Rift Maestro)",
                SignatureShowTitle = "PURPLE CITY QUANTUM RIFT SYMPHONY",
                PortalDestinationLabel = "Cross-Engine Warp: CS2 8m x 8m Plaza <-> UE5.8 Apex Vault",
                WorldPosition = new Vector3(175.0f, 40.0f, 160.0f),
                IsActive = true
            }
        };

        private void Awake()
        {
            ActiveInstance = this;
            RefreshMarqueeForNearestHero();
        }

        private int _lastTickedFrame = -1;

        private void Update()
        {
            TickFrame();
        }

        public void TickFrame()
        {
            if (_lastTickedFrame == Time.frameCount) return;
            _lastTickedFrame = Time.frameCount;

            // Allow hot-swapping the nearest active Top Hatter hero at the Cinema Portal via Alpha1-Alpha5 keys when Shift is held
            if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift))
            {
                if (Input.GetKeyDown(KeyCode.Alpha1)) SetHeroClosestToCinema(0);
                if (Input.GetKeyDown(KeyCode.Alpha2)) SetHeroClosestToCinema(1);
                if (Input.GetKeyDown(KeyCode.Alpha3)) SetHeroClosestToCinema(2);
                if (Input.GetKeyDown(KeyCode.Alpha4)) SetHeroClosestToCinema(3);
                if (Input.GetKeyDown(KeyCode.Alpha5)) SetHeroClosestToCinema(4);
            }
        }

        public void SetHeroClosestToCinema(int heroIndex)
        {
            if (heroIndex < 0 || heroIndex >= TopHatterRoster.Length)
            {
                return;
            }

            for (int i = 0; i < TopHatterRoster.Length; i++)
            {
                if (i == heroIndex)
                {
                    TopHatterRoster[i].WorldPosition = CinemaLotWorldPosition + new Vector3(4.0f, 0.0f, 4.0f);
                    TopHatterRoster[i].IsActive = true;
                }
                else
                {
                    TopHatterRoster[i].WorldPosition = CinemaLotWorldPosition + new Vector3(60.0f * (i + 1), 0.0f, 45.0f * (i + 1));
                }
            }

            RefreshMarqueeForNearestHero();
            MamaTowerCities2System.LastVoiceBroadcast =
                $"Mama AI: Cinema Portal Marquee updated for {TopHatterRoster[NearestActiveHeroIndex].HeroName}!";
        }

        public string RefreshMarqueeForNearestHero()
        {
            float bestDistSq = float.MaxValue;
            int bestIdx = 0;

            for (int i = 0; i < TopHatterRoster.Length; i++)
            {
                if (!TopHatterRoster[i].IsActive) continue;

                float dSq = (TopHatterRoster[i].WorldPosition - CinemaLotWorldPosition).sqrMagnitude;
                if (dSq < bestDistSq)
                {
                    bestDistSq = dSq;
                    bestIdx = i;
                }
            }

            NearestActiveHeroIndex = bestIdx;
            NearestHeroDistanceMeters = Mathf.Sqrt(bestDistSq);

            TopHatterHeroNode winner = TopHatterRoster[bestIdx];
            ActiveMarqueeHeadline =
                $"🎬 WONDER CINEMA MARQUEE: [{winner.HeroName.ToUpperInvariant()}] — \"{winner.SignatureShowTitle}\" ({NearestHeroDistanceMeters:F1}m)";
            ActivePortalTargetStage = winner.PortalDestinationLabel;

            return ActiveMarqueeHeadline;
        }
    }
}
