#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>
#include <stdbool.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 

#define LIGHTBLUE  CLITERAL(Color){ 0, 191, 255, 255 }    //LIGHTBLUE
#define LIGHTGREEN CLITERAL(Color){ 50, 205, 50, 255 }      //LIGHTGREEN
#define SILVERPURPLE CLITERAL(Color){ 214, 154, 219, 255 }    //SILVERBLUE
#define LIMEGREEN  CLITERAL(Color){ 125, 206, 160, 255 }      //LIMEGREEN
#define DARKRED    CLITERAL(Color){ 255, 69, 0, 255 }       //DARKRED
#define PINKMOLLY CLITERAL(Color){255, 82, 145, 255}
#define window_width 1600
#define window_height 900
#define BALL_COUNT 5
#define HEART_COUNT 1
#define SPIKE_COUNT 4

#define MAX_BALL_COUNT 15    // Giới hạn số lượng bóng tối đa
#define BALL_INCREMENT_TIME 15000 // Thời gian tăng bóng (ms)
#define MAX_SPIKE_COUNT 10   // Giới hạn số lượng spike tối đa
#define SPIKE_INCREMENT_TIME 30000 // Thời gian tăng spike (ms)
//chưa làm tăng số spike và ball theo thời gian
typedef enum GameState { MENU = 0, INSTRUCTION, GAME , GAME2 , ENDGAME} GameState;

int ballCount = BALL_COUNT;
int spikeCount = SPIKE_COUNT;

float ballTimer = 0.0f;
float spikeTimer = 0.0f;


typedef struct {
    int x, y;
    float speed;
    float size;
} Ball;

typedef struct{
    int x, y;
    int width;
    int height;
    float size;
    int velocity;
} Rodungbong;

typedef struct{
    int x, y;
    int width;
    int height;
    float velocity;
} Heart;

typedef struct{
    int x, y;
    int width;
    int height;
    float velocity;
    float size;
} Spike;

void display(Ball balls[], int score,int attempt, Heart heart[], Texture2D heart_texture, Spike spike[], Texture2D spike_texture) {
    for (int i = 0; i < ballCount; i++) {
        DrawCircle(balls[i].x, balls[i].y, balls[i].size, BLUE); 
    }
    for (int i = 0; i < HEART_COUNT; i++) {
        if( heart[i].y >= 0 ){ // chỉ vẽ trái tim nếu nó có trên màn hinh
        DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE);
    }
    }
    for (int i = 0; i < spikeCount; i++) {
        DrawTexture(spike_texture, spike[i].x, spike[i].y, WHITE);
    
    }
    DrawText(TextFormat("%d Lives", attempt), 1360, 20 , 40, RED); 
    DrawText(TextFormat("Score: %d", score), 20, 20, 40, BLUE);
    DrawText(TextFormat("Press Esc to exit"), 20, 800 , 20, RED);
}


int rodungbong_on_ground (Rodungbong rodungbong ){
    if( rodungbong.y >= window_height - rodungbong.height*2){
        return 1;
    }
    return 0;
}
// Kiểm tra va chạm giữa hai quả bóng
bool CheckCircleCollision(Ball ball1, Ball ball2) {
    float distance = sqrt(pow(ball1.x - ball2.x, 2) + pow(ball1.y - ball2.y, 2)); //circle collision
    return distance <= (ball1.size + ball2.size);
}

// Hàm khởi tạo thời gian rơi ngẫu nhiên cho từng quả bóng
void InitializeBallTimers(int ballTimers[], int ballCount, int maxDelay) {
    for (int i = 0; i < ballCount; i++) {
        ballTimers[i] = rand() % maxDelay; // Thời gian rơi ngẫu nhiên trong khoảng [0, maxDelay]
    }
}

