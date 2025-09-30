#include "ray.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp> // for length2 (faster than length)
#include <algorithm>

// Paczynski–Wiita acceleration (geometric units: G=1, c=1)
glm::vec3 accelerationPW(const glm::vec3& position, float M) {
    float r2 = glm::length2(position);
    float r = glm::sqrt(r2);
    float rs = 2.0f * M;  // Schwarzschild radius
    float EPS = 1e-4f;

    // Avoid division by zero near/inside horizon
    float denom = std::max(r - rs, EPS);
    float aMag = M / (denom * denom);

    return -aMag * (position / r); // direction toward center
}

// RK4 integrator for ray trajectory
void integrateRayRK4(Ray& ray, std::vector<glm::vec3>& path, float M, float stepSize, int steps) {
    path.clear();
    path.reserve(steps);

    for (int i = 0; i < steps; i++) {
        path.push_back(ray.position);
        // If inside event horizon — stop
        if (glm::length(ray.position) <= (2.0f * M + 1e-4f)) {
            break;
        }
        // --- k1 ---
        glm::vec3 k1_pos = ray.direction;
        glm::vec3 k1_dir = accelerationPW(ray.position, M);
        // --- k2 ---
        glm::vec3 pos_k2 = ray.position + 0.5f * stepSize * k1_pos;
        glm::vec3 dir_k2 = ray.direction + 0.5f * stepSize * k1_dir;
        glm::vec3 k2_pos = dir_k2;
        glm::vec3 k2_dir = accelerationPW(pos_k2, M);
        // --- k3 ---
        glm::vec3 pos_k3 = ray.position + 0.5f * stepSize * k2_pos;
        glm::vec3 dir_k3 = ray.direction + 0.5f * stepSize * k2_dir;
        glm::vec3 k3_pos = dir_k3;
        glm::vec3 k3_dir = accelerationPW(pos_k3, M);
        // --- k4 ---
        glm::vec3 pos_k4 = ray.position + stepSize * k3_pos;
        glm::vec3 dir_k4 = ray.direction + stepSize * k3_dir;
        glm::vec3 k4_pos = dir_k4;
        glm::vec3 k4_dir = accelerationPW(pos_k4, M);
        // Combine
        ray.position += (stepSize / 6.0f) * (k1_pos + 2.0f*k2_pos + 2.0f*k3_pos + k4_pos);
        ray.direction += (stepSize / 6.0f) * (k1_dir + 2.0f*k2_dir + 2.0f*k3_dir + k4_dir);
        // Normalize for photon-like motion
        ray.direction = glm::normalize(ray.direction);
    }
}


/*
Example Usage
Ray ray;
ray.position = glm::vec3(0.0f, 5.0f, 0.0f);
ray.direction = glm::normalize(glm::vec3(0.2f, -1.0f, 0.0f));

std::vector<glm::vec3> path;
float M = 1.0f;          // mass in geometric units
float stepSize = 0.05f;  // affine parameter step
int steps = 1000;

integrateRayRK4(ray, path, M, stepSize, steps);
*/
