#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <cmath>
#include <memory>
#include <fstream>
#include <map>

const int WIDTH = 800, HEIGHT = 600;
const int AGENTE_SIZE = 10;
const int PREDADOR_SIZE = 15;
const double AGENTE_SPEED = 8.0;
const double PREDADOR_SPEED = 10.0;
const int AGENTE_PERCEPTION_RADIUS = 30;
const int PREDADOR_PERCEPTION_RADIUS = 10;

// Enumeração para representar os tipos de predadores
enum class TipoPredador {
    AGUIA,
    COBRA,
    TIGRE
};

// Classe que representa um Predador
class Predador {
private:
    int id;
    TipoPredador tipo;  // Tipo do predador
    double x, y; // Coordenadas do predador
    double dx, dy; // Velocidade do predador
    SDL_Color cor; // Cor do predador

public:
        Predador(int i, TipoPredador t, double startX, double startY)
        : id(i), tipo(t), x(startX), y(startY), dx(0), dy(0)
    {
        // Define a cor com base no tipo de predador
        switch (tipo) {
            case TipoPredador::AGUIA:
                cor = { 255, 0, 0, 255 }; // Vermelho
                break;
            case TipoPredador::COBRA:
                cor = { 255, 255, 255, 255 }; // Cinza
                break;
            case TipoPredador::TIGRE:
                cor = { 0, 0, 255, 255 }; // Azul
                break;
        }
    }

    int getId() const {
        return id;
    }

    TipoPredador getTipo() const {
        return tipo;
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    void setDirection(double dirX, double dirY) {
        dx = dirX;
        dy = dirY;
    }

    void move() {
        x += dx * PREDADOR_SPEED;
        y += dy * PREDADOR_SPEED;

    // Garante que os predadores fiquem dentro da tela
    if (x < PREDADOR_SIZE / 2) x = PREDADOR_SIZE / 2;
    if (x > WIDTH - PREDADOR_SIZE / 2) x = WIDTH - PREDADOR_SIZE / 2;
    if (y < PREDADOR_SIZE / 2) y = PREDADOR_SIZE / 2;
    if (y > HEIGHT - PREDADOR_SIZE / 2) y = HEIGHT - PREDADOR_SIZE / 2;
    }

    // Novo método para obter a cor do predador
    SDL_Color getCor() const {
        return cor;
    }
};

// Classe que representa um Agente (macaco)
class Agente {
protected:
    std::vector<double> tabela;  // Vetor de símbolos associados aos predadores
    double raioPercepcaoPredador;  // Raio de percepção da presença de predadores
    double raioPercepcaoAlarme;  // Raio de percepção de alarmes
    double x, y; // Coordenadas do agente
    double dx, dy; // Velocidade do agente

public:
    Agente(double rp, double ra)
        : tabela(10), raioPercepcaoPredador(rp), raioPercepcaoAlarme(ra), dx(0), dy(0)
    {
        // Inicializa a tabela de símbolos associados aos predadores com valores aleatórios
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        for (auto& simbolo : tabela) {
            simbolo = distribution(generator);
        }

        // Inicializa as coordenadas do agente aleatoriamente
        x = distribution(generator) * WIDTH;
        y = distribution(generator) * HEIGHT;
    }

    virtual ~Agente() = default;

    virtual void dispararAlarme(const Predador& predador) = 0;
    virtual void aumentarValorSimbolo(int indiceSimbolo) = 0;

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double getRaioPercepcaoPredador() const {
        return raioPercepcaoPredador;
    }

    double getRaioPercepcaoAlarme() const {
        return raioPercepcaoAlarme;
    }

    const std::vector<double>& getTabela() const {
        return tabela;
    }

    void setDirection(double dirX, double dirY) {
        dx = dirX;
        dy = dirY;
    }

    void move() {
        x += dx * AGENTE_SPEED;
        y += dy * AGENTE_SPEED;

    // Garante que os agentes fiquem dentro da tela
    if (x < AGENTE_SIZE / 2) x = AGENTE_SIZE / 2;
    if (x > WIDTH - AGENTE_SIZE / 2) x = WIDTH - AGENTE_SIZE / 2;
    if (y < AGENTE_SIZE / 2) y = AGENTE_SIZE / 2;
    if (y > HEIGHT - AGENTE_SIZE / 2) y = HEIGHT - AGENTE_SIZE / 2;
    }

