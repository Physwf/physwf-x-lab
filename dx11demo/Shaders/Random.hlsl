// @param xy should be a integer position (e.g. pixel position on the screen), repeats each 128x128 pixels
// similar to a texture lookup but is only ALU
// ~13 ALU operations (3 frac, 6 *, 4 mad)
float PseudoRandom(float2 xy)
{
	float2 pos = frac(xy / 128.0f) * 128.0f + float2(-64.340622f, -72.465622f);
	
	// found by experimentation
	return frac(dot(pos.xyx * pos.xyy, float3(20.390625f, 60.703125f, 2.4281209f)));
}