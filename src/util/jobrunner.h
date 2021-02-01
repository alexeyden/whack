#ifndef JOBRUNNER_H
#define JOBRUNNER_H

#include <queue>
#include <mutex>
#include <future>
#include <atomic>

#include "backend/backend.h"
#include "util/util.h"

template<typename Result> class JobRunner;

template<typename Result> class Job
{
public:
	void execute() {
		_promise.set_value(work());
	}
	
protected:
	virtual Result work() = 0;
	
private:
	std::future<Result> future() {
		return _promise.get_future();
	}
	
	friend class JobRunner<Result>;
	
	std::promise<Result> _promise;
};

template<typename Result> class JobRunner
{
public:
	JobRunner(Backend *be, const std::string& name) {
		_done = false;
		
        _backend = be;
		_thread = _backend->createThread(threadEntry, name.c_str(), this);
	}
	
	~JobRunner() {
		_done = true;
		_cv.notify_one();
		_backend->waitThread(_thread);
	}
	
	std::future<Result> run(Job<Result>* job) {
		std::future<Result> future = job->future();
		
		_mutex.lock();
		_jobs.push(std::shared_ptr<Job<Result>>(job));
		_mutex.unlock();
		
		return future;
	}
	
private:
	static int threadEntry(void* s) {
        JobRunner* self = (JobRunner*) s;
		while(self->_done == false) {
			std::unique_lock<std::mutex> locker(self->_mutex);
			
			self->_cv.wait(locker, [&]() { return !self->_jobs.empty() || self->_done == true; });
			
			while(!self->_jobs.empty()) {
				auto job = self->_jobs.front();
				
				self->_jobs.pop();
				
				locker.unlock();
				
				job->execute();
				
				locker.lock();
			}
		}
		
        return 0;
	}

private:
    Backend* _backend;
	bool _done;
	
	std::queue<std::shared_ptr<Job<Result>>> _jobs;
	std::mutex _mutex;

	std::condition_variable _cv;
	SDL_Thread* _thread;
};

#endif // JOBRUNNER_H
