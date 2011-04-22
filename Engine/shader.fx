cbuffer cbPerObject
{
	float4x4 gWVP; 
};

struct VS_IN
{
	float3 iPosL  : POSITION;
    float4 iColor : COLOR;
	float3 offPos : vPPos;
};

struct VS_OUT
{
	float4 oPosH  : SV_POSITION;
    float4 oColor : COLOR0;
};


VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;

	// Transform to homogeneous clip space.
	float3 test=vIn.offPos;
	test=float3(1.0f,1.0f,1.0f);
	test=vIn.offPos;
	vOut.oPosH = mul(float4(vIn.iPosL+test, 1.0f), gWVP);
	
	// Just pass vertex color into the pixel shader.
    vOut.oColor = vIn.iColor;

	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target
{
    return pIn.oColor;
}

technique10 ColorTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
