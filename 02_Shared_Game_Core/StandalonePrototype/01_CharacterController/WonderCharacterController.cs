using System;
using UnityEngine;
using WonderWorld.Audio;
using WonderWorld.Tycoon;

namespace WonderWorld.Character
{
    /// <summary>
    /// 3D Third-Person Character & Pragmata Exosuit Traversal Controller.
    /// Supports WASD street walking, mouse look, [E] walk-in storefront interaction,
    /// and [V] 85 BPM anti-gravity hover-drive / skybridge grapple flight.
    /// </summary>
    public class WonderCharacterController : MonoBehaviour
    {
        public static WonderCharacterController Instance { get; private set; }

        [Header("Street Movement")]
        public float WalkSpeed = 6.5f;
        public float SprintMultiplier = 1.65f;
        public float JumpVelocity = 7.5f;
        public float Gravity = -19.62f;

        [Header("Hover-Drive & Exosuit Flight ([V] Toggle)")]
        public bool IsHoverDriveEngaged = false;
        public float HoverFlightSpeed = 24.0f;
        public float HoverClimbSpeed = 14.0f;
        public float TargetHoverAltitude = 18.0f;

        [Header("Camera & Interaction")]
        public float MouseSensitivity = 2.2f;
        public float InteractionRadius = 4.0f;

        private Vector3 _velocity;
        private float _yaw;
        private float _pitch = 12f;

        private void Awake()
        {
            Instance = this;
        }

        private void Update()
        {
            HandleCameraLook();

            if (Input.GetKeyDown(KeyCode.V))
            {
                ToggleHoverDrive();
            }

            if (Input.GetKeyDown(KeyCode.E))
            {
                TryInteractWithNearestStorefront();
            }

            if (IsHoverDriveEngaged)
            {
                SimulateHoverDriveFlight(Time.deltaTime);
            }
            else
            {
                SimulateGroundTraversal(Time.deltaTime);
            }
        }

        public void ToggleHoverDrive()
        {
            IsHoverDriveEngaged = !IsHoverDriveEngaged;
            if (IsHoverDriveEngaged)
            {
                TargetHoverAltitude = Mathf.Max(transform.position.y + 8.0f, 16.0f);
                Debug.Log("[WonderCharacter] ✈️ [V] 85 BPM Exosuit Hover-Drive ENGAGED!");
            }
            else
            {
                Debug.Log("[WonderCharacter] 🚶 [V] Returned to Street-Level Walk Mode.");
            }
        }

        private void HandleCameraLook()
        {
            float mouseX = Input.GetAxis("Mouse X") * MouseSensitivity;
            float mouseY = Input.GetAxis("Mouse Y") * MouseSensitivity;

            _yaw += mouseX;
            _pitch = Mathf.Clamp(_pitch - mouseY, -35f, 65f);
            transform.rotation = Quaternion.Euler(0f, _yaw, 0f);
        }

        private void SimulateGroundTraversal(float dt)
        {
            float moveX = 0f;
            float moveZ = 0f;
            if (Input.GetKey(KeyCode.W)) moveZ += 1f;
            if (Input.GetKey(KeyCode.S)) moveZ -= 1f;
            if (Input.GetKey(KeyCode.D)) moveX += 1f;
            if (Input.GetKey(KeyCode.A)) moveX -= 1f;

            bool sprinting = Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift);
            float speed = WalkSpeed * (sprinting ? SprintMultiplier : 1.0f);

            Vector3 moveDir = (transform.forward * moveZ + transform.right * moveX).normalized;
            Vector3 horizontalDisp = moveDir * (speed * dt);

            if (transform.position.y <= 0.01f)
            {
                transform.position = new Vector3(transform.position.x, 0f, transform.position.z);
                _velocity.y = 0f;
                if (Input.GetKeyDown(KeyCode.Space))
                {
                    _velocity.y = JumpVelocity;
                }
            }
            else
            {
                _velocity.y += Gravity * dt;
            }

            transform.position += horizontalDisp + new Vector3(0f, _velocity.y * dt, 0f);
        }

        private void SimulateHoverDriveFlight(float dt)
        {
            float moveX = 0f;
            float moveZ = 0f;
            if (Input.GetKey(KeyCode.W)) moveZ += 1f;
            if (Input.GetKey(KeyCode.S)) moveZ -= 1f;
            if (Input.GetKey(KeyCode.D)) moveX += 1f;
            if (Input.GetKey(KeyCode.A)) moveX -= 1f;

            if (Input.GetKey(KeyCode.Space)) TargetHoverAltitude += HoverClimbSpeed * dt;
            if (Input.GetKey(KeyCode.C) || Input.GetKey(KeyCode.LeftControl))
            {
                TargetHoverAltitude = Mathf.Max(2.0f, TargetHoverAltitude - HoverClimbSpeed * dt);
            }

            float harmonicBob = Harmonic85BpmSubClock.Instance != null
                ? Harmonic85BpmSubClock.Instance.GetHarmonicWaveOffset() * 0.45f
                : Mathf.Sin(Time.time * (85f / 60f) * Mathf.PI * 2f) * 0.45f;

            float boost = (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift)) ? 2.1f : 1.0f;
            Vector3 moveDir = (transform.forward * moveZ + transform.right * moveX).normalized;
            Vector3 nextPos = transform.position + moveDir * (HoverFlightSpeed * boost * dt);
            nextPos.y = Mathf.MoveTowards(transform.position.y, TargetHoverAltitude + harmonicBob, HoverClimbSpeed * 1.5f * dt);
            transform.position = nextPos;
        }

        private void TryInteractWithNearestStorefront()
        {
            if (WalkInStorefrontTycoon.ActiveInstance != null)
            {
                WalkInStorefrontTycoon.ActiveInstance.InteractAtPlayerPosition(transform.position);
            }
        }
    }
}
