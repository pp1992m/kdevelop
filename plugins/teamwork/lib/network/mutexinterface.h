/***************************************************************************
Copyright 2006 David Nolden <david.nolden.kdevelop@art-master.de>
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef MUTEXINTERFACE_H
#define MUTEXINTERFACE_H
///this ugly hack is necessary because the general concept of the boost thread-library wants
///to force us to use scoped lock-classes, but we want to do that work ourselves.
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/xtime.hpp>

class MutexInterfaceImpl {
    typedef boost::recursive_timed_mutex MutexType;
    mutable MutexType m_;
  public:
    MutexInterfaceImpl( const MutexInterfaceImpl& /*rhs*/ ) {
    }

    const MutexInterfaceImpl& operator=( const MutexInterfaceImpl& /*rhs*/ ) {
      return *this;
    }

    MutexInterfaceImpl() {}

    void lockCountUp() const {
    }

    bool tryLockCountUp() const {
      return m_.do_trylock();
    }

    ///timeout in nanoseconds, may have a significant delay, so it shouldn't be used too much
    bool tryLockCountUp( int timeout ) const {
      boost::xtime t;
      xtime_get( &t, boost::TIME_UTC );
      t.nsec += timeout;
      return m_.do_timedlock( t );
    }

    void lockCountDown() const {
      m_.do_unlock();
    };

    ~MutexInterfaceImpl() {}
};

typedef MutexInterfaceImpl MutexInterface;
#endif
