#pragma once

#include "HAL/PlatformMath.h"

#include "X0.h"

struct FVector;
struct  FVector4;
struct  FPlane;
struct  FBox;
struct  FRotator;
struct  FMatrix;
struct  FQuat;
struct  FTwoVectors;
struct  FTransform;
class  FSphere;
struct FVector2D;
struct FLinearColor;

#undef  PI
#define PI 					(3.1415926535897932f)
#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define BIG_NUMBER			(3.4e+38f)
#define EULERS_NUMBER       (2.71828182845904523536f)

// Copied from float.h
#define MAX_FLT 3.402823466e+38F

// Aux constants.
#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)

// Magic numbers for numerical precision.
#define DELTA			(0.00001f)

/**
 * Lengths of normalized vectors (These are half their maximum values
 * to assure that dot products with normalized vectors don't overflow).
 */
#define FLOAT_NORMAL_THRESH				(0.0001f)

 //
 // Magic numbers for numerical precision.
 //
#define THRESH_POINT_ON_PLANE			(0.10f)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20f)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.00002f)	/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015f)	/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002f)	/* Two normal points are same if within this distance */
#define THRESH_UVS_ARE_SAME			    (0.0009765625f)/* Two UV are same if within this threshold (1.0f/1024f) */
													/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
													/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25f)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01f)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.0001f)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_NORMALS_ARE_PARALLEL		(0.999845f)	/* Two unit vectors are parallel if abs(A dot B) is greater than or equal to this. This is roughly cosine(1.0 degrees). */
#define THRESH_NORMALS_ARE_ORTHOGONAL	(0.017455f)	/* Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less than or equal this. This is roughly cosine(89.0 degrees). */

#define THRESH_VECTOR_NORMALIZED		(0.01f)		/** Allowed error for a normalized vector (against squared magnitude) */
#define THRESH_QUAT_NORMALIZED			(0.01f)		/** Allowed error for a normalized quaternion (against squared magnitude) */

struct FMath : public FPlatformMath
{
	static inline int32 RandHelper(int32 A)
	{
		return A > 0 ? Min(TruncToInt(FRand() * A), A - 1) : 0;
	}

	static inline int32 RandRange(int32 Min, int32 Max)
	{
		const int32 Range = (Max - Min) + 1;
		return Min + RandHelper(Range);
	}

	static inline float RandRange(float InMin, float InMax)
	{
		return FRandRange(InMin, InMax);
	}

	static inline float FRandRange(float InMin, float InMax)
	{
		return InMin + (InMax - InMin) * FRand();
	}

	static inline bool RandBool()
	{
		return (RandRange(0, 1) == 1) ? true : false;
	}

	template <typename T>
	static inline bool IsPowerOfTwo(T Value)
	{
		return ((Value & (Value - 1)) == (T)0);
	}


	// Math Operations

	/** Returns highest of 3 values */
	template< class T >
	static inline T Max3(const T A, const T B, const T C)
	{
		return Max(Max(A, B), C);
	}

	/** Returns lowest of 3 values */
	template< class T >
	static inline T Min3(const T A, const T B, const T C)
	{
		return Min(Min(A, B), C);
	}

	/** Multiples value by itself */
	template< class T >
	static inline T Square(const T A)
	{
		return A * A;
	}

	/** Clamps X to be between Min and Max, inclusive */
	template< class T >
	static inline T Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	static inline float Log2(float Value)
	{
		// Cached value for fast conversions
		static const float LogToLog2 = 1.f / Loge(2.f);
		// Do the platform specific log and convert using the cached value
		return Loge(Value) * LogToLog2;
	}

	template<class T>
	static inline auto RadiansToDegrees(T const& RadVal) -> decltype(RadVal * (180.f / PI))
	{
		return RadVal * (180.f / PI);
	}

	/**
	 * Converts degrees to radians.
	 * @param	DegVal			Value in degrees.
	 * @return					Value in radians.
	 */
	template<class T>
	static inline auto DegreesToRadians(T const& DegVal) -> decltype(DegVal * (PI / 180.f))
	{
		return DegVal * (PI / 180.f);
	}