    virtual void renderizar(SDL_Renderer* renderer) = 0;
};

// Classe que representa o Macaco, que é um tipo específico de Agente
class Macaco : public Agente {
private:
    std::string nome;

public:
    Macaco(const std::string& n, double rp, double ra)
        : Agente(rp, ra), nome(n)
    {
    }

    void dispararAlarme(const Predador& predador) override {
        int indiceMelhorSimbolo = 0;
        double maiorValor = tabela[0];

        // Encontra o símbolo de alarme com maior valor associado ao predador
        for (size_t i = 1; i < tabela.size(); ++i) {
            if (tabela[i] > maiorValor) {
                maiorValor = tabela[i];
                indiceMelhorSimbolo = i;
            }
        }

        std::cout << nome << " disparou o alarme " << indiceMelhorSimbolo << " para o predador ";
        switch (predador.getTipo()) {
            case TipoPredador::AGUIA:
                std::cout << "aguia";
                break;
            case TipoPredador::COBRA:
                std::cout << "cobra";
                break;
            case TipoPredador::TIGRE:
                std::cout << "tigre";
                break;
        }
        std::cout << std::endl;
    }

    void aumentarValorSimbolo(int indiceSimbolo) override {
        // Aumenta o valor do símbolo na tabela seguindo uma função recursiva
        tabela[indiceSimbolo] += 0.1;
    }

    const std::string& getNome() const {
        return nome;
    }

    void renderizar(SDL_Renderer* renderer) override {
        // Renderiza o macaco como um círculo verde
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect rect = { static_cast<int>(x), static_cast<int>(y), AGENTE_SIZE, AGENTE_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }
};

// Classe que representa o Ambiente
class Ambiente {
private:
    static constexpr int TAMANHO_MATRIZ = 500;
    std::vector<std::shared_ptr<Agente>> agentes;
    std::vector<Predador> predadores;
    std::map<std::string, int> interacoesPorAgente;
  public:
    Ambiente(size_t numAgentes, size_t numPredadores) {
        // Redimensiona os vetores para o tamanho adequado
        agentes.reserve(numAgentes);
        predadores.reserve(numPredadores);

        // Inicializa os agentes (macacos)
        double rp = AGENTE_PERCEPTION_RADIUS;
        double ra = PREDADOR_PERCEPTION_RADIUS;
        for (size_t i = 0; i < numAgentes; ++i) {
            agentes.push_back(std::make_shared<Macaco>("Macaco " + std::to_string(i + 1), rp, ra));
        }

        // Inicializa os predadores (começando a partir do ID 1)
        predadores.emplace_back(1, TipoPredador::AGUIA, TAMANHO_MATRIZ/2.0, TAMANHO_MATRIZ/2.0);
        predadores.emplace_back(2, TipoPredador::COBRA, TAMANHO_MATRIZ/2.0, TAMANHO_MATRIZ/2.0);
        predadores.emplace_back(3, TipoPredador::TIGRE, TAMANHO_MATRIZ/2.0, TAMANHO_MATRIZ/2.0);
    }

