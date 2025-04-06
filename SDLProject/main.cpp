/**
* Author: [Aayush Daftary]
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Scene.h"
#include "LevelS.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Utility.h"

// ––––– CONSTANTS ––––– //
const int WINDOW_WIDTH = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED = 0.18f,
            BG_BLUE = 0.33f,
            BG_GREEN = 0.03f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr int CD_QUAL_FREQ = 44100,  // compact disk (CD) quality frequency
          AUDIO_CHAN_AMT  = 2,
          AUDIO_BUFF_SIZE = 4096;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const float LEVELC_WIN_THRESHOLD = 22.0f;
Scene* g_previous_scene = nullptr;

// ––––– GLOBAL VARIABLES ––––– //
Scene* g_current_scene;
LevelS* g_levelS;
LevelA* g_levelA;
LevelB* g_levelB;
LevelC* g_levelC;

Scene* g_levels[4];
GLuint g_font_texture_id;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_won = false;
bool g_game_lost = false;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

int g_total_lives = 3;
Mix_Chunk* g_lose_sfx;
Mix_Chunk* g_win_sfx;
Mix_Chunk* g_jump_sfx;
Mix_Music* g_bgm = nullptr;
float g_camera_x = 5.0f;

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene* scene, int lives)
{
//    if (g_current_scene != g_levelS && scene == g_levelS) {
//            Mix_HaltMusic();
//    }
    g_current_scene = scene;
    g_current_scene->initialise(lives); // Pass lives to new scene
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Rise of da Ai",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);
    g_font_texture_id = Utility::load_texture("font.png");
    g_lose_sfx = Mix_LoadWAV("lose.wav");
    g_win_sfx = Mix_LoadWAV("Victory.mp3");
    g_jump_sfx = Mix_LoadWAV("jump.wav");
    
    g_bgm = Mix_LoadMUS("in_the_lobby.mp3");
        Mix_PlayMusic(g_bgm, -1);
        Mix_VolumeMusic(50.0f);

    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_levelS = new LevelS();

    g_levels[0] = g_levelS;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;

    switch_to_scene(g_levels[0], g_total_lives);
}

void process_input() {
    // Reset movement if no input
    if (g_current_scene != g_levelS && g_current_scene->get_state().player)
    {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;

                    case SDLK_SPACE:
                        if (g_current_scene != g_levelS &&
                            g_current_scene->get_state().player->m_collided_bottom) {
                            g_current_scene->get_state().player->m_is_jumping = true;
                            Mix_PlayChannel(-1, g_jump_sfx, 0);
                        }
                        break;

                    case SDLK_RETURN:
                        Mix_HaltChannel(-1);
                        Mix_HaltMusic();
                        if (g_current_scene == g_levelS) {
                            g_total_lives = 3;
                            g_game_won = false;
                            g_game_lost = false;
                            switch_to_scene(g_levels[1], g_total_lives);
                            if (!Mix_PlayingMusic())
                            {
                                Mix_PlayMusic(g_bgm, -1);
                            }
                        }
                        break;

                    default: break;
                }
                break;
        }
    }

    // Continuous key state checking
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (g_current_scene != g_levelS && g_current_scene->get_state().player && g_total_lives > 0) {
        if (key_state[SDL_SCANCODE_LEFT]) {
            g_current_scene->get_state().player->m_movement.x = -1.0f;
            g_current_scene->get_state().player->m_animation_indices =
                g_current_scene->get_state().player->m_walking[Entity::LEFT];
        }
        else if (key_state[SDL_SCANCODE_RIGHT]) {
            g_current_scene->get_state().player->m_movement.x = 1.0f;
            g_current_scene->get_state().player->m_animation_indices =
                g_current_scene->get_state().player->m_walking[Entity::RIGHT];
        }
    }
}

void update() {
    float ticks = (float)SDL_GetTicks()/MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks =ticks;

    delta_time += g_accumulator;
    if (delta_time < FIXED_TIMESTEP) {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;
    g_previous_scene = g_current_scene;
    if (g_current_scene != g_levelS) {
        g_total_lives = g_current_scene->lives;
        
        if (g_total_lives <= 0 && !g_game_lost) {
            Mix_PlayChannel(-1, g_lose_sfx, 0);
            g_game_lost = true;
            switch_to_scene(g_levelS, 3);
        }
    }

    // Scene transitions
    if (g_current_scene->get_state().next_scene_id >= 0)
    {
        switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id], g_total_lives);
    }
    if (g_current_scene == g_levelS && g_previous_scene == g_levelC)
    {
        if (g_levelC->get_state().player->get_position().x > LEVELC_WIN_THRESHOLD)
        {
            Mix_HaltMusic();
            Mix_HaltChannel(-1);
            Mix_PlayChannel(-1, g_win_sfx, 0);
            g_game_won = true;
            //switch_to_scene(g_levelS, 3);
        }
        else if (g_total_lives <= 0)
        {
            Mix_HaltMusic();
            Mix_HaltChannel(-1);
            Mix_PlayChannel(-1, g_lose_sfx, 0);
            g_game_lost = true;
            //switch_to_scene(g_levelS, 3);
        }
    }

    // Camera system
    g_view_matrix = glm::mat4(1.0f);
    if (g_current_scene != g_levelS && g_current_scene->get_state().player)
    {
        float x_pos = g_current_scene->get_state().player->get_position().x;
        x_pos = glm::max(x_pos, LEVEL1_LEFT_EDGE);
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-x_pos, 3.75f, 0.0f));
        //g_view_matrix = glm::translate(glm::mat4(1.0f),
                           //glm::vec3(-x_pos, -g_current_scene->get_state().player->get_position().y, 0.0f));

        g_camera_x = x_pos;
    }
    else
    {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5.0f, 3.75f, 0.0f));
        g_camera_x = 5.0f;
    }
}

void render() {
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);

    // UI Rendering
    if (g_current_scene == g_levelS)
    {
        if (!g_game_won && !g_game_lost)
        {
            //Utility::draw_text(&g_shader_program, g_font_texture_id, "Rise of the ai", 0.8f, 0.1f, glm::vec3(-3.0f, 1.0f, 0.0f));
            //Utility::draw_text(&g_shader_program, g_font_texture_id, "Press ENter", 0.5f, 0.1f, glm::vec3(-2.5f, -1.0f, 0.0f));
        }
        
        if (g_game_won) {
            Utility::draw_text(&g_shader_program, g_font_texture_id, "Winner!", 0.5f, 0.1f, glm::vec3(2.5f, -6.0f, 0.0f));
        }
        else if (g_game_lost) {
            Utility::draw_text(&g_shader_program, g_font_texture_id, "Game Over!", 0.5, 0.1f, glm::vec3(2.5f, -6.0f, 0.0f));
        }
    }
    else
    {
        std::string lives_msg = "LIVES:" + std::to_string(g_total_lives);
        float text_x = g_camera_x + 1.0f;
        float text_y = 3.75f - 1.0f;
        Utility::draw_text(&g_shader_program, g_font_texture_id, lives_msg, 0.5f, 0.05f, glm::vec3(text_x-0.5f, -1.0f, 0.0f));
    
//        Utility::draw_text(&g_shader_program, g_font_texture_id, "LIVES: " + std::to_string(g_total_lives),
//                         0.5f, 0.1f, glm::vec3(3.0f, 2.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    SDL_Quit();
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_levelS;
    Mix_FreeChunk(g_lose_sfx);
    Mix_FreeChunk(g_win_sfx);
    Mix_FreeChunk(g_jump_sfx);
    Mix_FreeMusic(g_bgm);
    SDL_Quit();
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[]) {
    initialise();
    while (g_game_is_running) {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}
