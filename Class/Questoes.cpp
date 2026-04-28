#include "Libs/headers.h"



cv::Mat Q1() {
    cv::Mat imag(512, 512, CV_8UC1);
    for (int y = 0; y < imag.rows; y++) {
        uint8_t tom;
        if (y < 256) {
            tom = (uint8_t)y; // Escurece para o topo
        } else {
            tom = (uint8_t)(511 - y); // Escurece para o fundo
        }
        for (int x = 0; x < imag.cols; x++) {
            imag.at<uchar>(y, x) = tom;
        }
    }
    return imag;
}

cv::Mat Q2() {
    cv::Mat imag = cv::imread("Imagens/paris_a.JPG", cv::IMREAD_GRAYSCALE);
    if (imag.empty()) return cv::Mat::zeros(100, 100, CV_8UC1);
    
    cv::Mat saida(imag.cols, imag.rows, CV_8UC1);
    
    for (int y = 0; y < imag.rows; y++) {
        for (int x = 0; x < imag.cols; x++) {
            
            int novo_x = imag.rows - 1 - y;
            int novo_y = x;
            
            saida.at<uchar>(novo_y, novo_x) = imag.at<uchar>(y, x);
        }
    }
    
    return saida;
}


cv::Mat Q3() {
    
    cv::Mat imag = cv::imread("Imagens/paris_a.JPG", cv::IMREAD_GRAYSCALE);
    
    if (imag.empty()) {
        std::cerr << "Erro: Imagens/paris_a.JPG nao encontrada!" << std::endl;
        return cv::Mat::zeros(512, 512, CV_8UC1); 
    }
    
    cv::Mat saida(imag.rows, imag.cols, CV_8UC1);
    for (int y = 0; y < imag.rows; y++) {
        for (int x = 0; x < imag.cols; x++) {
            float alpha = (float)x / (float)imag.cols;
            saida.at<uchar>(y, x) = (uchar)(imag.at<uchar>(y, x) * alpha + 255 * (1.0f - alpha));
        }
    }
    return saida;
}

cv::Mat Q4(uint8_t percentual) {
    // 1. Verifique se o nome é .JPG (maiúsculo) conforme sua pasta
    cv::Mat imag = cv::imread("Imagens/paris_b.JPG", cv::IMREAD_GRAYSCALE);
    
    // 2. Proteção: Se a imagem não carregar, não tente processar
    if (imag.empty()) {
        return cv::Mat::zeros(512, 512, CV_8UC1); 
    }
    
    // 3. Criar a imagem de saída
    cv::Mat saida = imag.clone();
    float fator = percentual / 100.0f; // Converte 0-100 para 0.0-1.0

    for (int y = 0; y < saida.rows; y++) {
        for (int x = 0; x < saida.cols; x++) {
            // Ajuste de intensidade proporcional
            int novoValor = (int)(saida.at<uchar>(y, x) * fator);
            
            // Garante que o valor fique entre 0 e 255
            if (novoValor > 255) novoValor = 255;
            saida.at<uchar>(y, x) = (uchar)novoValor;
        }
    }
    
    return saida;
}
cv::Mat Q5(cv::Point p1, cv::Point p2) {

    cv::Mat imag = cv::imread("Imagens/Lena.jpg", cv::IMREAD_COLOR);
    if (imag.empty()) return cv::Mat();
    cv::line(imag, p1, p2, cv::Scalar(255, 0, 0), 2); 
    
    return imag;
}

