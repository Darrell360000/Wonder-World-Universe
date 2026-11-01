using System;
using System.IO;
using UnityEngine;

namespace WonderWorld.Bridge
{
    [System.Serializable]
    public class WonderLedgerData
    {
        public int municipalTreasury = 500000;
        public int wonderStars = 1500;
        public int activeBusinesses = 12;
        public float districtTension = 24.5f;
        public string activeRadioStation = "The Smooth Beat (85 BPM Harmonic)";
        public long lastSyncTimestamp;
    }

    public class WonderWorldLedgerBridge : MonoBehaviour
    {
        public static WonderWorldLedgerBridge Instance { get; private set; }

        private readonly string ledgerPath = @"D:\Games\Cities - Skylines II\02_Shared_Game_Core\State\WonderLedgerState.json";
        public WonderLedgerData CurrentLedger { get; private set; } = new WonderLedgerData();

        private float pollTimer = 0f;

        void Awake()
        {
            if (Instance == null) Instance = this;
            else Destroy(gameObject);
            DontDestroyOnLoad(gameObject);
        }

        void Start()
        {
            LoadLedger();
        }

        void Update()
        {
            pollTimer += Time.deltaTime;
            // Poll CS2 state every 2 seconds
            if (pollTimer >= 2.0f)
            {
                pollTimer = 0f;
                LoadLedger();
            }
        }

        public void LoadLedger()
        {
            try
            {
                if (File.Exists(ledgerPath))
                {
                    string json = File.ReadAllText(ledgerPath);
                    var parsed = JsonUtility.FromJson<WonderLedgerData>(json);
                    if (parsed != null)
                    {
                        CurrentLedger = parsed;
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.LogWarning($"[LedgerBridge] Read warning: {ex.Message}");
            }
        }

        public void SyncBusinessDividend(int profitEarned, int newWonderStars, int addedBusinesses = 0)
        {
            try
            {
                LoadLedger();
                CurrentLedger.municipalTreasury += profitEarned;
                CurrentLedger.wonderStars += newWonderStars;
                CurrentLedger.activeBusinesses += addedBusinesses;
                CurrentLedger.lastSyncTimestamp = DateTimeOffset.UtcNow.ToUnixTimeSeconds();

                string json = JsonUtility.ToJson(CurrentLedger, true);
                File.WriteAllText(ledgerPath, json);
                Debug.Log($"[LedgerBridge] Successfully synced +${profitEarned:N0} and +{newWonderStars} ⭐️W to CS2!");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[LedgerBridge] Write error: {ex.Message}");
            }
        }

        public void SyncArenaShowdownVictory(int bonusWonderStars = 250)
        {
            try
            {
                LoadLedger();
                CurrentLedger.districtTension = 0.0f;
                CurrentLedger.wonderStars += bonusWonderStars;
                CurrentLedger.lastSyncTimestamp = DateTimeOffset.UtcNow.ToUnixTimeSeconds();

                string json = JsonUtility.ToJson(CurrentLedger, true);
                File.WriteAllText(ledgerPath, json);
                Debug.Log($"[LedgerBridge] Arena Showdown Victory synced! Tension -> 0.0%, +{bonusWonderStars} ⭐️W!");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[LedgerBridge] Arena sync error: {ex.Message}");
            }
        }
    }
}
