using UnityEngine;
using UnityEngine.Events;

namespace WonderWorld
{
    /// <summary>
    /// Handles player interaction with the retro-futuristic arcade cabinet,
    /// launching the 2.5D mini-game and spawning the dimensional portal reward key
    /// (Obsidian Monolith for PS6, Quantum Grid for Xbox Helix, Holographic Prism for VR).
    /// </summary>
    public class ArcadeMachineController : MonoBehaviour
    {
        public static ArcadeMachineController Instance { get; private set; }

        [Header("Interaction Settings")]
        [Tooltip("Distance at which the player can interact with the machine.")]
        public float interactionDistance = 3.0f;
        public LayerMask playerLayer;
        public KeyCode interactKey = KeyCode.E;

        [Header("Reward Settings")]
        [Tooltip("The Portal Key prefab to spawn when the mini-game is completed.")]
        public GameObject portalKeyPrefab;
        public Transform keySpawnPoint;

        [Header("Events")]
        public UnityEvent onMiniGameStarted;
        public UnityEvent onMiniGameCompleted;

        private bool playerInRange = false;
        private bool miniGameActive = false;
        private bool keySpawned = false;
        private Transform playerTransform;

        void Awake()
        {
            if (Instance == null) Instance = this;
        }

        void Start()
        {
            GameObject player = GameObject.FindGameObjectWithTag("Player");
            if (player != null)
            {
                playerTransform = player.transform;
            }
        }

        void Update()
        {
            if (playerTransform == null) return;

            float distance = Vector3.Distance(transform.position, playerTransform.position);
            playerInRange = (distance <= interactionDistance);

            if (playerInRange && !miniGameActive && !keySpawned && Input.GetKeyDown(interactKey))
            {
                StartMiniGame();
            }
        }

        void StartMiniGame()
        {
            miniGameActive = true;
            onMiniGameStarted?.Invoke();
        }

        public void CompleteMiniGame()
        {
            if (!miniGameActive) return;
            miniGameActive = false;
            keySpawned = true;
            SpawnPortalKey();
            onMiniGameCompleted?.Invoke();
        }

        void SpawnPortalKey()
        {
            Vector3 spawnPos = (keySpawnPoint != null) ? keySpawnPoint.position : (transform.position + transform.forward * 1.5f + Vector3.up * 0.5f);
            Quaternion spawnRot = (keySpawnPoint != null) ? keySpawnPoint.rotation : Quaternion.identity;

            if (portalKeyPrefab != null)
            {
                Instantiate(portalKeyPrefab, spawnPos, spawnRot);
            }
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.cyan;
            Gizmos.DrawWireSphere(transform.position, interactionDistance);
        }
    }
}
