#ifndef GUI_H
#define GUI_H

#include "../Libs/headers.h"
#include <string>
#include <vector>

class Gui {
public:
    Gui();
    ~Gui();

    // Inicialização e Ciclo de Vida
    bool Init();
    void NewFrame();
    void Render();
    void Shutdown();
    bool ShouldClose();

    // Ferramentas de Interface
    void BeginWindow(const std::string& title);
    void EndWindow();
    
    // Função principal para exibir cv::Mat na tela
    void DrawImage(const cv::Mat& image, const std::string& label);

    // Converte cv::Mat para Textura OpenGL (ID único)
    unsigned int MatToTexture(const cv::Mat& mat);

private:
    GLFWwindow* window;
    const char* glsl_version;
    unsigned int currentTextureID; // Guarda a textura sendo exibida no momento
};

#endif