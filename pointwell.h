#include "types.h"

class PointWell {
    private:
        welltype current;
        welltype maximum;

    public:
        PointWell(welltype cur, welltype max);
        PointWell(welltype max);
        welltype getCurrent() const;
        welltype getMaximum() const;
        void setCurrent(welltype cur);
        void setMaximum(welltype max);
        void add(welltype amount);
        void sub(welltype amount);
};

// Constructor implementations
PointWell::PointWell(welltype cur, welltype max) : current(cur), maximum(max) {
    // Ensure current doesn't exceed maximum
    if (current > maximum) {
        current = maximum;
    }
}

PointWell::PointWell(welltype max) : current(max), maximum(max) {
    // Initialize with full health/mana/points
}

// Getter implementations
welltype PointWell::getCurrent() const {
    return current;
}

welltype PointWell::getMaximum() const {
    return maximum;
}

// Setter implementations
void PointWell::setCurrent(welltype cur) {
    if (cur >= 0 && cur <= maximum) {
        current = cur;
    }
}

void PointWell::setMaximum(welltype max) {
    if (max >= 0) {
        maximum = max;
        // Ensure current doesn't exceed new maximum
        if (current > maximum) {
            current = maximum;
        }
    }
}

// Add and subtract implementations
void PointWell::add(welltype amount) {
    if (amount > 0) {
        current += amount;
        // Ensure current doesn't exceed maximum
        if (current > maximum) {
            current = maximum;
        }
    }
}

void PointWell::sub(welltype amount) {
    if (amount > 0) {
        if (amount >= current) {
            current = 0;  // Can't go below 0
        } else {
            current -= amount;
        }
    }
}

