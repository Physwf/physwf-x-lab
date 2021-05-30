
class Math
{
public:

	template <typename T>
	static T Clamp(T Value, T Min, T Max)
	{
		if (Value < Min) return Min;
		if (Value > Max) return Max;
		return Value;
	}
};