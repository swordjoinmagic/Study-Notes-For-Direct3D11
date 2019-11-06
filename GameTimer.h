#pragma once
#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <windows.h>

// ������Ϸ��ʱ����
class GameTimer {
public:
	GameTimer();

	// ������Ϊ��λ
	float TotalTime() const;
	float DeltaTime() const;

	// �ڿ�ʼ��Ϣѭ��֮ǰ����
	void Reset();
	// �����ʱ����ͣʱ����
	void Start();
	// ��ͣ��ʱ��ʱ����
	void Stop();
	// ÿ֡���ڼ���deltaTime
	void Tick();
private:
	double mSecondsPerCount;
	double mDeltaTime;

	// ���ڼ�����Ϸ�����е���ʱ��(������stopTime����ʱ��)
	INT64 mBaseTime;
	INT64 mPausedTime;
	INT64 mStopTime;

	// ���ڼ���deltaTime
	INT64 mPrevTime;
	INT64 mCurrTime;

	bool mStopped;
};

#endif // !GAMETIMER_H
