

struct VS_IN	
{
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float4 color : COLOR0;	
	float2 uv : TEXTURE0;
};

struct VS_OUT	
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXTURE0;
};

cbuffer ViewBuffer : register(b0)	
{
	float4x4	View;	
}

cbuffer ProjectionBuffer : register(b1)		
{
	float4x4	Projection;		
}

cbuffer WolrdBuffer : register(b2)	
{
	float4x4	World;	
}

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	output.pos = float4(input.pos, 1.0f);

	output.pos = mul(output.pos, World);

	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	output.color = input.color;

	output.uv = input.uv;

	return output;
}