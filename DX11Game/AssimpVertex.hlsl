// Assimp用頂点シェーダ (AssimpVertex.hlsl)

#define MAX_BONE_MATRIX	64

// グローバル
cbuffer global : register(b0) {
	matrix	g_mtxWVP;			// ワールド×ビュー×射影行列
	matrix	g_mtxWorld;			// ワールド行列
	matrix	g_mtxTexture;		// テクスチャ行列
	float4	g_vCameraPos;		// 視点座標(ワールド空間)
	float4	g_vLightDir;		// 光源方向(ワールド空間)
	float4	g_vLightAmbient;	// 環境光
	float4	g_vLightDiffuse;	// 拡散光
	float4	g_vLightSpecular;	// 鏡面反射光
};

// ボーンのポーズ行列
cbuffer global_bones : register(b2) {
	matrix g_BoneWorld[MAX_BONE_MATRIX];
};

// パラメータ
struct VS_INPUT {
	float3	Pos		: POSITION;
	float3	Normal	: NORMAL;
	float2	Tex		: TEXCOORD0;
	uint4	Bone	: BONE_INDEX;	// ボーンのインデックス
	float4	Weight	: BONE_WEIGHT;	// ボーンの重み
	float3	Tangent	: TANGENT;
};

struct VS_OUTPUT {
	float4	Pos			  : SV_Position;
	float2	Tex			  : TEXCOORD0;
	float3	Normal		  : TEXCOORD1;
	float3	PosForPS	  : TEXCOORD2;
	float3  texSpaceLight : TEXCOORD3; // テクスチャ空間上のライト向き
	float3  Tangent       : TANGENT;
};

// スキニング後の頂点・法線
struct SKIN {
	float4 Pos;
	float3 Norm;
};

// 頂点をスキニング (ボーンにより移動)
SKIN SkinVert(VS_INPUT input)
{
	SKIN output;
	float4 Pos = float4(input.Pos, 1.0f);
	float3 Norm = input.Normal;
	if (input.Weight.x == 0.0f) {
		// ボーン無し
		output.Pos = Pos;
		output.Norm = Norm;
	} else {
		// ボーン0
		uint uBone = input.Bone.x;
		float fWeight = input.Weight.x;
		matrix m = g_BoneWorld[uBone];
		output.Pos = fWeight * mul(Pos, m);
		output.Norm = fWeight * mul(Norm, (float3x3)m);
		// ボーン1
		uBone = input.Bone.y;
		fWeight = input.Weight.y;
		m = g_BoneWorld[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3)m);
		// ボーン2
		uBone = input.Bone.z;
		fWeight = input.Weight.z;
		m = g_BoneWorld[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3)m);
		// ボーン3
		uBone = input.Bone.w;
		fWeight = input.Weight.w;
		m = g_BoneWorld[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3)m);
	}
	return output;
}

//
// 頂点シェーダ
//
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	SKIN vSkinned = SkinVert(input);
	output.Pos = mul(vSkinned.Pos, g_mtxWVP);
	output.Tex = mul(float4(input.Tex, 0.0f, 1.0f), g_mtxTexture).xy;
	output.Normal = mul(vSkinned.Norm, (float3x3)g_mtxWorld);
	output.PosForPS = mul(vSkinned.Pos, g_mtxWorld).xyz;



	// ワールド空間上のライトをテクスチャ空間に起きなおす
	//　法線と接ベクトルから姿勢行列を求める
	float3 worldNormal;
	float3 worldTangent;
	worldNormal = mul(vSkinned.Norm, (float3x3)g_mtxWorld);
	worldTangent = mul(input.Tangent, (float3x3)g_mtxWorld);


	float3 N = normalize(worldNormal/*input.Normal*/);
	float3 T = normalize(worldTangent/*input.Tangent*/);
	float3 B = normalize(cross(N, T));
	// テクスチャ空間からワールド空間に変換する姿勢行列を求め、
	// ライトに対してその逆行列をかける
	// 姿勢行列の逆行列は転置で求めることができる
	float3x3 invTexToWorld =
		float3x3(
			T.x, B.x, N.x,
			T.y, B.y, N.y,
			T.z, B.z, N.z
			);
	float3 L = normalize(g_vLightDir.xyz);
	output.texSpaceLight = mul(L, invTexToWorld);

	output.Tangent = worldTangent/*input.Tangent*/;

	return output;
}
