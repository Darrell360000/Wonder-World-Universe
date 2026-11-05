using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Events;

namespace WonderWorld
{
    /// <summary>
    /// Singleton manager for the 2.5D arcade platformer mini-game,
    /// tracking score, player lives, win conditions, and returning to the main 3D world.
    /// </summary>
    public class MiniGameManager : MonoBehaviour
    {
        public static MiniGameManager Instance { get; private set; }

        [Header("Scene Settings")]
        [Tooltip("Name of the main 3D game world scene to return to upon exiting.")]
        public string mainWorldSceneName = "MainWorldScene";

        [Header("Game State")]
        public int currentScore = 0;
        public int targetScoreToWin = 1000;
        public int playerLives = 3;

        private bool isGameActive = false;

        [Header("Events")]
        public UnityEvent onGameStart;
        public UnityEvent onScoreChanged;
        public UnityEvent onGameWon;
        public UnityEvent onGameOver;

        private void Awake()
        {
            if (Instance == null)
            {
                Instance = this;
                DontDestroyOnLoad(gameObject);
            }
            else
            {
                Destroy(gameObject);
            }
        }

        private void Start()
        {
            StartMiniGame();
        }

        public void StartMiniGame()
        {
            currentScore = 0;
            playerLives = 3;
            isGameActive = true;
            onScoreChanged?.Invoke();
            onGameStart?.Invoke();
        }

        public void AddScore(int amount)
        {
            if (!isGameActive) return;
            currentScore += amount;
            onScoreChanged?.Invoke();

            if (currentScore >= targetScoreToWin)
            {
                WinMiniGame();
            }
        }

        public void LoseLife()
        {
            if (!isGameActive) return;
            playerLives--;
            if (playerLives <= 0)
            {
                GameOver();
            }
        }

        private void WinMiniGame()
        {
            isGameActive = false;
            onGameWon?.Invoke();

            if (ArcadeMachineController.Instance != null)
            {
                ArcadeMachineController.Instance.CompleteMiniGame();
            }
        }

        private void GameOver()
        {
            isGameActive = false;
            onGameOver?.Invoke();
        }

        public void ReturnToMainWorld()
        {
            SceneManager.LoadScene(mainWorldSceneName);
        }
    }
}
