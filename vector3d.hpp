#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

// Class to manage 3D vectors with methods for various common vector operations 
class Vector3D {
public:
  // x, y, z values of 3D vector
  float x, y, z;
  // this is to easily print a given vector to std for debugging
  friend std::ostream& operator<<(std::ostream&, const Vector3D&);

  Vector3D(): x(0), y(0), z(0) {}
  
  Vector3D(float x, float y, float z): x(x), y(y), z(z) {}

  Vector3D operator+(Vector3D b) {
    return Vector3D(this->x + b.x, this->y + b.y, this->z + b.z);
  }

  Vector3D operator-(Vector3D b) {
    return Vector3D(this->x - b.x, this->y - b.y, this->z - b.z);
  }

  Vector3D operator*(float t) const {
    return Vector3D(this->x * t, this->y * t, this->z * t);
  }

  float dot(Vector3D b) const {
    return this->x * b.x + this->y * b.y + this->z * b.z;
  }

  Vector3D cross(Vector3D b) const {
    return Vector3D(this->y * b.z - this->z * b.y, this->z * b.x - this->x * b.z, this->x * b.y - this->y * b.x);
  }

  float abs() const {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
  }

  float absSquare() const {
    return this->x * this->x + this->y * this->y + this->z * this->z;
  }

  Vector3D unit() const {
    return *this * (1 / this->abs());
  }

};

// this is to easily print a given vector in a well-formatted manner to std for debugging
std::ostream& operator<<(std::ostream &out, const Vector3D &v) {
  out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return out;
}

#endif