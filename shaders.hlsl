struct vs_in
{
	float3 Position : POS;
};

struct vs_out
{
	float4 Clip : SV_POSITION;
};

vs_out vs_main(vs_in Input)
{
	vs_out Result;
	Result.Clip = float4(Input.Position, 1.0);
	return Result;
}

float4 ps_main(vs_out Input) : SV_TARGET
{
	float4 Result = float4(0.0, 1.0, 0.0, 1.0);
	return Result;
}
