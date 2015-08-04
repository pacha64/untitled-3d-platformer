// Timer.h: Timer class used for determining elapsed time and 
//              frames per second.
//
//////////////////////////////////////////////////////////////////////

#ifndef _E_TIMER_H
#define _E_TIMER_H

#pragma once

//////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// CLASSES
//////////////////////////////////////////////////////////////////////

class CTimer
{
private:
    //FPS variables
    float mAcumulatedTime;
    int mCurrentFps;
	int mLastFps;
    int mFramesElapsed;

public:

    //----------------------------------------------------------
    // Name:    CTimer::CTimer
    // Desc:    Default constructor
    // Args:    None
    // Rets:    None
    //----------------------------------------------------------
    CTimer( void )
        : mAcumulatedTime(0.0f),
		mCurrentFps(0),
		mLastFps(0),
		mFramesElapsed(0)
    {
	}

    virtual ~CTimer( void )
    {
	}

    void update(float timeElapsed)
    {
		int seconds = Math::IFloor(mAcumulatedTime);
		mFramesElapsed++;
		mCurrentFps++;
		mAcumulatedTime += timeElapsed;
        
		if (Math::IFloor(mAcumulatedTime) != seconds)
		{
			mLastFps = mCurrentFps;
			mCurrentFps = 0;
		}
    }

	inline int getElapsedSeconds()
	{
		return Math::IFloor(mAcumulatedTime);
	}
	inline int getFramesElapsed()
	{
		return mFramesElapsed;
	}
    inline int getFPS( void )
    {
		return mLastFps;
	}
};

#endif // _E_TIMER_H