using System.Collections.Generic;
using Game;
using Unity.Entities;
using UnityEngine;

namespace WonderWorldCS2.Systems
{
    public interface IWonderHeroCounterTarget
    {
        void TakeCounterDamage(int damage);
    }

    public sealed class SyndicateEnforcerEntity : MonoBehaviour, IWonderHeroDamageable, IWonderHeroCounterTarget, IWonderHeroPostureTarget
    {
        private const float AttackInterval = 60f / 85f * 5f;
        private const float AttackTelegraphDuration = 60f / 85f;
        public const int AttackDamage = 20;

        public int Health { get; private set; } = 100;
        public float HarmonicStrain { get; private set; }
        public bool IsAttacking { get; private set; }

        private float _attackTimer;
        private bool _attackDeflected;
        private Renderer _renderer;
        private Material _material;
        private Color _baseColor = new Color(0.2f, 0.2f, 0.2f);

        private void Start()
        {
            GameObject body = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            body.name = "SyndicateEnforcerBody";
            body.transform.SetParent(transform, false);
            body.transform.localPosition = new Vector3(0f, 1f, 0f);
            body.transform.localScale = new Vector3(0.8f, 1f, 0.8f);

            _renderer = body.GetComponent<Renderer>();
            if (_renderer == null) return;

            Shader shader = Shader.Find("HDRP/Lit") ?? Shader.Find("Standard") ?? Shader.Find("Diffuse");
            if (shader == null) return;

            _material = new Material(shader);
            if (_material.HasProperty("_BaseColor"))
            {
                _material.SetColor("_BaseColor", _baseColor);
            }
            if (_material.HasProperty("_Color"))
            {
                _material.SetColor("_Color", _baseColor);
            }
            _renderer.material = _material;
        }

        private void Update()
        {
            if (Health <= 0) return;

            _attackTimer += Time.deltaTime;
            if (!IsAttacking && _attackTimer >= AttackInterval - AttackTelegraphDuration)
            {
                IsAttacking = true;
                _attackDeflected = false;
                SetColor(new Color(1f, 0.1f, 0.2f));
                WonderHeroActionMechanics.Instance?.OpenCounterWindow(gameObject, AttackTelegraphDuration, AttackDamage);
            }

            if (_attackTimer >= AttackInterval)
            {
                if (!_attackDeflected)
                {
                    WonderHeroActionMechanics.Instance?.ResolveIncomingAttack(gameObject, AttackDamage);
                }

                _attackTimer = 0f;
                IsAttacking = false;
                SetColor(_baseColor);
            }
        }

        public void TakeWonderDamage(int damage, GameObject source)
        {
            ApplyDamage(damage);
        }

        public void TakeCounterDamage(int damage)
        {
            ApplyDamage(damage);
        }

        public void TakePostureDamage(float damage)
        {
            if (Health <= 0 || damage <= 0f) return;

            HarmonicStrain = Mathf.Min(100f, HarmonicStrain + damage);
            if (HarmonicStrain < 100f) return;

            ApplyDamage(Health);
        }

        public void RegisterAttackDeflection()
        {
            _attackDeflected = true;
        }

        private void ApplyDamage(int damage)
        {
            if (Health <= 0 || damage <= 0) return;

            Health = Mathf.Max(0, Health - damage);
            SetColor(Color.white);
            if (Health > 0) return;

            SetColor(new Color(0.12f, 0.12f, 0.12f));
            SyndicateEnforcerSpawnerSystem.Instance?.OnEnforcerDefeated(this);
            Destroy(gameObject, 0.2f);
        }

        private void SetColor(Color color)
        {
            if (_material == null) return;

            if (_material.HasProperty("_BaseColor"))
            {
                _material.SetColor("_BaseColor", color);
            }
            if (_material.HasProperty("_Color"))
            {
                _material.SetColor("_Color", color);
            }
        }

        private void OnDestroy()
        {
            if (_material != null)
            {
                Destroy(_material);
            }
        }
    }

    public partial class SyndicateEnforcerSpawnerSystem : GameSystemBase
    {
        public static SyndicateEnforcerSpawnerSystem Instance { get; private set; }

        public readonly List<SyndicateEnforcerEntity> ActiveEnforcers = new List<SyndicateEnforcerEntity>();
        public int MaxEnforcers = 5;
        public int ActiveEnforcerCount
        {
            get
            {
                RemoveInvalidEnforcers();
                return ActiveEnforcers.Count;
            }
        }

