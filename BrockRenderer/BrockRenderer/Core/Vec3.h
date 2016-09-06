#pragma once

template < typename T >
class _TVector3
{
public:
	union{
		T val[3];
		struct{
			T x,y,z;
		};
	};

    _TVector3()
        : x(0), y(0), z(0)
    {
    }
	_TVector3(const T v)
		: x(v), y(v), z(v)
	{
	}
    template <class T2>
	_TVector3(const _TVector3<T2>& vecCopy)
	{
		*this = vecCopy;
	}

    _TVector3(const T *fpVec)
        : x(fpVec[0]), y(fpVec[1]), z(fpVec[2])
    {
    }

    _TVector3(const T fX, const T fY, const T fZ)
        : x(fX), y(fY), z(fZ)
    {
    }


    /**
    * @brief normalize the vector so it has a magnitude of 1
    * @note same with Normalize()
    * @return
    */
    _TVector3 Normalize()
    {
        // T fLength = sqrt( x * x + y * y + z * z );
		T fLength = GetLength();
        // Will also work for zero-sized vectors, but will change nothing
        if ( fLength > 1e-06 )
        {
            T fInvLength = T(1.0f / fLength);
			x *= fInvLength;
			y *= fInvLength;
			z *= fInvLength;
        }

        return *this;
    }

   
    inline T& operator[]( int iIdx)
    {
		if (iIdx < 0 || iIdx > 3) throw std::out_of_range("vector3 out of range");
        return val[iIdx];
    }


    _TVector3 operator+ (const _TVector3& vec) const
    {
        return( _TVector3(
                    ( vec.x + x ),
                    ( vec.y + y ),
                    ( vec.z + z )));
    }

    _TVector3 operator- (const _TVector3& vec) const
    {
        return( _TVector3(
                    ( x - vec.x ),
                    ( y - vec.y ),
                    ( z - vec.z )));
    }


    void operator += (const _TVector3& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
    }

    /**
    * @brief subtract vector from stored vector
    * @param vec
    */
    void operator -= (const _TVector3& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
    }

    /**
    * @brief negate the vector
    * @return
    */
    _TVector3 operator-() const
    {
        return( _TVector3(-x, -y, -z) );
    }

    /**
    * @brief Scalar multiplication
    * @param fScalar
    * @return
    */
    _TVector3 operator *  (const T fScalar) const
    {
        return( _TVector3(
                    ( x * fScalar ),
                    ( y * fScalar ),
                    ( z * fScalar )));
    }

    /**
    * @brief Scalar division
    * @param fScalar
    * @return
    */
    _TVector3 operator /  (const T fScalar) const
    {
        T fInvScale = T(1.0f / fScalar);

        return( _TVector3(
                    ( x * fInvScale ),
                    ( y * fInvScale ),
                    ( z * fInvScale )));
    }

    /**
    * @brief Multiply the stored vector by a scalar
    * @param fScalar
    */
    void operator *= (const T fScalar)
    {
        x *= fScalar;
        y *= fScalar;
        z *= fScalar;
    }

    void operator *= (const _TVector3& vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;
    }

    /**
    * @brief Divide the stored vector by a scalar
    * @param fScalar
    */
    void operator /= (const T fScalar)
    {
        T fInvScale = 1 / fScalar;

        x *= fInvScale;
        y *= fInvScale;
        z *= fInvScale;
    }

    /**
    * @brief Set the stored vector equal to another
    * @param vec
    * @return
    */
    template <class T2>
    _TVector3<T> operator =  (const _TVector3<T2>& vec)
    {
        x = static_cast<T>(vec.x);
        y = static_cast<T>(vec.y);
        z = static_cast<T>(vec.z);
        return *(this);
    }

    /**
    * @brief See if the stored vector and another are equal
    * @param vec
    * @return
    */
    const bool operator == (const _TVector3& vec)  const
    {
        return (
                    (x == vec.x) &&
                    (y == vec.y) &&
                    (z == vec.z));
    }

    /**
    * @brief See if the stored vector and another are not equal
    * @param vec
    * @return
    */
    const bool	operator != (const _TVector3& vec)  const
    {
        return (!(*this == vec));
    }

    /*************************ZONGNING ADD **********************************/

    _TVector3 operator *  (const _TVector3& rhs) const
    {
        return _TVector3( rhs.x * x, rhs.y * y, rhs.z * z );
    }

    const bool operator > (const _TVector3& rhs) const
    {
        if( (x > rhs.x) && (y > rhs.y) && (z > rhs.z) )
            return true;
        return false;
    }

    const bool operator < (const _TVector3& rhs) const
    {
        if( x < rhs.x && y < rhs.y && z < rhs.z )
            return true;
        return false;
    }


    _TVector3 Cross(const _TVector3& vec) const
    {
        return ( _TVector3(
			(y * vec.z - z * vec.y),
			(z * vec.x - x * vec.z),
			(x * vec.y - y * vec.x)) );
    }

    T GetLength() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    /**
    * @brief calculate the dot product
    * @param vec
    * @return
    */
    T Dot(const _TVector3& vec) const
	{
		return( (vec.x * x) + (vec.y * y) + (vec.z * z) );
    }


    T Distance( const _TVector3& vec ) const
    {
		return T(::sqrt( 1.0 * (((x - vec.x) * (x - vec.x))
			+ ((y - vec.y) * (y - vec.y))
			+ ((z - vec.z) * (z - vec.z)) )));
    }
};

typedef _TVector3<float> vec3;
typedef _TVector3<double> vec3d;
typedef _TVector3<int>    vec3i;
typedef _TVector3<unsigned int> vec3u;

template <typename T>
inline std::ifstream& operator << ( std::ifstream& inf, _TVector3<T> a)
{
	inf>>a.x>>a.y>>a.z; 
	return inf;
}