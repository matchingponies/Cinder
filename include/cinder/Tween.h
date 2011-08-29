/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Based on the sc-Choreograph CinderBlock by David Wicks: http://sansumbrella.com/

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "TimelineItem.h"

#include "cinder/Cinder.h"
#include "cinder/CinderMath.h"
#include "cinder/Easing.h"
#include "cinder/Function.h"

#include <list>
#include <boost/utility.hpp>

namespace cinder {


template<typename T>
class Tween;
typedef std::function<float (float)> EaseFn;

template<typename T>
T tweenLerp( const T &start, const T &end, float time )
{
	return start * ( 1 - time ) + end * time;
}

//Our templated tween design
class TweenBase : public TimelineItem {
  public:
	typedef std::function<void ()>		StartFn;
	typedef std::function<void ()>		CompletionFn;
	typedef std::function<void ()>		UpdateFn;

	TweenBase( void *target, bool copyStartValue, float startTime, float duration, EaseFn easeFunction = easeNone );
	virtual ~TweenBase() {}

	//! change how the tween moves through time
	void	setEaseFn( EaseFn easeFunction ) { mEaseFunction = easeFunction; }
	EaseFn	getEaseFn() const { return mEaseFunction; }

	void			setStartFn( StartFn startFunction ) { mStartFunction = startFunction; }
	StartFn			getStartFn() const { return mStartFunction; }
	
	void			setUpdateFn( UpdateFn updateFunction ) { mUpdateFunction = updateFunction; }									
	UpdateFn		getUpdateFn() const { return mUpdateFunction; }
																																					
	void			setCompletionFn( CompletionFn completionFunction ) { mCompletionFunction = completionFunction; }
	CompletionFn	getCompletionFn() const { return mCompletionFunction; }
	
	
  protected:
	virtual void reset( bool unsetStarted )
	{
		TimelineItem::reset( unsetStarted );
	}

	virtual void complete()
	{
		if( mCompletionFunction )
			mCompletionFunction();
	}

  
	StartFn				mStartFunction;
	UpdateFn			mUpdateFunction;	
	CompletionFn		mCompletionFunction;
  
	EaseFn		mEaseFunction;
	float		mDuration;
	bool		mCopyStartValue;
};

template<typename T>
class TweenRef : public std::shared_ptr<Tween<T> > {
  public:
	TweenRef( const std::shared_ptr<Tween<T> > &sp )
		: std::shared_ptr<Tween<T> >( sp )
	{}
	TweenRef( Tween<T> *tween )
		: std::shared_ptr<Tween<T> >( tween )
	{}
	TweenRef()
		: std::shared_ptr<Tween<T> >()
	{}
};
		
template<typename T>
class Tween : public TweenBase {
  public:
	typedef std::function<T (const T&, const T&, float)>	LerpFn;

	// build a tween with a target, target value, duration, and optional ease function
	Tween( T *target, T endValue, float startTime, float duration,
			EaseFn easeFunction = easeNone, LerpFn lerpFunction = &tweenLerp<T> )
		: TweenBase( target, true, startTime, duration, easeFunction ), mStartValue( *target ), mEndValue( endValue ), mLerpFunction( lerpFunction )
	{
	}
	
	Tween( T *target, T startValue, T endValue, float startTime, float duration,
			EaseFn easeFunction = easeNone, LerpFn lerpFunction = &tweenLerp<T> )
		: TweenBase( target, false, startTime, duration, easeFunction ), mStartValue( startValue ), mEndValue( endValue ), mLerpFunction( lerpFunction )
	{
	}
	
	virtual ~Tween() {}
	
	//! Returns the starting value for the tween. If the tween will copy its target's value upon starting (isCopyStartValue()) and the tween has not started, this returns the value of its target when the tween was created
	T	getStartValue() const { return mStartValue; }
	T	getEndValue() const { return mEndValue; }			
	T*	getTarget() const { return reinterpret_cast<T*>( mTarget ); }

	TweenRef<T>		startFn( StartFn startFunction ) { mStartFunction = startFunction; return getThisRef(); }
	TweenRef<T>		updateFn( UpdateFn updateFunction ) { mUpdateFunction = updateFunction; return getThisRef(); }
	TweenRef<T>		completionFn( CompletionFn completionFunction ) { mCompletionFunction = completionFunction; return getThisRef(); }	
	
	//! Returns whether the tween will copy its target's value upon starting
	bool	isCopyStartValue() { return mCopyStartValue; }

	// these override their equivalents in TimelineItem so that we can return a TweenRef<T> instead of TimelineItemRef
	//! Pushes back the tween's start time by \a amt. Returns a reference to \a this
	TweenRef<T> startTime( float newTime ) { setStartTime( newTime ); return getThisRef(); }
	//! Pushes back the tween's start time by \a amt. Returns a reference to \a this
	TweenRef<T> delay( float amt ) { setStartTime( mStartTime + amt ); return getThisRef(); }
	//! Sets the tween's duration to \a newDuration. Returns a reference to \a this
	TweenRef<T> duration( float newDuration ) { setDuration( newDuration ); return getThisRef(); }			
	//! Sets whether the item will remove itself from the Timeline when it is complete
	TweenRef<T> autoRemove( bool autoRmv = true ) { setAutoRemove( autoRmv ); return getThisRef(); }
	//! Sets whether the item starts over when it is complete
	TweenRef<T> loop( bool doLoop = true ) { setLoop( doLoop ); return getThisRef(); }
	
	//! Returns a TweenRef<T> to \a this
	TweenRef<T> getThisRef(){ return TweenRef<T>( std::static_pointer_cast<Tween<T> >( shared_from_this() ) ); }

  protected:
	virtual void reverse()
	{
		std::swap( mStartValue, mEndValue );
	}
	
	virtual TimelineItemRef	cloneReverse() const
	{
		std::shared_ptr<Tween<T> > result( new Tween<T>( *this ) );
		std::swap( result->mStartValue, result->mEndValue );
		result->mCopyStartValue = false;
		return result;
	}
	
	virtual void start()
	{
		if( mCopyStartValue )
			mStartValue = *(reinterpret_cast<T*>( mTarget ) );
		if( mStartFunction )
			mStartFunction();
	}
	
	virtual void update( float relativeTime )
	{
		*reinterpret_cast<T*>(mTarget) = mLerpFunction( mStartValue, mEndValue, mEaseFunction( relativeTime ) );
		if( mUpdateFunction )
			mUpdateFunction();
	}
	

	T	mStartValue, mEndValue;	
	
	LerpFn				mLerpFunction;
};

typedef std::shared_ptr<TweenBase>	TweenBaseRef;

class TweenScope {
  public:
	TweenScope() {}
	TweenScope( const TweenScope &rhs ) {}	// do nothing for copy; these are our tweens alone
	TweenScope& operator=( const TweenScope &rhs ) { return *this; }	// do nothing for copy; these are our tweens alone	
	~TweenScope();
	
	TweenScope& operator+=( TimelineItemRef item );
	void add( TimelineItemRef item );

  private:
	std::list<std::weak_ptr<TimelineItem> >		mItems;
};

template<typename T>
class ValueTween : public Tween<T> {
  public:
	ValueTween( T startValue, T endValue, float startTime, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
		: Tween<T>( &mValue, startValue, endValue, startTime, duration, easeFunction, lerpFunction ), mValue( startValue )
	{
	}
	
	virtual void update( float relativeTime )
	{
		Tween<T>::update( relativeTime );
	}	
	
	T		getValue() const { return mValue; }
	
	T		mValue;
};

} //namespace cinder