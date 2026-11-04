using System.Collections;
using System.Reflection;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace WonderWorld.Tests
{
    public sealed class WonderCameraDirectorTests
    {
        [UnityTest]
        public IEnumerator InterruptedBlendRestoresFullViewportsAndFinishesRequestedMode()
        {
            GameObject manager = new GameObject("CameraDirector");
            manager.SetActive(false);
            GameObject strategyObject = new GameObject("StrategyCamera");
            GameObject brawlerObject = new GameObject("BrawlerCamera");
            Camera strategy = strategyObject.AddComponent<Camera>();
            Camera brawler = brawlerObject.AddComponent<Camera>();
            WonderCameraDirector director = manager.AddComponent<WonderCameraDirector>();
            SetField(director, "strategyCamera", strategy);
            SetField(director, "brawlerCamera", brawler);
            manager.SetActive(true);
            director.SwitchMode(WonderCameraMode.Brawler, true);

            Assert.IsFalse(strategyObject.activeSelf);
            Assert.IsTrue(brawlerObject.activeSelf);
            director.SwitchMode(WonderCameraMode.Strategy);
            Assert.IsTrue(director.IsBlending);
            yield return null;
            director.SwitchMode(WonderCameraMode.Brawler, true);
            yield return null;

            Assert.AreEqual(WonderCameraMode.Brawler, director.ActiveMode);
            Assert.IsFalse(director.IsBlending);
            Assert.IsFalse(strategyObject.activeSelf);
            Assert.IsTrue(brawlerObject.activeSelf);
            Assert.AreEqual(1f, strategy.rect.width);
            Assert.AreEqual(1f, brawler.rect.width);

            Object.DestroyImmediate(manager);
            Object.DestroyImmediate(strategyObject);
            Object.DestroyImmediate(brawlerObject);
        }

        private static void SetField(object target, string name, object value)
        {
            typeof(WonderCameraDirector).GetField(name, BindingFlags.Instance | BindingFlags.NonPublic).SetValue(target, value);
        }
    }
}
