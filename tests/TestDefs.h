#pragma once

#define TEST_VERIFY(a) if(!(a)) { printf("Verification failed: '%s'. Aborting.\n", #a); std::abort(); }
