#!/bin/bash

# Lista de objetos (Iniciando com Letra Maiúscula conforme sua estrutura Libs/Class)
TARGETS=(
    "Libs/glad/glad.o"
    "Libs/imgui/imgui.o"
    "Libs/imgui/imgui_draw.o"
    "Libs/imgui/imgui_tables.o"
    "Libs/imgui/imgui_widgets.o"
    "Libs/imgui/imgui_impl_glfw.o"
    "Libs/imgui/imgui_impl_opengl3.o"
    "Libs/imgui/imgui_stdlib.o"
    "Class/Gui.o"
    "Class/Questoes.o"
    "main.o"
)

EXECUTABLE="Av1_Thiago"

draw_progress_bar() {
    local current_step=$1
    local total_steps=$2
    local bar_width=40
    local percent=$(( (current_step * 100) / total_steps ))
    local progress=$(( (current_step * bar_width) / total_steps ))
    printf "\r["
    for ((i=0; i<bar_width; i++)); do
        if [ $i -lt $progress ]; then printf "="; else printf " "; fi
    done
    printf "] %d%% - %s" "$percent" "$3"
}

total_steps=$(( ${#TARGETS[@]} + 1 ))
current_step=0
has_error=0

echo "Iniciando build do AV1 (Sistemas de Visão)..."

for target in "${TARGETS[@]}"; do
    filename=$(basename "$target")
    draw_progress_bar $current_step $total_steps "Compilando: $filename..."
    
    # Chama o make especificando o alvo correto
    make "$target" > /dev/null 2>&1
    
    if [ $? -ne 0 ]; then
        has_error=1
        echo -e "\n\n\033[0;31mERRO ao construir: $target\033[0m"
        make "$target" # Mostra o erro detalhado se falhar
        break
    fi
    current_step=$((current_step + 1))
done

if [ $has_error -eq 0 ]; then
    draw_progress_bar $current_step $total_steps "Linkando..."
    make "$EXECUTABLE" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        has_error=1
        echo -e "\n\nERRO na linkagem final."
        make "$EXECUTABLE"
    fi
    current_step=$((current_step + 1))
fi

if [ $has_error -eq 0 ]; then
    echo -e "\n\033[0;32mBuild bem-sucedido!\033[0m"
    chmod +x $EXECUTABLE
    ./$EXECUTABLE
else
    echo -e "\n\033[0;31mBuild falhou.\033[0m"
fi