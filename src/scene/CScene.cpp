#include "CScene.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::addObject(std::shared_ptr<CSceneObject> object) { m_objects.push_back(object); }

const std::vector<std::shared_ptr<CSceneObject>>& CScene::getObjects() { return m_objects; }