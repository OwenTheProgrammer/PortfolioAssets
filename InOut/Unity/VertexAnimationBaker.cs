using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

public class VertexAnimationBaker : MonoBehaviour
{
    public string MeshBin;
    public string AnimBin;
    public string MeshAssetPath;
    public string OutputBinPath;
    private Mesh[] MeshParts;

    public void ExecuteMesh() {
        //Get all children from parent
        MeshParts = new Mesh[transform.childCount];
        MeshFilter[] Filters = transform.GetComponentsInChildren<MeshFilter>(true);
        for(int i = 0; i < MeshParts.Length; i++) MeshParts[i] = Filters[i].sharedMesh;

        //Make lists for meshes
        Mesh output = new Mesh();
        List<Vector3> Verts = new List<Vector3>();
        List<Vector3> Norms = new List<Vector3>();
        List<Vector2> UVs = new List<Vector2>();
        List<int> Tris = new List<int>();

        //Merge all mesh data into big mesh to preserve mesh ordering
        for (int i = 0; i < MeshParts.Length; i++) {
            Mesh part = MeshParts[i]; int ofs = 0;
            for (int j = 0; j < i; j++) ofs += MeshParts[j].vertexCount; //triangle offset
            for (int j = 0; j < part.vertexCount; j++) Verts.Add(transform.GetChild(i).position + part.vertices[j]); //relative+vertex
            for (int j = 0; j < part.triangles.Length; j++) Tris.Add(part.triangles[j] + ofs); //add triangles
            for (int j = 0; j < part.normals.Length; j++) Norms.Add(part.normals[j]); //add normals
            for (int j = 0; j < part.uv.Length; j++) UVs.Add(part.uv[j]);//add uvs
        }
        //Combine into a new mesh
        output.vertices = Verts.ToArray();
        output.normals = Norms.ToArray();
        output.uv = UVs.ToArray();
        output.triangles = Tris.ToArray();
        output.RecalculateBounds();
        output.RecalculateTangents();
        //Create unity asset
        AssetDatabase.CreateAsset(output, MeshAssetPath);
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();
    }

    public void BakeAnimation() {
        //Path structuring
        string MeshFile = Application.dataPath + AnimBin;
        string OutputPath = Application.dataPath + OutputBinPath;
        BinaryIO MeshData = new BinaryIO(MeshFile, BinaryIO.RWType.Read);
        int AnimFrameCount = MeshData.ReadSingleInt(MeshData.FindStringID("FC"));
        //build up the child mesh arrays
        List<Vector3> Disp = new List<Vector3>();
        List<int> Offsets = new List<int>();
        MeshParts = new Mesh[transform.childCount];
        //Find all MeshFilters (disabled objects inclusive)
        MeshFilter[] Filters = transform.GetComponentsInChildren<MeshFilter>(true);
        for (int i = 0; i < MeshParts.Length; i++) MeshParts[i] = Filters[i].sharedMesh;
        //Add each DestructionBaker frame
        for (int i = 0; i < AnimFrameCount; i++) {
            Disp.AddRange(MeshData.ReadVec3Array(MeshData.FindStringID($"FRM{i}")));
        }
        //Add each triangle offset for the meshes so we can index it in the shader
        for (int i = 0; i < MeshParts.Length; i++)
            for (int j = 0; j < MeshParts[i].vertexCount; j++)
                Offsets.Add(i);

        //Make a new binary file for unity
        BinaryIO outputFile = new BinaryIO(OutputPath, BinaryIO.RWType.Write);
        //Object count
        outputFile.AddStringID("OCNT", transform.childCount);
        //Triangle offsets
        outputFile.AddStringID("OFST");
        outputFile.WriteIntArray(Offsets.ToArray());
        //Animation data
        outputFile.AddStringID("FDATA");
        outputFile.WriteVec3Array(Disp.ToArray());
        outputFile.EndWrite();
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();
    }

}

#if UNITY_EDITOR
//Simple buttons at the bottom of the script to run each function
[CustomEditor(typeof(VertexAnimationBaker)), CanEditMultipleObjects]
public class VertexAnimationBakerEditor : Editor {
    public override void OnInspectorGUI() {
        base.OnInspectorGUI();
        VertexAnimationBaker script = (VertexAnimationBaker)target;
        if(GUILayout.Button("Bake Mesh")) {
            script.ExecuteMesh();
        }
        if (GUILayout.Button("Bake Animation")) {
            script.BakeAnimation();
        }
    }
}
#endif