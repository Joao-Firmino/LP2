#include "raylib.h"
#include <stdlib.h> // Para alocação dinâmica

int menu(void); // Menu do jogo
int game(void); // Roda o jogo
int instructions_screen(void); // Tela de intruções
int record_screen(void); // Tela de recorde
void game_over_screen(void); // Tela de game over
void success_screen(void); // Tela de sucesso
void save_record(int record); // Salva o recorde em um arquivo
int load_record(void); // Lê o recorde armazenado em um arquivo

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 600;
    int option;
    
    InitWindow(screenWidth, screenHeight, "Game - LP2");
    InitAudioDevice();
    
    SetTargetFPS(60);
    
    do
    {
        
        option = menu();
        if (option == 0)
        {
            if (!instructions_screen()) break;
            if (!game()) break;   
        }
        else if (option == 1) if (!record_screen()) break;   
        
    } while(option != 2);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}


// Cria o menu principal do jogo.
// Retorna 0, 1 ou 2. 
// 0 indica que o botão de iniciar o jogo foi pressionado.
// 1 indica que o botão de mostrar o recorde foi pressionado.
// 2 indica que o programa deve ser fechado.
int menu(void)
{
    Vector2 mouse_position  = { 0.0f, 0.0f };
    
    Image image;
    Texture2D menu_texture;
    Texture2D button_texture;
    
    // Para controle dos botões
    int button_state = 0;
    bool button_action = false;
    int which_button = -1;
    float rec_y[3];
    
    // Nome de cada botão
    char *button_names[] = {"Start", "Record", "Exit"};
    
    // Carrega a textura que vai ficar de fundo
    image = LoadImage("menu_texture.png");
    menu_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega a textura usada para fazer os botões
    image = LoadImage("button_texture.png");
    button_texture = LoadTextureFromImage(image);
    UnloadImage(image);    
    
    // Define a altura do retângulo que vai fazer o recorte da textura usada para fazer os botões
    float frame_height = (float)button_texture.height/3;
    
    // Define a área que cada botão vai ocupar
    Rectangle bounds[] = { 
                           { 380.0f, // x
                             220.0f, // y
                            (float)button_texture.width,
                             frame_height }, // Botão de start
                
                           { 380.0f, // x
                             320.0f, // y
                            (float)button_texture.width,
                             frame_height }, // Botão para ver recorde
               
                           { 380.0f, // x
                             420.0f, // y
                            (float)button_texture.width,
                             frame_height } // Botão para sair
                         };
                        
    float list[] = {69.0f, 56.0f, 83.0f}; // Auxilia na impressão dos nomes dos botões
    int i; // Usada em laços for
    
    
    while (!WindowShouldClose())
    {
        mouse_position = GetMousePosition();
        button_action = false;
        which_button = -1;
        
        if (CheckCollisionPointRec(mouse_position, bounds[0])) // Checa se o mouse está no primeiro botão
        {
            which_button = 0;
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) button_state = 2;
            else button_state = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) button_action = true;
        }
        else if (CheckCollisionPointRec(mouse_position, bounds[1])) // Checa se o mouse está no segundo botão
        {
            which_button = 1;
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) button_state = 2;
            else button_state = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) button_action = true;
        }
        else if (CheckCollisionPointRec(mouse_position, bounds[2])) // Checa se o mouse está no terceiro botão
        {
            which_button = 2;
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) button_state = 2;
            else button_state = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) button_action = true;
        }
        else button_state = 0;
        
        // Define valores necessários para o recorte da textura usada para fazer os botões
        for (i = 0; i < 3; ++i) rec_y[i] = 0.0f;
        if (which_button != -1) rec_y[which_button] = button_state * frame_height;
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            DrawTextureV(menu_texture, (Vector2){0, 0}, WHITE);
            DrawText("WELCOME!", 230, 100, 100, WHITE);
            for (int i = 0; i<3; ++i) // Desenha os botões
            {
                // Desenha um dos botões       
                DrawTextureRec( button_texture, 
                               (Rectangle){0.0f, rec_y[i], (float)button_texture.width, frame_height}, 
                               (Vector2){ bounds[i].x, bounds[i].y }, 
                                WHITE );
                                
                // Desenha o nome do botão
                DrawRectangleRounded((Rectangle){bounds[i].x + 45.0f, bounds[i].y + 38.0f, 130.0f, 30.0f}, 1.0f, 200.0f, BLACK); 
                DrawText(button_names[i], bounds[i].x + list[i], bounds[i].y + 40.0f, 30, WHITE);
            }
            
        EndDrawing();
        
        if (button_action) // Se algum botão foi acionado
        {
            UnloadTexture(button_texture);
            UnloadTexture(menu_texture);
            return which_button;
        }
        
    } 
   
    UnloadTexture(button_texture);
    UnloadTexture(menu_texture);
    return 2;
}


