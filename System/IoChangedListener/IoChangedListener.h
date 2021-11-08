#pragma once
#include "Scene.h"

class IoChangedListener		//���z�N���X
{
public:
	virtual void OnSceneChanged(const Scenes scene, const bool stackClear = false) = 0;
	virtual void SceneStackPop() = 0;
};
