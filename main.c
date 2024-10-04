#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>


#define window_width 1600
#define window_height 900
#define BALL_COUNT 15
#define HEART_COUNT 1

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

void display(Ball balls[], int score,int attempt, Heart heart[], Texture2D heart_texture) {
    for (int i = 0; i < BALL_COUNT; i++) {
        DrawCircle(balls[i].x, balls[i].y, balls[i].size, BLUE); 
    }
    for (int i = 0; i < HEART_COUNT; i++) {
        if( heart[i].y >= 0 ){ // chỉ vẽ trái tim nếu nó có trên màn hinh
        DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE);
    }
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
    float deltaTime = 0.1f ; // Thời gian trôi qua giữa các khung hình, chương trình đáng ra phải dùng 0.01 nhưng nếu vậy thì nó sẽ mất quá nhiều thời gian để bắt đầu rơi :D
    float gravity = 9.81f; // gia tốc
    InitWindow( window_width, window_height, "Catching Ball");
    SetTargetFPS( 100 ); 
    
    Image rodungbongImage = LoadImage("img/rodungbong.png");
    Texture2D rodungbong_texture = LoadTextureFromImage(rodungbongImage);

    Image heartImage = LoadImage("img/heart.png");
    Texture2D heart_texture = LoadTextureFromImage(heartImage);
    
    Image iconImage = LoadImage("img/icon.png");
    Texture2D icon_texture = LoadTextureFromImage(iconImage);

    
    
    Rodungbong rodungbong ={ 
        .x = window_width/2,
        .y = 994, 
        .height = 43,
        .width = 97,
        .velocity = 7
    };
    Heart heart [HEART_COUNT];
    int attempt = 0;
    for (int i = 0; i < HEART_COUNT; i++) {
    heart[i].y = -30; // khởi tạo cho nó trên cùng, không hiện ra 
    heart[i].velocity = 3.0; // tốc độ rơi
    heart[i].width = heart_texture.width;
    heart[i].height = heart_texture.height;
}
    Ball balls[BALL_COUNT];
    int score = 0;
    srand(time(NULL));
    for (int i = 0; i < BALL_COUNT; i++) {
        balls[i].x = 25 + rand() % (window_width - 50); // Giới hạn để không bị ra ngoài màn hình
        balls[i].y = 30; 
        balls[i].speed = gravity * deltaTime; // Tốc độ rơi tự do
        balls[i].size = 15.0; // bán kính
    }
    // GAME BẮT ĐẦU
    while (!WindowShouldClose())
    {
        SetWindowIcon(iconImage ); 
    BeginDrawing();
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
            balls[i].y += balls[i].speed * deltaTime / 2; // hàm g.t^2

            // Nếu bóng chạm đáy, kiểm tra va chạm với bóng người chơi
            if (balls[i].y >= window_height) {
                // Reset quả bóng để bắt đầu lại từ trên
                balls[i].y = 30; // Bắt đầu lại từ trên
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
                balls[i].y = 30; // Bắt đầu lại từ trên
                balls[i].speed = gravity * deltaTime; 
            }
        }
        if(score >= 10 && score % 10 == 0 ){
            for (int i = 0; i < HEART_COUNT; i++) { // tạo trái tim
            heart[i].x = 888 ; // Random x position
            heart[i].y = 30; // Start from the top
            heart[i].velocity = 3.0; // Falling speed
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
            }
            if (heart[i].y >= window_height) {
                heart[i].y = -30; // reset vị trí trái tim
            }
            DrawTexture(heart_texture, heart[i].x, heart[i].y, WHITE); // vẽ tim
        }
        }
    if(attempt == 10){ // tắt chương trình khi số lượng máu bằng 10
        return 0;
    }
    ClearBackground(WHITE);
    display(balls, score, attempt, heart, heart_texture); // Vẽ bóng và tim
    EndDrawing();
    }
    
    //chương trình auto nhận Esc là exit
    CloseWindow(); // Close window and exit the program
    
    return 0;
}