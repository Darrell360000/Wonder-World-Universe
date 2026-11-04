using System;
using System.Collections;
using System.Collections.Generic;

namespace UnityEngine
{
    public struct Vector3
    {
        public float x;
        public float y;
        public float z;

        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static Vector3 zero => new Vector3(0f, 0f, 0f);
        public static Vector3 forward => new Vector3(0f, 0f, 1f);
        public static Vector3 left => new Vector3(-1f, 0f, 0f);
        public static Vector3 right => new Vector3(1f, 0f, 0f);
        public static Vector3 up => new Vector3(0f, 1f, 0f);

        public static Vector3 operator +(Vector3 a, Vector3 b) => new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        public static Vector3 operator -(Vector3 a, Vector3 b) => new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        public static Vector3 operator -(Vector3 a) => new Vector3(-a.x, -a.y, -a.z);
        public static Vector3 operator *(Vector3 a, float d) => new Vector3(a.x * d, a.y * d, a.z * d);
        public static Vector3 operator /(Vector3 a, float d) => new Vector3(a.x / d, a.y / d, a.z / d);
        public static Vector3 operator *(float d, Vector3 a) => a * d;

        public float magnitude => Mathf.Sqrt(x * x + y * y + z * z);
        public Vector3 normalized => magnitude > 0f ? this / magnitude : zero;

        public static float Distance(Vector3 a, Vector3 b) => (a - b).magnitude;
        public static Vector3 Lerp(Vector3 a, Vector3 b, float t) => a + (b - a) * Mathf.Clamp01(t);

        public override string ToString() => $"({x}, {y}, {z})";
    }

    public struct Quaternion
    {
        public float x;
        public float y;
        public float z;
        public float w;

        public static Quaternion identity => new Quaternion(0f, 0f, 0f, 1f);

        public Quaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
    }

    public struct Rect
    {
        public float x;
        public float y;
        public float width;
        public float height;

        public Rect(float x, float y, float width, float height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }
    }

    public struct Bounds
    {
        public Vector3 center;
        public Vector3 extents;

        public Bounds(Vector3 center, Vector3 extents)
        {
            this.center = center;
            this.extents = extents;
        }
    }

    public struct LayerMask
    {
        public int value;

        public static implicit operator LayerMask(int value) => new LayerMask { value = value };
        public static implicit operator int(LayerMask layerMask) => layerMask.value;
    }

    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class SerializeFieldAttribute : Attribute { }

    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class HeaderAttribute : Attribute
    {
        public HeaderAttribute(string header) { }
    }

    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class MinAttribute : Attribute
    {
        public MinAttribute(float min) { }
    }

    [AttributeUsage(AttributeTargets.Class)]
    public sealed class DisallowMultipleComponentAttribute : Attribute { }

    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class NonSerializedAttribute : Attribute { }

    public class MonoBehaviour : Behaviour
    {
        public Coroutine StartCoroutine(IEnumerator routine) => new Coroutine();
        public void StopCoroutine(IEnumerator routine) { }
        public void StopCoroutine(Coroutine routine) { }
        public void StopAllCoroutines() { }
    }

    public class Coroutine { }

    public class Object
    {
        public static void DestroyImmediate(Object obj) { }
        public static void Destroy(Object obj) { }
    }

    public class GameObject : Object
    {
        private readonly Dictionary<Type, Component> cachedComponents = new Dictionary<Type, Component>();
        private readonly List<Component> components = new List<Component>();
        private bool activeState = true;

        public GameObject(string name = "")
        {
            name_ = name;
            transform = new Transform(this);
        }

        public Transform transform { get; set; }
        public string name_ { get; set; }
        public bool activeSelf => activeState;

        public T AddComponent<T>() where T : Component, new()
        {
            var component = new T();
            component.gameObject = this;
            component.transform = transform;
            cachedComponents[typeof(T)] = component;
            components.Add(component);
            return component;
        }

        public T GetComponent<T>() where T : class
        {
            var type = typeof(T);
            if (cachedComponents.TryGetValue(type, out var component))
                return component as T;
            foreach (var entry in components)
            {
                if (entry is T typed)
                    return typed;
            }
            return null;
        }

        public T GetComponentInParent<T>() where T : class => GetComponent<T>();

        public void SetActive(bool value) => activeState = value;

        public bool CompareTag(string tag) => string.Equals(name_, tag, StringComparison.Ordinal);
    }

    public class Component : Object
    {
        public GameObject gameObject { get; set; }
        public Transform transform { get; set; }
        public bool isActiveAndEnabled => gameObject != null && gameObject.activeSelf;

        public T GetComponent<T>() where T : class => gameObject?.GetComponent<T>();
        public T GetComponentInParent<T>() where T : class => gameObject?.GetComponentInParent<T>();
        public bool CompareTag(string tag) => gameObject != null && gameObject.CompareTag(tag);
        public virtual void SetActive(bool value) => gameObject?.SetActive(value);
    }

    public class Behaviour : Component { }

    public class Transform
    {
        private readonly List<Transform> children = new List<Transform>();

