/* 
	
	made by sunkue.
	
*/
#pragma once
#include<time.h>
#include<crtdbg.h>
#include<assert.h>

namespace timer {
	clock_t d_start{ 0 };

	void start() {
		d_start = clock();
	}

	void start_mas(const char message[] = "timer start :") {
		std::cout << message << '\n';
		d_start = clock();
	}


	clock_t end() {
		return clock() - d_start;
	}

	void end_mas() {
		std::cout <<"timer end : "<< timer::end() << " ms\n";
	}
}

namespace assert {
	void false_exit(bool b) {
		assert(b);
	}
}

namespace memory {
	/* this works on debug mode. use debug output */
	//debug show out put {addr}
	void check_now() {
		_CrtDumpMemoryLeaks();
	}

	void check_exit() {
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	void set_stop_point(const long addr) {
		_CrtSetBreakAlloc(addr);
	}
}