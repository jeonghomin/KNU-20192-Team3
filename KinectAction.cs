using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class KinectAction : MonoBehaviour
{
    [DllImport("NtKinectDll")] private static extern System.IntPtr getKinect();
    [DllImport("NtKinectDll")] private static extern int setSkeleton(System.IntPtr kinect, System.IntPtr data, System.IntPtr state, System.IntPtr id);

    private System.IntPtr kinect;
    int bodyCount = 6;
    int jointCount = 25;
    GameObject[] obj;
    int counter;

    void Start()
    {
        kinect = getKinect();
        obj = new GameObject[bodyCount * jointCount];
        for (int i = 0; i < obj.Length; i++)
        {
            obj[i] = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            obj[i].transform.position = new Vector3(0, 0, -10);
            obj[i].transform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
        }
    }
    void Update()
    {
        float[] data = new float[bodyCount * jointCount * 3];
        int[] state = new int[bodyCount * jointCount];
        int[] id = new int[bodyCount];
        GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
        GCHandle gch2 = GCHandle.Alloc(state, GCHandleType.Pinned);
        GCHandle gch3 = GCHandle.Alloc(id, GCHandleType.Pinned);
        int n = setSkeleton(kinect, gch.AddrOfPinnedObject(), gch2.AddrOfPinnedObject(), gch3.AddrOfPinnedObject());
        gch.Free();
        gch2.Free();
        gch3.Free();
        int idx = 0;
        float refs;
        
        for (int i = 0; i < bodyCount; i++)
        {
            for (int j = 0; j < jointCount; j++)
            {
                if (i < n)
                {

                    float x = data[idx++], y = data[idx++], z = data[idx++];
                    obj[i * jointCount + j].transform.position = new Vector3(x, y, z);

                    if (j == 1 || j == 2 || j == 3 || j == 20)
                    {
                        refs = Mathf.Abs(obj[i * jointCount + 0].transform.position.x) - Mathf.Abs(obj[i * jointCount + 2].transform.position.x);

                        if (Mathf.Abs(refs) <= 0.1)
                        {
                            obj[i * jointCount + j].GetComponent<Renderer>().material.color = new Color(0.0f, 1.0f, 0.0f, 0.0f);
                        }
                        else
                        {
                            obj[i * jointCount + j].GetComponent<Renderer>().material.color = new Color(1.0f, 0.0f, 0.0f, 0.0f);
                        }
                    }
                    else
                    {
                        obj[i * jointCount + j].GetComponent<Renderer>().material.color = new Color(0.0f, 1.0f, 0.0f, 0.0f);
                    }
                }
                else
                {
                    obj[i * jointCount + j].transform.position = new Vector3(0, 0, -10);
                    obj[i * jointCount + j].GetComponent<Renderer>().material.color = new Color(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
        }
    }
}