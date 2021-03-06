#pragma once
#include "Scene.h"

class Cube;
class LineGrid;
class MarchingCubesShaded;

class LyingShapesScene :
	public Scene
{
public:
	LyingShapesScene();
	~LyingShapesScene();

	void eventReshape(int width, int height);
	void draw(const Camera& camera);

private:
	std::vector<Cube*> cubes;
	LineGrid* grid;
	LineGrid* coords;
	MarchingCubesShaded* mCubes;
};

