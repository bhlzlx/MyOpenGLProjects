#include "PhCamera.h"

namespace ph
{
	PhCamera::PhCamera()
	{
		// view matrix
		eye = glm::vec3(0, 0, -1);
		lookAt = glm::vec3(0, 0, 0);
		topward = glm::vec3(0, 1, 0);
		leftward = glm::vec3(-1, 0, 0);
		forward = glm::vec3(0, 0, 1);
		matViewDirty = false;
		// projection
		projType = ProjectionTypePerpective;
		// projection matrix
		fov = 120;
		aspect = 4.0f / 3.0f;
		near = 0.1f;
		far = 1000;
		matProj = glm::perspective(fov, aspect, near, far);
		matProjDirty = false;
	}

	PhCamera::~PhCamera()
	{

	}

	void PhCamera::SetEye(glm::vec3 _eye)
	{
		this->eye = _eye;
		matViewDirty = true;
	}

	void PhCamera::SetLookAt(glm::vec3 _lookAt)
	{
		lookAt = _lookAt;
		matViewDirty = true;
	}

	void PhCamera::SetTop(glm::vec3 _top)
	{
		topward = _top;
		matViewDirty = true;
	}

	void PhCamera::SetMatView(glm::vec3 _eye, glm::vec3 _lookAt, glm::vec3 _top)
	{
		eye = _eye;
		lookAt = _lookAt;
		topward = _top;
		matViewDirty = true;
	}

	void PhCamera::Tick()
	{
		if (matViewDirty)
		{
			forward = glm::normalize(lookAt - eye);
			leftward = glm::normalize(glm::cross( glm::vec3(0, 1, 0), forward));
			topward = glm::normalize(glm::cross(forward, leftward));
			lookAt = eye + forward;
			matView = glm::lookAt(eye, lookAt, topward);
		}
		if (matProjDirty)
		{
			if (projType == ProjectionTypePerpective)
			{
				matProj = glm::perspective(fov, aspect, near, far);
			}
			else
			{
				matProj = glm::ortho<float>(left, right, bottom, top, near, far);
			}
		}
		matViewDirty = matProjDirty = false;
	}

	void PhCamera::SetProjectionType(ProjectionType _type)
	{
		projType = _type;
		matProjDirty = true;
	}

	void PhCamera::Perspective(float _fov, float _aspect, float _near, float _far)
	{
		fov = _fov; aspect = _aspect;
		near = _near; far = _far;
		matProjDirty = true;
	}

	void PhCamera::Ortho(float _left, float _right, float _bottom, float _top, float _near, float _far)
	{
		left = _left; right = _right; bottom = _bottom; top = _top;
		near = _near; far = _far;
		matProjDirty = true;
	}

	void PhCamera::Forward(float _distance)
	{
		glm::vec3 vForward = forward * _distance;
		eye += vForward;
		lookAt += vForward;
		matViewDirty = true;
	}

	void PhCamera::Backward(float _distance)
	{
		glm::vec3 vForward = forward * -_distance;
		eye += vForward;
		lookAt += vForward;
		matViewDirty = true;
	}

	void PhCamera::Leftward(float _distance)
	{
		glm::vec3 vForward = leftward * _distance;
		eye += vForward;
		lookAt += vForward;
		matViewDirty = true;
	}

	void PhCamera::Rightward(float _distance)
	{
		glm::vec3 vForward = leftward * -_distance;
		eye += vForward;
		lookAt += vForward;
		matViewDirty = true;
	}

	void PhCamera::RotateAxisY(float _angle)
	{
		_angle = _angle / 180.0f * glm::pi<float>();
		glm::quat q = glm::rotate(glm::quat(), _angle, topward);
		forward = glm::normalize(forward * q);
		topward = glm::normalize(topward * q);
		lookAt = eye + forward;
		matViewDirty = true;
	}

	void PhCamera::RotateAxisX(float _angle)
	{
		_angle = _angle / 180.0f * glm::pi<float>();
		glm::quat q = glm::rotate(glm::quat(), _angle, -leftward);
		forward = glm::normalize(forward * q);
		topward = glm::normalize(topward * q);
		lookAt = eye + forward;
		matViewDirty = true;
	}

	const glm::mat4x4 & PhCamera::GetViewMatrix()
	{
		return matView;
	}

	const glm::mat4x4 & PhCamera::GetProjMatrix()
	{
		return matProj;
	}

}

