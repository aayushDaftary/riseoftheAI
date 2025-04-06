/**
* Author: [Aayush Daftary]
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelS.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8
constexpr int CD_QUAL_FREQ    = 44100,  // compact disk (CD) quality frequency
          AUDIO_CHAN_AMT  = 2,
          AUDIO_BUFF_SIZE = 4096;


unsigned int LEVELS_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelS::~LevelS()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelS::initialise(int lives)
{
    this->lives = lives;
    m_game_state.next_scene_id = -1;


//    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
//    Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);

//    m_game_state.bgm = Mix_LoadMUS("in_the_lobby.mp3");
//    Mix_PlayMusic(m_game_state.bgm, -1);
//    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    //m_game_state.jump_sfx = Mix_LoadWAV("jump.wav");
}

void LevelS::update(float delta_time)
{
    //m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
}

void LevelS::render(ShaderProgram* program)
{
    //m_game_state.map->render(program);
    //m_game_state.player->render(program);

    GLuint font_texture = Utility::load_texture("font.png");
    Utility::draw_text(program, font_texture, "Samurai Jump", 1.0f, -0.45f, glm::vec3(1.5f, -2.0f, 0.0f));
    Utility::draw_text(program, font_texture, "Press Enter to Play", 0.8f, -0.45f, glm::vec3(1.5f, -4.0f, 0.0f));
}
