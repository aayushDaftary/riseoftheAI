/**
* Author: [Aayush Daftary]
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 22
#define LEVEL_HEIGHT 8

unsigned int LEVELB_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 0, 0, 1, 1
};

LevelB::~LevelB()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise(int lives)
{
    this->lives = lives;
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("world_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 16, 16);
    /**
     George's Stuff
     */
     // Existing
    m_game_state.player = new Entity();
    m_game_state.player->set_entity_type(PLAYER);
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_game_state.player->set_movement(glm::vec3(0.0f));
    m_game_state.player->m_speed = 2.5f;
    m_game_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.player->m_run_texture_id = Utility::load_texture("RUN.png");
    m_game_state.player->m_idle_texture_id = Utility::load_texture("IDLE.png");

    // Walking
    //m_game_state.player->m_walking[m_game_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    //m_game_state.player->m_walking[m_game_state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    //m_game_state.player->m_walking[m_game_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    //m_game_state.player->m_walking[m_game_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    m_game_state.player->m_animation_indices = m_game_state.player->m_walking[m_game_state.player->RIGHT];  // start George looking left
    m_game_state.player->m_animation_frames = 16;
    m_game_state.player->m_animation_index = 0;
    m_game_state.player->m_animation_time = 0.0f;
    m_game_state.player->m_animation_cols = 16;
    m_game_state.player->m_animation_rows = 1;
    m_game_state.player->set_height(1.0f);
    m_game_state.player->set_width(1.0f);
    m_game_state.player->m_jumping_power = 7.0f;

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("monster.png");

    m_game_state.enemies = new Entity[ENEMY_COUNT];
    m_game_state.enemies[0].set_entity_type(ENEMY);
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_ai_state(IDLE);
    m_game_state.enemies[0].m_texture_id = enemy_texture_id;
    m_game_state.enemies[0].set_position(glm::vec3(13.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].m_speed = 1.0f;
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_game_state.enemies[1].set_entity_type(ENEMY);
    m_game_state.enemies[1].set_ai_type(GUARD);
    m_game_state.enemies[1].set_ai_state(IDLE);
    m_game_state.enemies[1].m_texture_id = enemy_texture_id;
    m_game_state.enemies[1].set_position(glm::vec3(21.0f, 0.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].m_speed = 2.0f;
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

//    m_game_state.bgm = Mix_LoadMUS("in_the_lobby.mp3");
//    Mix_PlayMusic(m_game_state.bgm, -1);
//    Mix_VolumeMusic(50.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("jump.wav");
}

void LevelB::update(float delta_time)
{
    {
        m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.player, 1, m_game_state.map);
        }

        if (m_game_state.player->get_position().y < -10.0f || m_game_state.player->num_lives == 0) {
            m_game_state.next_scene_id = 2;
            lives--;
        }
        if (m_game_state.player->flag == true) {
            m_game_state.next_scene_id = 2;
        }

        if (m_game_state.player->get_position().x >= LEVEL_WIDTH - 1.0f) {
            m_game_state.next_scene_id = 3;
        }
    }
}

void LevelB::render(ShaderProgram* program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(program);
    }

    //GLuint font_texture = Utility::load_texture("font.png");
}
