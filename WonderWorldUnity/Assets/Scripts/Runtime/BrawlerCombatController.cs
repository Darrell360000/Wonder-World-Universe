using System;
using System.Collections.Generic;
using System.Collections;
using UnityEngine;
using UnityEngine.Events;

namespace WonderWorld
{
    public interface IDamageable
    {
        bool IsAlive { get; }
        void ApplyDamage(float amount, Vector3 direction);
    }

    [DisallowMultipleComponent]
    public sealed class BrawlerCombatController : MonoBehaviour
    {
        [Serializable]
        public sealed class CombatEvent : UnityEvent<int, float> { }

        [SerializeField, Min(0.05f)] private float comboWindow = 0.75f;
        [SerializeField, Min(0.05f)] private float attackCooldown = 0.22f;
        [SerializeField, Min(0.1f)] private float attackRadius = 1.15f;
        [SerializeField, Min(0f)] private float forwardOffset = 0.65f;
        [SerializeField, Min(1f)] private float maxHealth = 100f;
        [SerializeField, Min(1)] private int maxCombo = 3;
        [SerializeField] private LayerMask hittableMask = ~0;
        [SerializeField] private CombatEvent onAttackConnected = new CombatEvent();
        [SerializeField] private UnityEvent onHealthChanged;

        private float health;
        private int combo;
        private float lastHitTime = -10f;
        private float lastAttackTime = -10f;
        private bool attackLocked;

        public float Health => health;
        public int ComboStep => combo;
        public bool IsAlive => health > 0f;

        private void Awake() => health = maxHealth;

        public void ReceiveDamage(float amount, Vector3 direction)
        {
            if (!IsAlive || float.IsNaN(amount) || float.IsInfinity(amount) || amount <= 0f) return;
            health = Mathf.Max(0f, health - amount);
            onHealthChanged?.Invoke();
            if (!IsAlive) StopAllCoroutines();
        }

        public void TryAttack()
        {
            if (!IsAlive || attackLocked) return;
            float now = Time.time;
            if (now - lastAttackTime < attackCooldown) return;
            lastAttackTime = now;
            combo = now - lastHitTime <= comboWindow ? Mathf.Min(combo + 1, maxCombo) : 1;
            lastHitTime = now;
            attackLocked = true;
            StartCoroutine(UnlockAttack());

            float damage = 12f * (1f + (combo - 1) * 0.35f);
            Vector3 center = transform.position + transform.forward * forwardOffset;
            int hits = 0;
            HashSet<IDamageable> damagedTargets = new HashSet<IDamageable>();
            foreach (Collider candidate in Physics.OverlapSphere(center, attackRadius, hittableMask))
            {
                if (candidate.transform.IsChildOf(transform)) continue;
                IDamageable damageable = candidate.GetComponentInParent<IDamageable>();
                if (damageable == null || !damageable.IsAlive || !damagedTargets.Add(damageable)) continue;
                Vector3 direction = (candidate.bounds.center - transform.position).normalized;
                damageable.ApplyDamage(damage, direction);
                hits++;
            }
            onAttackConnected?.Invoke(combo, hits > 0 ? damage : 0f);
        }

        private IEnumerator UnlockAttack()
        {
            yield return new WaitForSeconds(attackCooldown);
            attackLocked = false;
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.cyan;
            Gizmos.DrawWireSphere(transform.position + transform.forward * forwardOffset, attackRadius);
        }
    }
}
