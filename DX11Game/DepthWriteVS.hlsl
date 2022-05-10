struct VS_IN
{
	float3	Position	: POSITION;
	float3	Normal		: NORMAL;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};
struct VS_OUT
{
	float4 Position : SV_POSITION;
	float4 shadowPos : TEXCOOR0;
};

cbuffer ShaderGlobal : register(b0)
{
	float4x4 WVP;
	float4   color;
};

VS_OUT main(VS_IN vin)
{
	VS_OUT vout;
	vout.Position = float4(vin.Position, 1.0f);
	vout.Position = mul(vout.Position, WVP);
	vout.shadowPos = vout.Position;
	return vout;
}