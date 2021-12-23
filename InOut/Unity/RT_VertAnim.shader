Shader "Game/Main_RT/VertexAnimation" {
	Properties {
		_MainTex("ColorMap", 2D) = "white" {}
		//How much the lighting contributes
		_LightWeight("Lighting Weight", Float) = 0.2
		//The shadow strength
		_ShadowAttenuation("Shadow Attenuation", Float) = 0.5
		//Step count for shadows
		_ShadowCascades("Shadow Cascades", Int) = 3
		//Animation time
		_Seconds("Time(s)", Float) = 4.0
		//Baked .bin data reference FPS
		_RefFPS("Reference FPS", Int) = 30
		//Animation playback speed
		_Speed("Speed", Float) = 1.0
	}
	SubShader{
		Tags {
			"RenderQueue" = "Opaque"
			"Queue" = "Geometry+10"
		}
		//blend alpha
		Blend SrcAlpha OneMinusSrcAlpha
		Pass {
			Name "Render Displacement"
			Lighting On
			Cull Off
			Tags {"LightMode" = "ForwardBase"}
			CGPROGRAM
			#pragma multi_compile_shadowcaster
			#pragma multi_compile_fwdbase
			#pragma vertex vert
			#pragma fragment frag
			#include "UnityCG.cginc"
			#include "UnityLightingCommon.cginc"
			#include "AutoLight.cginc"
			
			struct InputData {
				float4 vertex : POSITION;
				float3 normal : NORMAL;
				float2 uv : TEXCOORD0;
				//Vertex id for buffer indexing
				uint id : SV_VertexID;
			};

			struct v2f {
				float4 pos : SV_POSITION;
				float3 normal : NORMAL;
				float4 lightDirection : TEXCOORD0;
				LIGHTING_COORDS(0, 1)
				float2 uv : TEXCOORD2;
			};

			sampler2D _MainTex; float4 _MainTex_ST;
			float _LightWeight;
			float _ShadowAttenuation;
			int _ShadowCascades;
			float _Seconds; 
			int _RefFPS; 
			float _Speed;
			
			//Baked animation data
			StructuredBuffer<float3> VertexData;
			StructuredBuffer<int> OffsetData;
			int FrameStride;

			v2f vert(InputData i) {
				v2f o;
				int ObjectIndex = OffsetData[i.id];
				//Get current time and loop it into animation time frame
				float rt_ind = frac((_Time.y / _Seconds) * _Speed) * (float)_RefFPS;
				// frame range from current time for interpolation
				int2 fran = int2(floor(rt_ind), ceil(rt_ind)) * FrameStride + ObjectIndex;
				//Vertex displacement
				i.vertex.xyz += lerp(VertexData[fran.x], VertexData[fran.y], frac(rt_ind));
				o.pos = UnityObjectToClipPos(i.vertex);
				o.normal = UnityObjectToWorldNormal(i.normal);
				o.uv = TRANSFORM_TEX(i.uv, _MainTex);
				TRANSFER_VERTEX_TO_FRAGMENT(o);
				return o;
			}
			//Generate cartoon like cascaded shadows
			float1 GenerateShadows(v2f i, float Cascades, float Attenuation) {
				return max(ceil(LIGHT_ATTENUATION(i)*Cascades)/Cascades, (1-Attenuation));
			}

			fixed4 frag(v2f i) : SV_Target {
				//Sun light position
				float3 Light = _WorldSpaceLightPos0.xyz;
				//Sun angle
				float lightproduct = clamp(ceil(dot(i.normal, Light) * 2) / 2, 0, 1);
				//Generate model base color
				float4 ModelColor = float4(tex2D(_MainTex, i.uv).rgba * 1-_LightWeight);
				//Generate light color
				float4 LightColor = float4(_LightColor0.rgba * _LightWeight);
				//Remap sun angle to smooth out shading steps
				float LightShading = lerp(0.1, 0.2, lightproduct);
				//Calculate base color
				float3 BaseColor = clamp((ModelColor + LightColor + LightShading), 0, 1).rgb;
				//Generate shadows for model
				float ShadowPass = GenerateShadows(i, _ShadowCascades, _ShadowAttenuation);
				return float4(ShadowPass * BaseColor.rgb, 1);
			}
			ENDCG
		}
		Pass {
			Name "Bridge Lighting Pass"
			Tags {"LightMode" = "ShadowCaster"}

			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma multi_compile_shadowcaster
			#include "UnityCG.cginc"

			struct InputData {
				float4 vertex : POSITION;
				float3 normal : NORMAL;
				float4 texcoord : TEXCOORD0;
				uint id : SV_VertexID;
			};

			struct v2f {
				float4 pos : SV_POSITION;
			};

			float _Seconds; int _RefFPS; float _Speed;
			StructuredBuffer<float3> VertexData;
			StructuredBuffer<int> OffsetData;
			int FrameStride;

			v2f vert(InputData v) {
				v2f o;
				//Same vertex displacement for the shadows to work in real time
				int ObjectIndex = OffsetData[v.id];
				float rt_ind = frac((_Time.y / _Seconds) * _Speed) * (float)_RefFPS;
				int2 fran = int2(floor(rt_ind), ceil(rt_ind)) * FrameStride + ObjectIndex; 
				v.vertex.xyz += lerp(VertexData[fran.x], VertexData[fran.y], frac(rt_ind));
				TRANSFER_SHADOW_CASTER_NORMALOFFSET(o)
				return o;
			}

			//Standard unity fragment shading for shadows
			float4 frag(v2f i) : SV_Target {
				SHADOW_CASTER_FRAGMENT(i)
			}
			ENDCG
		}
	}
}