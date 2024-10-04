#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>


#define window_width 1920
#define window_height 1080
#define BALL_COUNT 5
#define HEART_COUNT 2

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
    int velocity;
} Heart;

void display(Ball balls[], int score, Heart heart[], Texture2D heart_texture) {
    for (int i = 0; i < BALL_COUNT; i++) {
        DrawCircle(balls[i].x, balls[i].y, balls[i].size, PURPLE); // Hiển thị các bóng
    }
    for (int i = 0; i < HEART_COUNT; i++) {
        DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE);
    }

    DrawText(TextFormat("Score: %d", score), 20, 20, 40, BLUE); // Hiển thị điểm số
}


int rodungbong_on_ground (Rodungbong rodungbong ){
    if( rodungbong.y >= window_height - rodungbong.height*2){
        return 1;
    }
    return 0;
}

int main(void)
{
    // Initialization
    InitWindow( window_width, window_height, "Catching Ball");

    SetTargetFPS( 100 ); // Set the target frames per second
    
    Image rodungbongImage = LoadImage("img/rodungbong.png");
    Texture2D rodungbong_texture = LoadTextureFromImage(rodungbongImage);

    Image heartImage = LoadImage("img/heart.png");
    Texture2D heart_texture = LoadTextureFromImage(heartImage);

    
    Rodungbong rodungbong ={ 
        .x = window_width/2,
        .y = 994, 
        .height = 43,
        .width = 97,
        .velocity = 7
    };
    Heart heart [HEART_COUNT];
    srand(time(NULL));
    for (int i = 0; i < HEART_COUNT; i++) {
    heart[i].x = 20 + rand() % (window_width - 40); // Random x position only once
    heart[i].y = 30; // Start from the top
    heart[i].velocity = 3 + rand() % 3; // Random falling speed
}
    Ball balls[BALL_COUNT];
    int score = 0;
    srand(time(NULL));
    for (int i = 0; i < BALL_COUNT; i++) {
        balls[i].x = 25 + rand() % (window_width - 50); // Giới hạn để không bị ra ngoài màn hình
        balls[i].y = 30; // Bắt đầu từ trên cùng
        balls[i].speed = sqrt(2*9.8*abs(balls[i].y))/5; // Tốc độ ngẫu nhiên
        balls[i].size = 30.0; // Đặt kích thước
    }
    // Main game loop
    while (!WindowShouldClose())
    {
    // Start drawing
    BeginDrawing();
    DrawTexture(rodungbong_texture, rodungbong.x, rodungbong.y, WHITE);
    ClearBackground(WHITE);
    // Cập nhật vị trí của bóng người chơi
    rodungbong.y += rodungbong.velocity;
    rodungbong.velocity += 1;

    // Kiểm tra bóng người chơi có chạm đất không
    if(rodungbong_on_ground( rodungbong )){
        rodungbong.velocity = 0;
        rodungbong.y = window_height - rodungbong.height*2;
    }

    // Điều khiển rổ bóng người chơi
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
            balls[i].y += balls[i].speed;

            // Nếu bóng chạm đáy, kiểm tra va chạm với bóng người chơi
            if (balls[i].y >= window_height) {
                // Reset quả bóng để bắt đầu lại từ trên
                balls[i].x = 25 + rand() % (window_width - 50);
                balls[i].y = 30; // Bắt đầu lại từ trên
                balls[i].speed = sqrt(2*9.8*abs(balls[i].y))/5; // Tốc độ ngẫu nhiên
            }

            // Kiểm tra va chạm với bóng người chơi
            if (balls[i].x + balls[i].size > rodungbong.x  &&
                balls[i].x < rodungbong.x + rodungbong.width &&
                balls[i].y + balls[i].size > rodungbong.y  &&
                balls[i].y < rodungbong.y + rodungbong.height) {
                score++;
                // Reset quả bóng sau khi va chạm
                balls[i].x = 25 + rand() % (window_width - 50);
                balls[i].y = 30; // Bắt đầu lại từ trên
                balls[i].speed = sqrt(2*9.8*abs(balls[i].y))/5; // Tốc độ ngẫu nhiên
            }
        }
        for (int i = 0; i < HEART_COUNT; i++) {
    // Move the heart downward by updating its y position
    heart[i].y += heart[i].velocity; 

    // Reset the heart if it goes beyond the screen (falls to the bottom)
    if (heart[i].y >= window_height) {
        heart[i].x = 20 + rand() % (window_width - 40); // Set a new random x position
        heart[i].y = 30; // Reset y to start from the top again
        heart[i].velocity = 5; // Randomize the falling speed again
    }
    DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE);}
        
    display(balls, score, heart, heart_texture); // Vẽ tất cả các bóng
    EndDrawing();
}


    // Close window and OpenGL context
    CloseWindow(); // Close window and exit the program

    return 0;
}
