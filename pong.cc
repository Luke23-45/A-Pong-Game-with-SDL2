#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;

const int PADDLE_WIDTH  = 15;
const int PADDLE_HEIGHT = 90;


//  HSV to RGB conversion
// h: 0-360, s: 0-1, v: 0-1

SDL_Color HSVtoRGB(float h, float s, float v) {
    int i = int(h / 60.0f) % 6;
    float f = (h / 60.0f) - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r, g, b;
    if (i == 0)       { r = v; g = t; b = p; }
    else if (i == 1)  { r = q; g = v; b = p; }
    else if (i == 2)  { r = p; g = v; b = t; }
    else if (i == 3)  { r = p; g = q; b = v; } 
    else if (i == 4)  { r = t; g = p; b = v; } 
    else              { r = v; g = p; b = q; }
    return SDL_Color{ static_cast<Uint8>(r * 255),
                      static_cast<Uint8>(g * 255),
                      static_cast<Uint8>(b * 255),
                      255 };
}


//  Draw a filled circle

void SDL_RenderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}


// Collision detection between a circle and a rectangle.

bool Collision(SDL_Rect *rect, int centerX, int centerY, int radius) {
    int closeX = std::max(rect->x, std::min(rect->x + rect->w, centerX));
    int closeY = std::max(rect->y, std::min(rect->y + rect->h, centerY));
    int diffX = centerX - closeX;
    int diffY = centerY - closeY;
    return (diffX * diffX + diffY * diffY <= radius * radius);
}


// Particle structure for collision effects