        private const float SpawnCheckInterval = 4f;
        private float _spawnCheckTimer;

        protected override void OnCreate()
        {
            base.OnCreate();
            Instance = this;
            Debug.Log("[SyndicateSpawner] Street Enforcer Encounter System initialized.");
        }

        protected override void OnDestroy()
        {
            if (Instance == this)
            {
                Instance = null;
            }
            base.OnDestroy();
        }

        public static SyndicateEnforcerSpawnerSystem EnsureRegisteredInWorld()
        {
            if (Instance != null) return Instance;

            World world = World.DefaultGameObjectInjectionWorld;
            if (world == null || !world.IsCreated) return null;

            Instance = world.GetOrCreateSystemManaged<SyndicateEnforcerSpawnerSystem>();
            var updateSystem = world.GetExistingSystemManaged<UpdateSystem>();
            updateSystem?.UpdateAt<SyndicateEnforcerSpawnerSystem>(SystemUpdatePhase.GameSimulation);
            return Instance;
        }

        protected override void OnUpdate()
        {
            _spawnCheckTimer += UnityEngine.Time.unscaledDeltaTime;
            if (_spawnCheckTimer < SpawnCheckInterval) return;

            _spawnCheckTimer = 0f;
            RemoveInvalidEnforcers();

            if (ActiveEnforcers.Count != 0 ||
                WonderUniverseIntegrationSystem.StaticDistrictTensionIndex < 20f)
            {
                return;
            }

            SpawnEnforcerGroup(3);
        }

        public void SpawnEnforcerGroup(int count)
        {
            if (count <= 0) return;

            RemoveInvalidEnforcers();
            Vector3 center;
            if (!TryGetSpawnCenter(out center))
            {
                Debug.LogWarning("[SyndicateSpawner] Cannot spawn an encounter because no active camera is available.");
                return;
            }

            int spawnCount = Mathf.Min(count, Mathf.Max(0, MaxEnforcers - ActiveEnforcers.Count));
            for (int i = 0; i < spawnCount; i++)
            {
                GameObject enforcerObject = new GameObject($"Syndicate_Thug_{ActiveEnforcers.Count + 1}");
                enforcerObject.transform.position = center + new Vector3((i - (spawnCount - 1) * 0.5f) * 3f, 0f, Random.Range(-2f, 2f));
                ActiveEnforcers.Add(enforcerObject.AddComponent<SyndicateEnforcerEntity>());
            }

            Debug.Log($"[SyndicateSpawner] Spawned {spawnCount} Syndicate Enforcers.");
        }

        public static void SpawnStreetBrawlWave()
        {
            EnsureRegisteredInWorld()?.SpawnEnforcerGroup(3);
        }

        public void OnEnforcerDefeated(SyndicateEnforcerEntity enforcer)
        {
            ActiveEnforcers.Remove(enforcer);
            RemoveInvalidEnforcers();
            if (ActiveEnforcers.Count != 0) return;

            WonderUniverseIntegrationSystem.RegisterStreetBrawlWaveClear();
            Debug.Log("[SyndicateSpawner] Wave cleared: district tension reduced and +100 Wonder Stars awarded.");
        }

        private void RemoveInvalidEnforcers()
        {
            ActiveEnforcers.RemoveAll(enforcer => enforcer == null);
        }

        private static bool TryGetSpawnCenter(out Vector3 center)
        {
            Camera camera = Camera.main;
            if (camera == null)
            {
                Camera[] cameras = Camera.allCameras;
                foreach (Camera candidate in cameras)
                {
                    if (candidate != null && candidate.enabled)
                    {
                        camera = candidate;
                        break;
                    }
                }
            }

            if (camera == null)
            {
                center = Vector3.zero;
                return false;
            }

            Vector3 forward = Vector3.ProjectOnPlane(camera.transform.forward, Vector3.up).normalized;
            if (forward.sqrMagnitude < 0.001f)
            {
                forward = Vector3.forward;
            }

            Vector3 candidateCenter = camera.transform.position + forward * 15f;
            Ray groundRay = new Ray(candidateCenter + Vector3.up * 100f, Vector3.down);
            center = Physics.Raycast(groundRay, out RaycastHit hit, 250f)
                ? hit.point
                : new Vector3(candidateCenter.x, 0f, candidateCenter.z);
            return true;
        }
    }
}