// Roda o jogo.
// Retorna 0 ou 1.
// 1 indica uma finalização normal.
// 0 indica que o programa deve ser fechado.
int game(void)
{   
    Image image; // Usada para carregar as texturas
    int i; // Usada em laços for
    
    // Para armazenar as texturas
    Texture2D blocks_texture;
    Texture2D gun_textures[12]; 
    Texture2D monster_texture;
    
    // Paths dos arquivos com as texturas da arma
    char *texture_paths[] = { "gun.png",
                              "gun2.png",
                              "gun3.png",
                              "gun4.png",
                              "gun5.png",
                              "gun6.png",
                              "gun7.png",
                              "gun8.png",
                              "gun9.png",
                              "gun10.png",
                              "gun11.png",
                              "gun12.png" };
                              
    // Paths dos arquivos com os sons que o BOSS faz durante o jogo
    char *sound_paths[] = { "monster_sound_1.mp3",
                            "monster_sound_2.mp3",
                            "monster_sound_3.mp3",
                            "monster_sound_4.mp3",
                            "monster_sound_5.mp3" };

    // Para a música de fundo e os sons usados durante o jogo
    Sound gun_sound;
    Sound thunder_sound;
    Music doom_music;
    Sound monster_sounds[5];
    
    // Para as animações da arma
    int sequence[] = {0, 1, 2, 3, 4, 5, 4, 3, 2, -1}; // Sequência de atirar
    int sequence2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 5, 4, 3, 2, -1}; // Sequência de atirar e recarregar
    float gun_y[] = {439.0f, 426.0f, 389.0f, 354.0f, 327.0f, 318.0f, 356.0f, 405.0f, 428.0f, 430.0f, 408.0f, 357.0f};
    float gun_temp[] = {0.02f, 0.08f, 0.02f, 0.02f, 0.02f, 0.08f, 0.08f, 0.08f, 0.2f, 0.2f, 0.08f, 0.08f};
    int *pt = sequence;
    
    // Câmera 3d
    Camera camera = { 0 };
        
    // Para o cálculo da trajetória do disparo
    Ray bullet_trajectory;
    RayCollision impact_information;
    
    // Para controlar ações periódicas
    double time = GetTime();
    double time2 = GetTime();
    double time3 = GetTime();
    double time4 = GetTime();
    double time5 = GetTime();
    
    // Posição do billboard com a textura do BOSS
    Vector3 bill_position = { GetRandomValue(-40, 40),
                              GetRandomValue(4, 16),
                              GetRandomValue(-40, 40) };
                                       
    // Posição dos pilares que apareçem no ambiente
    Vector3 positions[] = { (Vector3){24.0f, 10.0f, 18.0f},
                            (Vector3){24.0f, 10.0f, -18.0f},
                            (Vector3){-24.0f, 10.0f, -18.0f},
                            (Vector3){-24.0f, 10.0f, 18.0f} };
    
    // Define a câmera 3d
    camera.position = (Vector3){ 0.0f, 5.0f, 0.0f };
    camera.target = (Vector3){ 10000.0f, 5.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // Carrega as texturas da arma
    for (i = 0; i < 12; ++i){
        image = LoadImage(texture_paths[i]);
        gun_textures[i] = LoadTextureFromImage(image);
        UnloadImage(image);
    }
    
    // Carrega a textura usada no ambiente
    image = LoadImage("blocks_texture.png");
    blocks_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega a textura do BOSS
    image = LoadImage("monster_texture.png");
    monster_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega a música de fundo e os sons emitidos durante o jogo
    doom_music = LoadMusicStream("doom_music.mp3"); // Música de fundo
    SetMusicVolume(doom_music, 0.3f);
    gun_sound = LoadSound("gunshot_sound.mp3"); // Som de disparo
    SetSoundVolume(gun_sound, 0.7f); 
    thunder_sound = LoadSound("thunder_sound.mp3");  // Som de trovão
    SetSoundVolume(thunder_sound, 1.0f);
    
    // Carrega os sons que o BOSS emite durante o jogo
    for (i = 0; i<5; ++i) {
        monster_sounds[i] = LoadSound(sound_paths[i]); 
        SetSoundVolume(monster_sounds[i], 1.0f);
    }
    
    int monster_life = 200; // Vida do BOSS
    int record; // Recebe o recorde
    int time_taken; // Armazena em quanto tempo o BOSS foi derrotado
    
    BoundingBox monster_bbox; // "Corpo" do BOSS
    int munition = 2; // Para o controle do recarregamento da arma
        
    // Para testes
    //bullet_trajectory = (Ray){camera.position, camera.target};
    
    PlayMusicStream(doom_music);
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    while (!WindowShouldClose())
    {
        // Efeito periódico de trovão
        if (GetTime() - time2 >= 10.0f){
            PlaySound(thunder_sound);
            time2 = GetTime(); 
        }
        
        // Som periódico emitido pelo BOSS
        if (GetTime() - time4 >= 10.0f){
            PlaySound(monster_sounds[GetRandomValue(0, 4)]);
            time4 = GetTime(); 
        }
        
        UpdateCamera(&camera);
        UpdateMusicStream(doom_music);
        
        // Verifica se a arma foi disparada
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && *pt == 0) { 
            if (munition == 1) {
                pt = sequence2 + 1;
                munition = 2;
            }
            else {
                pt = sequence + 1; 
                --munition;
            }
            PlaySound(gun_sound);
            // Trajetória do disparo                
            bullet_trajectory = (Ray){camera.position, camera.target};
            // Cálculo do "corpo" do BOSS
            monster_bbox = (BoundingBox) { (Vector3){ bill_position.x - 2.0f, bill_position.y - 2.0f, bill_position.z - 2.0f },
                                           (Vector3){ bill_position.x + 2.0f, bill_position.y + 3.0f, bill_position.z + 2.0f } };
            // Verifica se o disparo acertou o BOSS
            impact_information = GetRayCollisionBox(bullet_trajectory, monster_bbox);  
            if (impact_information.hit) --monster_life;                   
        }  
                
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            // Desenha o ambiente 3d
            BeginMode3D(camera);

                DrawCubeTexture(blocks_texture, (Vector3){ 0.0f, 0.0f, 0.0f }, 100.0f, 1.0f, 100.0f, WHITE); // Chão
                DrawCubeTexture(blocks_texture, (Vector3){ 0.0f, 20.0f, 0.0f }, 100.0f, 1.0f, 100.0f, WHITE); // Teto
                DrawCubeTexture(blocks_texture, (Vector3){ 50.0f, 10.0f, 0.0f }, 1.0f, 20.0f, 100.0f, WHITE); // Parede
                DrawCubeTexture(blocks_texture, (Vector3){ -50.0f, 10.0f, 0.0f }, 1.0f, 20.0f, 100.0f, WHITE); // Parede    
                DrawCubeTexture(blocks_texture, (Vector3){ 0.0f, 10.0f, 50.0f }, 100.0f, 20.0f, 1.0f, WHITE); // Parede    
                DrawCubeTexture(blocks_texture, (Vector3){ 0.0f, 10.0f, -50.0f }, 100.0f, 20.0f, 1.0f, WHITE); // Parede    
                
                // Desenha os pilares que aparecem no ambiente
                for (i = 0; i < 4; ++i) DrawCubeTexture(blocks_texture, positions[i], 5.0f, 20.0f, 5.0f, WHITE);
                     
                // Para testes
                // DrawRay(bullet_trajectory, BLACK); // Trajetória do projétil
                // monster_bbox = (BoundingBox) { (Vector3){ bill_position.x - 2.0f, bill_position.y - 2.0f, bill_position.z - 2.0f },
                                               // (Vector3){ bill_position.x + 2.0f, bill_position.y + 3.0f, bill_position.z + 2.0f } };
                // DrawBoundingBox(monster_bbox, GREEN); // Bounding box do monstro
                // DrawSphere(bill_position, 0.2f, GREEN); // Esfera na posição do billboard
                // DrawSphere((Vector3){ bill_position.x - 2.0f, bill_position.y - 2.0f, bill_position.z - 2.0f }, 0.2f, GREEN);
                // DrawSphere((Vector3){ bill_position.x + 2.0f, bill_position.y + 3.0f, bill_position.z + 2.0f }, 0.2f, GREEN);
                     
                // Muda a posição do billboard (BOSS) periodicamente
                if (GetTime() - time >= 2.0f){
                    bill_position = (Vector3){GetRandomValue(-40, 40), GetRandomValue(4, 16), GetRandomValue(-40, 40)}; 
                    time = GetTime(); 
                }
                
                // Desenha o billboard (BOSS)
                DrawBillboard(camera, monster_texture, bill_position, 8.0f, WHITE);
                 
            EndMode3D();
                
            // Desenha a mira da arma
            DrawLineEx((Vector2){500.0f, 290.0f}, (Vector2){500.0f, 310.0f}, 1.0f, RED); // Linha vertical   
            DrawLineEx((Vector2){490.0f, 300.0f}, (Vector2){510.0f, 300.0f}, 1.0f, RED); // Linha horizontal
            
            // Animação da arma
            if (*pt == 0) {
                DrawTextureV(gun_textures[*pt], (Vector2){600.0f, gun_y[*pt]}, WHITE);
                time5 = GetTime();
            }
            else {
                DrawTextureV(gun_textures[*pt], (Vector2){600.0f, gun_y[*pt]}, WHITE);
                if (*(pt + 1) == -1) pt = sequence;
                else if (GetTime() - time5 >= gun_temp[*pt]) { 
                    ++pt;
                    time5 = GetTime();
                }
            }
            
            // Desenha a mensagem com os comandos do jogo
            DrawRectangle( 10, 10, 220, 70, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 220, 70, BLUE);
            DrawText("- Atire com o botão esquerdo do mouse", 20, 20, 10, BLACK);
            DrawText("- Ande com: W, A, S, D", 20, 40, 10, BLACK);
            DrawText("- Mova o mouse para olhar ao redor", 20, 60, 10, BLACK);
            
            // Desenha as informações da camera 3d
            DrawRectangle(890, 10, 100, 195, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(890, 10, 100, 195, BLUE);
            DrawText( TextFormat( "POSITION: \nx: %f\ny: %f\nz: %f", 
                                  camera.position.x, 
                                  camera.position.y, 
                                  camera.position.z ), 900, 15, 10, BLACK );
            DrawText( TextFormat( "TARGET: \nx: %f\ny: %f\nz: %f", 
                                  camera.target.x, 
                                  camera.target.y, 
                                  camera.target.z ), 900, 80, 10, BLACK );
            DrawText( TextFormat( "UP: \nx: %f\ny: %f\nz: %f", 
                                  camera.up.x, 
                                  camera.up.y, 
                                  camera.up.z ), 900, 145, 10, BLACK );
                                  
            time_taken = (int) GetTime() - time3;
                                  
            // Verifica se o BOOS foi derrotado
            if (monster_life == 0){
                SetCameraMode(camera, CAMERA_FREE);
                record = load_record();
                if (time_taken < record || record == -1) save_record(time_taken);
                success_screen(); // Tela de sucesso
                return 1;
            }
                                  
            // Desenha a vida do BOSS
            DrawRectangleRounded((Rectangle){10.0f, 100.0f, 70.0f, 25.0f}, 1.0f, 200.0f, Fade(RED, 0.9f));
            DrawText("BOSS", 18, 105, 20, BLACK );
            DrawRectangleRounded((Rectangle){10.0f, 130.0f, 11.0f, 400.0f}, 1.0f, 200.0f, WHITE);
            DrawRectangleRounded( (Rectangle){12.6f, 140.0f, 5.0f, (380.0f / 200.0f) * monster_life}, 
                                   1.0f, 200.0f, RED); 
            
            // Verifica se o tempo acabou
            if (300 - time_taken <= 0){ 
                SetCameraMode(camera, CAMERA_FREE);
                game_over_screen();
                return 1;
            }
            
            // Desenha o tempo restante
            DrawRectangleRounded((Rectangle){300.0f, 10.0f, 100.0f, 50.0f}, 1.0f, 200.0f, Fade(BLACK, 0.5f));
            DrawText( TextFormat("%03i", 300 - time_taken), 335, 26, 20, WHITE);
                    
        EndDrawing();
        
    }
    
    // Unloading das texturas
    for (i = 0; i<12; ++i) UnloadTexture(gun_textures[i]);
    UnloadTexture(blocks_texture);
    UnloadTexture(monster_texture);
    
    // Unloading da música e dos sons
    UnloadMusicStream(doom_music);
    UnloadSound(gun_sound);
    UnloadSound(thunder_sound);
    
    // Unloading dos sons do monstro
    for (i = 0; i<5; ++i) UnloadSound(monster_sounds[i]);
    
    return 0;
}


