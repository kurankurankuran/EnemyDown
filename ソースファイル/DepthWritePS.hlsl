struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 shadowPos : TEXCOOR0;
};

cbuffer ShaderGlobal : register(b0)
{
	float4x4 WVP;
	float4   color;
};

float4 main(PS_IN pin) : SV_TARGET
{
	//float4 color = color;
	//color.r = pin.shadowPos.z / pin.shadowPos.w;
	return color;
}