//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"
#include <windows.h>

namespace gps {
	float lastX, lastY, yaw, pitch;
	int firstMouse = 1;
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
	{
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	glm::mat4 Camera::getViewMatrix()
	{
		return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return cameraPosition;
	}

	void Camera::move(MOVE_DIRECTION direction, float speed)
	{
		printf("%f   %f   %f\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		switch (direction) {
		case MOVE_FORWARD:
			if ((cameraPosition + cameraDirection * speed).z>-11 && (cameraPosition + cameraDirection * speed).z < 11 && (cameraPosition + cameraDirection * speed).x>-11 && (cameraPosition + cameraDirection * speed).x < 11 && (cameraPosition + cameraDirection * speed).y>0.1 && (cameraPosition + cameraDirection * speed).y < 17)
				cameraPosition += cameraDirection * speed;
			break;

		case MOVE_BACKWARD:
			if ((cameraPosition - cameraDirection * speed).z > -11 && (cameraPosition - cameraDirection * speed).z < 11 && (cameraPosition - cameraDirection * speed).x>-11 && (cameraPosition - cameraDirection * speed).x < 11 && (cameraPosition - cameraDirection * speed).y>0.1 && (cameraPosition - cameraDirection * speed).y < 17)
			cameraPosition -= cameraDirection * speed;
			break;

		case MOVE_RIGHT:
			if ((cameraPosition + glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).z > -11 && (cameraPosition + glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).z < 11 && (cameraPosition + glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).x>-11 && (cameraPosition + glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).x < 11 && (cameraPosition + glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).y>0.1 && (cameraPosition + glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).y < 17)
			cameraPosition += glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
			break;

		case MOVE_LEFT:
			if ((cameraPosition - glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).z > -11 && (cameraPosition - glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).z < 11 && (cameraPosition - glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).x>-11 && (cameraPosition - glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).x < 11 && (cameraPosition - glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).y>0.1 && (cameraPosition - glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed).y < 17)
			cameraPosition -= glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
			break;

		case MOVE_UP:
			if ((cameraPosition + glm::vec3(0, 1, 0) * speed).z > -11 && (cameraPosition + glm::vec3(0, 1, 0) * speed).z < 11 && (cameraPosition + glm::vec3(0, 1, 0) * speed).x>-11 && (cameraPosition + glm::vec3(0, 1, 0) * speed).x < 11 && (cameraPosition + glm::vec3(0, 1, 0) * speed).y>0.1 && (cameraPosition + glm::vec3(0, 1, 0) * speed).y < 17)
			cameraPosition += glm::vec3(0, 1, 0) * speed;
			break;

		case MOVE_DOWN:
			if ((cameraPosition - glm::vec3(0, 1, 0) * speed).z > -11 && (cameraPosition - glm::vec3(0, 1, 0) * speed).z < 11 && (cameraPosition - glm::vec3(0, 1, 0) * speed).x>-11 && (cameraPosition - glm::vec3(0, 1, 0) * speed).x < 11 && (cameraPosition - glm::vec3(0, 1, 0) * speed).y>0.1 && (cameraPosition - glm::vec3(0, 1, 0) * speed).y < 17)
			cameraPosition -= glm::vec3(0, 1, 0) * speed;
			break;
		}
	}
	void Camera::tour()
	{
		//cameraPosition = glm::vec3(-5.0,1.9,10.7);
		cameraTarget = glm::vec3(-2.5, 4.20, 0.0);
		cameraDirection = glm::normalize(cameraTarget - cameraPosition);
	}
	void Camera::tour0()
	{
		cameraPosition = glm::vec3(-5.0, 1.9, 10.7);
		cameraTarget = glm::vec3(-2.5, 4.20, 0.0);
		cameraDirection = glm::normalize(cameraTarget - cameraPosition);
	}
	void Camera::rotate(float pitch, float yaw)
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = glm::normalize(front);
	}
	void Camera::movep(bool ok,int x,int y)
	{
		cameraTarget = glm::vec3(0, 0, 0);
		cameraDirection = glm::normalize(cameraTarget - cameraPosition);
		if (!ok) 
		{
			if (x==0) //down
				if ((cameraPosition - glm::vec3(0, 1, 0) * 0.01f).y > 0.1)
					cameraPosition -= glm::vec3(0, 1, 0) * 0.01f;
			if (x==1) //up
				cameraPosition += glm::vec3(0, 1, 0) * 0.01f;
		}
		else { 
			if (y==0)
				cameraPosition = glm::vec3(-7.23f,0.1f,7.45f);
			else if (y==1)
				cameraPosition = glm::vec3(8.44f, 0.1f, -10.15f);
		}
	}
	void Camera::mouse_callback(float xpos, float ypos) {

		if (firstMouse == 1)
		{
			lastX = 320;
			lastY = 240;
			firstMouse = 0;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.2;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		rotate(pitch, yaw);
	}
}
