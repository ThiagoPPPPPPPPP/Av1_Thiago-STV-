cmake_minimum_required(VERSION 3.15)
project(SistemasVisaoAV1)

# Padrão C++ 17
set(CMAKE_CXX_STANDARD 17)

# Encontra as bibliotecas (O GitHub vai baixar elas pra gente)
find_package(OpenCV REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Lista de todos os seus arquivos de código
set(SOURCES
    main.cpp
    Class/Gui.cpp
    Class/Questoes.cpp
    Libs/glad/glad.c
    Libs/imgui/imgui.cpp
    Libs/imgui/imgui_draw.cpp
    Libs/imgui/imgui_tables.cpp
    Libs/imgui/imgui_widgets.cpp
    Libs/imgui/imgui_impl_glfw.cpp
    Libs/imgui/imgui_impl_opengl3.cpp
    Libs/imgui/imgui_stdlib.cpp
)

# Cria o executável
add_executable(${PROJECT_NAME} ${SOURCES})

# Informa ao compilador onde achar os headers (.h)
# Coloquei todas as variações de pastas para ele não se perder!
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Libs
    ${CMAKE_SOURCE_DIR}/Libs/glad
    ${CMAKE_SOURCE_DIR}/Libs/imgui
)

# Linka o OpenCV e o GLFW no seu executável
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
    glfw
    opengl32
)