struct Particle {
    float x, y;
    float vx, vy;
    int life;      
    int maxLife;   
    float baseHue;   
};

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Initialize SDL_ttf for score rendering.
    if (TTF_Init() < 0) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Dynamic Pong", SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer 
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Load font for score rendering.
    TTF_Font* font = TTF_OpenFont("arial.ttf", 28);
    if (!font) {
        std::cout << "TTF_OpenFont Error: " << TTF_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Seed randomness 
    std::srand(static_cast<unsigned int>(SDL_GetTicks()));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(-3.0, 3.0);
    std::uniform_int_distribution<> start_pos_dist_x(15, WINDOW_WIDTH - 15); 
    std::uniform_int_distribution<> start_pos_dist_y(15, WINDOW_HEIGHT - 15);

    // Game variables
    int ballRadius = 15;
    int ballX = WINDOW_WIDTH / 2;
    int ballY = WINDOW_HEIGHT / 2;
    float ballSpeedX = 5.0f;
    float ballSpeedY = 5.0f;
    float initialBallSpeed = 5.0f; 
  
    
    // Player paddle (right side)
    int playerX = WINDOW_WIDTH - PADDLE_WIDTH - 20;
    int playerY = WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    int playerSpeed = 10;

    // AI paddle (left side)
    int aiX = 20;
    int aiY = WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    int aiSpeed = 6;

    int playerScore = 0;
    int aiScore = 0;

    // Particle vector for collision effects.
    std::vector<Particle> particles;
    const int PARTICLES_ON_HIT = 20;

    bool quit = false;
    SDL_Event e;
    Uint32 startTick, frameTime;

    // Function to reset ball position randomly
    auto resetBall = [&]() {
        ballX = start_pos_dist_x(gen);
        ballY = start_pos_dist_y(gen);
      
        ballSpeedX = initialBallSpeed * (ballSpeedX > 0 ? -1.0f : 1.0f);
        ballSpeedY = (std::rand() % 21) - 10; // Random vertical direction between -10 and 10
        if (ballSpeedY == 0) ballSpeedY = 1; // Ensure non-zero vertical speed

        // Normalizing the speed vector to initialBallSpeed.
        float speedMagnitude = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);
        ballSpeedX = ballSpeedX * initialBallSpeed / speedMagnitude;
        ballSpeedY = ballSpeedY * initialBallSpeed / speedMagnitude;
    };

    while (!quit) {
        startTick = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
        }


        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_UP]) {
            playerY -= playerSpeed;
            if (playerY < 2) playerY = 2;
        }
        if (keystates[SDL_SCANCODE_DOWN]) {
            playerY += playerSpeed;
            if (playerY > WINDOW_HEIGHT - PADDLE_HEIGHT - 2)
                playerY = WINDOW_HEIGHT - PADDLE_HEIGHT - 2;
        }

        ballX += ballSpeedX;
        ballY += ballSpeedY;

        // Bounce off top and bottom walls.
        if (ballY - ballRadius*2 <= 0){
          ballSpeedY = -ballSpeedY + 1;
        }
            
      
        if( ballY + ballRadius >= WINDOW_HEIGHT){
          ballSpeedY = -ballSpeedY;
        }


        if (ballX - ballRadius < 0) {
            playerScore++; // Player scores if AI misses
            resetBall();
        } else if (ballX + ballRadius > WINDOW_WIDTH) {
            aiScore++; // AI scores if Player misses
            resetBall();
        }


        int aiCenter = aiY + PADDLE_HEIGHT / 2;
        if (ballSpeedX < 0) {
            if (ballY < aiCenter - 5)
                aiY -= aiSpeed;
            else if (ballY > aiCenter + 5)
                aiY += aiSpeed;
        } else {

            if (aiCenter < WINDOW_HEIGHT / 2)
                aiY += aiSpeed;
            else if (aiCenter > WINDOW_HEIGHT / 2)
                aiY -= aiSpeed;
        }

        if (aiY < 2) aiY = 2;
        if (aiY > WINDOW_HEIGHT - PADDLE_HEIGHT - 2) aiY = WINDOW_HEIGHT - PADDLE_HEIGHT - 2;


        SDL_Rect playerPaddle = { playerX, playerY, PADDLE_WIDTH, PADDLE_HEIGHT };
        SDL_Rect aiPaddle     = { aiX, aiY, PADDLE_WIDTH, PADDLE_HEIGHT };

        bool collisionHappened = false;



        const float speedIncreaseFactor = 1.05f;
        const float maxSpeed = 20.0f;
        if (Collision(&playerPaddle, ballX, ballY, ballRadius)) {
            // Calculate offset (range [-1, 1])
            float offset = (ballY - (playerY + PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2.0f);
            // Get current speed
            float speed = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);
            // Reverse horizontal direction (ball moves left)
            ballSpeedX = -std::abs(ballSpeedX);
            ballSpeedY = speed * offset + distrib(gen)+1;
            // Increase speed
            speed *= speedIncreaseFactor;
            if (speed > maxSpeed) speed = maxSpeed;
            float angle = std::atan2(ballSpeedY, ballSpeedX);
            ballSpeedX = speed * std::cos(angle);
            ballSpeedY = speed * std::sin(angle);
            collisionHappened = true;
        }
        if (Collision(&aiPaddle, ballX, ballY, ballRadius)) {
            float offset = (ballY - (aiY + PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2.0f);
            float speed = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);
            // Reverse horizontal direction (ball moves right)
            ballSpeedX = std::abs(ballSpeedX);
            ballSpeedY = speed * offset + distrib(gen);
            speed *= speedIncreaseFactor;
            if (speed > maxSpeed) speed = maxSpeed;
            float angle = std::atan2(ballSpeedY, ballSpeedX);
            ballSpeedX = speed * std::cos(angle);
            ballSpeedY = speed * std::sin(angle);
            collisionHappened = true;
        }

        //  Spawn particles on collision 
        if (collisionHappened) {
            for (int i = 0; i < PARTICLES_ON_HIT; i++) {
                Particle p;
                p.x = ballX;
                p.y = ballY;
                float angle = (std::rand() % 360) * M_PI / 180.0f;
                float speed = 3.0f + (std::rand() % 100) / 50.0f; // Speed between 3 and 5
                p.vx = std::cos(angle) * speed;
                p.vy = std::sin(angle) * speed;
                p.maxLife = p.life = 30 + std::rand() % 20;
                p.baseHue = std::rand() % 360;
                particles.push_back(p);
            }
        }

        //  Update Particles 
        for (size_t i = 0; i < particles.size(); ) {
            Particle &p = particles[i];
            p.x += p.vx;
            p.y += p.vy;
            p.life--;
            if (p.life <= 0) {
                particles.erase(particles.begin() + i);
            } else {
                ++i;
            }
        }


        // Clear screen with a dark background.
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Draw center dotted line.
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200);
        for (int y = 10; y < WINDOW_HEIGHT; y += 20) {
            SDL_Rect dot = { WINDOW_WIDTH / 2 - 2, y, 4, 10 };
            SDL_RenderFillRect(renderer, &dot);
        }

        // Draw paddles
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &playerPaddle);
        SDL_RenderFillRect(renderer, &aiPaddle);

        // Draw dynamic ball 
        float dynamicHue = std::fmod(SDL_GetTicks() / 10.0f, 360.0f);
        SDL_Color ballColor = HSVtoRGB(dynamicHue, 1.0f, 1.0f);
        SDL_SetRenderDrawColor(renderer, ballColor.r, ballColor.g, ballColor.b, ballColor.a);
        SDL_RenderFillCircle(renderer, ballX, ballY, ballRadius);

        // Draw particles
        for (const auto &p : particles) {
            float lifeRatio = static_cast<float>(p.life) / p.maxLife;
            float particleHue = std::fmod(p.baseHue + dynamicHue, 360.0f);
            SDL_Color particleColor = HSVtoRGB(particleHue, 1.0f, 1.0f);
            particleColor.a = static_cast<Uint8>(lifeRatio * 255);
            SDL_SetRenderDrawColor(renderer, particleColor.r, particleColor.g, particleColor.b, particleColor.a);
            SDL_RenderFillCircle(renderer, static_cast<int>(p.x), static_cast<int>(p.y), 3);
        }

        // Render score text
        SDL_Color textColor = { 255, 255, 255, 255 };

        std::string playerScoreText = "Player: " + std::to_string(playerScore);
        SDL_Surface* playerTextSurface = TTF_RenderText_Blended(font, playerScoreText.c_str(), textColor);
        if (playerTextSurface) {
            SDL_Texture* playerTextTexture = SDL_CreateTextureFromSurface(renderer, playerTextSurface);
            SDL_Rect playerTextRect;
            playerTextRect.x = WINDOW_WIDTH * 3/4 - playerTextSurface->w / 2;
            playerTextRect.y = 10;
            playerTextRect.w = playerTextSurface->w;
            playerTextRect.h = playerTextSurface->h;
            SDL_RenderCopy(renderer, playerTextTexture, NULL, &playerTextRect);
            SDL_DestroyTexture(playerTextTexture);
            SDL_FreeSurface(playerTextSurface);
        }

        std::string aiScoreText = "AI: " + std::to_string(aiScore);
        SDL_Surface* aiTextSurface = TTF_RenderText_Blended(font, aiScoreText.c_str(), textColor);
        if (aiTextSurface) {
            SDL_Texture* aiTextTexture = SDL_CreateTextureFromSurface(renderer, aiTextSurface);
            SDL_Rect aiTextRect;
            aiTextRect.x = WINDOW_WIDTH / 4 - aiTextSurface->w / 2;
            aiTextRect.y = 10;
            aiTextRect.w = aiTextSurface->w;
            aiTextRect.h = aiTextSurface->h;
            SDL_RenderCopy(renderer, aiTextTexture, NULL, &aiTextRect);
            SDL_DestroyTexture(aiTextTexture);
            SDL_FreeSurface(aiTextSurface);
        }

        SDL_RenderPresent(renderer);

        // Frame rate control (~60 FPS)
        frameTime = SDL_GetTicks() - startTick;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }

    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