// Cập nhật vị trí bóng dựa trên thời gian rơi
void UpdateBallsWithTimers(Ball balls[], int ballCount, int ballTimers[], int currentTime, float deltaTime, float gravity, float do_kho_game) {
    for (int i = 0; i < ballCount; i++) {
        if (currentTime >= ballTimers[i]) { // Kiểm tra nếu đã đến thời gian rơi
            balls[i].speed += gravity * deltaTime * do_kho_game;
            balls[i].y += balls[i].speed * deltaTime / 2;

            // Reset bóng nếu chạm đáy
            if (balls[i].y >= window_height) {
                balls[i].y = -200 - rand() % (80);
                balls[i].speed = gravity * deltaTime * do_kho_game;
            }
        }
    }
}


bool CheckspikeCollision(Spike spike1, Spike spike2) {
    float distance = sqrt(pow(spike1.x - spike2.x, 2) + pow(spike1.y - spike2.y, 2)); //circle collision
    return distance <= (spike1.width + spike2.width);
}

// Hàm khởi tạo thời gian rơi ngẫu nhiên cho từng quả bóng
void InitializeSpikeTimers(int spikeTimers[], int spikeCount, int maxDelay) {
    for (int i = 0; i < spikeCount; i++) {
        spikeTimers[i] = rand() % maxDelay; // Thời gian rơi ngẫu nhiên trong khoảng [0, maxDelay]
    }
}

void UpdatespikeWithTimers(Spike spikes[], int spikeCount, int spikeTimers[], int currentTime, float deltaTime, float gravity,float do_kho_game) {
    for (int i = 0; i < spikeCount; i++) {
        if (currentTime >= spikeTimers[i]) { 
            spikes[i].velocity += gravity * deltaTime * do_kho_game;
            spikes[i].y += spikes[i].velocity * deltaTime / 2;

            if (spikes[i].y >= window_height) {
                spikes[i].y = -90 - rand() % (80); // Reset vị trí spike
                spikes[i].velocity = gravity * deltaTime * do_kho_game;
            }
        }
    }
}




