#include "Gui.h"

Gui::Gui() : window(nullptr), glsl_version("#version 460 core"), currentTextureID(0) {}
Gui::~Gui() {
    Shutdown();
}

bool Gui::Init() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 800, "AV1 - Sistemas de Visão", NULL, NULL);
    if (!window) return false;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return true;
}
void Gui::DrawImage(const cv::Mat& image, const std::string& label) {
    if (image.empty()) return;

    // Garante que a matriz seja perfeitamente contínua na memória
    cv::Mat drawMat = image;
    if (!drawMat.isContinuous()) {
        drawMat = drawMat.clone();
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    if (currentTextureID == 0) {
        glGenTextures(1, &currentTextureID);
    }
    
    glBindTexture(GL_TEXTURE_2D, currentTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (drawMat.channels() == 3) {
        // A MÁGICA: OpenGL nativamente aceita BGR! 
        // Removemos o cv::cvtColor que estava estourando a memória RAM a 60 FPS.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drawMat.cols, drawMat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, drawMat.data);
        
        // Reseta os canais de cor (Evita que a imagem fique vermelha após abrir uma P&B)
        GLint sw[] = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, sw);
        
    } else if (drawMat.channels() == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, drawMat.cols, drawMat.rows, 0, GL_RED, GL_UNSIGNED_BYTE, drawMat.data);
        
        // Aplica filtro para P&B
        GLint sw[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, sw);
    }

    float width = ImGui::GetContentRegionAvail().x;
    float height = width * ((float)drawMat.rows / drawMat.cols);
    
    ImGui::Image((void*)(intptr_t)currentTextureID, ImVec2(width, height));
}

void Gui::NewFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::Render() {
    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

bool Gui::ShouldClose() { return glfwWindowShouldClose(window); }
void Gui::Shutdown() {
    // BD é o Backend Data do ImGui. Esse check evita o crash de Assertion.
    if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().BackendRendererUserData != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}