#include "GameTimer.h"

// ���캯�����ڲ�ѯ���ܼ�������Ƶ��
GameTimer::GameTimer() : 
	mSecondsPerCount(0.0),mDeltaTime(-1.0),mBaseTime(0),
	mPausedTime(0),mPrevTime(0),mCurrTime(0),mStopped(false){

	// ��ѯƵ��
	INT64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

// ����deltaTime
void GameTimer::Tick() {
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	INT64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// ��֡����һ֡��ʱ���
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	// ���浱ǰ֡��prev��(�����ϴμ���)
	mPrevTime = mCurrTime;

	/// ʹʱ���Ϊ�Ǹ�ֵ
	if (mDeltaTime < 0.0)
		mDeltaTime = 0.0;

}

// ��Ϣѭ��֮ǰ���õĺ���
// ��Ϊ��һ�ε���ʱ��Ϊû��prev,�ᵼ��deltaTime=currTime,
// ������Ϸ��ʼǰʹ��reset��������prevΪ��ǰcurrTime
void GameTimer::Reset() {
	INT64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

// ��ͣ��ʱ��
void GameTimer::Stop() {
	if (!mStopped) {
		INT64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// ����ǰʱ�̼�Ϊֹͣʱ��
		mStopTime = currTime;
		mStopped = true;
	}
}

// ���ڻָ�����ͣ�ļ�ʱ��
void GameTimer::Start() {
	INT64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// ������ڽ����ͣ״̬
	if (mStopped) {
		// ������ͣʱ��
		// �ۼ���ͣʱ��
		mPausedTime += startTime - mStopTime;

		// ���¿�ʼ��ʱ��
		mPrevTime = startTime;

		// ������ͣ״̬
		mStopped = false;
		mStopTime = 0;
	}
}

// ����Ե���Reset������,��������ͣʱ��,�ܵ���Ϸʱ��
float GameTimer::TotalTime() const{
	// �����жϵ�ǰ�Ƿ�����ͣ״̬
	if (mStopped) {
		return (float)((mStopTime-mPausedTime-mBaseTime)*mSecondsPerCount);
	} else {
		return (float)((mCurrTime - mPausedTime - mBaseTime)*mSecondsPerCount);
	}
}

float GameTimer::DeltaTime() const{
	return static_cast<float>(mDeltaTime);
}