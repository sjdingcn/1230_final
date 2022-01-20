#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"

class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);
    void indicate_file_cange(){file_change_flag=true;}

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    std::vector<CS123ScenePrimitive> m_primitives;
    std::vector<glm::mat4x4> m_transforms;
    std::vector<CS123SceneLightData> m_lights;
    CS123SceneGlobalData m_global;

    void traverseTree(CS123SceneNode node, glm::mat4x4 prev_trans);
    bool file_change_flag = false;

};

#endif // SCENE_H
