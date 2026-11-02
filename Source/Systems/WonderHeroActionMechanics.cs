using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace WonderWorldCS2.Systems
{
    public interface IWonderHeroDamageable
    {
        void TakeWonderDamage(int damage, GameObject source);
    }

    public interface IWonderHeroPostureTarget
    {
        void TakePostureDamage(float damage);
        void RegisterAttackDeflection();
    }

    public sealed class WonderHeroActionMechanics : MonoBehaviour
    {
        private enum SonicDiscState
        {
            Equipped,
            Thrown,
            Recalling
        }

        public static WonderHeroActionMechanics Instance { get; private set; }

        [Header("Bullet Time (Harmonic Dilation)")]
        public float bulletTimeScale = 0.25f;
        public float bulletTimeEnergy = 100f;
        public float bulletTimeDrainPerSecond = 15f;
        public float bulletTimeRechargePerSecond = 10f;

        [Header("Sonic Disc")]
        public GameObject sonicDiscPrefab;
        public Transform gauntletSocket;
        public float recallSpeed = 45f;
        public float discThrowSpeed = 35f;
        public int discDamage = 150;

        [Header("85 BPM Freeflow Combat")]
        public float counterWindowDuration = 0.35f;
        public float comboDuration = 3.5f;

        [Header("Sekiro Posture / Harmonic Strain")]
        public float playerPosture;
        public float maxPosture = 100f;
        public float perfectParryWindow = 0.22f;

        [Header("Bloodborne Bio-Rally")]
        public int currentHealth = 100;
        public int maxHealth = 100;
        public int rallyPotentialHealth;
        public float rallyWindowTimer;
        public float maxRallyDuration = 4f * 60f / 85f;

        private readonly HashSet<int> _hitTargetIds = new HashSet<int>();
        private GameObject _activeDisc;
        private Rigidbody _activeDiscBody;
        private SonicDiscState _discState = SonicDiscState.Equipped;
        private float _counterWindowRemaining;
        private GameObject _counterTarget;
        private float _comboRemaining;
        private float _timeScaleBeforeBulletTime = 1f;
        private float _fixedDeltaTimeBeforeBulletTime = 0.02f;
        private float _bulletTimeAppliedScale;
        private float _bulletTimeAppliedFixedDeltaTime;
        private int _counterIncomingDamage = SyndicateEnforcerEntity.AttackDamage;

        public bool IsBulletTimeActive { get; private set; }
        public float BulletTimeEnergyPercent => Mathf.Clamp(bulletTimeEnergy, 0f, 100f);
        public float PlayerPosturePercent => maxPosture > 0f ? Mathf.Clamp(playerPosture / maxPosture * 100f, 0f, 100f) : 0f;
        public float CurrentHealthPercent => maxHealth > 0 ? Mathf.Clamp((float)currentHealth / maxHealth * 100f, 0f, 100f) : 0f;
        public float RallyPotentialPercent => maxHealth > 0 ? Mathf.Clamp((float)rallyPotentialHealth / maxHealth * 100f, 0f, 100f) : 0f;
        public float DepletedHealthPercent => Mathf.Max(0f, 100f - CurrentHealthPercent - RallyPotentialPercent);
        public int ComboCounter { get; private set; }
        public bool CounterWindowOpen => _counterWindowRemaining > 0f;
        public float CounterWindowRemaining => _counterWindowRemaining;
        public bool IsDiscThrown => _discState != SonicDiscState.Equipped;

        public string SonicDiscStatus
        {
            get
            {
                switch (_discState)
                {
                    case SonicDiscState.Thrown:
                        return "Thrown";
                    case SonicDiscState.Recalling:
                        return "Recalling";
                    default:
                        return "Equipped";
                }
            }
        }

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(this);
                return;
            }

            Instance = this;
            bulletTimeEnergy = Mathf.Clamp(bulletTimeEnergy, 0f, 100f);
            maxHealth = Mathf.Max(1, maxHealth);
            currentHealth = Mathf.Clamp(currentHealth, 0, maxHealth);
            rallyPotentialHealth = Mathf.Clamp(rallyPotentialHealth, 0, maxHealth - currentHealth);
            maxPosture = Mathf.Max(1f, maxPosture);
            playerPosture = Mathf.Clamp(playerPosture, 0f, maxPosture);
            maxRallyDuration = Mathf.Max(0.1f, maxRallyDuration);
        }

        private void Update()
        {
            HandleBulletTimeInput();
            HandleDiscInput();
            HandleFreeflowCombatTiming();
            UpdateRallyDecay();

            if (_discState == SonicDiscState.Thrown && _activeDisc != null)
            {
                ApplyDiscCollisionDamage(_activeDisc.transform.position, 1.0f);
            }
        }

        private void OnDestroy()
        {
            StopBulletTime();

            if (_activeDisc != null)
            {
                Destroy(_activeDisc);
            }

            if (Instance == this)
            {
                Instance = null;
            }
        }

        private void HandleBulletTimeInput()
        {
            if (Input.GetKeyDown(KeyCode.LeftShift) || Input.GetKeyDown(KeyCode.RightShift))
            {
                if (IsBulletTimeActive)
                {
                    StopBulletTime();
                }
                else if (bulletTimeEnergy > 10f && Time.timeScale > 0f)
                {
                    StartBulletTime();
                }
            }

            if (IsBulletTimeActive)
            {
                bulletTimeEnergy = Mathf.Max(0f, bulletTimeEnergy - Time.unscaledDeltaTime * bulletTimeDrainPerSecond);
                if (bulletTimeEnergy <= 0f)
                {
                    StopBulletTime();
                }
            }
            else
            {
                bulletTimeEnergy = Mathf.Min(100f, bulletTimeEnergy + Time.unscaledDeltaTime * bulletTimeRechargePerSecond);
            }
        }

        private void StartBulletTime()
        {
            _timeScaleBeforeBulletTime = Time.timeScale;
            _fixedDeltaTimeBeforeBulletTime = Time.fixedDeltaTime;
            _bulletTimeAppliedScale = _timeScaleBeforeBulletTime * Mathf.Clamp(bulletTimeScale, 0.05f, 1f);
            _bulletTimeAppliedFixedDeltaTime = _fixedDeltaTimeBeforeBulletTime * Mathf.Clamp(bulletTimeScale, 0.05f, 1f);
            IsBulletTimeActive = true;
            Time.timeScale = _bulletTimeAppliedScale;
            Time.fixedDeltaTime = _bulletTimeAppliedFixedDeltaTime;
        }

        private void StopBulletTime()
        {
            if (!IsBulletTimeActive) return;

            IsBulletTimeActive = false;
            if (Mathf.Approximately(Time.timeScale, _bulletTimeAppliedScale))
            {
                Time.timeScale = _timeScaleBeforeBulletTime;
            }

            if (Mathf.Approximately(Time.fixedDeltaTime, _bulletTimeAppliedFixedDeltaTime))
            {
                Time.fixedDeltaTime = _fixedDeltaTimeBeforeBulletTime;
            }
        }

        private void HandleDiscInput()
        {
            if (_discState == SonicDiscState.Equipped &&
                Input.GetMouseButtonDown(0) &&
                Input.GetMouseButton(1))
            {
                ThrowSonicDisc();
            }

            if (_discState == SonicDiscState.Thrown && Input.GetKeyDown(KeyCode.R))
            {
                StartCoroutine(RecallDiscRoutine());
            }
        }

        private void ThrowSonicDisc()
        {
            Camera aimCamera = GetAimCamera();
            Vector3 socketPosition = GetSocketPosition(aimCamera);
            Quaternion rotation = aimCamera != null ? aimCamera.transform.rotation : transform.rotation;

            _activeDisc = sonicDiscPrefab != null
                ? Instantiate(sonicDiscPrefab, socketPosition, rotation)
                : CreateFallbackDisc(socketPosition, rotation);

            _activeDisc.name = "WonderWorld_SonicDisc";
            _activeDiscBody = _activeDisc.GetComponent<Rigidbody>();
            if (_activeDiscBody == null)
            {
                _activeDiscBody = _activeDisc.AddComponent<Rigidbody>();
            }

            if (_activeDisc.GetComponent<Collider>() == null)
            {
                _activeDisc.AddComponent<SphereCollider>();
            }

            _activeDiscBody.isKinematic = false;
            _activeDiscBody.useGravity = false;
            _activeDiscBody.collisionDetectionMode = CollisionDetectionMode.ContinuousDynamic;
            _activeDiscBody.velocity = Vector3.zero;
            _hitTargetIds.Clear();

            Ray aimRay = aimCamera != null
                ? aimCamera.ScreenPointToRay(Input.mousePosition)
                : new Ray(socketPosition, transform.forward);
            Vector3 targetPoint = aimRay.GetPoint(50f);
            Vector3 throwDirection = (targetPoint - socketPosition).normalized;
            if (throwDirection.sqrMagnitude < 0.001f)
            {
                throwDirection = transform.forward;
            }

            _activeDiscBody.velocity = throwDirection * Mathf.Max(1f, discThrowSpeed);
            _activeDiscBody.angularVelocity = throwDirection * 12f;
            _discState = SonicDiscState.Thrown;
        }

        private static GameObject CreateFallbackDisc(Vector3 position, Quaternion rotation)
        {
            GameObject disc = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            disc.transform.SetPositionAndRotation(position, rotation);
            disc.transform.localScale = new Vector3(0.35f, 0.06f, 0.35f);
            return disc;
        }

        private IEnumerator RecallDiscRoutine()
        {
            _discState = SonicDiscState.Recalling;
            if (_activeDiscBody != null)
            {
                _activeDiscBody.velocity = Vector3.zero;
                _activeDiscBody.angularVelocity = Vector3.zero;
                _activeDiscBody.isKinematic = true;
            }

            while (_activeDisc != null)
            {
                Vector3 socketPosition = GetSocketPosition(GetAimCamera());
                if (Vector3.Distance(_activeDisc.transform.position, socketPosition) <= 1.5f)
                {
                    break;
                }

                _activeDisc.transform.position = Vector3.MoveTowards(
                    _activeDisc.transform.position,
                    socketPosition,
                    Mathf.Max(1f, recallSpeed) * Time.unscaledDeltaTime);
                ApplyDiscCollisionDamage(_activeDisc.transform.position, 1.2f);
                yield return null;
            }

            if (_activeDisc != null)
            {
                Destroy(_activeDisc);
            }

            _activeDisc = null;
            _activeDiscBody = null;
            _discState = SonicDiscState.Equipped;
        }

        private void ApplyDiscCollisionDamage(Vector3 position, float radius)
        {
            Collider[] hits = Physics.OverlapSphere(position, radius);
            foreach (Collider hit in hits)
            {
                if (hit == null || _activeDisc == null || hit.transform == _activeDisc.transform ||
                    hit.transform.IsChildOf(_activeDisc.transform))
                {
                    continue;
                }

                IWonderHeroDamageable damageable = FindDamageable(hit);
                if (damageable == null && !HasEnemyTag(hit.transform))
                {
                    continue;
                }

                GameObject target = hit.attachedRigidbody != null
                    ? hit.attachedRigidbody.gameObject
                    : hit.transform.root.gameObject;
                if (!_hitTargetIds.Add(target.GetInstanceID()))
                {
                    continue;
                }

                if (damageable != null)
                {
                    damageable.TakeWonderDamage(discDamage, gameObject);
                }
                else
                {
                    target.SendMessageUpwards("TakeDamage", discDamage, SendMessageOptions.DontRequireReceiver);
                }

                RegisterHit(target, discDamage);
            }
        }

        private static IWonderHeroDamageable FindDamageable(Collider hit)
        {
            MonoBehaviour[] behaviours = hit.GetComponentsInParent<MonoBehaviour>(true);
            foreach (MonoBehaviour behaviour in behaviours)
            {
                if (behaviour is IWonderHeroDamageable damageable)
                {
                    return damageable;
                }
            }

            return null;
        }

        private static bool HasEnemyTag(Transform current)
        {
            while (current != null)
            {
                string tag = current.gameObject.tag;
                if (tag == "Enemy" || tag == "Nemesis")
                {
                    return true;
                }

                current = current.parent;
            }

            return false;
        }

        private Camera GetAimCamera()
        {
            if (Camera.main != null) return Camera.main;

            Camera[] cameras = Camera.allCameras;
            foreach (Camera camera in cameras)
            {
                if (camera != null && camera.enabled) return camera;
            }

            return null;
        }

        private Vector3 GetSocketPosition(Camera aimCamera)
        {
            if (gauntletSocket != null) return gauntletSocket.position;
            if (aimCamera != null) return aimCamera.transform.position + aimCamera.transform.forward * 0.75f;
            return transform.position + transform.forward * 0.75f;
        }

        private void HandleFreeflowCombatTiming()
        {
            float deltaTime = Time.unscaledDeltaTime;
            if (_comboRemaining > 0f)
            {
                _comboRemaining = Mathf.Max(0f, _comboRemaining - deltaTime);
                if (_comboRemaining <= 0f)
                {
                    ComboCounter = 0;
                }
            }

            if (_counterWindowRemaining > 0f)
            {
                _counterWindowRemaining = Mathf.Max(0f, _counterWindowRemaining - deltaTime);
                if (_counterWindowRemaining > 0f && Input.GetMouseButtonDown(1) && !Input.GetMouseButton(0))
                {
                    TryDeflectEnemyAttack(_counterTarget, _counterIncomingDamage);
                }

                if (_counterWindowRemaining <= 0f)
                {
                    _counterTarget = null;
                }
            }
        }

        public void OpenCounterWindow()
        {
            OpenCounterWindow(null, counterWindowDuration);
        }

        public void OpenCounterWindow(GameObject attacker, float duration)
        {
            OpenCounterWindow(attacker, duration, SyndicateEnforcerEntity.AttackDamage);
        }

        public void OpenCounterWindow(GameObject attacker, float duration, int incomingDamage)
        {
            _counterTarget = attacker;
            _counterWindowRemaining = Mathf.Max(0.05f, duration);
            _counterIncomingDamage = Mathf.Max(0, incomingDamage);
        }

        public bool TryDeflectEnemyAttack(GameObject enemy, int incomingDamage)
        {
            if (enemy == null ||
                enemy != _counterTarget ||
                _counterWindowRemaining <= 0f ||
                _counterWindowRemaining > Mathf.Max(0.01f, perfectParryWindow))
            {
                return false;
            }

            _counterWindowRemaining = 0f;
            _counterTarget = null;

            MonoBehaviour[] behaviours = enemy.GetComponentsInParent<MonoBehaviour>(true);
            foreach (MonoBehaviour behaviour in behaviours)
            {
                if (behaviour is IWonderHeroPostureTarget postureTarget)
                {
                    postureTarget.RegisterAttackDeflection();
                    postureTarget.TakePostureDamage(45f);
                    break;
                }
            }

            playerPosture = Mathf.Max(0f, playerPosture - 45f);
            ComboCounter += 2;
            _comboRemaining = Mathf.Max(0.1f, 4f);
            ApplyRallyRecovery(45);

            Debug.Log($"[WonderHeroActionMechanics] Perfect harmonic deflection! {incomingDamage} incoming damage prevented; 45 posture damage inflicted.");
            return true;
        }

        public void ResolveIncomingAttack(GameObject attacker, int incomingDamage)
        {
            if (attacker == _counterTarget)
            {
                _counterTarget = null;
                _counterWindowRemaining = 0f;
            }

            TakeHeroDamage(incomingDamage);
        }

        public void TakeHeroDamage(int damage)
        {
            if (damage <= 0 || currentHealth <= 0) return;

            int appliedDamage = Mathf.Min(damage, currentHealth);
            currentHealth -= appliedDamage;
            rallyPotentialHealth = Mathf.Min(maxHealth - currentHealth, rallyPotentialHealth + appliedDamage);
            rallyWindowTimer = Mathf.Max(0.1f, maxRallyDuration);
            playerPosture = Mathf.Min(maxPosture, playerPosture + appliedDamage);

            Debug.Log($"[WonderHeroActionMechanics] Hit taken! Health: {currentHealth}. Rally potential: {rallyPotentialHealth}.");
        }

        private void UpdateRallyDecay()
        {
            if (rallyWindowTimer <= 0f) return;

            rallyWindowTimer = Mathf.Max(0f, rallyWindowTimer - Time.unscaledDeltaTime);
            if (rallyWindowTimer <= 0f)
            {
                rallyPotentialHealth = 0;
            }
        }

        public void ApplyRallyRecovery(int damageDealt)
        {
            if (damageDealt <= 0 || rallyPotentialHealth <= 0 || rallyWindowTimer <= 0f) return;

            int healthRecovered = Mathf.Min(rallyPotentialHealth, Mathf.RoundToInt(damageDealt * 0.5f));
            if (healthRecovered <= 0) return;

            currentHealth = Mathf.Min(maxHealth, currentHealth + healthRecovered);
            rallyPotentialHealth -= healthRecovered;
            if (rallyPotentialHealth <= 0)
            {
                rallyWindowTimer = 0f;
            }

            Debug.Log($"[WonderHeroActionMechanics] Bio-Rally recovered {healthRecovered} HP. Health: {currentHealth}.");
        }

        public void RegisterHit(GameObject target, int damage)
        {
            if (target == null) return;

            ComboCounter++;
            _comboRemaining = Mathf.Max(0.1f, comboDuration);
            ApplyRallyRecovery(damage);
            Debug.Log($"[WonderHeroActionMechanics] Hit {target.name} for {damage} damage. Combo x{ComboCounter}.");
        }
    }
}
