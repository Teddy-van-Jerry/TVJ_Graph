/*
 * File: TVJ_Graph.h
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

#include "TVJ_Vector.h"

namespace tvj
{
	vector<vector<size_t>> static_path;

	template<typename Elem>
	class graph
	{

		// disable the macro will show middle result
#define _NO_MID_OUTPUT_IN_CRITICAL_PATH_IN_TVJ_GRAPH

	protected:
		class G_Node
		{
			Elem data_;
			int weight_ = 1;
			int edge_weight_ = 1;

		public:
			G_Node();
			G_Node(const Elem& data, int weight = 1, int edge_weight = 1);
		};

	public:
		graph() noexcept;

		graph(size_t vertices_num, const Elem& data, int weight = 1, int edge_weight = 1) noexcept;

		~graph();

		size_t v_count() const noexcept;

		graph& add_vertex(const Elem& data, int weight = 1, int edge_weight = 1);

		graph& add_vertices(size_t number, const Elem& data, int weight = 1, int edge_weight = 1);

		graph& add_edge(size_t v1, size_t v2, int weight = 1, bool directed = true);

		size_t critical_path(size_t v_first, size_t v_last, vector<vector<size_t>>& paths = static_path, bool* ok = nullptr) const;

		size_t critical_path(vector<vector<size_t>>& paths = static_path, bool* ok = nullptr) const;

	protected:
		vector<G_Node> vertices_;

		vector<vector<size_t>> adjvec_;
		vector<vector<int>> edge_w_;

		size_t v_count_ = 0;

	};

	template<typename Elem>
	inline graph<Elem>::G_Node::G_Node() { }

	template<typename Elem>
	inline graph<Elem>::G_Node::G_Node(const Elem& data, int weight, int edge_weight)
		: data_(data), weight_(weight), edge_weight_(edge_weight) { }

	template<typename Elem>
	inline graph<Elem>::graph() noexcept { }

	template<typename Elem>
	inline graph<Elem>::graph(size_t vertices_num, const Elem& data, int weight, int edge_weight) noexcept
		: v_count_(0)
	{
		add_vertices(vertices_num, data, weight, edge_weight);
	}

	template<typename Elem>
	inline graph<Elem>::~graph()
	{
		vertices_.no_delete();
		adjvec_.no_delete();
		edge_w_.no_delete();
	}

	template<typename Elem>
	inline size_t graph<Elem>::v_count() const noexcept
	{
		return this->v_count_;
	}

	template<typename Elem>
	inline graph<Elem>& graph<Elem>::add_vertex(const Elem& data, int weight, int edge_weight)
	{
		return add_vertices(data, 1);
	}

	template<typename Elem>
	inline graph<Elem>& graph<Elem>::add_vertices(size_t number, const Elem& data, int weight, int edge_weight)
	{
		v_count_ += number;
		
		for (size_t i = 0; i != number; i++)
		{
			vector<size_t>* e1 = new vector<size_t>(0, 0);
			vector<int>* e2 = new vector<int>(0, 0);
			G_Node node__(data, weight, edge_weight);
			vertices_.push_back(node__);
			adjvec_.push_back(*e1);
			edge_w_.push_back(*e2);
		}
		return *this;
	}

	template<typename Elem>
	inline graph<Elem>& graph<Elem>::add_edge(size_t v1, size_t v2, int weight, bool directed)
	{
		this->adjvec_[v1].push_back(v2);
		this->edge_w_[v1].push_back(weight);
		if (!directed)
		{
			this->adjvec_[v2].push_back(v1);
			this->edge_w_[v2].push_back(weight);
		}
		return *this;
	}

	template<typename Elem>
	inline size_t graph<Elem>::critical_path(vector<vector<size_t>>& paths, bool* ok) const
	{

#ifndef _NO_MID_OUTPUT_IN_CRITICAL_PATH_IN_TVJ_GRAPH
		std::cout << "================== MID OUTPUT IN CRITICAL PATH IN TVJ GRAPH ==================" << std::endl;
		std::cout << "Adjacent Lists:" << std::endl;
		for (size_t i = 0; i != v_count_; i++)
		{
			std::cout << i << ": ";
			for (size_t j = 0; j != adjvec_[i].size(); j++)
			{
				std::cout << adjvec_[i][j] << ' ';
			}
			std::cout << std::endl;
		}
		std::cout << "------------------------------------------------------------------------------" << std::endl;
#endif

		vector<size_t>* empty_vec__ = new vector<size_t>(1, 0);
		paths.clear();
		paths.push_back(*empty_vec__);

		vector<int> ve(this->v_count_, 0);
		for (size_t i = 0; i != ve.size(); i++)
		{
			for (size_t j = 0; j != adjvec_[i].size(); j++)
			{
				if (ve[adjvec_[i][j]] < ve[i] + edge_w_[i][j])
				{
					ve[adjvec_[i][j]] = ve[i] + edge_w_[i][j];
				}
			}
		}

#ifndef _NO_MID_OUTPUT_IN_CRITICAL_PATH_IN_TVJ_GRAPH
		std::cout << "ve: ";
		for (const auto& t : ve)
		{
			std::cout << t << ' ';
		}
		std::cout << "\n------------------------------------------------------------------------------" << std::endl;
#endif

		vector<int> vl(this->v_count_, *--ve.end());

		for (size_t i__ = vl.size(); i__ != 0; i__--) // here i__ is lager by 1
		{
			size_t i = i__ - 1;
			for (size_t j = 0; j != adjvec_[i].size(); j++)
			{
				if (vl[i] > vl[adjvec_[i][j]] - edge_w_[i][j])
				{
					vl[i] = vl[adjvec_[i][j]] - edge_w_[i][j];
				}
			}
		}

#ifndef _NO_MID_OUTPUT_IN_CRITICAL_PATH_IN_TVJ_GRAPH
		std::cout << "vl: ";
		for (const auto& t : vl)
		{
			std::cout << t << ' ';
		}
		std::cout << "\n------------------------------------------------------------------------------" << std::endl;

		std::cout << "Critical Activities: " << std::endl;
#endif

		struct edge
		{
			size_t from, to;
		};

		vector<edge> critical_activities__;
		for (size_t i = 0; i != this->v_count_; i++)
		{
			for (size_t j = 0; j != adjvec_[i].size(); j++)
			{
				if (ve[i] == vl[adjvec_[i][j]] - edge_w_[i][j])
				{
					edge e__ = { i, adjvec_[i][j] };
					critical_activities__.push_back(e__);

#ifndef _NO_MID_OUTPUT_IN_CRITICAL_PATH_IN_TVJ_GRAPH
					std::cout << '(' << i << ", " << adjvec_[i][j] << ')' << std::endl;
#endif

				}
			}
		}

		for(size_t i = 0; i != critical_activities__.size(); i++)
		{
			auto* original_paths__ = new auto(paths);
			paths.clear();
			size_t j = 0;
			for (; j < critical_activities__.size() - i; j++)
			{
				if (critical_activities__[i].from != critical_activities__[i + j].from) break;
				else
				{
					auto* another__ = new auto(*original_paths__);
					for (auto path : *another__)
					{
						path.push_back(critical_activities__[i + j].to);
						paths.push_back(path);
						path.no_delete();
					}					
				}
			}
			i = i + 2 * j - 2; // move to the next target
		}
		critical_activities__.no_delete();

#ifndef _NO_MID_OUTPUT_IN_CRITICAL_PATH_IN_TVJ_GRAPH
		std::cout << "==============================================================================" << std::endl;
#endif

		return *--ve.end();
	}
}

// ALL RIGHTS RESERVED (C) 2021 Teddy van Jerry