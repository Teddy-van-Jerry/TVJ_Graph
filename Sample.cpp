/*
 * File: Sample.cpp
 * Project: TVJ_Graph
 * --------------------------
 *
 * @author: Teddy van Jerry
 * @licence: The MIT Licence
 * @compiler: at least C++/11 (tested on MSVC)
 *
 * @version 1.0 2021/05/21
 * - initial version
 *
 */

#include "TVJ_Graph.h"
using namespace tvj;

int main()
{
	// ------- Set the Graph ------- //
	graph<int> Test(9, 0);
	Test.add_edge(0, 1, 6)
		.add_edge(0, 2, 4)
		.add_edge(0, 3, 5)
		.add_edge(1, 4, 1)
		.add_edge(2, 4, 1)
		.add_edge(3, 5, 2)
		.add_edge(4, 6, 9)
		.add_edge(4, 7, 7)
		.add_edge(5, 7, 4)
		.add_edge(6, 8, 2)
		.add_edge(7, 8, 4);
	
	// ------- Calculate the Critical Path ------- //
	vector<vector<size_t>> c_paths;
	std::cout << "The length of critical path of the graph is "<< Test.critical_path(c_paths) << '.' << std::endl;

	for (const auto& path : c_paths)
	{
		for (auto it = path.begin(); it != path.end() && it != path.end() - 1; it++)
		{
			std::cout << *it << "->";
		}
		std::cout << *--path.end() << std::endl;
	}

	c_paths.no_delete(); // due to potential problems in tvj::vector

	return 0;
}

// ALL RIGHTS RESERVED (C) 2021 Teddy van Jerry