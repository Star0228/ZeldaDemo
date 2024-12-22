#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Terrain.h"
#include "geometry/BoxGeometry.h"
#include "GameWindow.h"
#include "Shader.h"
#include "Camera.h"
#include "Def.h"
#include "Water.h"

enum PlayerState {
    IDLE_LAND = 0,
    WALKING_LAND,
    RUNNING_LAND,
    IDLE_WATER,
    SWIMMING_WATER,
    FAST_SWIMMING_WATER,
    IDLE_CLIMB,
    CLIMBING,
    JUMPING,
    LAND_TO_CLIMB,
    CLIMB_TO_LAND,
    CLIMB_TO_WATER,
    WATER_TO_CLIMB
};

enum moveDirection {
    MOVE_FORWARD = 0,
    MOVE_BACKWARD = 1,
    MOVE_LEFT = 2,
    MOVE_RIGHT = 3,
    MOVE_STATIC = -1
};

class Player {
private:
    int state;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 upVector;
    glm::vec3 length;
    glm::vec3 swimLength;
    glm::vec3 color;
    glm::vec3 landColor;
    glm::vec3 swimColor;
    float alpha=1.0f;
    float speed;

    // 速度
    float walkSpeed;
    float runSpeed;
    float swimSpeed;
    float fastSwimSpeed;
    float climbSpeed;
    float jumpHorizenSpeed;
    float jumpUpSpeed;
    // 跳跃相关参数
    float jumpHeight;
    glm::vec3 jumpDirection;
    float targetJumpHeight;
    bool jumpUp;
    // 游泳相关参数
    bool swimFlag; // 初次进水由0变1，初次出水由1变0
    // 攀爬相关参数
    glm::vec3 climbcolor;
    float climbtheta;
    float climbtheta_delta;
    glm::vec3 climbUpvector;
    glm::vec3 climbRotateAxis;
    int climbCount = 0;
    float climbCount_sum = 0;


public:
    Player(glm::vec3 initialPosition, glm::vec3 fixedLength, Terrain* terrain);
    ~Player();
    void draw(Shader& player_shader);
    void ProcessMoveInput(moveDirection move_Direction, bool shift, bool jump, Terrain* terrain, float deltaTime);
    // void Transfer();
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDirection() const { return direction; }
    void setDirection(const glm::vec3& newDirection) { direction = newDirection; }
    glm::vec3 getUpVector() const { return upVector; }
    void setUpVector(const glm::vec3& newUpVector) { upVector = newUpVector; }
    glm::vec3 getLength() const { return length; }
    void setLength(const glm::vec3& newLength) { length = newLength; }
    glm::vec3 getColor() const { return color; }
    void setColor(const glm::vec3& newColor) { color = newColor; }
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getWalkSpeed() const { return walkSpeed; }
    void setWalkSpeed(float newWalkSpeed) { walkSpeed = newWalkSpeed; }
    float getRunSpeed() const { return runSpeed; }
    void setRunSpeed(float newRunSpeed) { runSpeed = newRunSpeed; }
    float getSwimSpeed() const { return swimSpeed; }
    void setSwimSpeed(float newSwimSpeed) { swimSpeed = newSwimSpeed; }
    float getFastSwimSpeed() const { return fastSwimSpeed; }
    void setFastSwimSpeed(float newFastSwimSpeed) { fastSwimSpeed = newFastSwimSpeed; }
    float getClimbSpeed() const { return climbSpeed; }
    void setClimbSpeed(float newClimbSpeed) { climbSpeed = newClimbSpeed; }
    float getJumpHorizenSpeed() const { return jumpHorizenSpeed; }
    void setJumpHorizenSpeed(float newJumpHorizenSpeed) { jumpHorizenSpeed = newJumpHorizenSpeed; }
    float getJumpUpSpeed() const { return jumpUpSpeed; }
    void setJumpUpSpeed(float newJumpUpSpeed) { jumpUpSpeed = newJumpUpSpeed; }
    float getJumpHeight() const { return jumpHeight; }
    void setJumpHeight(float newJumpHeight) { jumpHeight = newJumpHeight; }
    void setAlpha(float newAlpha) { alpha = newAlpha; }

private:
    void DoJump(Terrain* terrain, float deltaTime);
    void Update(Terrain* terrain);
    void Rebind();
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    BoxGeometry boxGeometry;
    void InitializeBuffers();
};