bool Q6(cv::VideoCapture& vid, const cv::Mat& bg_gray, cv::Mat& frame_saida) {
    cv::Mat frame;
    if (!vid.read(frame)) {
        printf("[DEBUG] Falha ao ler frame do vídeo (ou fim do vídeo).\n");
        return false; 
    }
    // Tenta ler o próximo quadro
    if (!vid.read(frame)) {
        return false; // Fim do vídeo ou erro de leitura
    }

    if (frame.empty()) {
        printf("[DEBUG] Frame vazio recebido!\n");
        return false;
    }
    cv::Mat frame_gray, diff, thresh;
    // 1. O vídeo é colorido (BGR), precisamos de cinza para comparar com corredor.bmp
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

    // 2. Subtração de fundo: onde houver diferença, há movimento 
    cv::absdiff(bg_gray, frame_gray, diff);

    // 3. Limiar para remover ruídos pequenos
    cv::threshold(diff, thresh, 50, 255, cv::THRESH_BINARY);

    // 4. Detecção de contornos da pessoa 
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& c : contours) {
        if (cv::contourArea(c) > 500) { // Só desenha se for um objeto grande (a pessoa)
            cv::Rect boundingBox = cv::boundingRect(c);
            cv::rectangle(frame, boundingBox, cv::Scalar(0, 255, 0), 2); // Retângulo verde
        }
    }

    frame_saida = frame;
    return true;
}
cv::Mat Q7(std::string& stats_out) {
    printf("[Q7] Passo 1: Carregando pecas.jpg...\n");
    // Carrega a imagem
    cv::Mat imag = cv::imread("Imagens/pecas.jpg", cv::IMREAD_COLOR);
    
    if (imag.empty()) {
        printf("[Q7] ERRO: Nao achou pecas.jpg\n");
        stats_out = "Erro: pecas.jpg nao encontrada";
        return cv::Mat::zeros(100, 100, CV_8UC3);
    }

    printf("[Q7] Passo 2: Processando imagem...\n");
    cv::Mat gray, bin;
    cv::cvtColor(imag, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(7, 7), 0);
    cv::threshold(gray, bin, 210, 255, cv::THRESH_BINARY_INV);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9));
    cv::morphologyEx(bin, bin, cv::MORPH_CLOSE, kernel);
    printf("[Q7] Passo 3: Encontrando contornos...\n");
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::findContours(bin, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat saida = imag.clone();

    
    if (hierarchy.empty() || contours.empty()) {
        printf("[Q7] Aviso: Nenhum contorno encontrado.\n");
        stats_out = "Nenhuma peca detectada.";
        return saida;
    }

printf("[Q7] Passo 4: Iniciando loop de contagem (Seguro)...\n");
    
    int porcas = 0;
    double areaMin = 9999999.0, areaMax = 0.0, somaAreas = 0.0;
    
    
    for (size_t i = 0; i < contours.size(); i++) {
        
        if (hierarchy[i][3] != -1) continue;

        double area = cv::contourArea(contours[i]);

        // Ignora ruídos e sujeiras da imagem
        if (area < 100) continue;

        int filhoIdx = hierarchy[i][2];
        
        if (filhoIdx != -1 && filhoIdx < (int)contours.size()) {
            
            double areaBuraco = cv::contourArea(contours[filhoIdx]);
            
            if (areaBuraco > 150) { 
                porcas++;
                somaAreas += area;

                if (area > areaMax) areaMax = area;
                if (area < areaMin) areaMin = area;

                // Desenha o contorno verde usando cast (int) para evitar warnings
                cv::drawContours(saida, contours, (int)i, cv::Scalar(0, 255, 0), 2);
            }
        }
    }

    double areaMedia = (porcas > 0) ? (somaAreas / porcas) : 0.0;
    // Prevenção extra de bug visual caso não ache nenhuma
    if (porcas == 0 || areaMin == 9999999.0) areaMin = 0.0;

    printf("[Q7] Passo 5: Finalizando... Porcas: %d\n", porcas);
    
    std::ostringstream oss;
    oss << "Porcas encontradas: " << porcas << "\n"
        << "Area da maior porca: " << areaMax << "\n"
        << "Area da menor porca: " << areaMin << "\n"
        << "Tamanho medio da area: " << areaMedia;
        
    stats_out = oss.str();
    
    // Boa prática em Linux: Limpa a memória dos vetores explicitamente antes do return
    contours.clear();
    hierarchy.clear();
    
    return saida;
}