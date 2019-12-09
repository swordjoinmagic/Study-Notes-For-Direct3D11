#pragma once

#include "D3DUtils.h"
#include "MathF.h"

namespace SJM {

	enum CameraMovement {
		FORWAD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	const float YAW = 90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 10.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
	const float NEARDISTANCE = 0.1f;
	const float FARDISTANCE = 50;

	class Camera {
	private:
		// �Ի���Ϊ��λ
		float fov;
		float3 worldUp;
	public:
		float3 pos;
		float3 rotation;

		// ��������淽��
		float3 forward;
		// �����ָ���ҵķ���
		float3 right;
		// �����ָ���ϵķ���
		float3 up;

		// ������(��x����ת�Ƕ�)��ƫ����(��y����ת�Ƕ�)
		float Yaw, Pitch;

		// ������ƶ��ٶ��Լ����ת��ʱ�����ж�
		float MovementSpeed;
		float MouseSensitivity;

		// �ԽǶ�Ϊ��λ
		float fovAngle;
		float Near, Far;
		float aspect;
		Camera(float3 _pos, float3 _rotation, float _aspect) :
			pos(_pos), rotation(_rotation), fovAngle(ZOOM), Near(NEARDISTANCE), Far(FARDISTANCE), aspect(_aspect),
			worldUp(0, 1, 0),Yaw(YAW),Pitch(PITCH),forward(0,0,1),
			MovementSpeed(SPEED),MouseSensitivity(SENSITIVITY){
			fov = MathF::Radians(fovAngle);
			updateCameraDirection();
		}

		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjMatrix();

		// ��������¼�
		void ProcessKeyBoard(CameraMovement direction, float deltaTime);

		// ����ƶ����µ������ƫ���Ǻ͸����Ǳ仯
		void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
		
		// ����������
		void ProcessMouseScroll(float yOffset);

	private:
		// ���ݵ�ǰ�����Ǻ�ƫ���Ǹ��µ�ǰ���������
		void updateCameraDirection();
	};
}