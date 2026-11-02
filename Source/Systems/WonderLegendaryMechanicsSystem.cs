using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace WonderWorldCS2.Systems
{
    public sealed class WonderLegendaryMechanicsSystem : MonoBehaviour
    {
        private const float RailDetectionRadius = 2.5f;
        private const float RailSurgeMultiplier = 1.35f;
        private const float MaxRailSpeed = 65f;
        private const float PortalRadius = 1.5f;
        private const float PortalCooldownDuration = 0.75f;
        private const float RallyStrikeDamage = 25f;

        public static WonderLegendaryMechanicsSystem Instance { get; private set; }

        [Header("Zelda: Ascend and Ultra-Fuse")]
        public float maxAscendCeilingHeight = 50f;
        public bool isAscending;
        public GameObject fusedAttachment;

        [Header("Spider-Man: Neon Rail Grind")]
        public bool isRailGrinding;
        public float currentGrindSpeed = 30f;

        [Header("Portal Rifts")]
        public Vector3 cyanPortalPos;
        public Vector3 magentaPortalPos;
        public bool cyanPlaced;
        public bool magentaPlaced;

        [Header("Outer Wilds: Knowledge Ciphers")]
        public HashSet<string> DiscoveredFrequencies = new HashSet<string> { "FREQ_85BPM_EVON_HUB" };

        public bool IsAscending => isAscending;
        public bool IsRailGrinding => isRailGrinding;
        public float CurrentGrindSpeed => isRailGrinding ? currentGrindSpeed : 0f;
        public int ActivePortalCount => (cyanPlaced ? 1 : 0) + (magentaPlaced ? 1 : 0);
        public float EnemyPostureStrain
        {
            get
            {
                var spawner = SyndicateEnforcerSpawnerSystem.Instance;
                if (spawner == null) return 0f;

                float strain = 0f;
                foreach (SyndicateEnforcerEntity enforcer in spawner.ActiveEnforcers)
                {
                    if (enforcer != null)
                    {
                        strain = Mathf.Max(strain, enforcer.HarmonicStrain);
                    }
                }

                return Mathf.Clamp(strain, 0f, 100f);
            }
        }

        public int RallyBuffer => WonderHeroActionMechanics.Instance != null
            ? WonderHeroActionMechanics.Instance.rallyPotentialHealth
            : 0;

        private Transform _heroTransform;
        private CharacterController _characterController;
        private Rigidbody _heroBody;
        private Vector3 _railDirection;
        private Vector3 _momentum;
        private Vector3 _cyanPortalNormal = Vector3.forward;
        private Vector3 _magentaPortalNormal = Vector3.forward;
        private float _portalCooldown;
        private Coroutine _ascendRoutine;
        private bool _characterControllerWasEnabled;
        private Rigidbody _fusedBody;
        private bool _fusedBodyWasKinematic;

        public string AscendStatus => isAscending ? "ASCENDING" : "READY";

        public string GrindStatus => isRailGrinding
            ? string.Format(System.Globalization.CultureInfo.InvariantCulture, "{0:F1} m/s", currentGrindSpeed)
            : "OFF";

        public string PortalStatus => string.Format(
            System.Globalization.CultureInfo.InvariantCulture,
            "Cyan {0} | Magenta {1}",
            cyanPlaced ? "SET" : "—",
            magentaPlaced ? "SET" : "—");

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }

            Instance = this;
            ResolveHeroTransform();
        }

        private void Update()
        {
            ResolveHeroTransform();
            if (_heroTransform == null) return;

            UpdateAscend();
            UpdateUltraFuse();
            UpdateRailGrind();
            UpdatePortals();
            UpdateKnowledgeWeb();
            UpdateCombatRhythm();
        }

        private void OnDestroy()
        {
            if (_ascendRoutine != null)
            {
                StopCoroutine(_ascendRoutine);
                RestoreCharacterController();
            }

            if (fusedAttachment != null && _heroTransform != null &&
                fusedAttachment.transform.IsChildOf(_heroTransform))
            {
                fusedAttachment.transform.SetParent(null, true);
            }
            RestoreFusedBody();

            if (Instance == this)
            {
                Instance = null;
            }
        }

        private void ResolveHeroTransform()
        {
            Camera mainCamera = Camera.main;
            bool cameraChanged = _heroTransform != null &&
                _heroTransform.GetComponent<Camera>() != null &&
                mainCamera != null &&
                _heroTransform != mainCamera.transform;
            if (_heroTransform != null && _heroTransform != transform && !cameraChanged)
            {
                return;
            }

            if (_characterController == null)
            {
                _characterController = GetComponent<CharacterController>();
            }

            if (_characterController != null)
            {
                _heroTransform = _characterController.transform;
                _heroBody = _heroTransform.GetComponent<Rigidbody>();
                return;
            }

            if (mainCamera != null)
            {
                _heroTransform = mainCamera.transform;
                _heroBody = _heroTransform.GetComponentInParent<Rigidbody>();
                return;
            }

            if (_heroTransform == null)
            {
                _heroTransform = transform;
                _heroBody = GetComponent<Rigidbody>();
            }
        }

        private void UpdateAscend()
        {
            if (!Input.GetKeyDown(KeyCode.C) || isAscending) return;

            Vector3 origin = _heroTransform.position;
            if (!Physics.Raycast(origin, Vector3.up, out RaycastHit hit, Mathf.Max(0f, maxAscendCeilingHeight)))
            {
                return;
            }

            _ascendRoutine = StartCoroutine(AscendRoutine(hit.point + Vector3.up * 4f));
        }

        private IEnumerator AscendRoutine(Vector3 destination)
        {
            isAscending = true;
            _characterControllerWasEnabled = _characterController != null && _characterController.enabled;
            if (_characterControllerWasEnabled)
            {
                _characterController.enabled = false;
            }

            Vector3 start = _heroTransform.position;
            float elapsed = 0f;
            float duration = 60f / 85f;
            while (elapsed < duration && _heroTransform != null)
            {
                elapsed += Time.unscaledDeltaTime;
                _heroTransform.position = Vector3.Lerp(start, destination, Mathf.Clamp01(elapsed / duration));
                yield return null;
            }

            if (_heroTransform != null)
            {
                _heroTransform.position = destination;
            }

            if (_characterControllerWasEnabled && _characterController != null)
            {
                _characterController.enabled = true;
            }

            _characterControllerWasEnabled = false;
            _ascendRoutine = null;
            isAscending = false;
            Debug.Log("[WonderLegendary] Ascend completed through the ceiling.");
        }

        private void RestoreCharacterController()
        {
            if (_characterController != null && _characterControllerWasEnabled)
            {
                _characterController.enabled = true;
            }

            _characterControllerWasEnabled = false;
            _ascendRoutine = null;
            isAscending = false;
        }

        private void UpdateUltraFuse()
        {
            if (!Input.GetKeyDown(KeyCode.G)) return;

            if (fusedAttachment != null)
            {
                fusedAttachment.transform.SetParent(null, true);
                RestoreFusedBody();
                Debug.Log($"[WonderLegendary] Unfused {fusedAttachment.name}.");
                fusedAttachment = null;
                return;
            }

            Collider[] nearby = Physics.OverlapSphere(_heroTransform.position, 4f);
            Collider nearest = null;
            float nearestDistance = float.MaxValue;
            foreach (Collider candidate in nearby)
            {
                if (!CanFuse(candidate)) continue;

                float distance = (candidate.ClosestPoint(_heroTransform.position) - _heroTransform.position).sqrMagnitude;
                if (distance < nearestDistance)
                {
                    nearest = candidate;
                    nearestDistance = distance;
                }
            }

            if (nearest == null) return;

            fusedAttachment = nearest.attachedRigidbody != null
                ? nearest.attachedRigidbody.gameObject
                : nearest.gameObject;
            _fusedBody = fusedAttachment.GetComponent<Rigidbody>();
            if (_fusedBody != null)
            {
                _fusedBodyWasKinematic = _fusedBody.isKinematic;
                _fusedBody.isKinematic = true;
            }
            fusedAttachment.transform.SetParent(_heroTransform, false);
            fusedAttachment.transform.localPosition = new Vector3(0.65f, 0.55f, 0.8f);
            fusedAttachment.transform.localRotation = Quaternion.identity;
            Debug.Log($"[WonderLegendary] Ultra-Fused {fusedAttachment.name} to the hero.");
        }

        private void RestoreFusedBody()
        {
            if (_fusedBody != null)
            {
                _fusedBody.isKinematic = _fusedBodyWasKinematic;
            }

            _fusedBody = null;
            _fusedBodyWasKinematic = false;
        }

        private bool CanFuse(Collider candidate)
        {
            if (candidate == null || candidate.isTrigger ||
                candidate.transform == _heroTransform || candidate.transform.IsChildOf(_heroTransform) ||
                candidate.transform.root == _heroTransform.root)
            {
                return false;
            }

            if (candidate.attachedRigidbody != null) return true;

            string tag = candidate.gameObject.tag;
            string objectName = candidate.gameObject.name;
            return tag == "Fuseable" ||
                   objectName.IndexOf("fuseable", StringComparison.OrdinalIgnoreCase) >= 0 ||
                   objectName.IndexOf("fusable", StringComparison.OrdinalIgnoreCase) >= 0;
        }

        private void UpdateRailGrind()
        {
            if (!isRailGrinding)
            {
                TryStartRailGrind();
                if (!isRailGrinding) return;
            }

            if (Input.GetKeyDown(KeyCode.Space))
            {
                currentGrindSpeed = Mathf.Min(MaxRailSpeed, currentGrindSpeed * RailSurgeMultiplier);
                Debug.Log($"[WonderLegendary] 85 BPM rail surge: {currentGrindSpeed:F1} m/s.");
            }

            if (Input.GetKeyDown(KeyCode.LeftControl))
            {
                isRailGrinding = false;
                _momentum = _railDirection * currentGrindSpeed;
                Debug.Log("[WonderLegendary] Dismounted the neon rail.");
                return;
            }

            float deltaTime = Time.unscaledDeltaTime;
            Vector3 movement = _railDirection * currentGrindSpeed * deltaTime;
            if (_characterController != null && _characterController.enabled)
            {
                _characterController.Move(movement);
            }
            else
            {
                _heroTransform.position += movement;
            }

            _momentum = _railDirection * currentGrindSpeed;
        }

        private void TryStartRailGrind()
        {
            Collider[] nearby = Physics.OverlapSphere(_heroTransform.position, RailDetectionRadius);
            foreach (Collider candidate in nearby)
            {
                if (!IsRail(candidate)) continue;

                Vector3 direction = Vector3.ProjectOnPlane(_heroTransform.forward, Vector3.up).normalized;
                if (direction.sqrMagnitude < 0.001f)
                {
                    direction = Vector3.forward;
                }

                _railDirection = direction;
                currentGrindSpeed = Mathf.Max(1f, currentGrindSpeed);
                isRailGrinding = true;
                Debug.Log($"[WonderLegendary] Neon rail grind started on {candidate.name}.");
                return;
            }
        }

        private static bool IsRail(Collider candidate)
        {
            if (candidate == null || candidate.isTrigger) return false;

            string tag = candidate.gameObject.tag;
            return tag == "Rail" || tag == "NeonRail" ||
                   candidate.gameObject.name.IndexOf("rail", StringComparison.OrdinalIgnoreCase) >= 0;
        }

        private void UpdatePortals()
        {
            if (Input.GetKeyDown(KeyCode.Q))
            {
                PlacePortal(true);
            }

            if (Input.GetKeyDown(KeyCode.E) &&
                !Input.GetKey(KeyCode.LeftControl) &&
                !Input.GetKey(KeyCode.RightControl))
            {
                PlacePortal(false);
            }

            _portalCooldown = Mathf.Max(0f, _portalCooldown - Time.unscaledDeltaTime);
            if (_portalCooldown > 0f || !cyanPlaced || !magentaPlaced) return;

            if (Vector3.Distance(_heroTransform.position, cyanPortalPos) < PortalRadius)
            {
                TravelThroughPortal(cyanPortalPos, _cyanPortalNormal, magentaPortalPos, _magentaPortalNormal);
            }
            else if (Vector3.Distance(_heroTransform.position, magentaPortalPos) < PortalRadius)
            {
                TravelThroughPortal(magentaPortalPos, _magentaPortalNormal, cyanPortalPos, _cyanPortalNormal);
            }
        }

        private void PlacePortal(bool cyan)
        {
            Camera aimCamera = Camera.main;
            Transform aim = aimCamera != null ? aimCamera.transform : _heroTransform;
            if (aim == null || !Physics.Raycast(aim.position, aim.forward, out RaycastHit hit, 100f))
            {
                return;
            }

            if (cyan)
            {
                cyanPortalPos = hit.point + hit.normal * 0.5f;
                _cyanPortalNormal = hit.normal;
                cyanPlaced = true;
                Debug.Log("[WonderLegendary] Cyan portal placed.");
            }
            else
            {
                magentaPortalPos = hit.point + hit.normal * 0.5f;
                _magentaPortalNormal = hit.normal;
                magentaPlaced = true;
                Debug.Log("[WonderLegendary] Magenta portal placed.");
            }
        }

        private void TravelThroughPortal(Vector3 entry, Vector3 entryNormal, Vector3 exit, Vector3 exitNormal)
        {
            Quaternion rotation = Quaternion.FromToRotation(-entryNormal, exitNormal);
            Vector3 exitPosition = exit + exitNormal * 2f;
            Vector3 rotatedMomentum = rotation * GetMomentum();

            if (_characterController != null && _characterController.enabled)
            {
                _characterController.enabled = false;
                _heroTransform.SetPositionAndRotation(exitPosition, rotation * _heroTransform.rotation);
                _characterController.enabled = true;
            }
            else
            {
                _heroTransform.SetPositionAndRotation(exitPosition, rotation * _heroTransform.rotation);
            }

            if (_heroBody != null)
            {
                _heroBody.velocity = rotatedMomentum;
            }

            _momentum = rotatedMomentum;
            _portalCooldown = PortalCooldownDuration;
            Debug.Log($"[WonderLegendary] Portal momentum conserved from {entry} to {exit}.");
        }

        private Vector3 GetMomentum()
        {
            if (_heroBody != null)
            {
                return _heroBody.velocity;
            }

            return _momentum.sqrMagnitude > 0.01f
                ? _momentum
                : _heroTransform.forward * Mathf.Max(1f, currentGrindSpeed);
        }

        private void UpdateKnowledgeWeb()
        {
            if (!Input.GetKeyDown(KeyCode.J)) return;

            Debug.Log("=== OUTER WILDS FREQUENCY LOG ===");
            if (DiscoveredFrequencies.Count == 0)
            {
                Debug.Log("[WonderLegendary] No acoustic ciphers discovered yet.");
                return;
            }

            foreach (string frequency in DiscoveredFrequencies)
            {
                Debug.Log($"[WonderLegendary] Discovered frequency: {frequency}");
            }
        }

        public void DiscoverFrequency(string frequency)
        {
            if (string.IsNullOrWhiteSpace(frequency)) return;
            if (DiscoveredFrequencies.Add(frequency))
            {
                Debug.Log($"[WonderLegendary] New acoustic cipher discovered: {frequency}");
            }
        }

        private void UpdateCombatRhythm()
        {
            if (!Input.GetMouseButtonDown(0) || Input.GetMouseButton(1)) return;

            WonderHeroActionMechanics hero = WonderHeroActionMechanics.Instance;
            if (hero == null) return;

            Collider[] nearby = Physics.OverlapSphere(_heroTransform.position + _heroTransform.forward, 2f);
            foreach (Collider candidate in nearby)
            {
                SyndicateEnforcerEntity enforcer = candidate.GetComponentInParent<SyndicateEnforcerEntity>();
                if (enforcer == null || enforcer.Health <= 0) continue;

                enforcer.TakeWonderDamage(25, gameObject);
                hero.RegisterHit(enforcer.gameObject, 25);
                break;
            }
        }

        public void RegisterDamageTaken(int damage)
        {
            WonderHeroActionMechanics.Instance?.TakeHeroDamage(damage);
        }
    }
}
