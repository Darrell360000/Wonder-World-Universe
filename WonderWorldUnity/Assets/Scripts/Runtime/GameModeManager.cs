using UnityEngine;
using UnityEngine.Events;

namespace WonderWorld
{
    /// <summary>
    /// Swaps between the macro city-building overview (RTS / top-down perspective)
    /// and street-level brawler perspective (third-person character focus).
    /// </summary>
    public class GameModeManager : MonoBehaviour
    {
        public static GameModeManager Instance { get; private set; }

        public enum GameMode
        {
            CityBuilderMacro,
            StreetLevelBrawler
        }

        [Header("Current Mode")]
        public GameMode currentMode = GameMode.CityBuilderMacro;

        [Header("Camera References")]
        [Tooltip("Camera used for the macro city-building overview.")]
        public Camera macroCamera;

        [Tooltip("Camera used for the street-level brawler mode.")]
        public Camera brawlerCamera;

        [Header("Transition Settings")]
        [Tooltip("Speed at which the camera blends positions during transition.")]
        public float transitionSpeed = 5f;

        [Header("Target References")]
        [Tooltip("The transform the macro camera orbits/follows (e.g., City Center cursor).")]
        public Transform macroTarget;

        [Tooltip("The playable brawler character transform.")]
        public Transform brawlerPlayerTransform;

        [Header("Events")]
        public UnityEvent onMacroModeActivated;
        public UnityEvent onBrawlerModeActivated;

        private void Awake()
        {
            if (Instance == null)
            {
                Instance = this;
            }
            else
            {
                Destroy(gameObject);
                return;
            }
        }

        private void Start()
        {
            ApplyCameraStateInstant();
        }

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.Tab))
            {
                ToggleGameMode();
            }
        }

        public void ToggleGameMode()
        {
            SwitchToMode(currentMode == GameMode.CityBuilderMacro ? GameMode.StreetLevelBrawler : GameMode.CityBuilderMacro);
        }

        public void SwitchToMode(GameMode newMode)
        {
            currentMode = newMode;
            if (currentMode == GameMode.CityBuilderMacro)
            {
                if (macroCamera != null) macroCamera.gameObject.SetActive(true);
                if (brawlerCamera != null) brawlerCamera.gameObject.SetActive(false);
                onMacroModeActivated?.Invoke();
            }
            else
            {
                if (macroCamera != null) macroCamera.gameObject.SetActive(false);
                if (brawlerCamera != null) brawlerCamera.gameObject.SetActive(true);
                onBrawlerModeActivated?.Invoke();
            }
        }

        private void ApplyCameraStateInstant()
        {
            if (currentMode == GameMode.CityBuilderMacro)
            {
                if (macroCamera != null) macroCamera.gameObject.SetActive(true);
                if (brawlerCamera != null) brawlerCamera.gameObject.SetActive(false);
            }
            else
            {
                if (macroCamera != null) macroCamera.gameObject.SetActive(false);
                if (brawlerCamera != null) brawlerCamera.gameObject.SetActive(true);
            }
        }
    }
}
