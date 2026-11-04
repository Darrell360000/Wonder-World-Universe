using System.Collections;
using UnityEngine;

namespace WonderWorld
{
    public enum WonderCameraMode { Strategy, Brawler }

    [DisallowMultipleComponent]
    public sealed class WonderCameraDirector : MonoBehaviour
    {
        [SerializeField] private Camera strategyCamera;
        [SerializeField] private Camera brawlerCamera;
        [SerializeField, Min(0f)] private float blendSeconds = 0.35f;

        public WonderCameraMode ActiveMode { get; private set; } = WonderCameraMode.Brawler;
        public bool IsBlending { get; private set; }

        private Coroutine blendRoutine;
        private float lowerDepth;
        private float upperDepth;
public static WonderCameraDirector Instance { get; private set; }

        private void Reset()
        {
            strategyCamera = transform.Find("StrategyCamera")?.GetComponent<Camera>();
            brawlerCamera = transform.Find("BrawlerCamera")?.GetComponent<Camera>();
        }

        private void Awake()
        {
            Instance = this;
            CacheDepths();
        }

        private void OnEnable()
        {
            if (HasValidCameras())
                ApplyModeImmediately(ActiveMode);
        }

        private void OnDisable()
        {
            if (blendRoutine != null)
                StopCoroutine(blendRoutine);
            blendRoutine = null;
            IsBlending = false;

            if (HasValidCameras())
                ApplyModeImmediately(ActiveMode);
        }

        public void SwitchMode(WonderCameraMode mode, bool instant = false)
        {
            if (mode != WonderCameraMode.Strategy && mode != WonderCameraMode.Brawler)
            {
                Debug.LogError("WonderCameraDirector received an invalid camera mode.", this);
                return;
            }
            if (!HasValidCameras())
            {
                Debug.LogError("WonderCameraDirector requires two distinct Strategy and Brawler cameras.", this);
                return;
            }
            if (!isActiveAndEnabled)
            {
                ActiveMode = mode;
                return;
            }

            if (blendRoutine != null)
                StopCoroutine(blendRoutine);
            blendRoutine = null;
            IsBlending = false;

            Camera from = mode == WonderCameraMode.Strategy ? brawlerCamera : strategyCamera;
            Camera to = mode == WonderCameraMode.Strategy ? strategyCamera : brawlerCamera;
            from.gameObject.SetActive(true);
            to.gameObject.SetActive(true);
            from.rect = new Rect(0f, 0f, 1f, 1f);
            to.rect = new Rect(0f, 0f, 1f, 1f);
            from.depth = lowerDepth;
            to.depth = upperDepth;
            ActiveMode = mode;

            float duration = instant ? 0f : Mathf.Max(0f, blendSeconds);
            if (duration <= 0f)
            {
                FinishTransition(from, to);
                return;
            }

            to.rect = new Rect(0f, 0f, 0f, 1f);
            blendRoutine = StartCoroutine(Blend(from, to, duration));
        }

        public void ToggleCamera() => SwitchMode(ActiveMode == WonderCameraMode.Strategy
            ? WonderCameraMode.Brawler : WonderCameraMode.Strategy);

        private void CacheDepths()
        {
            if (!HasValidCameras())
                return;
            lowerDepth = Mathf.Min(strategyCamera.depth, brawlerCamera.depth);
            upperDepth = Mathf.Max(strategyCamera.depth, brawlerCamera.depth);
            if (Mathf.Approximately(lowerDepth, upperDepth))
                upperDepth = lowerDepth + 1f;
        }

        private bool HasValidCameras()
        {
            return strategyCamera != null && brawlerCamera != null
                && strategyCamera != brawlerCamera
                && strategyCamera.gameObject != brawlerCamera.gameObject;
        }

        private void ApplyModeImmediately(WonderCameraMode mode)
        {
            Camera active = mode == WonderCameraMode.Strategy ? strategyCamera : brawlerCamera;
            Camera inactive = mode == WonderCameraMode.Strategy ? brawlerCamera : strategyCamera;
            active.depth = upperDepth;
            inactive.depth = lowerDepth;
            active.rect = new Rect(0f, 0f, 1f, 1f);
            inactive.rect = new Rect(0f, 0f, 1f, 1f);
            active.gameObject.SetActive(true);
            inactive.gameObject.SetActive(false);
            ActiveMode = mode;
            IsBlending = false;
            blendRoutine = null;
        }

        private IEnumerator Blend(Camera from, Camera to, float duration)
        {
            IsBlending = true;
            float elapsed = 0f;
            while (elapsed < duration)
            {
                elapsed += Time.unscaledDeltaTime;
                float t = Mathf.Clamp01(elapsed / duration);
                to.rect = new Rect(0f, 0f, Mathf.SmoothStep(0f, 1f, t), 1f);
                yield return null;
            }

            FinishTransition(from, to);
        }

        private void FinishTransition(Camera from, Camera to)
        {
            to.rect = new Rect(0f, 0f, 1f, 1f);
            from.rect = new Rect(0f, 0f, 1f, 1f);
            from.gameObject.SetActive(false);
            IsBlending = false;
            blendRoutine = null;
        }
    }
}
