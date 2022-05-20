ALL: 000_exec_policy 002_find 003_teleoperate0 003_teleoperate1

000_exec_policy: 000_exec_policy.cpp
	g++ -std=c++20 $^ -o $@ -L$(LIBRARY_PATH) -ltbb

002_find: 002_find.cpp
	g++ -std=c++17 $^ -o $@

003_teleoperate0: 003_teleoperate0.cpp
	g++ -std=c++17 -pthread $^ -o $@

003_teleoperate1: 003_teleoperate1.cpp
	g++ -std=c++20 -pthread $^ -o $@