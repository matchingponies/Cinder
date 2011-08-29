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

#include "cinder/Cinder.h"

namespace cinder
{
	typedef std::shared_ptr<class TimelineItem>	TimelineItemRef;

	//! Base interface for anything that can go on a Timeline
	class TimelineItem : public std::enable_shared_from_this<TimelineItem>
	{
	  public:
		TimelineItem( class Timeline *parent = 0 );
		TimelineItem( class Timeline *parent, void *target, float startTime, float duration );
		virtual ~TimelineItem() {}
		
		//! Returns the item's target pointer
		void* getTarget() const { return mTarget; }

		//! Returns the item's start time
		float			getStartTime() const { return mStartTime; }
		//! Set the items's start time to \a newTime
		void			setStartTime( float newTime );
		//! Set the items's start time to \a newTime. Returns a reference to \a this
		TimelineItemRef startTime( float newTime ) { setStartTime( newTime ); return shared_from_this(); }

		//! Pushes back the item's start time by \a amt. Returns a reference to \a this
		TimelineItemRef delay( float amt ) { setStartTime( mStartTime + amt ); return shared_from_this(); }

		//! Returns the item's duration
		float			getDuration() const { return mDuration; }			
		//! Sets the item's duration to \a newDuration.
		void			setDuration( float newDuration );
		//! Sets the item's duration to \a newDuration. Returns a reference to \a this
		TimelineItemRef duration( float newDuration ) { setDuration( newDuration ); return shared_from_this(); }

		//! Returns whether the item starts over when it is complete
		bool			getLoop() const { return mLoop; }
		//! Sets whether the item starts over when it is complete
		void			setLoop( bool doLoop = true ) { mLoop = doLoop; }
		//! Sets whether the item starts over when it is complete. Returns a reference to \a this
		TimelineItemRef loop( bool doLoop = true ) { setLoop( doLoop ); return shared_from_this(); }

		//! Returns whether the item ever is marked as complete
		bool			getInfinite() const { return mLoop; }
		//! Sets whether the item ever is marked as complete
		void			setInfinite( bool infinite = true ) { mInfinite = infinite; }
		//! Sets whether the item ever is marked as complete. Returns a reference to \a this
		TimelineItemRef infinite( bool inf = true ) { setInfinite( inf ); return shared_from_this(); }

		//! Returns the time of the item's competion, equivalent to getStartTime() + getDuration().
		float			getEndTime() const { return mStartTime + mDuration; }


		//! Removes the item from its parent Timeline
		void cancel();
		//! Marks the item as not completed, and if \a unsetStarted, marks the item as not started
		virtual void reset( bool unsetStarted = false ) { if( unsetStarted ) mHasStarted = false; mComplete = false; }
		
		//! Returns whether the item has started
		bool hasStarted() const { return mHasStarted; }			
		//! Returns whether the item has completed
		bool isComplete() { return mComplete; }
		
		//! Should the item remove itself from the Timeline when it is complete
		bool	getAutoRemove() const { return mAutoRemove; }
		//! Sets whether the item will remove itself from the Timeline when it is complete
		void	setAutoRemove( bool autoRemove = true ) { mAutoRemove = autoRemove; }
		//! Sets whether the item will remove itself from the Timeline when it is complete
		TimelineItemRef autoRemove( bool autoRmv = true ) { mAutoRemove = autoRmv; return shared_from_this(); }
		
		virtual void start() = 0;
		virtual void loopStart() {}
		virtual void update( float relativeTime ) = 0;
		virtual void complete() = 0;
		//! Call update() only at the beginning of each loop (for example Cues exhibit require this behavior)
		virtual bool updateAtLoopStart() { return false; }
		virtual void				reverse() = 0;
		//! Creates a cloned item which runs in reverse relative to a timeline of duration \a timelineDuration
		virtual TimelineItemRef		cloneReverse() const = 0;
		//! go to a specific time, generally called by the parent Timeline only.
		void stepTo( float time );
		
	  protected:
		//! Converts time from absolute to absolute based on item's looping attributes
		float	loopTime( float absTime );

		static bool refIsMarkedForRemoval( const TimelineItemRef &item ) { return item->mMarkedForRemoval; }

		class Timeline	*mParent;
		
		void	*mTarget;
		float	mStartTime;
		bool	mHasStarted, mComplete, mMarkedForRemoval;
		bool	mInfinite;
		bool	mLoop;
		bool	mUseAbsoluteTime;
		bool	mAutoRemove;
		int32_t	mLastLoopIteration;
		
		friend class Timeline;
	  private:
		float	mDuration, mInvDuration;
	};
}
