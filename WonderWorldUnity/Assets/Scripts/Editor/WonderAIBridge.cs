// This script enables Antigravity AI to communicate with the Unity Editor.
// Place it under Assets/Scripts/Editor so it only compiles in the Unity Editor.

#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;
using System.IO;

[InitializeOnLoad]
public static class WonderAIBridge
{
    // Path to the command bus that Antigravity writes JSONL messages to.
    private static readonly string CommandBusPath =
        @"C:\Users\Admin\Documents\Unreal Projects\WonderTowerTycoon\WonderPipeline\06_Engine_Command_Logs\unity_cs2_command_bus.jsonl";

    static WonderAIBridge()
    {
        // Register an update callback so we poll the command bus each editor frame.
        EditorApplication.update += CheckIncomingCommands;
        Debug.Log("[WonderAIBridge] Antigravity AI listener initialized in Unity Editor.");
    }

    private static void CheckIncomingCommands()
    {
        if (!File.Exists(CommandBusPath)) return;

        string[] lines = File.ReadAllLines(CommandBusPath);
        if (lines.Length == 0) return;

        foreach (var line in lines)
        {
            if (string.IsNullOrWhiteSpace(line)) continue;

            // Extend this block with additional command keywords as needed.
            if (line.Contains("SETUP_HOVER_VEHICLE"))
            {
                SetupHoverVehicleScene();
            }
        }

        // Clear the bus after processing to avoid re-running commands.
        File.WriteAllText(CommandBusPath, string.Empty);
    }

    [MenuItem("WonderWorld/AI Setup Hover Vehicle")]
    public static void SetupHoverVehicleScene()
    {
        // Find or create a root GameObject for the hover vehicle.
        GameObject vehicle = GameObject.Find("HoverVehicle");
        if (vehicle == null)
        {
            vehicle = new GameObject("HoverVehicle");
            vehicle.AddComponent<Rigidbody>();
        }

        // Ensure the Antigravity hover controller component is attached.
        var hover = vehicle.GetComponent<WonderWorld.Vehicles.HoverDriveVehicle>();
        if (hover == null)
        {
            hover = vehicle.AddComponent<WonderWorld.Vehicles.HoverDriveVehicle>();
        }

        // Ensure a visible mesh (placeholder cube) exists for quick visual feedback.
        var meshFilter = vehicle.GetComponent<MeshFilter>();
        if (meshFilter == null) meshFilter = vehicle.AddComponent<MeshFilter>();
        var meshRenderer = vehicle.GetComponent<MeshRenderer>();
        if (meshRenderer == null) meshRenderer = vehicle.AddComponent<MeshRenderer>();

        // Assign a basic cube mesh if none is set (optional placeholder).
        if (meshFilter.sharedMesh == null)
        {
            meshFilter.sharedMesh = PrimitiveType.Cube.GetMesh(); // UnityEditor helper – works in editor only.
        }

        Debug.Log("[WonderAIBridge] HoverVehicle setup completed with visible mesh and physics controller!");
    }
}
#endif