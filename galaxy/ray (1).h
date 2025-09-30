#pragma once
#include <glm/glm.hpp>
#include <vector>

// Represents a single ray's state
struct Ray {
    glm::vec3 position;  // current position of ray
    glm::vec3 direction; // normalized velocity/direction vector
};

// Compute Paczyński–Wiita acceleration at a given position
glm::vec3 accelerationPW(const glm::vec3& position, float M);

// Advance ray using 4th-order Runge-Kutta
void integrateRayRK4(Ray& ray, std::vector<glm::vec3>& path, float M, float stepSize, int steps);
