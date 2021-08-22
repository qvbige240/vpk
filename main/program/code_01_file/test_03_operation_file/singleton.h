#ifndef EVENTSERVICE_BASE_SINGLETON_H
#define EVENTSERVICE_BASE_SINGLETON_H

// #include "criticalsection.h"
#include <pthread.h>

#define DISALLOW_ASSIGN(TypeName) void operator=(const TypeName &)

// A macro to disallow the evil copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &);              \
  DISALLOW_ASSIGN(TypeName)

#define TRACK_OWNER(x) x

class CriticalSection {
 public:
  CriticalSection() {
    pthread_mutexattr_t mutex_attribute;
    pthread_mutexattr_init(&mutex_attribute);
    //Set the "recursive" attribute, which allows a thread to repeatedly lock a mutex 
    //without causing a deadlock, But need to unlock the same number of times.
    pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex_, &mutex_attribute);
    pthread_mutexattr_destroy(&mutex_attribute);
    TRACK_OWNER(thread_ = 0);
  }
  ~CriticalSection() {
    pthread_mutex_destroy(&mutex_);
  }
  void Enter() {
    pthread_mutex_lock(&mutex_);
    TRACK_OWNER(thread_ = pthread_self());
  }
  bool TryEnter() {
    if (pthread_mutex_trylock(&mutex_) == 0) {
      TRACK_OWNER(thread_ = pthread_self());
      return true;
    }
    return false;
  }
  void Leave() {
    TRACK_OWNER(thread_ = 0);
    pthread_mutex_unlock(&mutex_);
  }

#if CS_TRACK_OWNER
  bool CurrentThreadIsOwner() const {
    return pthread_equal(thread_, pthread_self());
  }
#endif  // CS_TRACK_OWNER

 private:
  pthread_mutex_t mutex_;
  TRACK_OWNER(pthread_t thread_);
};

class CritScope {
 public:
  explicit CritScope(CriticalSection *pcrit) {
    pcrit_ = pcrit;
    if(pcrit_) {
      pcrit_->Enter();
    }
  }
  ~CritScope() {
    if(pcrit_) {
      pcrit_->Leave();
    }
  }
  
 private:
  CriticalSection *pcrit_;
  DISALLOW_COPY_AND_ASSIGN(CritScope);
};

namespace vzes {

template <typename T>
class Singleton {
 public:
  static T *GetInstance() {
    if (instance_ == NULL) {
      CritScope cs(&mutex_);
      if (instance_ == NULL) {
        instance_ = new T();
      }
    }
    return instance_;
  }

  static void Destroy() {
    CritScope cs(&mutex_);
    if (instance_ != NULL) {
      delete instance_;
      instance_ = NULL;
    }
  }

 private:
  static CriticalSection   mutex_;
  static T                *instance_;
};

template <typename T>
T *Singleton<T>::instance_ = NULL;

template <typename T>
CriticalSection Singleton<T>::mutex_;

}  // namespace vzes

#endif  // EVENTSERVICE_BASE_SINGLETON_H
