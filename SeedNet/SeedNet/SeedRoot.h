#pragma once

#include <Windows.h>
#include <process.h>
#include <memory>
#include <WinBase.h>
#include <time.h>
#include <mmsystem.h>
#include <WinSock2.h>

#include <queue>
#include <vector>
#include <stack>
#include <list>
#include <map>

#include <string>

#include <iostream>
#include <fstream>

// boost
#include <boost/pool/pool_alloc.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/atomic.hpp>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "SeedDefinition.h"
#include "SeedDataStructure.h"

#include "SeedLock.h"
#include "SeedSocket.h"

#include "SeedThread.h"

#include "SeedLog.h"
