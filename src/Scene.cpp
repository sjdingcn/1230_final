#include "Scene.h"
//#include "Camera.h"
#include "CS123ISceneParser.h"

#include "glm/gtx/transform.hpp"


Scene::Scene()
{
}

Scene::Scene(Scene &scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(scene.m_global);

    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.
    int n_pri = scene.m_primitives.size();
    for (int i=0; i<n_pri; i++)
    {
        addPrimitive(scene.m_primitives[i], scene.m_transforms[i]);
    }
    int n_lgt = scene.m_lights.size();
    for (int i=0; i<n_lgt; i++)
    {
        addLight(scene.m_lights[i]);
    }
}


Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::traverseTree(CS123SceneNode node, glm::mat4x4 prev_trans)
{
    glm::mat4x4 tf(1.0f);
    // parse transformation
    for (CS123SceneTransformation* trans:node.transformations)
    {
        TransformationType type = trans->type;
        if(type == TRANSFORMATION_ROTATE){
            tf = tf*glm::rotate(trans->angle, trans->rotate);
        }
        else if(type == TRANSFORMATION_SCALE){
            tf = tf*glm::scale(trans->scale);
        }
        else if(type == TRANSFORMATION_TRANSLATE){
            tf = tf*glm::translate(trans->translate);
        }
        else if(type == TRANSFORMATION_MATRIX){
            tf = tf*trans->matrix;
        }
    }
//    transformation = trs*rot*scl*transformation;
    // parse primitive
    for(CS123ScenePrimitive* prim : node.primitives)
    {
        CS123ScenePrimitive t_prim = *prim;
        addPrimitive(t_prim, prev_trans*tf);
    }
    // traverse children
    for(CS123SceneNode* child : node.children)
    {
        traverseTree(*child, prev_trans*tf);
    }
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()
    CS123SceneGlobalData tmp_global;
    parser->getGlobalData(tmp_global);
    sceneToFill->setGlobal(tmp_global);

    int light_num = parser->getNumLights();
    for(int i=0;i<light_num;i++)
    {
        CS123SceneLightData tmp_light;
        parser->getLightData(i, tmp_light);
        sceneToFill->addLight(tmp_light);
    }

    CS123SceneNode &root = *parser->getRootNode();
    sceneToFill->traverseTree(root, glm::mat4x4(1.0f));
    sceneToFill->indicate_file_cange();
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
    CS123ScenePrimitive prim = scenePrimitive;
    m_primitives.push_back(prim);
    glm::mat4x4 mat = matrix;
    m_transforms.push_back(mat);
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    CS123SceneLightData light = sceneLight;
    m_lights.push_back(light);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    CS123SceneGlobalData glo = global;
    m_global = glo;
}