int main(void)
{  

    char *filename = "highscore.txt";
    float do_kho_game;
    // Giá trị ban đầu của thanh bar
    float value = 1.0f; 
    // Giá trị tối thiểu và tối đa của thanh bar
    float minValue = 0.6f;
    float maxValue = 2.0f;
    // Số lượng nấc trên thanh bar
    int numSteps = 10;

    int attempt = 3;
    float deltaTime = 0.05f ; // Thời gian trôi qua giữa các khung hình, chương trình đáng ra phải dùng 0.01 nhưng nếu vậy thì nó sẽ mất quá nhiều thời gian để bắt đầu rơi :D
    float gravity = 9.81f; // gia tốc
    // Trong vòng lặp GAME
    
    InitWindow( window_width, window_height, "Catching Ball : GO");
    GameState gameState = MENU;
    SetTargetFPS( 100 ); 
    InitAudioDevice();  // Khởi tạo hệ thống âm thanh

    Sound gainheart = LoadSound("music/gain.mp3");
    Sound loseheart = LoadSound("music/hurt.mp3");


    Music music = LoadMusicStream("music/background.mp3");
    PlayMusicStream(music);
    
    Image backgroundImage = LoadImage("img/background.png");
    Texture2D background_texture = LoadTextureFromImage(backgroundImage);

    Image rodungbongImage = LoadImage("img/rodungbong.png");
    Texture2D rodungbong_texture = LoadTextureFromImage(rodungbongImage);

    Image heartImage = LoadImage("img/heart.png");
    Texture2D heart_texture = LoadTextureFromImage(heartImage);
    
    Image iconImage = LoadImage("img/icon.png");
    Texture2D icon_texture = LoadTextureFromImage(iconImage);

    Image spikeImage = LoadImage("img/spike.png");
    Texture2D spike_texture = LoadTextureFromImage(spikeImage);
    
    
    Rodungbong rodungbong ={ 
        .x = window_width/2,
        .y = 994, 
        .height = 43,
        .width = 97,
        
    };
    Heart heart [HEART_COUNT];
    for (int i = 0; i < HEART_COUNT; i++) {
    heart[i].y = -30; // khởi tạo cho nó trên cùng, không hiện ra 
    heart[i].velocity = 3.0; // tốc độ rơi
    heart[i].width = heart_texture.width;
    heart[i].height = heart_texture.height;
}
    Spike spike [SPIKE_COUNT];
    srand(time(NULL));
    for (int i = 0; i < SPIKE_COUNT; i++) {
    spike[i].x = 30 + rand() % (window_width - 60);
    spike[i].y = -300 - rand() % (80);; // khởi tạo cho nó trên cùng, không hiện ra 
    spike[i].velocity = gravity * deltaTime * do_kho_game; // tốc độ rơi
    spike[i].width = spike_texture.width;
    spike[i].height = spike_texture.height;
    spike[i].size = sqrt(pow(spike[i].width,2) + pow(spike[i].height,2));
}
    Ball balls[BALL_COUNT];
    int score = 0;
    srand(time(NULL));
    for (int i = 0; i < BALL_COUNT; i++) {
        balls[i].x = 25 + rand() % (window_width - 50); // Giới hạn để không bị ra ngoài màn hình
        balls[i].y = -200 - rand() % (80); 
        balls[i].speed = gravity * deltaTime * do_kho_game; // Tốc độ rơi tự do
        balls[i].size = 20.0; // bán kính
    }

    int ballTimers[BALL_COUNT]; // Mảng lưu thời gian rơi của từng quả bóng
    int spikeTimers[SPIKE_COUNT];
    int currentTime = 0; // Biến đếm thời gian
    InitializeBallTimers(ballTimers, BALL_COUNT, 2000); // Khởi tạo thời gian rơi ngẫu nhiên, maxDelay = 2000ms
    InitializeBallTimers(spikeTimers, SPIKE_COUNT, 2000); // Khởi tạo thời gian rơi ngẫu nhiên, maxDelay = 2000ms
    

    int highScore = 0; 
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &highScore);
        fclose(fp);
    }
    // GAME BẮT ĐẦU
    while (!WindowShouldClose())
    {
        
        switch (gameState)
        {
            case MENU:
            {
                // Khi nhấn phím Spacebar, chuyển sang trạng thái GAME
                if (IsKeyPressed(KEY_SPACE)) 
                {
                    gameState = INSTRUCTION;
                }
            } break;
            case INSTRUCTION:
            {
                // Khi nhấn phím Spacebar, chuyển sang trạng thái GAME
                if (IsKeyPressed(KEY_P)) 
                {
                    gameState = GAME;
                }
            } break;

            case GAME:
            {
                // Trong màn chơi, nếu nhấn phím END, quay lại MENU
                if (IsKeyPressed(KEY_END)) 
                {
                    gameState = MENU;
                }
            } break;

            case ENDGAME:
            {
                if(IsKeyPressed(KEY_SPACE)){
                    gameState = GAME;
                }
            } break;
        }
    GuiSliderBar((Rectangle){ 100, 100, 400, 20 }, NULL, NULL, &value, minValue, maxValue);
        do_kho_game = value;
        int currentStep = (int)((value - minValue) / (maxValue - minValue) * numSteps);
    
    SetWindowIcon(iconImage ); 
    UpdateMusicStream(music);
    BeginDrawing();
    ClearBackground(WHITE);
    DrawTexture( background_texture, 0 , 0 ,WHITE);

    if (gameState == MENU)
        {
            const char *titleText = "Catching Ball: GO";
        const char *startText = "Press SPACE to START";
        const char *exitText = "Press ESC to EXIT";

        // Cỡ chữ
        int titleFontSize = 120;
        int infoFontSize = 80;

        // Tính toán chiều rộng của mỗi chuỗi văn bản
        int titleWidth = MeasureText(titleText, titleFontSize);
        int startWidth = MeasureText(startText, infoFontSize);
        int exitWidth = MeasureText(exitText, infoFontSize);

        // Căn chỉnh văn bản vào giữa theo trục x
        int titleX = (window_width - titleWidth) / 2;
        int startX = (window_width - startWidth) / 2;
        int exitX = (window_width - exitWidth) / 2;

        // Căn chỉnh vị trí y cách đều
        int titleY = 300;  
        int startY = titleY + 200;            
        int exitY = startY + 100; 
        DrawText(titleText, titleX, titleY, titleFontSize, DARKBLUE);
        DrawText(startText, startX, startY, infoFontSize, MAGENTA);
        DrawText(exitText, exitX, exitY, infoFontSize, RED);
        }
    else if(gameState == INSTRUCTION){
        const char *title1Text = "Tutorial";
        const char *tutorial1Text = "Move the basket left and right using A and D to catch balls.";
        const char *tutorial2Text = "Earn points for every ball caught";
        const char *tutorial3Text = "Lose heart if a you hit the spikes!";
        const char *tutorial4Text = "Aim for the highest score!";
        const char *tutorial5Text = "GOOD LUCK :D";
        const char *dokhoText = " Difficulty :";
        // Cỡ chữ
        int title1FontSize = 120;
        int info1FontSize = 50;

        // Tính toán chiều rộng của mỗi chuỗi văn bản
        int title1Width = MeasureText(title1Text, title1FontSize);
        int tutorial1Width = MeasureText(tutorial1Text, info1FontSize);
        int tutorial2Width = MeasureText(tutorial2Text, info1FontSize);
        int tutorial3Width = MeasureText(tutorial3Text, info1FontSize);
        int tutorial4Width = MeasureText(tutorial4Text, info1FontSize);
        int tutorial5Width = MeasureText(tutorial5Text, info1FontSize);
        int dokhoWidth = MeasureText(dokhoText, info1FontSize);

        // Căn chỉnh văn bản vào giữa theo trục x
        int title1X = (window_width - title1Width) / 2;
        int tutorial1X = 50;
        int tutorial2X = 50;
        int tutorial3X = 50;
        int tutorial4X = 50;
        int tutorial5X = 50;
        int dokhoX = 60;

        // Căn chỉnh vị trí y cách đều
        int title1Y = 100;  
        int tutorial1Y = title1Y + 100; 
        int tutorial2Y = tutorial1Y + 100;
        int tutorial3Y = tutorial2Y + 100;   
        int tutorial4Y = tutorial3Y + 100; 
        int tutorial5Y = tutorial4Y + 100;        
        int dokhoY = tutorial1Y + 600; 
        DrawText(title1Text, title1X, title1Y, title1FontSize, LIGHTBLUE);
        DrawText(tutorial1Text, tutorial1X, tutorial1Y, info1FontSize, LIMEGREEN);
        DrawText(tutorial2Text, tutorial2X, tutorial2Y, info1FontSize, LIMEGREEN);
        DrawText(tutorial3Text, tutorial3X, tutorial3Y, info1FontSize, LIMEGREEN);
        DrawText(tutorial4Text, tutorial4X, tutorial4Y, info1FontSize, LIGHTBLUE);
        DrawText(tutorial5Text, tutorial5X, tutorial5Y, 100, PINKMOLLY);
        DrawText(dokhoText, dokhoX, dokhoY, 60, GOLD);
        GuiSliderBar((Rectangle){ 580, 800, 400, 50 }, 
                         NULL, NULL, 
                         &value, minValue, maxValue);
        DrawText("EASY", 420, 805, 50, LIMEGREEN); 
        DrawText("|NORMAL|", 750, 860, 20, ORANGE);
        DrawText("VERY HARD", 1000, 805, 50, DARKRED); 
        do_kho_game = currentStep;
    }

    else if (gameState == GAME){
            DrawTexture(rodungbong_texture, rodungbong.x, rodungbong.y, WHITE);
            currentTime += GetFrameTime() * 1000; // Cập nhật thời gian hiện tại
            UpdateBallsWithTimers(balls, BALL_COUNT, ballTimers, currentTime, deltaTime, gravity,do_kho_game);
            UpdatespikeWithTimers(spike, SPIKE_COUNT, spikeTimers, currentTime, deltaTime, gravity, do_kho_game);
            ballTimer += GetFrameTime() * 1000.0f; // Cập nhật thời gian bóng
            spikeTimer += GetFrameTime() * 1000.0f; // Cập nhật thời gian gai

            if (ballTimer >= BALL_INCREMENT_TIME && ballCount < MAX_BALL_COUNT)
            {
                ballCount++;
                ballTimer = 0.0f;
            }

            if (spikeTimer >= SPIKE_INCREMENT_TIME && spikeCount < MAX_SPIKE_COUNT)
            {
                spikeCount++;
                spikeTimer = 0.0f;
            }
            
    currentTime += GetFrameTime() * 1000; 
            // Cập nhật vị trí của rổ đựng bóng
            rodungbong.y += rodungbong.velocity;
            rodungbong.velocity += 1;

            // Kiểm tra rổ đựng bóng có ở vị trí
            if(rodungbong_on_ground( rodungbong )){
            rodungbong.velocity = 0;
            rodungbong.y = window_height - rodungbong.height*2;
            }

            // Điều khiển rổ đựng bóng người chơi
            if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
            rodungbong.x -= 15;
            if(rodungbong.x <= 0){
            rodungbong.x = 0;
            }
            }
            if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
            rodungbong.x += 15;
            if(rodungbong.x >= window_width - rodungbong.width){
            rodungbong.x = window_width - rodungbong.width;
            }
            }

                // Cập nhật các quả bóng rơi
                for (int i = 0; i < BALL_COUNT; i++) {
                balls[i].speed += gravity * deltaTime * do_kho_game; // Cập nhật tốc độ theo thời gian
                balls[i].y += balls[i].speed * deltaTime / 2; // Hàm g.t^2 trong vật lý

                    // Kiểm tra va chạm giữa các quả bóng
                    for (int j = 0; j < BALL_COUNT; j++) {
                        if (i != j && CheckCircleCollision(balls[i], balls[j])) {
                        // Nếu va chạm, đẩy một trong hai quả bóng lên vị trí mới
                    balls[j].x = 25 + rand() % (window_width - 50);
                    balls[j].y = -60 - rand() % (80);
                    balls[j].speed = gravity * deltaTime * do_kho_game;
                    }
                    }
                    

                    // Nếu bóng chạm đáy, kiểm tra va chạm với rổ và reset bóng
                    if (balls[i].y >= window_height) {
                    balls[i].y = -200 - rand() % (80);
                    balls[i].speed = gravity * deltaTime * do_kho_game;
                    }

                    // Kiểm tra va chạm với rổ đựng bóng
                    if (balls[i].x + balls[i].size > rodungbong.x  &&
                    balls[i].x - balls[i].size < rodungbong.x + rodungbong.width &&
                    balls[i].y + balls[i].size > rodungbong.y &&
                    balls[i].y < rodungbong.y + rodungbong.height) {
                    score++;
                    // Reset quả bóng sau khi va chạm
                    balls[i].x = 25 + rand() % (window_width - 50);
                    balls[i].y = -200 - rand() % (80);
                    balls[i].speed = gravity * deltaTime * do_kho_game;
                    }
                    }

                    if(score >= 10 && score % 10 == 0 ){
                    for (int i = 0; i < HEART_COUNT; i++) { // tạo trái tim
                    heart[i].x = 888 ;
                    heart[i].y = 30; 
                    heart[i].velocity = 3.0; 
                    }
                    }
            
                for (int i = 0; i < HEART_COUNT; i++) {
                if (heart[i].y > 0) { // nếu có trái tim thì sẽ cho vào hàng chờ ,rơi xuống
                heart[i].y += heart[i].velocity;
                
                    // kiểm tra va chạm
                    if (heart[i].x + heart[i].width > rodungbong.x &&
                        heart[i].x - heart[i].width < rodungbong.x + rodungbong.width && 
                        heart[i].y + heart[i].height > rodungbong.y &&
                        heart[i].y < rodungbong.y + rodungbong.height) {
                        attempt++; // tăng mạng
                        heart[i].y = -30; // reset vị trí
                        PlaySound(gainheart);
                    }
                    if (heart[i].y >= window_height) {
                        heart[i].y = -30; // reset vị trí trái tim
                    }
                DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE); // vẽ tim
                }
                }
                
            
                for (int i = 0; i < SPIKE_COUNT; i++) {
            // nếu có sì pai thì sẽ cho vào hàng chờ ,rơi xuống
                spike[i].velocity += gravity * deltaTime * do_kho_game; // Cập nhật tốc độ theo thời gian
                spike[i].y += spike[i].velocity * deltaTime / 2;

                    for (int j = 0; j < SPIKE_COUNT; j++) {
                        if (i != j && CheckspikeCollision(spike[i], spike[j])) {
                        // Nếu va chạm, đẩy một trong hai quả bóng lên vị trí mới
                    spike[j].x = 25 + rand() % (window_width - 50);
                    spike[j].y = -300 - rand() % (80);
                    spike[j].velocity = gravity * deltaTime * do_kho_game;
                    }
                    }
                    if (spike[i].y >= window_height) {
                        spike[i].y = -300 - rand() % (80);; // reset vị trí sì pai
                        spike[i].velocity = gravity * deltaTime * do_kho_game;
                    }
                    // kiểm tra va chạm
                    if (spike[i].x + spike[i].width > rodungbong.x &&
                        spike[i].x - spike[i].width < rodungbong.x + rodungbong.width && 
                        spike[i].y + spike[i].height > rodungbong.y &&
                        spike[i].y < rodungbong.y + rodungbong.height) {
                        attempt--; // trừ mạng
                        spike[i].x = 30 + rand() % (window_width - 60) ; // 
                        spike[i].y = -300 - rand() % (80); // 
                        spike[i].velocity = gravity * deltaTime * do_kho_game;
                        PlaySound(loseheart);
                    }
                
            DrawTexture(spike_texture, spike[i].x, spike[i].y, BLACK); // vẽ tim
        
            }
            if( attempt <= 0){ // tắt chương trình khi số lượng máu bằng 10
            gameState = ENDGAME;
            UnloadTexture( heart_texture);
            UnloadTexture(spike_texture);
            UnloadTexture(rodungbong_texture);
            }
            display(balls, score, attempt, heart, heart_texture, spike, spike_texture); // Vẽ bóng và tim và sì pai
        
    }
    else if(gameState == ENDGAME){
            if (score > highScore) {
            highScore = score; 
            fp = fopen(filename, "w");
            if (fp != NULL) {
            fprintf(fp, "%d", highScore);
            fclose(fp);
             }
            }   
            const char *endText = "YOU LOSE";
            char scoreText[50];
            sprintf(scoreText, "Your score: %d", score);
            const char *instructionText = "Press ESC to exit";
            char highScoreText[50];
            sprintf(highScoreText, "High Score: %d", highScore);
    
            int titleFontSize = 120;
            int infoFontSize = 80;

        
            int endWidth = MeasureText(endText, titleFontSize);
            int scoreWidth = MeasureText(scoreText, infoFontSize);
            int highScoreWidth = MeasureText(highScoreText, infoFontSize);
            int instructionWidth = MeasureText(instructionText, infoFontSize - 20);


            int endX = (window_width - endWidth) / 2;
            int scoreX = (window_width - scoreWidth) / 2;
            int highScoreX = (window_width - highScoreWidth) / 2;
            int instructionX = (window_width - instructionWidth) / 2;

            int endY = 300;  
            int scoreY = endY + 200;            
            int highScoreY = scoreY + 100; 
            int instructionY = highScoreY + 100; 

            DrawText(endText, endX, endY, titleFontSize, RED);
            DrawText(scoreText, scoreX, scoreY, infoFontSize, MAGENTA);
            DrawText(highScoreText, highScoreX, highScoreY, infoFontSize, GREEN);
            DrawText(instructionText, instructionX, instructionY, infoFontSize, LIGHTGRAY);
         
        }

    
        EndDrawing();
    }
    
    //chương trình auto nhận Esc là exit
    UnloadSound(gainheart);
    UnloadSound(loseheart);
    UnloadMusicStream(music);  // Giải phóng tài nguyên nhạc
    CloseAudioDevice();
    CloseWindow(); // Close window and exit the program
    
    return 0;
}