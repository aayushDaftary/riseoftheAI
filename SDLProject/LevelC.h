#include "Scene.h"

class LevelC : public Scene {
public:
    int ENEMY_COUNT = 2;

    ~LevelC();

    void initialise(int lives)override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
