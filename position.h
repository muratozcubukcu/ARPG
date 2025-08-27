#ifndef POSITION_H
#define POSITION_H

#include <cmath>
#include <iostream>
#include <string>

class Position {
private:
    double x, y, z;

public:
    // Constructors
    Position() : x(0.0), y(0.0), z(0.0) {}
    Position(double x, double y, double z) : x(x), y(y), z(z) {}
    
    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }
    
    // Setters
    void setX(double newX) { x = newX; }
    void setY(double newY) { y = newY; }
    void setZ(double newZ) { z = newZ; }
    void set(double newX, double newY, double newZ) { x = newX; y = newY; z = newZ; }
    
    // Movement
    void move(double deltaX, double deltaY, double deltaZ) {
        x += deltaX;
        y += deltaY;
        z += deltaZ;
    }
    
    // Distance calculations
    double distanceTo(const Position& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    double distanceTo(double otherX, double otherY, double otherZ) const {
        double dx = x - otherX;
        double dy = y - otherY;
        double dz = z - otherZ;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    // 2D distance (ignoring Z for top-down view)
    double distance2D(const Position& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx*dx + dy*dy);
    }
    
    // Operators
    Position operator+(const Position& other) const {
        return Position(x + other.x, y + other.y, z + other.z);
    }
    
    Position operator-(const Position& other) const {
        return Position(x - other.x, y - other.y, z - other.z);
    }
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    
    // Utility methods
    bool isWithinRange(const Position& other, double range) const {
        return distanceTo(other) <= range;
    }
    
    // String representation
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
    
    // Vector3 operations for projectile calculations
    Position operator*(double scalar) const {
        return Position(x * scalar, y * scalar, z * scalar);
    }
    
    double length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    Position normalize() const {
        double len = length();
        if (len == 0.0) return Position(0, 0, 0);
        return Position(x/len, y/len, z/len);
    }
    
    double dot(const Position& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};

// Stream operator for easy output
inline std::ostream& operator<<(std::ostream& os, const Position& pos) {
    os << pos.toString();
    return os;
}

#endif // POSITION_H
