
template <typename T,int NumberComponents>
class GeneralizedColor
{
protected:
	T Components[NumberComponents];
public:
	GeneralizedColor() {}
	GeneralizedColor(T Value)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Value;
	}
	GeneralizedColor(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = rhs.Components[i];
	}

	GeneralizedColor operator+(const GeneralizedColor& rhs) const
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Components[i] + rhs.Components[i];
		return Result;
	}
	GeneralizedColor operator-(const GeneralizedColor& rhs) const
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Components[i] - rhs.Components[i];
		return Result;
	}
	GeneralizedColor operator*(const GeneralizedColor& rhs) const
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Components[i] * rhs.Components[i];
		return Result;
	}
	GeneralizedColor operator*(float Value) const
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Components[i] * Value;
		return Result;
	}
	GeneralizedColor operator/(const GeneralizedColor& rhs) const
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Components[i] / rhs.Components[i];
		return Result;
	}
	GeneralizedColor operator/(float Value) const
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
			Result.Components[i] = Components[i] / Value;
		return Result;
	}

	GeneralizedColor& operator+=(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] += rhs.Components[i];
		return *this;
	}
	GeneralizedColor& operator-=(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] -= rhs.Components[i];
		return *this;
	}
	GeneralizedColor& operator*=(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] *= rhs.Components[i];
		return *this;
	}
	GeneralizedColor& operator*=(float Value)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] *= Value;
		return *this;
	}
	GeneralizedColor& operator/=(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] /= rhs.Components[i];
		return *this;
	}
	GeneralizedColor& operator/=(float Value)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] /= Value;
		return *this;
	}

	bool operator==(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			if (Components[i] != rhs.Components[i]) return false;
		return true;
	}
	bool operator!=(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			if (Components[i] == rhs.Components[i]) return false;
		return true;
	}
};
//Long Medium Short
class LMSColor : public GeneralizedColor<float,3>
{
public:
	LMSColor(){}
	LMSColor(float Value) : GeneralizedColor<float, 3>(Value) {}
	LMSColor(const LMSColor& rhs) 
	{
		for (int i = 0; i < 3; ++i)
			Components[i] /= rhs.Components[i];
	}
	LMSColor(const GeneralizedColor<float, 3>& v) : GeneralizedColor<float, 3>(v) {}
};

class DiscreteLMSColor : public GeneralizedColor<unsigned char, 3>
{

};

inline LMSColor XYZToRGB(const LMSColor& XYZ)
{

}
inline LMSColor RGBToXYZ(const LMSColor& XYZ)
{

}

typedef LMSColor LinearColor;


