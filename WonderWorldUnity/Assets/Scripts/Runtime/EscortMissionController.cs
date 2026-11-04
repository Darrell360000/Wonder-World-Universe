using System;
using System.Collections.Generic;
using UnityEngine;
using NonSerialized = UnityEngine.NonSerializedAttribute;
using UnityEngine.Events;

namespace WonderWorld
{
    [Serializable]
    public sealed class EscortPact
    {
        public Transform Subject;
        [Min(0f)] public float Health = 100f;
        [Min(0f)] public float FailureDistance = 20f;
        [NonSerialized] public float LastHealth;
    }

    public enum EscortMissionState { Inactive, Active, Succeeded, Failed }

    [DisallowMultipleComponent]
    public sealed class EscortMissionController : MonoBehaviour
    {
        [SerializeField] private Transform destination;
        [SerializeField] private List<EscortPact> escorts = new List<EscortPact>();
        [SerializeField, Min(0.5f)] private float completionRadius = 3f;
        [SerializeField, Min(0f)] private float maximumEscortDuration = 300f;
        [SerializeField] private UnityEvent onMissionStarted;
        [SerializeField] private UnityEvent onMissionSucceeded;
        [SerializeField] private UnityEvent onMissionFailed;
        [SerializeField] private UnityEvent<float> onProgressChanged;

        private readonly List<float> initialEscortDistances = new List<float>();
        private float elapsed;

        public EscortMissionState State { get; private set; } = EscortMissionState.Inactive;
        public float Progress { get; private set; }

        public IReadOnlyList<EscortPact> Escorts => escorts;

        private void Awake()
        {
            if (escorts == null) return;
            foreach (EscortPact escort in escorts)
                if (escort != null)
                    escort.LastHealth = escort.Health;
        }

        public void StartMission()
        {
            if (destination == null || escorts == null || escorts.Count == 0)
            {
                Debug.LogError("Escort mission needs a destination and at least one escort.", this);
                return;
            }
            if (float.IsNaN(completionRadius) || float.IsInfinity(completionRadius) || completionRadius <= 0f
                || float.IsNaN(maximumEscortDuration) || float.IsInfinity(maximumEscortDuration))
            {
                Debug.LogError("Escort mission has invalid completion or duration settings.", this);
                return;
            }

            initialEscortDistances.Clear();
            HashSet<Transform> uniqueSubjects = new HashSet<Transform>();
            foreach (EscortPact escort in escorts)
            {
                if (escort == null || escort.Subject == null || !uniqueSubjects.Add(escort.Subject)
                    || float.IsNaN(escort.Health) || float.IsInfinity(escort.Health) || escort.Health <= 0f
                    || float.IsNaN(escort.FailureDistance) || float.IsInfinity(escort.FailureDistance)
                    || escort.FailureDistance < 0f)
                {
                    initialEscortDistances.Clear();
                    Debug.LogError("Escort mission contains an invalid, duplicate, or defeated escort.", this);
                    return;
                }
                float initialDistance = Vector3.Distance(escort.Subject.position, destination.position);
                if (float.IsNaN(initialDistance) || float.IsInfinity(initialDistance))
                {
                    initialEscortDistances.Clear();
                    Debug.LogError("Escort mission has a non-finite route distance.", this);
                    return;
                }
                initialEscortDistances.Add(initialDistance);
                escort.LastHealth = escort.Health;
            }

            elapsed = 0f;
            Progress = 0f;
            State = EscortMissionState.Active;
            onMissionStarted?.Invoke();
            onProgressChanged?.Invoke(0f);
        }

        public void ReportEscortDamage(Transform subject, float damage)
        {
            if (State != EscortMissionState.Active || escorts == null || subject == null
                || float.IsNaN(damage) || float.IsInfinity(damage) || damage <= 0f)
                return;

            EscortPact escort = escorts.Find(e => e != null && e.Subject == subject);
            if (escort == null) return;
            if (float.IsNaN(escort.Health) || float.IsInfinity(escort.Health) || escort.Health <= 0f)
            {
                FailMission();
                return;
            }
            escort.Health = Mathf.Max(0f, escort.Health - damage);
            escort.LastHealth = escort.Health;
            if (escort.Health <= 0f) FailMission();
        }

        private void Update()
        {
            if (State != EscortMissionState.Active) return;
            elapsed += Time.deltaTime;
            if (destination == null || escorts == null || escorts.Count == 0
                || escorts.Count != initialEscortDistances.Count)
            {
                FailMission();
                return;
            }

            float progressTotal = 0f;
            bool allEscortsArrived = true;
            for (int i = 0; i < escorts.Count; i++)
            {
                EscortPact escort = escorts[i];
                if (escort == null || escort.Subject == null)
                {
                    FailMission();
                    return;
                }

                float distance = Vector3.Distance(escort.Subject.position, destination.position);
                if (float.IsNaN(distance) || float.IsInfinity(distance)
                    || float.IsNaN(escort.FailureDistance) || float.IsInfinity(escort.FailureDistance)
                    || distance > escort.FailureDistance)
                {
                    FailMission();
                    return;
                }

                bool arrived = distance <= completionRadius;
                allEscortsArrived &= arrived;
                float initialDistance = initialEscortDistances[i];
                float routeLength = initialDistance - completionRadius;
                float escortProgress = routeLength <= 0f
                    ? (arrived ? 1f : 0f)
                    : Mathf.Clamp01((initialDistance - distance) / routeLength);
                progressTotal += escortProgress;
            }

            if (allEscortsArrived)
            {
                CompleteMission();
                return;
            }
            if (maximumEscortDuration > 0f && elapsed >= maximumEscortDuration)
            {
                FailMission();
                return;
            }

            // Progress represents the average route completion and never regresses.
            float measuredProgress = progressTotal / escorts.Count;
            if (measuredProgress > Progress + 0.001f)
            {
                Progress = measuredProgress;
                onProgressChanged?.Invoke(Progress);
            }
        }

        public void CompleteMission()
        {
            if (State != EscortMissionState.Active) return;
            State = EscortMissionState.Succeeded;
            Progress = 1f;
            onProgressChanged?.Invoke(1f);
            onMissionSucceeded?.Invoke();
        }

        public void FailMission()
        {
            if (State != EscortMissionState.Active) return;
            State = EscortMissionState.Failed;
            onMissionFailed?.Invoke();
        }
    }
}
