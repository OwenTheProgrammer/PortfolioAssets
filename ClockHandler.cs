using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class ClockHandler : MonoBehaviour {
    [Header("Settings")]
    public Color OnColor, OffColor;
    public Texture2D ClockDefault;
    [Header("Driver Links")]
    public Material[] LinkedShaders;

    //Handled with BinFileLoader
    [HideInInspector] public int[][] HourTable;
    [HideInInspector] public int[][] DisplayTruthTable;
    private Texture2D TimeTexture;

    //Generate buffer for the time shader
    public void GenerateTimeMap(ref Texture2D tex, Color OnColor, Color OffColor) {
        //if texture is null then make a new one
        tex ??= new Texture2D(5,7,TextureFormat.RGB24,false);
        byte[] ImageData = new byte[tex.width*tex.height*3];
        //Setup of the image data doesn't matter
        // for(int i = 0; i < ImageData.Length; i+=3)
        //     for(int j = 0 ; j < 3; j++)
        //         ImageData[i+j] = OffColor[j];
        //tex.SetPixelData(ImageData, 0, 0);
        //SetPixelData is faster, I will convert the rest later
        /*for (int x = 0; x < tex.width; x++)
            for (int y = 0; y < tex.height; y++)
                tex.SetPixel(x, y, Color.black);*/

        //Get the system time
        System.DateTime RLTime = System.DateTime.Now;
        int hour = RLTime.Hour;
        int minute = RLTime.Minute;

        //Look up time on hour table
        int[] HourArray = HourTable[hour];
        int mintens = Mathf.FloorToInt(minute/10f);
        int minones = minute-(mintens*10);

        //Convert with truth table for leds
        // int[] HLeft = DisplayTruthTable[HourArray[0]];
        // int[] HRight = DisplayTruthTable[HourArray[1]];
        // int[] MLeft = DisplayTruthTable[mintens+1];
        // int[] MRight = DisplayTruthTable[minones+1];

        //Write colors to the display texture
        //COMPLETE: Convert SetPixel to ImageData bytes
        Hashtable ValueTable = new Hashtable {
            {0,DisplayTruthTable[HourArray[0]]},{1,DisplayTruthTable[HourArray[1]]},
            {3,DisplayTruthTable[mintens+1]},{4,DisplayTruthTable[minones+1]}
        };
        foreach(int x in (new int[]{0,1,3,4})) {
            for(int y=0;y<tex.height;y++) {
                for(int j=0;j<3;j++) {
                    ImageData[(tex.height*y+x)*3+j]=
                    ValueTable[x][y]==1?OnColor[j]:OffColor[j];
                }
            }
            //tex.SetPixel(x,y,ValueTable[x][y]==1?OnColor:OffColor);
        }
        // for(int y=0;y<tex.height;y++)tex.SetPixel(0,y,HLeft[y]==1?OnColor:OffColor);
        // for(int y=0;y<tex.height;y++)tex.SetPixel(1,y,HRight[y]==1?OnColor:OffColor);
        // for(int y=0;y<tex.height;y++)tex.SetPixel(3,y,MLeft[y]==1?OnColor:OffColor);
        // for(int y=0;y<tex.height;y++)tex.SetPixel(4,y,MRight[y]==1?OnColor:OffColor);
        tex.SetPixelData(ImageData, 0, 0);
        tex.filterMode = FilterMode.Point; tex.Apply();
    }

    //calculate lead time
    private float TimeUntilMinute() {
        System.DateTime rtc = System.DateTime.Now;
        return(60f-rtc.Second)+(1f-(rtc.Millisecond/1000f));
    }

    //Recalculate every minute(threaded)
    IEnumerator OnTimeChange() {
        while (true) {
            GenerateTimeMap(ref TimeTexture, OnColor, OffColor);
            foreach (Material shader in LinkedShaders)
                shader.SetTexture("_DispTex", TimeTexture);
            yield return new WaitForSecondsRealtime(TimeUntilMinute());
        }
    }
    //Switch the state of the dot point (threaded)
    IEnumerator UpdateBlink() {
        bool State = true;
        while (true) {
            if(TimeTexture != null) {
                //Write dot point
                State = !State;
                TimeTexture.SetPixel(2, 6, State?OnColor:OffColor);
                TimeTexture.Apply();
                //Update all shader values
                foreach (Material shader in LinkedShaders)
                    shader.SetTexture("_DispTex", TimeTexture);
            }
            yield return new WaitForSecondsRealtime(1f);
        }
    }

    //Start up the processing threads
    private void Awake() {
        StartCoroutine("OnTimeChange");
        StartCoroutine("UpdateBlink");
    }

    //Remove links and relink to the default for editor
    private void OnDestroy() {
        foreach (Material shader in LinkedShaders)
            shader.SetTexture("_DispTex", TimeTexture);
    }
}