        public Transform(GameObject owner = null)
        {
            gameObject = owner;
        }

        public GameObject gameObject { get; private set; }
        public Vector3 position { get; set; }
        public Quaternion rotation { get; set; }
        public Vector3 localPosition { get; set; }
        public Quaternion localRotation { get; set; }
        public Transform parent { get; private set; }

        public Vector3 forward => new Vector3(0f, 0f, 1f);

        public bool IsChildOf(Transform parentTransform) => parentTransform != null && (this.parent == parentTransform || (this.parent != null && this.parent.IsChildOf(parentTransform)));

        public void SetParent(Transform parent, bool worldPositionStays = false)
        {
            if (this.parent != null && this.parent.children.Contains(this))
                this.parent.children.Remove(this);
            this.parent = parent;
            if (parent != null && !parent.children.Contains(this))
                parent.children.Add(this);
        }

        public Transform Find(string name)
        {
            if (gameObject != null && gameObject.name_ == name)
                return this;
            return null;
        }

        public T GetComponent<T>() where T : class => gameObject?.GetComponent<T>();
        public T GetComponentInParent<T>() where T : class => gameObject?.GetComponentInParent<T>();
    }

    public class Collider : Component
    {
        public Bounds bounds { get; set; }
    }

    public class SphereCollider : Collider { }

    public class Camera : Behaviour
    {
        public float depth { get; set; }
        public Rect rect { get; set; }
    }

    public static class Mathf
    {
        public const float PI = 3.141592653589793f;

        public static float Min(float a, float b) => a < b ? a : b;
        public static int Min(int a, int b) => a < b ? a : b;
        public static float Max(float a, float b) => a > b ? a : b;
        public static int Max(int a, int b) => a > b ? a : b;
        public static float Clamp(float value, float min, float max) => value < min ? min : value > max ? max : value;
        public static float Clamp01(float value) => Clamp(value, 0f, 1f);
        public static int RoundToInt(float value) => (int)Math.Round(value);
        public static float SmoothStep(float edge0, float edge1, float t)
        {
            t = Clamp01((t - edge0) / (edge1 - edge0));
            return t * t * (3f - 2f * t);
        }
        public static bool Approximately(float a, float b) => Math.Abs(a - b) <= 0.000001f;
        public static float Sqrt(float value) => (float)Math.Sqrt(value);
        public static float Abs(float value) => Math.Abs(value);
    }

    public static class Physics
    {
        public static Collider[] OverlapSphere(Vector3 center, float radius, LayerMask layerMask = default) => Array.Empty<Collider>();
        public static void SyncTransforms() { }
    }

    public static class Time
    {
        public static float time { get; set; }
        public static float deltaTime { get; set; }
        public static float unscaledDeltaTime { get; set; }
    }

    public class YieldInstruction { }

    public class WaitForSeconds : YieldInstruction
    {
        public WaitForSeconds(float seconds) { }
    }

    public static class Debug
    {
        public static void Log(string message) { }
        public static void LogWarning(string message) { }
        public static void LogError(string message, Object context = null) { }
    }

    public struct Color
    {
        public float r;
        public float g;
        public float b;
        public float a;

        public static Color cyan => new Color(0f, 1f, 1f, 1f);

        public Color(float r, float g, float b, float a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }
    }

    public static class Gizmos
    {
        public static Color color { get; set; }
        public static void DrawWireSphere(Vector3 center, float radius) { }
    }
}

namespace UnityEngine.Events
{
    public class UnityEvent
    {
        private readonly List<Action> listeners = new List<Action>();

        public void AddListener(Action callback) => listeners.Add(callback);
        public void RemoveListener(Action callback) => listeners.Remove(callback);
        public void Invoke()
        {
            foreach (var callback in listeners)
                callback?.Invoke();
        }
    }

    public class UnityEvent<T0>
    {
        private readonly List<Action<T0>> listeners = new List<Action<T0>>();

        public void AddListener(Action<T0> callback) => listeners.Add(callback);
        public void RemoveListener(Action<T0> callback) => listeners.Remove(callback);
        public void Invoke(T0 arg0)
        {
            foreach (var callback in listeners)
                callback?.Invoke(arg0);
        }
    }

    public class UnityEvent<T0, T1>
    {
        private readonly List<Action<T0, T1>> listeners = new List<Action<T0, T1>>();

        public void AddListener(Action<T0, T1> callback) => listeners.Add(callback);
        public void RemoveListener(Action<T0, T1> callback) => listeners.Remove(callback);
        public void Invoke(T0 arg0, T1 arg1)
        {
            foreach (var callback in listeners)
                callback?.Invoke(arg0, arg1);
        }
    }
}

namespace UnityEngine.TestTools
{
    public enum LogType
    {
        Error,
        Warning,
        Log,
    }

    public static class LogAssert
    {
        public static void Expect(LogType type, string message) { }
    }

    [AttributeUsage(AttributeTargets.Method)]
    public sealed class UnityTestAttribute : Attribute { }
}
