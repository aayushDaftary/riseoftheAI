#include "Scene.h"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 2;

    ~LevelB();

    void initialise(int lives) override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
