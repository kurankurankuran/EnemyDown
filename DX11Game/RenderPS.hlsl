// ÉpÉâÉÅÅ[É^
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
	float4 color = pin.Diffuse;
	if (1 != tex.Sample(samp, pin.TexCoord).r)
	{
		color.rgb = float3(1.0f,0.0f,0.0f) * tex.Sample(samp, pin.TexCoord).r;
	}
	else
	{
		color = pin.Diffuse;
	}

	return color;
}