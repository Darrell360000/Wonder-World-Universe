using NUnit.Framework;
using System.Reflection;
using UnityEngine;

namespace WonderWorld.Tests
{
    public sealed class BrawlerCombatControllerTests
    {
        [Test]
        public void OneAttackDamagesEachTargetOnlyOnceAcrossMultipleColliders()
        {
            GameObject attacker = new GameObject("Brawler");
            BrawlerCombatController controller = attacker.AddComponent<BrawlerCombatController>();
            SetHealth(controller, 100f);
            GameObject target = new GameObject("MultiColliderTarget");
            target.transform.position = Vector3.forward * 0.8f;
            TestDamageableTarget damageable = target.AddComponent<TestDamageableTarget>();
            AddChildCollider(target.transform, "LeftHitbox", Vector3.left * 0.15f);
            AddChildCollider(target.transform, "RightHitbox", Vector3.right * 0.15f);
            Physics.SyncTransforms();

            controller.TryAttack();

            Assert.AreEqual(1, damageable.HitCount);
            Assert.Greater(damageable.TotalDamage, 0f);
            Object.DestroyImmediate(attacker);
            Object.DestroyImmediate(target);
        }

        [Test]
        public void NonFiniteDamageDoesNotCorruptPlayerHealth()
        {
            GameObject player = new GameObject("Brawler");
            BrawlerCombatController controller = player.AddComponent<BrawlerCombatController>();
            SetHealth(controller, 100f);
            controller.ReceiveDamage(float.NaN, Vector3.zero);
            controller.ReceiveDamage(float.PositiveInfinity, Vector3.zero);
            Assert.AreEqual(100f, controller.Health);
            Object.DestroyImmediate(player);
        }

        private static void SetHealth(BrawlerCombatController controller, float value)
        {
            typeof(BrawlerCombatController).GetField("health", BindingFlags.Instance | BindingFlags.NonPublic).SetValue(controller, value);
        }

        private static void AddChildCollider(Transform parent, string name, Vector3 localPosition)
        {
            GameObject child = new GameObject(name);
            child.transform.SetParent(parent, false);
            child.transform.localPosition = localPosition;
            child.AddComponent<SphereCollider>();
        }
    }

    public sealed class TestDamageableTarget : MonoBehaviour, WonderWorld.IDamageable
    {
        public int HitCount { get; private set; }
        public float TotalDamage { get; private set; }
        public bool IsAlive => true;

        public void ApplyDamage(float amount, Vector3 direction)
        {
            HitCount++;
            TotalDamage += amount;
        }
    }
}
