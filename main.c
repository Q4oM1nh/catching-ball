#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>


#define window_width 1280
#define window_height 720
#define BALL_COUNT 8
#define HEART_COUNT 1
#define SPIKE_COUNT 3

typedef enum GameState { MENU = 0, GAME ,ENDGAME} GameState;


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
} Spike;

void display(Ball balls[], int score,int attempt, Heart heart[], Texture2D heart_texture, Spike spike[], Texture2D spike_texture) {
    for (int i = 0; i < BALL_COUNT; i++) {
        DrawCircle(balls[i].x, balls[i].y, balls[i].size, BLUE); 
    }
    for (int i = 0; i < HEART_COUNT; i++) {
        if( heart[i].y >= 0 ){ // chỉ vẽ trái tim nếu nó có trên màn hinh
        DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE);
    }
    }
    for (int i = 0; i < SPIKE_COUNT; i++) {
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

int main(void)
{   
    
    char *filename = "highscore.txt";
    

    int attempt = 3;
    float deltaTime = 0.1f ; // Thời gian trôi qua giữa các khung hình, chương trình đáng ra phải dùng 0.01 nhưng nếu vậy thì nó sẽ mất quá nhiều thời gian để bắt đầu rơi :D
    float gravity = 9.81f; // gia tốc
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
        .velocity = 7
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
    spike[i].y = -90;; // khởi tạo cho nó trên cùng, không hiện ra 
    spike[i].velocity = gravity * deltaTime; // tốc độ rơi
    spike[i].width = spike_texture.width;
    spike[i].height = spike_texture.height;
}
    Ball balls[BALL_COUNT];
    int score = 0;
    srand(time(NULL));
    for (int i = 0; i < BALL_COUNT; i++) {
        balls[i].x = 25 + rand() % (window_width - 50); // Giới hạn để không bị ra ngoài màn hình
        balls[i].y = -30; 
        balls[i].speed = gravity * deltaTime; // Tốc độ rơi tự do
        balls[i].size = 15.0; // bán kính
    }

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
                    gameState = GAME;
                }
            } break;

            case GAME:
            {
                // Trong màn chơi, nếu nhấn phím Escape, quay lại MENU
                if (IsKeyPressed(KEY_ESCAPE)) 
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

        else if (gameState == GAME)
        {
            DrawTexture(rodungbong_texture, rodungbong.x, rodungbong.y, WHITE);
    
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
            balls[i].speed += gravity * deltaTime; // Cập nhật tốc độ theo thời gian
            balls[i].y += balls[i].speed * deltaTime / 2; // hàm g.t^2 trong vật lú

            // Nếu bóng chạm đáy, kiểm tra va chạm với bóng người chơi
            if (balls[i].y >= window_height) {
                // Reset quả bóng để bắt đầu lại từ trên
                balls[i].y = -30; // Bắt đầu lại từ trên
                balls[i].speed = gravity * deltaTime; // Tốc độ vật lú
            }

            // Kiểm tra va chạm với bóng người chơi
            if (balls[i].x + balls[i].size > rodungbong.x  &&
                balls[i].x - balls[i].size < rodungbong.x + rodungbong.width && 
                balls[i].y + balls[i].size > rodungbong.y  &&
                balls[i].y < rodungbong.y + rodungbong.height) {
                score++;
                // Reset quả bóng sau khi va chạm
                balls[i].x = 25 + rand() % (window_width - 50);
                balls[i].y = -30; // Bắt đầu lại từ trên
                balls[i].speed = gravity * deltaTime; 
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
            spike[i].velocity += gravity * deltaTime; // Cập nhật tốc độ theo thời gian
            spike[i].y += spike[i].velocity * deltaTime / 2;
            
            if (spike[i].y >= window_height) {
                spike[i].y = -90;; // reset vị trí sì pai
                spike[i].velocity = gravity * deltaTime;
            }
            // kiểm tra va chạm
            if (spike[i].x + spike[i].width > rodungbong.x &&
                spike[i].x - spike[i].width < rodungbong.x + rodungbong.width && 
                spike[i].y + spike[i].height > rodungbong.y &&
                spike[i].y < rodungbong.y + rodungbong.height) {
                attempt--; // trừ mạng
                spike[i].x = 30 + rand() % (window_width - 60) ; // 
                spike[i].y = -90;; // 
                spike[i].velocity = gravity * deltaTime;
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
        else if(gameState == ENDGAME)
        {
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