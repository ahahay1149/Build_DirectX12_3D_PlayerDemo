
struct VS_IN      
{
    float3 pos : POSITION0;
    float3 nor : NORMAL0;
    float4 color : COLOR0;  
    float2 uv : TEXTURE0;
    
    uint4   indices0 : BLENDINDICES0;
    uint4   indices1 : BLENDINDICES1;
    float4  weights0 : BLENDWEIGHT0;
    float4  weights1 : BLENDWEIGHT1;
};

struct VS_OUT   
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXTURE0;
};

cbuffer ViewBuffer : register(b0)       
{
    float4x4    View;       
}

cbuffer ProjectionBuffer : register(b1) 
{
    float4x4    Projection; 
}

cbuffer WorldBuffer : register(b2)      
{
    float4x4    World;      
}

cbuffer MeshBone : register(b3)
{
    float4x4    bones[2];   
}

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    output.pos = float4(input.pos, 1.0f);

    float4x4 boneMtx;        
    boneMtx = bones[input.indices0[0]] * input.weights0[0];
    boneMtx += bones[input.indices0[1]] * input.weights0[1];
    boneMtx += bones[input.indices0[2]] * input.weights0[2];
    boneMtx += bones[input.indices0[3]] * input.weights0[3];
    boneMtx += bones[input.indices1[0]] * input.weights1[0];
    boneMtx += bones[input.indices1[1]] * input.weights1[1];
    boneMtx += bones[input.indices1[2]] * input.weights1[2];
    boneMtx += bones[input.indices1[3]] * input.weights1[3];

    output.pos = mul(boneMtx, output.pos);             Å@
    output.pos = mul(output.pos, World);              
    output.pos = mul(output.pos, View);               
    output.pos = mul(output.pos, Projection);         
    output.color = input.color;                     
    output.uv = input.uv;                           

    return output;
}