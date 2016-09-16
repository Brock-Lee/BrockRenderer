#pragma once
template < typename T >
class _TVector4
{
public:
	union {
		T val[4];
		struct {
			T x, y, z, w;
		};
		struct {
			T X, Y, Z, W;
		};
		struct {
			T r, g, b, a;
		};
		struct {
			T R, G, B, A;
		};
	};
    _TVector4()
        : x(0), y(0), z(0), w(0)
    {
    }
	_TVector4(const T v)
		: x(v), y(v), z(v),w(v)
	{
	}
	

    _TVector4(const _TVector4<T>& vecCopy)
    {
		*this = vecCopy;
    }
	
    _TVector4(const T *fpVec)
        : x(fpVec[0]), y(fpVec[1]), z(fpVec[2]), w(fpVec[3])
    {
    }

    _TVector4(const T fX, const T fY, const T fZ, const T fW)
        : x(fX), y(fY), z(fZ), w(fW)
    {
    }
	_TVector4(const _TVector3<T> &v, T ww)
		: x(v.x), y(v.y), z(v.z), w(ww)
	{
	}
    operator  T *()
    {
        return m_fVec;
    }
    operator const T *() const
    {
        return m_fVec;
    }

    _TVector4 Normalize()
    {
		T fLength = GetLength();
        // Will also work for zero-sized vectors, but will change nothing
        if ( fLength > 1e-06 )
        {
            T fInvLength = T(1.0f / fLength);
            x *= fInvLength;
            y *= fInvLength;
            z *= fInvLength;
			w *= fInvLength;
        }

        return *this;
    }

    inline T& operator[]( int iIdx)
    {
        return val[iIdx];
    }

    _TVector4 operator+ (const _TVector4& vec) const
    {
        return( _TVector4(
                    ( vec.x + x ),
                    ( vec.y + y ),
                    ( vec.z + z ),
					( vec.w + w)));
    }

    _TVector4 operator- (const _TVector4& vec) const
    {
        return( _TVector4(
                    ( x - vec.x ),
                    ( y - vec.y ),
                    ( z - vec.z ),
					( w - vec.w)));
    }

    void operator += (const _TVector4& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
		w += vec.w;
    }

    void operator -= (const _TVector4& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
		w -= vec.w;
    }

    _TVector4 operator-() const
    {
        return( _TVector4(-x, -y, -z, -w) );
    }

    _TVector4 operator *  (const T fScalar) const
    {
        return( _TVector4(
                    ( x * fScalar ),
                    ( y * fScalar ),
                    ( z * fScalar ),
					( w * fScalar )));
    }


    _TVector4 operator /  (const T fScalar) const
    {
        T fInvScale = T(1.0f / fScalar);

        return( _TVector4(
                    ( x * fInvScale ),
                    ( y * fInvScale ),
                    ( z * fInvScale ),
					( w * fInvScale )));
    }

    void operator *= (const T fScalar)
    {
        x *= fScalar;
        y *= fScalar;
        z *= fScalar;
		w *= fScalar;
    }

    void operator *= (const _TVector4& vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;
		w *= vec.w;
    }

    void operator /= (const T fScalar)
    {
        T fInvScale = 1 / fScalar;

        x *= fInvScale;
        y *= fInvScale;
        z *= fInvScale;
		w *= fInvScale;
    }

    template <class T2>
    _TVector4<T> operator =  (const _TVector4<T2>& vec)
    {
        x = static_cast<T>(vec.x);
        y = static_cast<T>(vec.y);
        z = static_cast<T>(vec.z);
		w = static_cast<T>(vec.w);
        return *(this);
    }

	template <class T2>
	_TVector4<T> operator =  (const _TVector3<T2>& vec)
	{
		x = static_cast<T>(vec.x);
		y = static_cast<T>(vec.y);
		z = static_cast<T>(vec.z);
		return *(this);
	}

    const bool operator == (const _TVector4& vec)  const
    {
        return (
                    (x == vec.x) &&
                    (y == vec.y) &&
                    (z == vec.z) && 
					(w == vec.w));
    }

    const bool	operator != (const _TVector4& vec)  const
    {
        return (!(*this == vec));
    }

    _TVector4 operator *  (const _TVector4& rhs) const
    {
        return _TVector4( rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w);
    }

    const bool operator > (const _TVector4& rhs) const
    {
        if( (x > rhs.x) && (y > rhs.y) && (z > rhs.z) && (w > rhs.w))
            return true;
        return false;
    }

    const bool operator < (const _TVector4& rhs) const
    {
        if( x < rhs.x && y < rhs.y && z < rhs.z && w < rhs.w)
            return true;
        return false;
    }


    T GetLength() const
    {
        return T(sqrt(1.0 * (x * x + y * y + z * z + w * w)));
    }

    T Dot(const _TVector4& vec) const
    {
        return( (vec.x * x) + (vec.y * y) + (vec.z * z) + vec.w * w);
    }
 
    T Distance( const _TVector4& vec ) const
    {
        return T(::sqrt( 1.0 * (((x - vec.x) * (x - vec.x))
                        + ((y - vec.y) * (y - vec.y))
                        + ((z - vec.z) * (z - vec.z)) 
						+ ((w - vec.w) * (w - vec.w)))));
    }


};


typedef _TVector4<float>  vec4;
typedef _TVector4<double> vec4d;
typedef _TVector4<int>    vec4i;
typedef _TVector4<unsigned int> vec4u;
typedef _TVector4<unsigned char> vec4uc;