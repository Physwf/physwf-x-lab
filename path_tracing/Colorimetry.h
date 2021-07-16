#pragma once

inline void XYZToRGB(const float xyz[3], float rgb[3]) {
	rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
	rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
	rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const float rgb[3], float xyz[3]) {
	xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
	xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
	xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

template <typename T,int NumberComponents>
class GeneralizedColor
{
protected:
	T Components[NumberComponents];
public:
	GeneralizedColor() 
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] = 0;
	}
	GeneralizedColor(T Value)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] = Value;
	}
	GeneralizedColor(const GeneralizedColor& rhs)
	{
		for (int i = 0; i < NumberComponents; ++i)
			Components[i] = rhs.Components[i];
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
	friend inline GeneralizedColor operator*(float Value, const GeneralizedColor& Color)
	{
		return Color * Value;
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
	float MaxComponentValue() const
	{
		float m = Components[0];
		for (int i = 1; i < NumberComponents; ++i)
		{
			m = std::max(m, Components[i]);
		}
		return m;
	}
	bool IsBlack() const
	{
		for (int i = 0; i < NumberComponents; ++i)
			if (Components[i] != 0) return false;
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


//Long Medium Short
class LMSColor : public GeneralizedColor<float,3>
{
public:
	LMSColor(){}
	LMSColor(float Value) : GeneralizedColor<float, 3>(Value) {}
	LMSColor(float r, float g, float b)
	{
		Components[0] = r;
		Components[1] = g;
		Components[2] = b;
	}
	LMSColor(const LMSColor& rhs) 
	{
		for (int i = 0; i < 3; ++i)
			Components[i] = rhs.Components[i];
	}
	LMSColor(const GeneralizedColor<float, 3>& v) : GeneralizedColor<float, 3>(v) {}

	void ToXYZ(float xyz[3]) const { RGBToXYZ(Components,xyz); }

	float operator[](int i) const
	{
		return Components[i];
	}

	float& operator[](int i)
	{
		return Components[i];
	}
};

class DiscreteLMSColor : public GeneralizedColor<unsigned char, 3>
{

};

typedef LMSColor LinearColor;


