

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXTURE0;
};

Texture2D		Texture : register(t0[0]);

SamplerState	Sampler : register(s0[0]);

float4 main(PS_IN input) : SV_Target
{
	float4 tex_color = Texture.Sample(Sampler, input.uv);
	float4 finalColor = tex_color * input.color;

	return finalColor;
}