using System;
using System.Collections.Generic;
using UnityEngine;
using WonderWorld.Bridge;

namespace WonderWorld.Tycoon
{
    [Serializable]
    public class StorefrontLeaseUnit
    {
        public string StoreId;
        public string StoreName;
        public Vector3 EntrancePosition;
        public bool IsLeased;
        public int InstalledShelves;
        public int InventoryUnits;
        public int DailyRevenueUsd;
    }

    /// <summary>
    /// Big Ambitions Walk-In Storefront Leasing, Interior Shelf Setup & Pragmata Drone Logistics.
    /// Press [E] near a storefront entrance to sign a lease, place shelves, restock via drone,
    /// and sync daily commercial dividends directly to Cities: Skylines II via WonderWorldLedgerBridge.
    /// </summary>
    public class WalkInStorefrontTycoon : MonoBehaviour
    {
        public static WalkInStorefrontTycoon ActiveInstance { get; private set; }

        public List<StorefrontLeaseUnit> DistrictStorefronts = new List<StorefrontLeaseUnit>
        {
            new StorefrontLeaseUnit
            {
                StoreId = "STORE_PURPLE_01",
                StoreName = "Mama's Gold-Leaf Boutique & Cafe",
                EntrancePosition = new Vector3(12f, 0f, 18f),
                IsLeased = true,
                InstalledShelves = 6,
                InventoryUnits = 240,
                DailyRevenueUsd = 3850
            },
            new StorefrontLeaseUnit
            {
                StoreId = "STORE_PURPLE_02",
                StoreName = "Darrell X-Gen Vinyl & Sonic Gear",
                EntrancePosition = new Vector3(28f, 0f, 18f),
                IsLeased = false,
                InstalledShelves = 0,
                InventoryUnits = 0,
                DailyRevenueUsd = 0
            },
            new StorefrontLeaseUnit
            {
                StoreId = "STORE_PURPLE_03",
                StoreName = "Top Hatter Cinema Merch Emporium",
                EntrancePosition = new Vector3(44f, 0f, 18f),
                IsLeased = false,
                InstalledShelves = 0,
                InventoryUnits = 0,
                DailyRevenueUsd = 0
            }
        };

        private float _dividendTimer = 0f;

        private void Awake()
        {
            ActiveInstance = this;
        }

        private void Update()
        {
            _dividendTimer += Time.deltaTime;
            if (_dividendTimer >= 10.0f)
            {
                _dividendTimer = 0f;
                CollectAndSyncStorefrontDividends();
            }
        }

        public void InteractAtPlayerPosition(Vector3 playerPos)
        {
            StorefrontLeaseUnit nearest = null;
            float bestDist = float.MaxValue;

            foreach (var store in DistrictStorefronts)
            {
                float d = Vector3.Distance(playerPos, store.EntrancePosition);
                if (d < bestDist)
                {
                    bestDist = d;
                    nearest = store;
                }
            }

            if (nearest == null) return;

            if (!nearest.IsLeased)
            {
                nearest.IsLeased = true;
                nearest.InstalledShelves = 4;
                nearest.InventoryUnits = 150;
                nearest.DailyRevenueUsd = 2500;
                Debug.Log($"[WalkInTycoon] 🏪 Leased [{nearest.StoreName}]! Installed 4 shelves & dispatched Pragmata restock drone.");
                WonderWorldLedgerBridge.Instance?.SyncBusinessDividend(nearest.DailyRevenueUsd, 15, addedBusinesses: 1);
            }
            else
            {
                nearest.InstalledShelves += 2;
                nearest.InventoryUnits += 100;
                nearest.DailyRevenueUsd += 1250;
                Debug.Log($"[WalkInTycoon] 📦 Upgraded [{nearest.StoreName}] -> {nearest.InstalledShelves} shelves (${nearest.DailyRevenueUsd:N0}/day)!");
                WonderWorldLedgerBridge.Instance?.SyncBusinessDividend(nearest.DailyRevenueUsd, 10, addedBusinesses: 0);
            }
        }

        public void CollectAndSyncStorefrontDividends()
        {
            int totalProfit = 0;
            foreach (var store in DistrictStorefronts)
            {
                if (store.IsLeased && store.InventoryUnits > 0)
                {
                    totalProfit += store.DailyRevenueUsd;
                }
            }

            if (totalProfit > 0)
            {
                WonderWorldLedgerBridge.Instance?.SyncBusinessDividend(totalProfit, 5);
            }
        }
    }
}
