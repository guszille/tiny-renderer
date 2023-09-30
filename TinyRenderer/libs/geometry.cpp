#include "geometry.h"

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v)
{
	s << "(" << v.x << ", " << v.y << ")\n";

	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";

	return s;
}
