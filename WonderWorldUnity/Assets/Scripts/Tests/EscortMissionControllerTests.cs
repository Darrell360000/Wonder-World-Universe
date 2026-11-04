using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
using System.Reflection;
using System.Collections.Generic;

namespace WonderWorld.Tests
{
    public sealed class EscortMissionControllerTests
    {
        [Test]
        public void StartMissionRequiresEscortsAndDestination()
        {
            GameObject missionObject = new GameObject("Mission");
            EscortMissionController mission = missionObject.AddComponent<EscortMissionController>();
            LogAssert.Expect(LogType.Error, "Escort mission needs a destination and at least one escort.");
            mission.StartMission();
            Assert.AreEqual(EscortMissionState.Inactive, mission.State);
            Object.DestroyImmediate(missionObject);
        }

        [Test]
        public void ReceivingFatalDamageFailsActiveMission()
        {
            GameObject missionObject = new GameObject("Mission");
            EscortMissionController mission = missionObject.AddComponent<EscortMissionController>();
            GameObject destination = new GameObject("Destination");
            GameObject subject = new GameObject("Escort");
            SetField(mission, "destination", destination.transform);
            SetField(mission, "escorts", new List<EscortPact> { new EscortPact { Subject = subject.transform, Health = 10f } });
            SetField(mission, "completionRadius", 3f);
            mission.StartMission();
            mission.ReportEscortDamage(subject.transform, 10f);
            Assert.AreEqual(EscortMissionState.Failed, mission.State);
            Object.DestroyImmediate(destination);
            Object.DestroyImmediate(subject);
            Object.DestroyImmediate(missionObject);
        }

        [Test]
        public void MissionProgressTracksRouteAndRequiresEveryEscortToArrive()
        {
            GameObject missionObject = new GameObject("Mission");
            EscortMissionController mission = missionObject.AddComponent<EscortMissionController>();
            GameObject destination = new GameObject("Destination");
            destination.transform.position = Vector3.right * 10f;
            GameObject first = new GameObject("EscortOne");
            GameObject second = new GameObject("EscortTwo");
            first.transform.position = Vector3.zero;
            second.transform.position = Vector3.zero;
            SetField(mission, "destination", destination.transform);
            SetField(mission, "completionRadius", 1f);
            SetField(mission, "escorts", new List<EscortPact>
            {
                new EscortPact { Subject = first.transform, Health = 10f, FailureDistance = 50f },
                new EscortPact { Subject = second.transform, Health = 10f, FailureDistance = 50f }
            });
            mission.StartMission();

            first.transform.position = Vector3.right * 5f;
            second.transform.position = Vector3.right * 5f;
            InvokeUpdate(mission);
            Assert.Greater(mission.Progress, 0.5f);
            first.transform.position = destination.transform.position;
            InvokeUpdate(mission);
            Assert.AreEqual(EscortMissionState.Active, mission.State);
            second.transform.position = destination.transform.position;
            InvokeUpdate(mission);
            Assert.AreEqual(EscortMissionState.Succeeded, mission.State);
            Assert.AreEqual(1f, mission.Progress);

            Object.DestroyImmediate(destination);
            Object.DestroyImmediate(first);
            Object.DestroyImmediate(second);
            Object.DestroyImmediate(missionObject);
        }

        private static void SetField(object target, string name, object value)
        {
            typeof(EscortMissionController).GetField(name, BindingFlags.Instance | BindingFlags.NonPublic).SetValue(target, value);
        }

        private static void InvokeUpdate(EscortMissionController mission)
        {
            typeof(EscortMissionController).GetMethod("Update", BindingFlags.Instance | BindingFlags.NonPublic).Invoke(mission, null);
        }
    }
}