// Cria a tela com as intruções para o jogo.
// Retorna 0 ou 1.
// 1 indica uma finalização normal.
// 0 indica que o programa deve ser fechado.
int instructions_screen(void)
{
    Vector2 mouse_position  = { 0.0f, 0.0f };
    
    Image image;
    Texture2D menu_texture;
    Texture2D button_texture;
    
    // Para controle do botão
    int button_state = 0;
    bool button_action = false;
    
    // Carrega a textura usada como plano de fundo
    image = LoadImage("menu_texture.png");
    menu_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega a textura usada para fazer o botão
    image = LoadImage("button_texture.png");
    button_texture = LoadTextureFromImage(image);
    UnloadImage(image);    
    
    // Define a altura do retângulo que vai fazer o recorte da textura usada para fazer o botão
    float frame_height = (float)button_texture.height/3;
    
    // Define a área que o botão vai ocupar
    Rectangle bounds = {380.0f, 420.0f, (float)button_texture.width, frame_height};
    
    while (!WindowShouldClose())
    {
        mouse_position = GetMousePosition();
        button_action = false;
        
        if (CheckCollisionPointRec(mouse_position, bounds)) // Checa se o mouse entá sobre o botão
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) button_state = 2;
            else button_state = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) button_action = true;
        }
        else button_state = 0;
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            DrawTextureV(menu_texture, (Vector2){0, 0}, WHITE);
            DrawText("Tente derrotar o BOSS dentro do tempo", 170, 200, 30, WHITE);
            // Desenha o botão       
            DrawTextureRec( button_texture, 
                           (Rectangle){0.0f, button_state * frame_height, (float)button_texture.width, frame_height}, 
                           (Vector2){ bounds.x, bounds.y }, 
                            WHITE );
            // Desenha o nome do botão
            DrawRectangleRounded((Rectangle){bounds.x + 45.0f, bounds.y + 38.0f, 130.0f, 30.0f}, 1.0f, 200.0f, BLACK);
            DrawText("Continue", bounds.x + 59.0f, bounds.y + 42.0f, 25, WHITE);
            
        EndDrawing();
        
        if (button_action) // Se o botão foi acionado
        {
            UnloadTexture(button_texture);
            UnloadTexture(menu_texture);
            return 1;
        }
        
    } 
    
    UnloadTexture(button_texture);
    UnloadTexture(menu_texture);
    return 0;
}


