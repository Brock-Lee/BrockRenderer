class Timer
{
private:
	LARGE_INTEGER _freq;
	LARGE_INTEGER _start;
	LARGE_INTEGER _stop;
public:

	Timer()
	{
		QueryPerformanceFrequency(&_freq);
	}

	void Start()
	{
		QueryPerformanceCounter(&_start);
	}

	void Stop()
	{
		QueryPerformanceCounter(&_stop);
	}

	double Elapse()
	{
		return 1e3*(_stop.QuadPart - _start.QuadPart) / _freq.QuadPart;
	}

	long long Ticks()
	{
		return _stop.QuadPart - _start.QuadPart;
	}
};
extern Timer* g_timer;