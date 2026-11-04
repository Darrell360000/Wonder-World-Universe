using System;
using UnityEngine;
using UnityEngine.Events;

namespace WonderWorld
{
    public enum TopHatterHeroSlot
    {
        Slot1_DarrellXGen = 0,
        Slot2_SpiderPulseHeroine = 1,
        Slot3_DivineHandsTopHatter = 2,
        Slot4_JaxsonIronRiffThorne = 3,
        Slot5_DimensionRiftOrVesperOrTao = 4
    }

    /// <summary>
    /// Manages hot-swapping between the 5 Playable Top-Hatter Heroes (Keys 1-5) at the exact same street coordinates
    /// for tag-team combos and the 3-Phase Syndicate Overlord Boss Encounter.
    /// </summary>
    [DisallowMultipleComponent]
    public sealed class HeroManager : MonoBehaviour
    {
        [Header("5-Hero Top-Hatter Squad Prefabs / Instances")]
        public GameObject[] Heroes = new GameObject[5];

        [Header("Active Squad State")]
        public int ActiveHeroIndex = 0;

        public UnityEvent<int, string> OnHeroHotSwapped;

        private static readonly string[] HeroNames = new[]
        {
            "Darrell X-Gen (The Master of Sound Magic)",
            "Spider-Pulse Heroine (The Acrobatic Scout)",
            "The Divine Hands Top-Hatter (Reality Shaper)",
            "Jaxson 'Iron Riff' Thorne (Heavy Industrial Brawler)",
            "Vesper Vance / Dimension-Rift Maestro / Tao X Zen"
        };

        public string GetActiveHeroName() =>
            (ActiveHeroIndex >= 0 && ActiveHeroIndex < HeroNames.Length) ? HeroNames[ActiveHeroIndex] : "Unknown Hero";

        public void HotSwapToHero(int slotIndex)
        {
            if (slotIndex < 0 || slotIndex >= 5 || Heroes == null || slotIndex >= Heroes.Length)
            {
                return;
            }

            Vector3 currentPos = transform.position;
            Quaternion currentRot = transform.rotation;

            if (ActiveHeroIndex >= 0 && ActiveHeroIndex < Heroes.Length && Heroes[ActiveHeroIndex] != null)
            {
                currentPos = Heroes[ActiveHeroIndex].transform.position;
                currentRot = Heroes[ActiveHeroIndex].transform.rotation;
                Heroes[ActiveHeroIndex].SetActive(false);
            }

            ActiveHeroIndex = slotIndex;

            if (Heroes[ActiveHeroIndex] != null)
            {
                Heroes[ActiveHeroIndex].transform.position = currentPos;
                Heroes[ActiveHeroIndex].transform.rotation = currentRot;
                Heroes[ActiveHeroIndex].SetActive(true);
            }

            OnHeroHotSwapped?.Invoke(ActiveHeroIndex, GetActiveHeroName());
        }
    }
}