// Cria a tela de game over.
// Retorna 0 ou 1.
// 1 indica uma finalização normal.
// 0 indica que o programa deve ser fechado.
void game_over_screen(void)
{
    Image image;
    Texture2D menu_texture;
    
    // Carrega a textura usada como plano de fundo
    image = LoadImage("menu_texture.png");
    menu_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega o som de GAME OVER
    Sound game_over_sound = LoadSound("game_over_sound.mp3");
    SetSoundVolume(game_over_sound, 1.0f);
    
    double time = GetTime();
    
    PlaySound(game_over_sound);
    while ( (GetTime() - time) < 3.0)
    {
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            DrawTextureV(menu_texture, (Vector2){0, 0}, WHITE); // Desenha o plano de fundo
            DrawText("GAME OVER", 400, 200, 30, WHITE);
            
        EndDrawing();           
    } 
    
    UnloadTexture(menu_texture);
    UnloadSound(game_over_sound);
}



// Cria a tela mostrada quando o BOSS é derrotado dentro do tempo.
// Retorna 0 ou 1.
// 1 indica uma finalização normal.
// 0 indica que o programa deve ser fechado.
void success_screen(void)
{
    Image image;
    Texture2D menu_texture;
    
    // Carrega a textura usada como plano de fundo
    image = LoadImage("menu_texture.png");
    menu_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega o som de sucesso
    Sound success_sound = LoadSound("success_sound.mp3");
    SetSoundVolume(success_sound, 1.0f);
    
    double time = GetTime();
    
    PlaySound(success_sound);
    while ( (GetTime() - time) < 5.0)
    {
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            DrawTextureV(menu_texture, (Vector2){0, 0}, WHITE); // Desenha o plano de fundo
            DrawText("CONGRATULATIONS!!!", 350, 200, 30, WHITE);
            
        EndDrawing();           
    } 
    
    UnloadTexture(menu_texture);
    UnloadSound(success_sound); 
}


