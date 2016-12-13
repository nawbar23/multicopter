// ========== robo3D_Lib ==========
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __FLAGS__
#define __FLAGS__

template <class _T> class Flags
{
	_T value;

public:
	Flags(void) :
		value(0)
	{
	}

	Flags(const _T _value):
		value(_value)
	{
	}

	_T& getFlagsVector(void)
	{
		return value;
	}

	const _T& getFlagsVector(void) const
	{
		return value;
	}

	bool getFlagState(const unsigned id) const
	{
		return (value & (1 << id)) != 0;
	}

	void setFlagState(const unsigned id, const bool state)
	{
		if (state)
		{
			value |= 1 << id;
		}
		else
		{
			value &= ~(1 << id);
		}
	}
};

#endif // __FLAGS__