	template< class T, class U >
	static inline T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}

	static inline void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (INV_PI*0.5f)*Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = Value - (2.0f*PI)*quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign * p;
	}

	/** Divides two integers and rounds up */
	template <class T>
	static FORCEINLINE T DivideAndRoundUp(T Dividend, T Divisor)
	{
		return (Dividend + Divisor - 1) / Divisor;
	}

	/** Divides two integers and rounds down */
	template <class T>
	static FORCEINLINE T DivideAndRoundDown(T Dividend, T Divisor)
	{
		return Dividend / Divisor;
	}

	/** Divides two integers and rounds to nearest */
	template <class T>
	static FORCEINLINE T DivideAndRoundNearest(T Dividend, T Divisor)
	{
		return (Dividend >= 0)
			? (Dividend + Divisor / 2) / Divisor
			: (Dividend - Divisor / 2 + 1) / Divisor;
	}

	/**
	* Converts a floating point number to an integer which is closer to zero, "smaller" in absolute value: 0.1 becomes 0, -0.1 becomes 0
	* @param F		Floating point value to convert
	* @return		The rounded integer
	*/
	static FORCEINLINE float RoundToZero(float F)
	{
		return (F < 0.0f) ? CeilToFloat(F) : FloorToFloat(F);
	}

	static FORCEINLINE double RoundToZero(double F)
	{
		return (F < 0.0) ? CeilToDouble(F) : FloorToDouble(F);
	}

	/**
	 *	Checks if two floating point numbers are nearly equal.
	 *	@param A				First number to compare
	 *	@param B				Second number to compare
	 *	@param ErrorTolerance	Maximum allowed difference for considering them as 'nearly equal'
	 *	@return					true if A and B are nearly equal
	 */
	static FORCEINLINE bool IsNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<float>(A - B) <= ErrorTolerance;
	}

	/**
	 *	Checks if two floating point numbers are nearly equal.
	 *	@param A				First number to compare
	 *	@param B				Second number to compare
	 *	@param ErrorTolerance	Maximum allowed difference for considering them as 'nearly equal'
	 *	@return					true if A and B are nearly equal
	 */
	static FORCEINLINE bool IsNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<double>(A - B) <= ErrorTolerance;
	}

	/**
	 *	Checks if a floating point number is nearly zero.
	 *	@param Value			Number to compare
	 *	@param ErrorTolerance	Maximum allowed difference for considering Value as 'nearly zero'
	 *	@return					true if Value is nearly zero
	 */
	static FORCEINLINE bool IsNearlyZero(float Value, float ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<float>(Value) <= ErrorTolerance;
	}

	/**
	 *	Checks if a floating point number is nearly zero.
	 *	@param Value			Number to compare
	 *	@param ErrorTolerance	Maximum allowed difference for considering Value as 'nearly zero'
	 *	@return					true if Value is nearly zero
	 */
	static FORCEINLINE bool IsNearlyZero(double Value, double ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<double>(Value) <= ErrorTolerance;
	}

	// Geometry intersection 

	/**
	 * Find the intersection of a ray and a plane.  The ray has a start point with an infinite length.  Assumes that the
	 * line and plane do indeed intersect; you must make sure they're not parallel before calling.
	 *
	 * @param RayOrigin	The start point of the ray
	 * @param RayDirection	The direction the ray is pointing (normalized vector)
	 * @param Plane	The plane to intersect with
	 *
	 * @return The point of intersection between the ray and the plane.
	 */
	static FVector RayPlaneIntersection(const FVector& RayOrigin, const FVector& RayDirection, const FPlane& Plane);

	/**
	 * Find the intersection of a line and an offset plane. Assumes that the
	 * line and plane do indeed intersect; you must make sure they're not
	 * parallel before calling.
	 *
	 * @param Point1 the first point defining the line
	 * @param Point2 the second point defining the line
	 * @param PlaneOrigin the origin of the plane
	 * @param PlaneNormal the normal of the plane
	 *
	 * @return The point of intersection between the line and the plane.
	 */
	static FVector LinePlaneIntersection(const FVector &Point1, const FVector &Point2, const FVector &PlaneOrigin, const FVector &PlaneNormal);

	/**
	 * Find the intersection of a line and a plane. Assumes that the line and
	 * plane do indeed intersect; you must make sure they're not parallel before
	 * calling.
	 *
	 * @param Point1 the first point defining the line
	 * @param Point2 the second point defining the line
	 * @param Plane the plane
	 *
	 * @return The point of intersection between the line and the plane.
	 */
	static FVector LinePlaneIntersection(const FVector &Point1, const FVector &Point2, const FPlane  &Plane);


	// @parma InOutScissorRect should be set to View.ViewRect before the call
	// @return 0: light is not visible, 1:use scissor rect, 2: no scissor rect needed
	static X0_API uint32 ComputeProjectedSphereScissorRect(struct FIntRect& InOutScissorRect, FVector SphereOrigin, float Radius, FVector ViewOrigin, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix);

	// @param ConeOrigin Cone origin
	// @param ConeDirection Cone direction
	// @param ConeRadius Cone Radius
	// @param CosConeAngle Cos of the cone angle
	// @param SinConeAngle Sin of the cone angle
	// @return Minimal bounding sphere encompassing given cone
	static FSphere ComputeBoundingSphereForCone(FVector const& ConeOrigin, FVector const& ConeDirection, float ConeRadius, float CosConeAngle, float SinConeAngle);

	/**
	 * Determine if a plane and an AABB intersect
	 * @param P - the plane to test
	 * @param AABB - the axis aligned bounding box to test
	 * @return if collision occurs
	 */
	static X0_API bool PlaneAABBIntersection(const FPlane& P, const FBox& AABB);

	/**
	 * Performs a sphere vs box intersection test using Arvo's algorithm:
	 *
	 *	for each i in (x, y, z)
	 *		if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	 *		else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2
	 *
	 * @param Sphere the center of the sphere being tested against the AABB
	 * @param RadiusSquared the size of the sphere being tested
	 * @param AABB the box being tested against
	 *
	 * @return Whether the sphere/box intersect or not.
	 */
	static bool SphereAABBIntersection(const FVector& SphereCenter, const float RadiusSquared, const FBox& AABB);

	/**
	 * Converts a sphere into a point plus radius squared for the test above
	 */
	static bool SphereAABBIntersection(const FSphere& Sphere, const FBox& AABB);

	/** Determines whether a point is inside a box. */
	static bool PointBoxIntersection(const FVector& Point, const FBox& Box);

	/** Determines whether a line intersects a box. */
	static bool LineBoxIntersection(const FBox& Box, const FVector& Start, const FVector& End, const FVector& Direction);

	/** Determines whether a line intersects a box. This overload avoids the need to do the reciprocal every time. */
	static bool LineBoxIntersection(const FBox& Box, const FVector& Start, const FVector& End, const FVector& Direction, const FVector& OneOverDirection);

	/* Swept-Box vs Box test */
	static X0_API bool LineExtentBoxIntersection(const FBox& inBox, const FVector& Start, const FVector& End, const FVector& Extent, FVector& HitLocation, FVector& HitNormal, float& HitTime);

	/** Determines whether a line intersects a sphere. */
	static bool LineSphereIntersection(const FVector& Start, const FVector& Dir, float Length, const FVector& Origin, float Radius);

	/**
	 * Assumes the cone tip is at 0,0,0 (means the SphereCenter is relative to the cone tip)
	 * @return true: cone and sphere do intersect, false otherwise
	 */
	static X0_API bool SphereConeIntersection(const FVector& SphereCenter, float SphereRadius, const FVector& ConeAxis, float ConeAngleSin, float ConeAngleCos);

	/** Find the point on the line segment from LineStart to LineEnd which is closest to Point */
	static X0_API FVector ClosestPointOnLine(const FVector& LineStart, const FVector& LineEnd, const FVector& Point);

	/** Find the point on the infinite line between two points (LineStart, LineEnd) which is closest to Point */
	static X0_API FVector ClosestPointOnInfiniteLine(const FVector& LineStart, const FVector& LineEnd, const FVector& Point);

	/** Compute intersection point of three planes. Return 1 if valid, 0 if infinite. */
	static bool IntersectPlanes3(FVector& I, const FPlane& P1, const FPlane& P2, const FPlane& P3);

	/**
	 * Compute intersection point and direction of line joining two planes.
	 * Return 1 if valid, 0 if infinite.
	 */
	static bool IntersectPlanes2(FVector& I, FVector& D, const FPlane& P1, const FPlane& P2);

	/**
	 * Calculates the distance of a given Point in world space to a given line,
	 * defined by the vector couple (Origin, Direction).
	 *
	 * @param	Point				Point to check distance to line
	 * @param	Direction			Vector indicating the direction of the line. Not required to be normalized.
	 * @param	Origin				Point of reference used to calculate distance
	 * @param	OutClosestPoint	optional point that represents the closest point projected onto Axis
	 *
	 * @return	distance of Point from line defined by (Origin, Direction)
	 */
	static X0_API float PointDistToLine(const FVector &Point, const FVector &Direction, const FVector &Origin, FVector &OutClosestPoint);
	static X0_API float PointDistToLine(const FVector &Point, const FVector &Direction, const FVector &Origin);

	/**
	 * Returns closest point on a segment to a given point.
	 * The idea is to project point on line formed by segment.
	 * Then we see if the closest point on the line is outside of segment or inside.
	 *
	 * @param	Point			point for which we find the closest point on the segment
	 * @param	StartPoint		StartPoint of segment
	 * @param	EndPoint		EndPoint of segment
	 *
	 * @return	point on the segment defined by (StartPoint, EndPoint) that is closest to Point.
	 */
	static X0_API FVector ClosestPointOnSegment(const FVector &Point, const FVector &StartPoint, const FVector &EndPoint);

	/**
	* FVector2D version of ClosestPointOnSegment.
	* Returns closest point on a segment to a given 2D point.
	* The idea is to project point on line formed by segment.
	* Then we see if the closest point on the line is outside of segment or inside.
	*
	* @param	Point			point for which we find the closest point on the segment
	* @param	StartPoint		StartPoint of segment
	* @param	EndPoint		EndPoint of segment
	*
	* @return	point on the segment defined by (StartPoint, EndPoint) that is closest to Point.
	*/
	static X0_API FVector2D ClosestPointOnSegment2D(const FVector2D &Point, const FVector2D &StartPoint, const FVector2D &EndPoint);

	/**
	 * Returns distance from a point to the closest point on a segment.
	 *
	 * @param	Point			point to check distance for
	 * @param	StartPoint		StartPoint of segment
	 * @param	EndPoint		EndPoint of segment
	 *
	 * @return	closest distance from Point to segment defined by (StartPoint, EndPoint).
	 */
	static X0_API float PointDistToSegment(const FVector &Point, const FVector &StartPoint, const FVector &EndPoint);

	/**
	 * Returns square of the distance from a point to the closest point on a segment.
	 *
	 * @param	Point			point to check distance for
	 * @param	StartPoint		StartPoint of segment
	 * @param	EndPoint		EndPoint of segment
	 *
	 * @return	square of the closest distance from Point to segment defined by (StartPoint, EndPoint).
	 */
	static X0_API float PointDistToSegmentSquared(const FVector &Point, const FVector &StartPoint, const FVector &EndPoint);

	/**
	 * Find closest points between 2 segments.
	 *
	 * If either segment may have a length of 0, use SegmentDistToSegmentSafe instance.
	 *
	 * @param	(A1, B1)	defines the first segment.
	 * @param	(A2, B2)	defines the second segment.
	 * @param	OutP1		Closest point on segment 1 to segment 2.
	 * @param	OutP2		Closest point on segment 2 to segment 1.
	 */
	static X0_API void SegmentDistToSegment(FVector A1, FVector B1, FVector A2, FVector B2, FVector& OutP1, FVector& OutP2);

	/**
	 * Find closest points between 2 segments.
	 *
	 * This is the safe version, and will check both segments' lengths.
	 * Use this if either (or both) of the segments lengths may be 0.
	 *
	 * @param	(A1, B1)	defines the first segment.
	 * @param	(A2, B2)	defines the second segment.
	 * @param	OutP1		Closest point on segment 1 to segment 2.
	 * @param	OutP2		Closest point on segment 2 to segment 1.
	 */
	static X0_API void SegmentDistToSegmentSafe(FVector A1, FVector B1, FVector A2, FVector B2, FVector& OutP1, FVector& OutP2);

	/**
	 * returns the time (t) of the intersection of the passed segment and a plane (could be <0 or >1)
	 * @param StartPoint - start point of segment
	 * @param EndPoint   - end point of segment
	 * @param Plane		- plane to intersect with
	 * @return time(T) of intersection
	 */
	static X0_API float GetTForSegmentPlaneIntersect(const FVector& StartPoint, const FVector& EndPoint, const FPlane& Plane);

	/**
	 * Returns true if there is an intersection between the segment specified by StartPoint and Endpoint, and
	 * the plane on which polygon Plane lies. If there is an intersection, the point is placed in out_IntersectionPoint
	 * @param StartPoint - start point of segment
	 * @param EndPoint   - end point of segment
	 * @param Plane		- plane to intersect with
	 * @param out_IntersectionPoint - out var for the point on the segment that intersects the mesh (if any)
	 * @return true if intersection occurred
	 */
	static X0_API bool SegmentPlaneIntersection(const FVector& StartPoint, const FVector& EndPoint, const FPlane& Plane, FVector& out_IntersectionPoint);


	/**
	* Returns true if there is an intersection between the segment specified by StartPoint and Endpoint, and
	* the Triangle defined by A, B and C. If there is an intersection, the point is placed in out_IntersectionPoint
	* @param StartPoint - start point of segment
	* @param EndPoint   - end point of segment
	* @param A, B, C	- points defining the triangle
	* @param OutIntersectPoint - out var for the point on the segment that intersects the triangle (if any)
	* @param OutNormal - out var for the triangle normal
	* @return true if intersection occurred
	*/
	static X0_API bool SegmentTriangleIntersection(const FVector& StartPoint, const FVector& EndPoint, const FVector& A, const FVector& B, const FVector& C, FVector& OutIntersectPoint, FVector& OutTriangleNormal);

	/**
	 * Returns true if there is an intersection between the segment specified by SegmentStartA and SegmentEndA, and
	 * the segment specified by SegmentStartB and SegmentEndB, in 2D space. If there is an intersection, the point is placed in out_IntersectionPoint
	 * @param SegmentStartA - start point of first segment
	 * @param SegmentEndA   - end point of first segment
	 * @param SegmentStartB - start point of second segment
	 * @param SegmentEndB   - end point of second segment
	 * @param out_IntersectionPoint - out var for the intersection point (if any)
	 * @return true if intersection occurred
	 */
	static X0_API bool SegmentIntersection2D(const FVector& SegmentStartA, const FVector& SegmentEndA, const FVector& SegmentStartB, const FVector& SegmentEndB, FVector& out_IntersectionPoint);


	/**
	 * Returns closest point on a triangle to a point.
	 * The idea is to identify the halfplanes that the point is
	 * in relative to each triangle segment "plane"
	 *
	 * @param	Point			point to check distance for
	 * @param	A,B,C			counter clockwise ordering of points defining a triangle
	 *
	 * @return	Point on triangle ABC closest to given point
	 */
	static X0_API FVector ClosestPointOnTriangleToPoint(const FVector& Point, const FVector& A, const FVector& B, const FVector& C);

	/**
	 * Returns closest point on a tetrahedron to a point.
	 * The idea is to identify the halfplanes that the point is
	 * in relative to each face of the tetrahedron
	 *
	 * @param	Point			point to check distance for
	 * @param	A,B,C,D			four points defining a tetrahedron
	 *
	 * @return	Point on tetrahedron ABCD closest to given point
	 */
	static X0_API FVector ClosestPointOnTetrahedronToPoint(const FVector& Point, const FVector& A, const FVector& B, const FVector& C, const FVector& D);

	/**
	 * Find closest point on a Sphere to a Line.
	 * When line intersects		Sphere, then closest point to LineOrigin is returned.
	 * @param SphereOrigin		Origin of Sphere
	 * @param SphereRadius		Radius of Sphere
	 * @param LineOrigin		Origin of line
	 * @param LineDir			Direction of line. Needs to be normalized!!
	 * @param OutClosestPoint	Closest point on sphere to given line.
	 */
	static X0_API void SphereDistToLine(FVector SphereOrigin, float SphereRadius, FVector LineOrigin, FVector LineDir, FVector& OutClosestPoint);

	/**
	 * Calculates whether a Point is within a cone segment, and also what percentage within the cone (100% is along the center line, whereas 0% is along the edge)
	 *
	 * @param Point - The Point in question
	 * @param ConeStartPoint - the beginning of the cone (with the smallest radius)
	 * @param ConeLine - the line out from the start point that ends at the largest radius point of the cone
	 * @param radiusAtStart - the radius at the ConeStartPoint (0 for a 'proper' cone)
	 * @param radiusAtEnd - the largest radius of the cone
	 * @param percentageOut - output variable the holds how much within the cone the point is (1 = on center line, 0 = on exact edge or outside cone).
	 *
	 * @return true if the point is within the cone, false otherwise.
	 */
	static X0_API bool GetDistanceWithinConeSegment(FVector Point, FVector ConeStartPoint, FVector ConeLine, float RadiusAtStart, float RadiusAtEnd, float &PercentageOut);

	/**
	 * Determines whether a given set of points are coplanar, with a tolerance. Any three points or less are always coplanar.
	 *
	 * @param Points - The set of points to determine coplanarity for.
	 * @param Tolerance - Larger numbers means more variance is allowed.
	 *
	 * @return Whether the points are relatively coplanar, based on the tolerance
	 */
	static X0_API bool PointsAreCoplanar(const TArray<FVector>& Points, const float Tolerance = 0.1f);
};
