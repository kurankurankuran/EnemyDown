// Assimp�p���_�V�F�[�_ (AssimpVertex.hlsl)

#define MAX_BONE_MATRIX	64

// �O���[�o��
cbuffer global : register(b0) {
	matrix	g_mtxWVP;			// ���[���h�~�r���[�~�ˉe�s��
	matrix	g_mtxWorld;			// ���[���h�s��
	matrix	g_mtxTexture;		// �e�N�X�`���s��
	float4	g_vCameraPos;		// ���_���W(���[���h���)
	float4	g_vLightDir;		// ��������(���[���h���)
	float4	g_vLightAmbient;	// ����
	float4	g_vLightDiffuse;	// �g�U��
	float4	g_vLightSpecular;	// ���ʔ��ˌ�
};

// �{�[���̃|�[�Y�s��
cbuffer global_bones : register(b2) {
	matrix g_BoneWorld[MAX_BONE_MATRIX];
};

// �p�����[�^
struct VS_INPUT {
	float3	Pos		: POSITION;
	float3	Normal	: NORMAL;
	float2	Tex		: TEXCOORD0;
	uint4	Bone	: BONE_INDEX;	// �{�[���̃C���f�b�N�X
	float4	Weight	: BONE_WEIGHT;	// �{�[���̏d��
	float3	Tangent	: TANGENT;
};

struct VS_OUTPUT {
	float4	Pos			  : SV_Position;
	float2	Tex			  : TEXCOORD0;
	float3	Normal		  : TEXCOORD1;
	float3	PosForPS	  : TEXCOORD2;
	float3  texSpaceLight : TEXCOORD3; // �e�N�X�`����ԏ�̃��C�g����
	float3  Tangent       : TANGENT;
};

// �X�L�j���O��̒��_�E�@��
struct SKIN {
	float4 Pos;
	float3 Norm;
};

// ���_���X�L�j���O (�{�[���ɂ��ړ�)
SKIN SkinVert(VS_INPUT input)
{
	SKIN output;
	float4 Pos = float4(input.Pos, 1.0f);
	float3 Norm = input.Normal;
	if (input.Weight.x == 0.0f) {
		// �{�[������
		output.Pos = Pos;
		output.Norm = Norm;
	} else {
		// �{�[��0
		uint uBone = input.Bone.x;
		float fWeight = input.Weight.x;
		matrix m = g_BoneWorld[uBone];
		output.Pos = fWeight * mul(Pos, m);
		output.Norm = fWeight * mul(Norm, (float3x3)m);
		// �{�[��1
		uBone = input.Bone.y;
		fWeight = input.Weight.y;
		m = g_BoneWorld[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3)m);
		// �{�[��2
		uBone = input.Bone.z;
		fWeight = input.Weight.z;
		m = g_BoneWorld[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3)m);
		// �{�[��3
		uBone = input.Bone.w;
		fWeight = input.Weight.w;
		m = g_BoneWorld[uBone];
		output.Pos += fWeight * mul(Pos, m);
		output.Norm += fWeight * mul(Norm, (float3x3)m);
	}
	return output;
}

//
// ���_�V�F�[�_
//
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	SKIN vSkinned = SkinVert(input);
	output.Pos = mul(vSkinned.Pos, g_mtxWVP);
	output.Tex = mul(float4(input.Tex, 0.0f, 1.0f), g_mtxTexture).xy;
	output.Normal = mul(vSkinned.Norm, (float3x3)g_mtxWorld);
	output.PosForPS = mul(vSkinned.Pos, g_mtxWorld).xyz;



	// ���[���h��ԏ�̃��C�g���e�N�X�`����ԂɋN���Ȃ���
	//�@�@���Ɛڃx�N�g������p���s������߂�
	float3 worldNormal;
	float3 worldTangent;
	worldNormal = mul(vSkinned.Norm, (float3x3)g_mtxWorld);
	worldTangent = mul(input.Tangent, (float3x3)g_mtxWorld);


	float3 N = normalize(worldNormal/*input.Normal*/);
	float3 T = normalize(worldTangent/*input.Tangent*/);
	float3 B = normalize(cross(N, T));
	// �e�N�X�`����Ԃ��烏�[���h��Ԃɕϊ�����p���s������߁A
	// ���C�g�ɑ΂��Ă��̋t�s���������
	// �p���s��̋t�s��͓]�u�ŋ��߂邱�Ƃ��ł���
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
