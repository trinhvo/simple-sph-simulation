#define GLM_FORCE_RADIANS
#include "Camera.h"
#include "Utility.h"
#include <GL\glew.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>

Camera::Camera( glm::vec2 windowSize, ProjectionType type, glm::vec2 zLimits) :
	windowSize( windowSize ), projectionType( type ), projectionZLimit( zLimits ),
	position({ 15.0f, 10.0f, 15.0f }), forward({ -0.577f, -0.577f, -0.577f }), up({ 0.0f, 1.0f, 0.0f }), axis(glm::cross(forward, up)),
	resizePolicy(ResizePolicy::DynamicRatio), viewportPos(0.0f, 0.0f), viewportSize( windowSize ), 
	viewportRatio( windowSize.x/windowSize.y )
{
	setupView();
	refreshProjection();
	refreshViewProjection();
}


Camera::~Camera(void)
{
}

// VIEW
void Camera::setupView()
{
	view = glm::lookAt(position, position + forward, -glm::cross(forward,axis));
	refreshViewProjection();
}

void Camera::applyOffset(glm::vec3 offset)
{
	position += offset;
	setupView();
}

void Camera::setMoveForward(bool movement) {
	moveForward = movement;
}

void Camera::setMoveBackward(bool movement) {
	moveBackward = movement;
}

void Camera::setMoveLeft(bool movement) {
	moveLeft = movement;
}

void Camera::setMoveRight(bool movement) {
	moveRight = movement;
}

void Camera::rotate(int idx, int idy)
{
	float dx = angleToRadians( -100.f * idx / viewportSize.x );
	float dy = angleToRadians( -100.f * idy / viewportSize.y );

	auto pitch = glm::angleAxis(dy, axis);
	auto heading = glm::angleAxis(dx, up);

	glm::quat temp = glm::cross(pitch, heading);
	temp = glm::normalize(temp);
	//update the direction from the quaternion
	forward = glm::rotate(temp, forward);
	axis = glm::rotate(heading, axis);
	// if up changes, camera becomes too free
	//up = glm::rotate(pitch, up);

	setupView();
}

void Camera::update(float dt)
{
	float speed = 30.0f;
	int forwardMovement = (moveForward ? 1 : 0) + (moveBackward ? -1 : 0);
	if (forwardMovement != 0) {
		applyOffset( forward * (forwardMovement * dt * speed) );
	}
	int sideMovement = (moveLeft ? -1 : 0) + (moveRight ? 1 : 0);
	if (sideMovement != 0) {
		applyOffset( axis * (sideMovement * dt * speed) );
	}
}

const glm::mat4 & Camera::getView() const
{
	return view;
}

// PROJECTION

void Camera::refreshProjection()
{
	if (projectionType == PERSPECTIVE) {
		setupPerspectiveProjection( 55.0f, viewportSize.x/viewportSize.y, projectionZLimit.x, projectionZLimit.y);
	}
	else {
		// TODO: projection start and size are dependant on the world and cameras relative eye offset
		// Ortho size could and should be open to outside interpretations, for now it is fixed
		setupOtrhographicProjection({ 0.0f,0.0f }, { viewportSize.x,viewportSize.y }, projectionZLimit.x, projectionZLimit.y);
	}
}

void Camera::setupOtrhographicProjection(glm::vec2 projectionStart, glm::vec2 projectionSize, float zNear, float zFar)
{
	projectionType = ProjectionType::OTRHOGRAPHIC;
	this->projectionStart = projectionStart;
	this->projectionSize = projectionSize;
	projection = glm::ortho(
		projectionStart.x, projectionStart.x + projectionSize.x, 
		projectionStart.y, projectionStart.y + projectionSize.y, 
		zNear, zFar);
	refreshViewProjection();
}

void Camera::setupPerspectiveProjection(float fovy, float aspectRatio, float zNear, float zFar)
{
	projectionType = ProjectionType::PERSPECTIVE;
	projection = glm::perspective( angleToRadians(fovy), aspectRatio, zNear, zFar);
	refreshViewProjection();
}

const glm::mat4 & Camera::getProjection() const
{
	return projection;
}

// VIEW PROJECTION
void Camera::refreshViewProjection()
{
	viewProjection = projection * view;
}

const glm::mat4& Camera::getViewProjection() const
{
	return viewProjection;
}


// VIEWPORT
void Camera::windowDidResize(float width, float height)
{
	if (resizePolicy == ResizePolicy::DynamicRatio) 
	{
		windowSize.x = width;
		windowSize.y = height;
		setupViewport(0.f, 0.f, windowSize.x, windowSize.y);
	}
	else if (resizePolicy == ResizePolicy::FixedRatio)
	{
		float newWidth = viewportRatio * height;
		float newHeight = height;
		float vX = 0.0f;
		float vY = 0.0f;
		if (newWidth > width) {
			newHeight = height * width / newWidth;
			newWidth = width;
			vY = (height - newHeight) / 2.0f;
		}
		else {
			vX = (width - newWidth) / 2.0f;
		}
		setupViewport(vX, vY, newWidth, newHeight);
	}
	refreshProjection();
}

void Camera::setupViewport(float x, float y, float width, float height)
{
	viewportPos.x = x;
	viewportPos.y = y;
	viewportSize.x = width;
	viewportSize.y = height;
}

void Camera::loadViewport()
{
	glViewport((int)viewportPos.x, (int)viewportPos.y, (int)viewportSize.x, (int)viewportSize.y);
}

void Camera::setResizePolicy(ResizePolicy policy)
{
	resizePolicy = policy;
	windowDidResize(windowSize.x, windowSize.y);
}

void Camera::setViewportRatio(float ratio)
{
	viewportRatio = ratio;
}
