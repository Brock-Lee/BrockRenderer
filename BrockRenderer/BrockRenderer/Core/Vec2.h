#pragma once

template < typename T >
class _TVector2
{
public:
	union{
		T val[2];
		struct{
			T x,y;
		};
	};
	_TVector2(): x(0), y(0) {}

	_TVector2(const _TVector2& vecCopy)
		: x(vecCopy.x), y(vecCopy.y) { }

	_TVector2(const T *fpVec)
		: x(fpVec[0]), y(fpVec[1]) { }

	_TVector2(const T &fX, const T &fY)
		: x(fX), y(fY) { }
	T& operator[](int iIdx) {
		if (iIdx < 0 || iIdx > 2) throw std::out_of_range("vector2 out of range");
		return val[iIdx];
	}
	T Dot(const _TVector2& vec) const {
		return( (vec.x * x) + (vec.y * y) );
	}
	void Normalize() {
		T fInvMag = 1 / GetLength();
		x *= fInvMag;
		y *= fInvMag;
	}
	T GetLength() const
	{
		return T(sqrt(1.0 * (x * x + y * y)));
	}

	_TVector2 operator+ (const _TVector2& vec) const {
        return( _TVector2((vec[0] + x), (vec[1] + y)) );
    }

    _TVector2 operator- (const _TVector2& vec) const {
        return( _TVector2(
                    ( x - vec[0] ),
                    ( y - vec[1] )));
    }

    const void operator += (const _TVector2& vec)
    {
        x += vec[0];
        y += vec[1];
    }

    const void operator -= (const _TVector2& vec)
    {
        x -= vec[0];
        y -= vec[1];
    }
	_TVector2 operator *  (const T fScalar) const
    {
        return( _TVector2(
                    ( x * fScalar ),
                    ( y * fScalar )));
    }

    _TVector2 operator /  (const T fScalar) const
    {
        T fInvScale = 1 / fScalar;

		return( _TVector2(
			( x * fInvScale ),
			( y * fInvScale )));
    }
	const void operator *= (const T fScalar)
    {
        x *= fScalar;
        y *= fScalar;
    }

    const void operator /= (const T fScalar)
    {
        T fInvScale = 1 / fScalar;

        x *= fInvScale;
        y *= fInvScale;
    }
	template <class T2>
    const void operator = (const _TVector2<T2>& vec)
    {
        x = static_cast<T>(vec[0]);
        y = static_cast<T>(vec[1]);
    }

    /**
    * @brief See if the stored vector and another are equal
    * @param vec
    * @return
    */
    const bool	operator == (const _TVector2& vec)  const
    {
        return (
                    (x == vec.x) &&
                    (y == vec.y));
    }

    /**
    * @brief See if the stored vector and another are not equal
    * @param vec
    * @return
    */
    const bool	operator != (const _TVector2& vec)  const
    {
        return (!(*this == vec));
    }


	const bool operator > (const _TVector2& rhs) const
	{
		if ((x > rhs.x) || ((x == rhs.x) && (y > rhs.y)))
			return true;
		return false;
	}

	const bool operator < (const _TVector2& rhs) const
	{
		if ((x < rhs.x) || ((x == rhs.x) && (y < rhs.y)))
			return true;
		return false;
	}

    T Distance( const _TVector2& vec ) const
    {
        return T(::sqrt( 1.0 * ((x - vec.x) * (x - vec.x)) + ((y - vec.y) * (y - vec.y)) ));
    }
};
typedef  _TVector2<float> vec2;
typedef _TVector2<double> vec2d;
typedef _TVector2<int> vec2i;
typedef _TVector2<unsigned int> vec2u;