// Cria a tela que mostra o recorde.
// Retorna 0 ou 1.
// 1 indica uma finalização normal.
// 0 indica que o programa deve ser fechado.
int record_screen(void)
{
    Vector2 mouse_position  = { 0.0f, 0.0f };
    
    Image image;
    Texture2D menu_texture;
    Texture2D button_texture;
    
    // Para controle do botão
    int button_state = 0;
    bool button_action = false;
    
    // Carrega a textura usada como plano de fundo
    image = LoadImage("menu_texture.png");
    menu_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    
    // Carrega a textura usada para fazer o botão
    image = LoadImage("button_texture.png");
    button_texture = LoadTextureFromImage(image);
    UnloadImage(image);    
    
    // Define a altura do retângulo que vai fazer o recorte da textura usada para fazer o botão
    float frame_height = (float)button_texture.height/3;
    
    // Define a área que o botão vai ocupar
    Rectangle bounds = {380.0f, 420.0f, (float)button_texture.width, frame_height};
    
    int record = load_record(); // Recebe o último record
    
    while (!WindowShouldClose())
    {
        mouse_position = GetMousePosition();
        button_action = false;
        
        if (CheckCollisionPointRec(mouse_position, bounds)) // Checa se o mouse está sobre o botão
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) button_state = 2;
            else button_state = 1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) button_action = true;
        }
        else button_state = 0;
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            DrawTextureV(menu_texture, (Vector2){0, 0}, WHITE);
            
            if (record == -1) DrawText("NENHUM RECORDE FOI REGISTRADO AINDA", 170, 200, 30, WHITE);
            else DrawText(TextFormat("RECORDE: BOSS derrotado em %i segundos", record), 170, 200, 30, WHITE);
            
            // Desenha o botão        
            DrawTextureRec( button_texture, 
                           (Rectangle){0.0f, button_state * frame_height, (float)button_texture.width, frame_height}, 
                           (Vector2){ bounds.x, bounds.y }, 
                            WHITE );
            // Desenha o nome do botão
            DrawRectangleRounded((Rectangle){bounds.x + 45.0f, bounds.y + 38.0f, 130.0f, 30.0f}, 1.0f, 200.0f, BLACK);            
            DrawText("Exit", bounds.x + 90.0f, bounds.y + 42.0f, 25, WHITE);
            
        EndDrawing();
        
        if (button_action) // Se o botão for acionado
        {
            UnloadTexture(button_texture);
            UnloadTexture(menu_texture);
            return 1;
        }
        
    } 
    
    UnloadTexture(button_texture);
    UnloadTexture(menu_texture);
    return 0;
}


// Salva um recorde em um arquivo.
// Recebe o recorde a ser salvo.
void save_record(int record)
{
    unsigned int data_size = 0;
    unsigned char *file_data = LoadFileData("record_file.txt", &data_size);

    if (file_data != NULL && data_size == sizeof(int))
    {
        *( (int *)file_data ) = record;
        SaveFileData("record_file.txt", file_data, sizeof(int));
        UnloadFileData(file_data);
    }
    else
    {
        file_data = (unsigned char *)RL_MALLOC(sizeof(int));
        if (file_data){
            *( (int *)file_data ) = record;
            SaveFileData("record_file.txt", file_data, sizeof(int));
            UnloadFileData(file_data);
        }
    }
}


// Ler um recorde salvo em um arquivo.
// Retorna o recorde lido ou o valor -1 caso o recorde não exista.
int load_record(void)
{
    int record = -1;
    unsigned int data_size = 0;
    unsigned char *file_data = LoadFileData("record_file.txt", &data_size);

    if (file_data != NULL && data_size == sizeof(int))
    {
        record = *( (int *)file_data );
        UnloadFileData(file_data);
    }
    return record;
}
