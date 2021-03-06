#pragma once

#include "WindowManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include "AverageValue.h"
#include "Camera.h"

class Scene;

class LearningWindowManager : public WindowManager
{
public:
	LearningWindowManager(const std::string& configFile);
	static void open();
	void addScene( Scene* scene );

protected:
	virtual void windowWillRun();
	virtual void windowWillClose();
	virtual void drawGLScene();
	virtual void drawSFMLScene(sf::RenderWindow& window);
	virtual void updateScene( float dt );

	void eventReshape( int width, int height );
	void eventKeyboardDown( sf::Keyboard::Key keyPressed );
	void eventKeyboardUp( sf::Keyboard::Key keyPressed );
	void eventMouseMotion(sf::Event::MouseMoveEvent mEvent);

	void handleCameraMove(sf::Keyboard::Key key, bool pressed);

private:	
	std::vector<Scene*> scenes;
	Camera camera;
	AverageValue averageFrameTime;
};