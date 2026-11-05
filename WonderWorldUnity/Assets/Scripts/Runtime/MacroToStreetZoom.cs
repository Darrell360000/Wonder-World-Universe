using UnityEngine;
using System.Collections;

namespace WonderWorld
{
    /// <summary>
    /// Raycasts from mouse cursor when clicking a building in macro view,
    /// smoothly zooming the camera down to street-level brawler mode.
    /// </summary>
    public class MacroToStreetZoom : MonoBehaviour
    {
        [Header("Camera References")]
        public Camera macroCamera;
        public Camera brawlerCamera;

        [Header("Raycast Settings")]
        public LayerMask buildingLayerMask;
        public KeyCode clickKey = KeyCode.Mouse0;

        [Header("Transition Settings")]
        public float zoomDuration = 1.5f;

        [Tooltip("Offset distance from the clicked building when arriving at street level.")]
        public Vector3 streetOffset = new Vector3(0f, 3f, -5f);

        private bool isZooming = false;

        void Update()
        {
            if (!isZooming && macroCamera != null && macroCamera.gameObject.activeSelf && Input.GetKeyDown(clickKey))
            {
                TrySelectBuildingAndZoom();
            }
        }

        void TrySelectBuildingAndZoom()
        {
            Ray ray = macroCamera.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, Mathf.Infinity, buildingLayerMask))
            {
                Transform clickedBuilding = hit.transform;
                Vector3 targetPosition = clickedBuilding.position + streetOffset;
                Quaternion targetRotation = Quaternion.LookRotation(clickedBuilding.position - targetPosition);
                StartCoroutine(SmoothZoomTransition(targetPosition, targetRotation));
            }
        }

        IEnumerator SmoothZoomTransition(Vector3 targetPos, Quaternion targetRot)
        {
            isZooming = true;
            if (brawlerCamera != null && macroCamera != null)
            {
                brawlerCamera.transform.position = macroCamera.transform.position;
                brawlerCamera.transform.rotation = macroCamera.transform.rotation;
                brawlerCamera.gameObject.SetActive(true);
                macroCamera.gameObject.SetActive(false);

                float elapsedTime = 0f;
                Vector3 startingPos = brawlerCamera.transform.position;
                Quaternion startingRot = brawlerCamera.transform.rotation;

                while (elapsedTime < zoomDuration)
                {
                    elapsedTime += Time.deltaTime;
                    float t = Mathf.SmoothStep(0f, 1f, elapsedTime / zoomDuration);
                    brawlerCamera.transform.position = Vector3.Lerp(startingPos, targetPos, t);
                    brawlerCamera.transform.rotation = Quaternion.Slerp(startingRot, targetRot, t);
                    yield return null;
                }

                brawlerCamera.transform.position = targetPos;
                brawlerCamera.transform.rotation = targetRot;
            }

            isZooming = false;
            if (GameModeManager.Instance != null)
            {
                GameModeManager.Instance.SwitchToMode(GameModeManager.GameMode.StreetLevelBrawler);
            }
        }
    }
}
