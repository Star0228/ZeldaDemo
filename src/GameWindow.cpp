//
// Created by mf on 2024/12/5.
//

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameWindow.h"
#include "Camera.h"
#include "Light.h"
#include "Def.h"
#include "Player.h"

Camera camera;
Light sun;

// mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool imguiFocus = false;

// timing (per-frame time logic)
float currentFrame;
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// window
bool mainMenu = false;
bool ESC_pressed = false;
bool ALT_pressed = false;
bool Q_pressed = false;

GLFWwindow* Create_glfw_Window() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ZeldaDemo", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // tell GLFW to capture our mouse
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }
    glEnable(GL_DEPTH_TEST);  // configure global opengl state
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return window;
}

// 渲染前处理
void RenderLoopPreProcess(GLFWwindow* window, Player* player, Terrain* terrain,
                          Bomb* playerBomb, BroadLeaf* broadLeaf, WhiteBirch* whiteBirch, TreeApple* treeApple,
                          WoodBox* woodBoxs, int numWoodbox, MetalBox_breakable* metalBox_breakables, int numMetalBox,
                          MetalBox_B* metalBox_Bs, int numMetalBox_B, MetalBox_C* metalBox_Cs, int numMetalBox_C,
                          int numBroadLeaf,  int numWhiteBirch, int numTreeApple) {
    // per-frame time logic
    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 处理人物相关逻辑处理
    processInput(window, player, terrain, playerBomb, broadLeaf, whiteBirch, treeApple,
                 woodBoxs, numWoodbox, metalBox_breakables, numMetalBox,
                 metalBox_Bs, numMetalBox_B, metalBox_Cs, numMetalBox_C,
                 numBroadLeaf, numWhiteBirch, numTreeApple);

    // 处理部分Object的逻辑
    processBreak(window, player, playerBomb, terrain, woodBoxs, numWoodbox, metalBox_breakables, numMetalBox,
                 broadLeaf, numBroadLeaf, whiteBirch, numWhiteBirch, treeApple, numTreeApple);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return;
}

// 渲染后处理
void RenderLoopPostProcess(GLFWwindow* window) {
    // Rendering ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
    return;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window, Player* player, Terrain* terrain, Bomb* playerBomb,
                  BroadLeaf* broadLeaf, WhiteBirch* whiteBirch, TreeApple* treeApple,
                  WoodBox* woodBoxs, int numWoodbox, MetalBox_breakable* metalBox_breakables, int numMetalBox,
                  MetalBox_B* metalBox_Bs, int numMetalBox_B, MetalBox_C* metalBox_Cs, int numMetalBox_C,
                  int numBroadLeaf,  int numWhiteBirch, int numTreeApple) {
    moveDirection move_Direction = moveDirection::MOVE_STATIC;
    bool shift = false;
    bool jump = false;
    bool fly = false;
    bool bomb_state = false;
    bool reset = false;
    bool mouseLeft = false;
    bool mouseRight = false;
    // 移动：WASD + 上下
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        move_Direction = moveDirection::MOVE_FORWARD;
    // camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        move_Direction = moveDirection::MOVE_BACKWARD;
    // camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        move_Direction = moveDirection::MOVE_LEFT;
    // camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        move_Direction = moveDirection::MOVE_RIGHT;
    // camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)   // shift
        shift = true;
    // camera.ProcessKeyboard(DOWN, deltaTime / 2);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // only space
        jump = true;
    // camera.ProcessKeyboard(UP, deltaTime / 2);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) // fly
        fly = true;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !Q_pressed) { // throw bomb
        bomb_state = true;
        Q_pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
        Q_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // reset
        reset = true;


    // 检测左键
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        mouseLeft = true;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        mouseRight = true;
    // 检测 ESC 键的按下事件，用于打开/关闭主菜单
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !ESC_pressed) {
        mainMenu = !mainMenu;
        imguiFocus = mainMenu; // 默认开关菜单时也切换焦点
        ChangeFocus(window, imguiFocus);
        ESC_pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        ESC_pressed = false;
    // 检测 Left Alt 键的按下事件，用于切换鼠标焦点
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && !ALT_pressed) {
        imguiFocus = !imguiFocus;
        ChangeFocus(window, imguiFocus);
        ALT_pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
        ALT_pressed = false;

    player->ProcessMoveInput(move_Direction, shift, jump, fly, bomb_state, reset,
    mouseLeft, mouseRight, terrain, playerBomb, deltaTime, broadLeaf, whiteBirch, treeApple,
    woodBoxs, numWoodbox, metalBox_breakables, numMetalBox, metalBox_Bs, numMetalBox_B, metalBox_Cs, numMetalBox_C);
                         }

