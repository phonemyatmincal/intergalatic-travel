#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(char direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    glm::vec3 Position;
    glm::vec3 Forward;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;

private:
    void updateCameraVectors();
};