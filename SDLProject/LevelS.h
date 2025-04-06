#include "Scene.h"

class LevelS : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~LevelS();

    void initialise(int lives)override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