    void interacao() {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<double> distribution(-1.0, 1.0);

        // Atualiza a direção dos agentes e predadores
        for (const auto& agente : agentes) {
            agente->setDirection(distribution(generator), distribution(generator));
        }
        for (auto& predador : predadores) {
            predador.setDirection(distribution(generator), distribution(generator));
        }

        // Move os agentes e predadores
        for (const auto& agente : agentes) {
            agente->move();
        }
        for (auto& predador : predadores) {
            predador.move();
        }

        // Verifica a percepção dos agentes em relação aos predadores
        for (const auto& agente : agentes) {
            double agenteX = agente->getX();
            double agenteY = agente->getY();
            bool interagiu = false; 
           
            for (const auto& predador : predadores) {
                double predadorX = predador.getX();
                double predadorY = predador.getY();

                double distancia = std::sqrt(std::pow(agenteX - predadorX, 2) + std::pow(agenteY - predadorY, 2));

                if (distancia <= agente->getRaioPercepcaoPredador()) {
                    // Agente avistou o predador, dispara o alarme
                    agente->dispararAlarme(predador);
                    // Atualiza o contador de interações para o agente
                    interacoesPorAgente[static_cast<Macaco*>(agente.get())->getNome()]++;
                    interagiu = true;
                }
            }
        }
    }

void renderizar(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Renderiza os agentes
    for (const auto& agente : agentes) {
        // Renderiza o raio de percepção do agente (círculo amarelo)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
        double x = agente->getX();
        double y = agente->getY();
        double raio = agente->getRaioPercepcaoPredador();
        SDL_Rect rect = { static_cast<int>(x - raio), static_cast<int>(y - raio), static_cast<int>(raio * 2), static_cast<int>(raio * 2) };
        SDL_RenderDrawRect(renderer, &rect);

        // Verifica se o raio está dentro dos limites da tela antes de renderizá-lo
        if (x - raio >= 0 && x + raio <= WIDTH && y - raio >= 0 && y + raio <= HEIGHT) {
            SDL_Rect rect = { static_cast<int>(x - raio), static_cast<int>(y - raio), static_cast<int>(raio * 2), static_cast<int>(raio * 2) };
            SDL_RenderDrawRect(renderer, &rect);
        }

        agente->renderizar(renderer);
    }

    // Renderiza os predadores como círculos vermelhos
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto& predador : predadores) {
        int x = static_cast<int>(predador.getX());
        int y = static_cast<int>(predador.getY());
        SDL_Rect rect = { x, y, PREDADOR_SIZE, PREDADOR_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    for (const auto& predador : predadores) {
        SDL_SetRenderDrawColor(renderer, predador.getCor().r, predador.getCor().g, predador.getCor().b, predador.getCor().a);
        int x = static_cast<int>(predador.getX());
        int y = static_cast<int>(predador.getY());
        SDL_Rect rect = { x, y, PREDADOR_SIZE, PREDADOR_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
}

  void gerarLogInteracoesPorAgente() {
        std::ofstream arquivo("log_interacoes_agentes.txt");
        if (arquivo.is_open()) {
            arquivo << "Log de Interacoes por Agente:" << std::endl;
            arquivo << "----------------------------------------------" << std::endl;
            for (const auto& par : interacoesPorAgente) {
                arquivo << par.first << " interagiu " << par.second << " vezes." << std::endl;
            }
            arquivo.close();
            std::cout << "Log gerado com sucesso!" << std::endl;
        } else {
            std::cout << "Erro ao criar o arquivo de log!" << std::endl;
        }
    }
};

int main(int argc, char* argv[])
{
    // Criação do ambiente com 6 agentes e 3 predadores
    Ambiente ambiente(6, 3);

    // Inicialização do SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Trabalho de Inteligência Artificial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        std::cout << "Nao foi possível criar a janela " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        std::cout << "Não foi possível criar o renderizador " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Event windowEvent;

    // Variáveis para controle do loop de jogo
    const int FPS = 60;  // Frames por segundo desejados
    const int FrameDelay = 1000 / FPS;  // Tempo de atraso entre os frames
    Uint32 frameStart;  // Hora de início do frame
    int frameTime;  // Tempo que levou para executar o frame

    bool quit = false;
    while (!quit) {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Atualiza a posição dos agentes e predadores
        ambiente.interacao();

        // Renderiza a cena
        ambiente.renderizar(renderer);

        // Calcula o tempo que levou para executar o frame
        frameTime = SDL_GetTicks() - frameStart;

        // Se ainda houver tempo sobrando no frame, faz um pequeno atraso
        if (FrameDelay > frameTime) {
            SDL_Delay(FrameDelay - frameTime);
        }
    }

    // Gerar o log de interações por agente quando a janela for fechada
    ambiente.gerarLogInteracoesPorAgente();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
  
