#include "Libs/headers.h"
#include "Class/Gui.h"
#include "Class/Questoes.h"

int main()
{
    std::vector<std::string> arquivos = {
        "Imagens/paris_a.JPG", "Imagens/paris_b.JPG",
        "Imagens/Lena.jpg", "Imagens/video.mp4",
        "Imagens/corredor.bmp", "Imagens/pecas.jpg"};

    printf("--- CHECKLIST DE ARQUIVOS ---\n");
    for (const auto &caminho : arquivos)
    {
        cv::Mat test = cv::imread(caminho);
        if (test.empty() && caminho.find("mp4") == std::string::npos)
        {
            printf("[ERRO CRÍTICO] Arquivo nao encontrado ou corrompido: %s\n", caminho.c_str());
        }
        else
        {
            printf("[OK] %s detectado.\n", caminho.c_str());
        }
    }
    printf("-----------------------------\n");

    Gui gui;
    if (!gui.Init())
        return -1;

    cv::Mat imagemExibicao;
    std::string tituloStatus = "Selecione uma questão";
    int questaoAtual = 0;

    // Variáveis Q4
    int percentualQ4 = 100;

    // Variáveis Q5
    cv::Point p1(-1, -1), p2(-1, -1);
    int cliquesContagem = 0;
    cv::Mat baseLena;

    // Variáveis Q6
    cv::VideoCapture cap;
    cv::Mat fundoCorredor = cv::imread("Imagens/corredor.bmp", cv::IMREAD_GRAYSCALE);
    if (fundoCorredor.empty())
    {
        printf("[ERRO] Não foi possível carregar Imagens/corredor.bmp\n");
    }
    bool rodandoVideo = false;

    while (!gui.ShouldClose())
    {
        gui.NewFrame();

        // --- PAINEL LATERAL DE CONTROLE ---
        ImGui::Begin("Avaliação I - Sistemas de Visão");
        if (ImGui::Button("Q1: Degradê", ImVec2(-1, 30)))
        {
            questaoAtual = 1;
            imagemExibicao = Q1();
        }
        if (ImGui::Button("Q2: Rotação", ImVec2(-1, 30)))
        {
            questaoAtual = 2;
            imagemExibicao = Q2();
        }
        if (ImGui::Button("Q3: Transição Branco", ImVec2(-1, 30)))
        {
            questaoAtual = 3;
            imagemExibicao = Q3();
        }
        if (ImGui::Button("Q4: Controle de Brilho", ImVec2(-1, 30)))
        {
            questaoAtual = 4;
        }
        if (ImGui::Button("Q5: Desenhar Reta", ImVec2(-1, 30)))
        {
            questaoAtual = 5;
            cliquesContagem = 0;
            baseLena = cv::imread("Imagens/Lena.jpg", cv::IMREAD_COLOR);
            imagemExibicao = baseLena.clone();
        }
        if (ImGui::Button("Q6: Detecção Vídeo", ImVec2(-1, 30)))
        {
            questaoAtual = 6;

            // Tenta abrir o vídeo. O CAP_FFMPEG é essencial no Zorin.
            if (!cap.open("Imagens/video.mp4", cv::CAP_FFMPEG))
            {
                std::cout << "ERRO: Caminho ou Codec inválido!" << std::endl;
            }

            // Agora sim verificamos se deu certo SEM dar release
            rodandoVideo = cap.isOpened();

            if (rodandoVideo)
            {
                tituloStatus = "Vídeo carregado com sucesso.";
                std::cout << "Vídeo pronto para rodar!" << std::endl;
            }
            else
            {
                tituloStatus = "ERRO: OpenCV não conseguiu abrir o arquivo.";
            }
        }

        if (ImGui::Button("Q7: Contagem Porcas", ImVec2(-1, 30)))
        {
            questaoAtual = 7;
            // Chame a função passando a variável que já existe no main
            imagemExibicao = Q7(tituloStatus);
        }
        ImGui::End();

        // 2. Janela de Visualização
        ImGui::Begin("Visualização do Resultado");
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "%s", tituloStatus.c_str());
        ImGui::Separator();

        if (questaoAtual == 6 && rodandoVideo)
        {
            // Se Q6 retornar false, o vídeo chegou ao fim
            if (!Q6(cap, fundoCorredor, imagemExibicao))
            {
                cap.set(cv::CAP_PROP_POS_FRAMES, 0); // Loop automático
            }
        }

        ImGui::End();

        // --- JANELA PRINCIPAL DE EXIBIÇÃO ---
        ImGui::Begin("Visualização do Resultado");

        // Exibe o status (importante para a Q7 mostrar os dados)
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "%s", tituloStatus.c_str());
        ImGui::Separator();

        if (questaoAtual == 4)
        {
            ImGui::Text("Ajuste a intensidade (0%% a 100%%):");
            if (ImGui::SliderInt("##slider", &percentualQ4, 0, 100))
            {
                imagemExibicao = Q4((uint8_t)percentualQ4);
            }
        }

        if (questaoAtual == 5)
        {
            ImGui::Text("Clique 2 vezes na imagem para definir a reta (%d/2)", cliquesContagem);
            if (ImGui::Button("Resetar Reta"))
            {
                cliquesContagem = 0;
                imagemExibicao = baseLena.clone();
            }
        }

        if (questaoAtual == 6 && rodandoVideo)
        {
            // Aqui ele apenas lê o próximo frame e processa
            if (!Q6(cap, fundoCorredor, imagemExibicao))
            {
                ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
                gui.DrawImage(imagemExibicao, "Resultado");

                cap.set(cv::CAP_PROP_POS_FRAMES, 0); // Reinicia o vídeo ao chegar no fim
            }
        }

        // DESENHA A IMAGEM

        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        if (!imagemExibicao.empty()) 
{
    // Verifica se a imagem tem largura e altura válidas
    if (imagemExibicao.cols > 0 && imagemExibicao.rows > 0) 
    {
        gui.DrawImage(imagemExibicao, "Resultado");
    }
}
else 
{
    // Se por algum motivo a imagem sumir, desenha um fundo preto para não crashar
    cv::Mat dummy = cv::Mat::zeros(480, 640, CV_8UC3);
    gui.DrawImage(dummy, "Resultado");
}

        // Captura de Cliques para a Q5
        if (questaoAtual == 5 && ImGui::IsItemClicked())
        {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            float display_w = ImGui::GetItemRectSize().x;
            float display_h = ImGui::GetItemRectSize().y;

            if (imagemExibicao.cols > 0)
            {
                int img_x = (int)((mouse_pos.x - canvas_pos.x) * imagemExibicao.cols / display_w);
                int img_y = (int)((mouse_pos.y - canvas_pos.y) * imagemExibicao.rows / display_h);

                if (cliquesContagem == 0)
                {
                    p1 = cv::Point(img_x, img_y);
                    cliquesContagem = 1;
                }
                else if (cliquesContagem == 1)
                {
                    p2 = cv::Point(img_x, img_y);
                    cliquesContagem = 2;
                    imagemExibicao = Q5(p1, p2);
                }
            }
        }

        ImGui::End();
        gui.Render();
    }

    gui.Shutdown();
    return 0;
}