void processBreak(GLFWwindow* window, Player* player, Bomb* bomb, Terrain* terrain,  WoodBox* woodBoxs,
                  int numWoodbox, MetalBox_breakable* metalBox_breakables , int numMetalBox,
                  BroadLeaf* boradLeaf, int numBroadLeaf, WhiteBirch* whiteBirch, int numWhiteBirch,
                  TreeApple* treeApple,int numTreeApple) {
    for (int i = 0; i < numWoodbox; i++) {
        if (woodBoxs[i].breakable && !woodBoxs[i].breaked && bomb->explode && glm::distance(bomb->position, woodBoxs[i].position) < 5.0f)
            woodBoxs[i].breakBox();
        if (player->isAttack() && glm::distance(player->getPosition()+player->getDirection() * glm::vec3(4.0f), woodBoxs[i].position) < 1.0f)
            woodBoxs[i].breakBox();
    }
    for (int i = 0; i < numMetalBox; i++) {
        if (metalBox_breakables[i].breakable && !metalBox_breakables[i].breaked && bomb->explode
            && glm::distance(bomb->position, metalBox_breakables[i].position) < 5.0f)
            metalBox_breakables[i].breakBox();
        if (player->isAttack() && glm::distance(player->getPosition()+player->getDirection()* glm::vec3(4.0f), metalBox_breakables[i].position) < 1.0f)
            metalBox_breakables[i].breakBox();
    }

    for (int i = 0; i < numWhiteBirch; i++) {
        // if (whiteBirch[i].breakable && !whiteBirch[i].breaked && bomb->explode
        //     && glm::distance(bomb->position, whiteBirch[i].position) < 5.0f) {
        //     whiteBirch[i].breakTree();
        // }
        if (player->isAttack() && glm::distance(player->getPosition()+player->getDirection()* glm::vec3(4.0f),whiteBirch[i].position) < 1.0f)
            whiteBirch[i].breakTree();
    }
    for (int i = 0; i < numTreeApple; i++) {
        // if (treeApple[i].breakable && !treeApple[i].breaked && bomb->explode
        //     && glm::distance(bomb->position, treeApple[i].position) < 5.0f) {
        //     treeApple[i].breakTree();
        // }
        if (player->isAttack() && glm::distance(player->getPosition()+player->getDirection()* glm::vec3(4.0f), treeApple[i].position) < 1.0f)
             treeApple[i].breakTree();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
void mouse_callback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn) {
    if (imguiFocus)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // camera.ProcessMouseMovement(xoffset, yoffset);
    camera.ProcessMouseOrbit(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
    if (imguiFocus)
        return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


// util functions
void ChangeFocus(GLFWwindow* window, bool flag) {
    if (flag) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // glfw 释放鼠标
        ImGui::GetIO().WantCaptureMouse = true; // 告诉 ImGui 捕获鼠标
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // glfw 捕获鼠标
        ImGui::GetIO().WantCaptureMouse = false; // 告诉 ImGui 释放鼠标
    }
    // 将鼠标位置设置到窗口中心（避免偏转）
    glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
    lastX = SCR_WIDTH / 2.0f;
    lastY = SCR_HEIGHT / 2.0f;
}

namespace ImGui { // add to ImGui namespace
// 初始化 ImGui
void InitImGui(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 允许键盘控制
    // Setup Dear ImGui style

    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends 设置渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}
void EndImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void SliderFloatWithDefault(const char* label, float* v, float v_min, float v_max, float v_default) {
    ImGui::SliderFloat(("##" + std::string(label)).c_str(), v, v_min, v_max); // 注意避免重名
    ImGui::SameLine(); // 在同一行显示
    if (ImGui::Button(label)) // 将 label 作为按钮显示
        *v = v_default;
}
void SliderFloat3WithDefault(const char* label, float* v, float v_min, float v_max, float v_default) {
    ImGui::SliderFloat3(("##" + std::string(label)).c_str(), v, v_min, v_max);
    ImGui::SameLine();
    if (ImGui::Button(label)) {
        v[0] = v_default;
        v[1] = v_default;
        v[2] = v_default;
    }
}
void ColorEdit3WithDefault(const char* label, float* v, glm::vec3 v_default) {
    ImGui::ColorEdit3(("##" + std::string(label)).c_str(), v);
    ImGui::SameLine();
    if (ImGui::Button(label)) {
        v[0] = v_default.r;
        v[1] = v_default.g;
        v[2] = v_default.b;
    }
}
} // namespace ImGui
