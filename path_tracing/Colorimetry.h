
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

	friend GeneralizedColor Sqrt(const GeneralizedColor& Value)
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
		{
			Result.Components[i] = std::sqrtf(Value.Components[i]);
		}
		return Result;
	}

	friend GeneralizedColor Pow(const GeneralizedColor& Value,float exponent)
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
		{
			Result.Components[i] = std::powf(Value.Components[i], exponent);
		}
		return Result;
	}

	friend GeneralizedColor Exp(const GeneralizedColor& Value)
	{
		GeneralizedColor Result;
		for (int i = 0; i < NumberComponents; ++i)
		{
			Result.Components[i] = std::exp(Value.Components[i]);
		}
		return Result;
	}
};

template<typename T,int NumberComponents>
inline  GeneralizedColor<T, NumberComponents> operator*(float Value, const GeneralizedColor<T, NumberComponents>& Color)
{
	return Color * Value;
}

template<typename T, int NumberComponents>
inline  GeneralizedColor<T, NumberComponents> operator*(const GeneralizedColor<T, NumberComponents>& lhs, const GeneralizedColor<T, NumberComponents>& rhs)
{
	return lhs.operator*(rhs);
}


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


