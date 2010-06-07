# Copyright (c) 2008 Peter Shinners

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


from operator import isNumberType
from math import hypot, sin, cos, atan2
getattr = getattr  # copy builtin to global for speed


__all__ = ["Vec2"]



class Vec2(object):
    """Immutable floating point 2D vector class.
        The vector acts as an indexable
        and iterable pair of floating point numbers. Operators are
        overridden to perform elemntwise operations. The length related
        methods are safe against zero length vectors, and will not
        raise division exceptions. All vector arguments accept any number
        or object with a pair of numbers."""
        
    __slots__ = ("__vec2__")

    def __init__(self, x, y=None):
        """Initialize a new vector from any number or pair of numbers."""
        _none = None
        if y is _none:
            if isNumberType(x):
                x += 0.0
                self.__vec2__ = (x, x)
            else:
                x, y = x
                self.__vec2__ = (x + 0.0, y + 0.0)
        elif x is _none:
            self.__vec2__ = y
        else:
            self.__vec2__ = (x + 0.0, y + 0.0)

    @property
    def x(self):
        return self.__vec2__[0]

    @property
    def y(self):
        return self.__vec2__[1]

    def __repr__(self):
        return "Vec2(%f, %f)" % self.__vec2__

    def __iter__(self):
        return self.__vec2__.__iter__()

    def __len__(self):
        """The number of dimensions for the vector, always two."""
        return 2

    def __getitem__(self, key):
        """Index the vector as a two item container"""
        return self.__vec2__[key]
    
    def length(self):
        """Length of the vector"""
        return hypot(*self.__vec2__)

    def length2(self):
        """Squared length of the vector"""
        vx, vy = self.__vec2__
        return vx * vx + vy * vy

    def setlength(self, value):
        """Assign the length of the vector. If the vector
            currently has a zero length vector, it is moved only
            in positive y."""
        vx, vy = self.__vec2__
        l = hypot(vx, vy)
        if l:
            l = value / l
            return Vec2((vx * l, vy * l))
        return Vec2(None, (0.0, l))

    def normalize(self):
        """Set the length of the vector to exactly one, in place,
            unless the vector has zero length."""
        vx, vy = self.__vec2__
        l = hypot(vx, vy)
        if l:
            return Vec2(None, (vx / l, vy / l))
        return self
        
    def distance(self, other):
        """The distance between two vectors."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return hypot(vx - ox, vy - oy)
    
    def distance2(self, other):
        """The squared distance between two vectors."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return (vx - ox) ** 2.0 +  (vy - oy) ** 2.0

    def setdistance(self, other, distance):
        """Set the distance to another vector, in place. If the
            two vectors are in the same position, the vector is moved
            in positive y."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        dx = vx - ox
        dy = vy - oy
        l = hypot(dx, dy)
        if l:
            l = distance / l
            return Vec2((ox + vx * l, oy + vy * l))
        return Vec2(None, (ox, oy + distance)) 
 
    def perp(self):
        """Make the vector a perpendicular, and keep the same length.
            The perpendicular will be rotated 90 degrees clockwise."""
        vx, vy = self.__vec2__
        return Vec2(None, (vy, -vx))

    def rotate(self, degrees, pivot=None):
        radians = degrees * -0.017453292519943295  # pi/180
        c = cos(radians)
        s = sin(radians)
        vx, vy = self.__vec2__
        if pivot is None:
            return Vec2(None, (vx*c - vy*s, vx*s + vy*c))
        if isNumberType(pivot):
            ox = oy = pivot
        else:
            ox, oy = getattr(pivot, "__vec2__", pivot)
        vx -= ox
        vy -= oy
        x = vx*c - vy*s
        y = vx*s + vy*c
        return Vec2(None, (x + ox, y + oy))

    def angle(self, other=None):
        vx, vy = self.__vec2__
        if not vx or not vy:
            return 0.0
        if other is None:
            return atan2(vx, vy) * 57.295779513082323  # 180/pi
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        cross = vx * oy - vy * ox
        dot = vx * ox + vy * oy
        return atan2(cross, dot) * 57.295779513082323  # 180/pi

    def setangle(self, degrees, other=None):
        radians = degrees * 0.017453292519943295  # pi/180
        c = cos(radians)
        s = sin(radians)
        vx, vy = self.__vec2__
        if other is None:
            l = hypot(vx, vy)
            return Vec2(None, (l*s, l*c))
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        l = hypot(vx - ox, vy - oy)
        return Vec2(None, (ox + l*s, oy + l*c))

    def cross(self, other):
        """Cross multiplication with another vector."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return vx * oy - vy * ox

    def dot(self, other):
        """Dot product with another vector."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return vx * ox + vy * oy

    def atan2(self):
        """Pass the coordinates to the atan2 function"""
        return atan2(*self.__vec2__)

    def project(self, other):
        """Project onto another vector. The other vector does not
            need to be normalized.""" 
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
            length2 = other ** 2
            length2 += length2
        else:
            ox, oy = getattr(other, "__vec2__", other)
            length2 = ox ** 2 + oy ** 2
        if not length2:
            return self
        dot = vx * ox + vy * oy
        length = dot / length2
        return Vec2(None, (ox * length, oy * length))
            
    def reflect(self, other):
        """Reflect the direction of the vector based on the direction
            of another. The other vector does not need to be normalized."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
            length2 = other ** 2
            length2 += length2
        else:
            ox, oy = getattr(other, "__vec2__", other)
            length2 = ox ** 2 + oy ** 2
        if not length2:
            return self
        dot = vx * ox + vy * oy
        length = dot / length2 * 2
        return Vec2(None, (ox * length - vx, oy * length - vy))

    def almost(self, other, epsilon=1e-7):
        """Compare if two vectors are almost equal."""
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        dx = vx - ox
        dy = vy - oy
        nepsilon = -epsilon
        return (dx < epsilon and dy > nepsilon 
                    and dy < epsilon and dy > nepsilon)
        
    def floor(self):
        """Create a pair of integers of the floor from each value."""
        vx, vy = self.__vec2__
        _i = int
        return (_i(vx // 1.0), _i(vy // 1.0))  # NOTE, not a vec

    def ceil(self):
        """Create a pair of integers of the ceil from each value."""
        vx, vy = self.__vec2__
        _i = int
        return (_i(vx // 1.0) + 1, _i(vy // 1.0) + 1)  # NOTE, not a vec
        
    def __complex__(self):
        return complex(*self.__vec2__)
       
    def __neg__(self):
        vx, vy = self.__vec2__
        return Vec2(None, (-vx, -vy))

    def __pos__(self):
        return self

    def __abs__(self):
        vx, vy = self.__vec2__
        _a = abs
        return Vec2(None, (_a(vx + 0.0), _a(vy + 0.0)))

    def __invert__(self):
        vx, vy = self.__vec2__
        return Vec2(None, (vy, -vx))

    def __cmp__(self, other):
        if isNumberType(other):
            ovec = other, other
        else:
            ovec = getattr(other, "__vec2__", None)
            if ovec is None:
                ovec = tuple(other)                
        return cmp(self.__vec2__, ovec)

    def __nonzero__(self):
        vx, vy = self.__vec2__
        return bool(vx or vy)

    def __add__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (vx + ox, vy + oy))

    __radd__ = __add__
        
    def __sub__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (vx - ox, vy - oy))

    def __rsub__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (ox - vx, oy - vy))
    
    def __mul__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (vx * ox, vy * oy))
    
    __rmul__ = __mul__
        
    def __div__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (vx / ox, vy / oy))

    def __rdiv__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (ox / vx, oy / vy))
    
    __truediv__ = __div__
    __rtruediv__ = __rdiv__

    def __floordiv__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (vx // ox, vy // oy))

    def __rfloordiv__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (ox // vx, oy // vy))
    
    __xor__ = dot
    __rxor__ = dot
    __mod__ = cross

    def __rmod__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return ox * vy - oy * vx

    def __pow__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            return Vec2(None, (vx * other, vy * other))
        else:
            ox, oy = getattr(other, "__vec2__", other)
            return Vec2(None, (vx ** ox, vy ** oy))

    def __rpow__(self, other):
        vx, vy = self.__vec2__
        if isNumberType(other):
            ox = oy = other
        else:
            ox, oy = getattr(other, "__vec2__", other)
        return Vec2(None, (ox ** vx, oy ** vy))
    
    def __getstate__(self):
        return self.__vec2__
    
    def __setstate__(self, value):
        self.__vec2__ = value


