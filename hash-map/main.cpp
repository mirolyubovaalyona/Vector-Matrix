

#include <vector>
#include <string>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>

#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <thread>
#include <future>
#include <chrono>
#include <iterator>
#include <memory>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hash_map.hpp"



// *************************
// *************************
// *************************
// *************************
// *************************

#include "hash_map.hpp"
using namespace fefu;

// *************************
// *************************
// *************************
// *************************
// *************************


using namespace std;


template<typename T>
class custom_allocator
{
public:
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = typename std::add_lvalue_reference<T>::type;
	using const_reference = typename std::add_lvalue_reference<const T>::type;
	using value_type = T;

public:
	int x;

	custom_allocator() : x(rand()) { }

	custom_allocator(const custom_allocator& other) noexcept : x(other.x) { }

	template <class U>
	custom_allocator(const custom_allocator<U>& other) noexcept : x(other.x) { }

	~custom_allocator() = default;

	pointer allocate(size_type n)
	{
		return static_cast<pointer>(::operator new(n * sizeof(value_type)));
	}

	void deallocate(pointer p, size_type) noexcept
	{
		::operator delete(p);
	}

	bool operator==(const custom_allocator&) const
	{
		return true;
	}

	bool operator!=(const custom_allocator&) const
	{
		return false;
	}
};

template<auto N, auto... Ns>
struct is_npack_contain : std::bool_constant<((N == Ns) || ...)> { };

template<auto N, auto... Ns>
constexpr bool is_npack_contain_v = is_npack_contain<N, Ns...>::value;

enum ff
{
	dc, cc, mc, ca, ma
};

template<ff... Forbidden>
struct declarator
{
	template<typename Dummy = void>
	constexpr void check_dc()
	{
		if constexpr (is_npack_contain_v<dc, Forbidden...>) static_assert (!std::is_same_v<Dummy, Dummy>, "Default constructor is deleted");
	}

	template<typename Dummy = void>
	constexpr void check_cc()
	{
		if constexpr (is_npack_contain_v<cc, Forbidden...>) static_assert (!std::is_same_v<Dummy, Dummy>, "Copy constructor is deleted");
	}

	template<typename Dummy = void>
	constexpr void check_mc()
	{
		if constexpr (is_npack_contain_v<mc, Forbidden...>) static_assert (!std::is_same_v<Dummy, Dummy>, "Move constructor is deleted");
	}

	template<typename Dummy = void>
	constexpr void check_ca()
	{
		if constexpr (is_npack_contain_v<ca, Forbidden...>) static_assert (!std::is_same_v<Dummy, Dummy>, "Copy assignment is deleted");
	}

	template<typename Dummy = void>
	constexpr void check_ma()
	{
		if constexpr (is_npack_contain_v<ma, Forbidden...>) static_assert (!std::is_same_v<Dummy, Dummy>, "Move assignment is deleted");
	}

	int x;
	declarator(int x) : x(x) { }
	declarator() : x(0) { check_dc(); }
	declarator(const declarator& other) : x(other.x) { check_cc(); }
	declarator(declarator&& other) : x(std::move(other.x)) { check_mc(); }
	declarator& operator=(const declarator&) { check_ca(); return *this; }
	declarator& operator=(declarator&&) { check_ma(); return *this; }
};

using atype = custom_allocator<std::pair<const int, int>>;
using hmint = hash_map<int, int>;
using hminta = hash_map<int, int, std::hash<int>, std::equal_to<int>, atype>;

TEST_CASE("_ctor()")
{
	srand(time(nullptr));
	hmint m;
	REQUIRE(m.size() == 0);
}

TEST_CASE("_ctor(n)")
{
	hmint m(2048);
	REQUIRE(m.bucket_count() == 2048);
}

TEST_CASE("_ctor(a)")
{
	atype a(4);
	hminta m(a);
	REQUIRE(m.get_allocator().x == 4);
}

TEST_CASE("_ctor(l, n)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hmint m(l, 2048);
	REQUIRE(m[1] == 2);
	REQUIRE(m[3] == 4);
	REQUIRE(m.bucket_count() == 2048);
}

TEST_CASE("_ctor(b, e, n")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hmint m(l.begin(), l.end(), 2048);
	REQUIRE(m[1] == 2);
	REQUIRE(m[3] == 4);
	REQUIRE(m.bucket_count() == 2048);
}

TEST_CASE("_ctor(cref)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hminta m1(l.begin(), l.end(), 2048);
	hminta m2(m1);
	REQUIRE(m1[1] == m2[1]);
	REQUIRE(m1[3] == m2[3]);
	REQUIRE(m1.size() == m2.size());
	REQUIRE(m1.get_allocator().x == m2.get_allocator().x);
}

TEST_CASE("_ctor(cref, a)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hminta m1(l.begin(), l.end(), 2048);
	atype a;
	hminta m2(m1, a);
	REQUIRE(m1[1] == m2[1]);
	REQUIRE(m1[3] == m2[3]);
	REQUIRE(m1.size() == m2.size());
	REQUIRE(m2.get_allocator().x == a.x);
}

TEST_CASE("_ctor(rref)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hminta m1(l.begin(), l.end(), 2048);
	hminta m2(std::move(m1));
	REQUIRE(m2[1] == 2);
	REQUIRE(m2[3] == 4);
	REQUIRE(m1.get_allocator().x == m2.get_allocator().x);
}

TEST_CASE("_ctor(rref, a)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hminta m1(l.begin(), l.end(), 2048);
	atype a;
	hminta m2(std::move(m1), a);
	REQUIRE(m2[1] == 2);
	REQUIRE(m2[3] == 4);
	REQUIRE(m2.get_allocator().x == a.x);
}

TEST_CASE("_=(cref)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hminta m1(l.begin(), l.end(), 2048);
	hminta m2;
	m2 = m1;
	REQUIRE(m1[1] == m2[1]);
	REQUIRE(m1[3] == m2[3]);
	REQUIRE(m1.size() == m2.size());
	REQUIRE(m1.get_allocator().x != m2.get_allocator().x);
}

TEST_CASE("_=(rref)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hminta m1(l.begin(), l.end(), 2048);
	hminta m2;
	m2 = std::move(m1);
	REQUIRE(m2[1] == 2);
	REQUIRE(m2[3] == 4);
	REQUIRE(m2.size() == 2);
	REQUIRE(m1.get_allocator().x != m2.get_allocator().x);
}

TEST_CASE("_=(l)")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hmint m;
	m = l;
	REQUIRE(m[1] == 2);
	REQUIRE(m[3] == 4);
	REQUIRE(m.size() == 2);
}

TEST_CASE("_empty")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hmint m(l);
	REQUIRE(!m.empty());
	m = hmint();
	REQUIRE(m.empty());
}

TEST_CASE("_begin_end")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hmint m(l);
	unordered_map<int, int> sm(m.begin(), m.end());
	REQUIRE(sm.size() == 2);
	REQUIRE(sm[1] == 2);
	REQUIRE(sm[3] == 4);
}

TEST_CASE("_contains_count")
{
	std::initializer_list<pair<const int, int>> l{ {1, 2}, {3, 4} };
	hmint m(l);
	REQUIRE(m.contains(1));
	REQUIRE(m.count(3) == 1);
}

TEST_CASE("_emplace")
{
	hmint m;
	m.emplace(1, 2);
	REQUIRE(m.contains(1));
}

TEST_CASE("_insert_copy_deleted")
{
	hash_map<int, declarator<cc, ca>> m;
	pair<const int, declarator<cc, ca>> p(piecewise_construct, std::tuple<int>(1), std::tuple<>());
	m.insert(std::move(p));
}

TEST_CASE("_try_emplace")
{
	hash_map<int, declarator<dc, cc, ca>> m;
	m.try_emplace(1, 4);
	REQUIRE(m.at(1).x == 4);
}

TEST_CASE("_swap")
{
	hash_map<int, declarator<dc, cc, ca>> m1;
	hash_map<int, declarator<dc, cc, ca>> m2;
	m1.try_emplace(1, 4);
	m2.try_emplace(5, 2);
	m1.swap(m2);
	REQUIRE(m1.at(5).x == 2);
	REQUIRE(m2.at(1).x == 4);
}

TEST_CASE("_merge_ref")
{
	hash_map<int, declarator<dc>> m1;
	m1.try_emplace(1, 4);
	hash_map<int, declarator<dc>> m2;
	m2.try_emplace(5, 2);
	m1.merge(m2);
	REQUIRE(m1.at(5).x == 2);
	REQUIRE(m2.size() == 0);
}

TEST_CASE("_merge_rref")
{
	hash_map<int, declarator<dc, cc, ca>> m1;
	m1.try_emplace(1, 4);
	hash_map<int, declarator<dc, cc, ca>> m2;
	m2.try_emplace(5, 2);
	m1.merge(std::move(m2));
	REQUIRE(m1.at(5).x == 2);
}

TEST_CASE("_swap_compile_time_check")
{
	hash_map<int, declarator<dc, cc, ca, mc, ma>> m1;
	hash_map<int, declarator<dc, cc, ca, mc, ma>> m2;
	m1.swap(m2);
}

TEST_CASE("_insert_or_assign_compile_time_check")
{
	hash_map<int, declarator<dc, cc, ca>> m;
	m.insert_or_assign(1, 2);
}

TEST_CASE("_load_factor")
{
	hmint m;
	for (int i = 0; i < 100; ++i)
		m.insert({ i, 1 });
	for (int i = 0; i < 100; ++i)
		m.erase(i);
	REQUIRE(m.load_factor() > 0.0f);
}

template<typename TimePoint>
class time_meter
{
public:
	using time_point = TimePoint;

private:
	std::function<time_point()> m_fnow;
	std::function<double(time_point, time_point)> m_fsec;
	time_point m_begin;
	time_point m_stop;
	bool m_stopped;

public:
	template<typename FNow, typename FSec>
	time_meter(FNow&& now, FSec&& sec) : m_fnow(std::forward<FNow>(now)), m_fsec(std::forward<FSec>(sec)), m_begin(m_fnow()), m_stopped(false) { }

	double seconds() const
	{
		if (m_stopped)
			return m_fsec(m_begin, m_stop);
		return m_fsec(m_begin, m_fnow());
	}

	void restart()
	{
		m_stopped = false;
		m_begin = m_fnow();
	}

	void stop()
	{
		if (m_stopped)
			return;
		m_stop = m_fnow();
		m_stopped = true;
	}

	void start()
	{
		if (!m_stopped)
			return;
		m_stopped = false;
		m_begin += m_fnow() - m_stop;
	}
};

auto create_tm()
{
	using tm_type = time_meter<std::chrono::high_resolution_clock::time_point>;

	static const auto get_sec = [](tm_type::time_point p1, tm_type::time_point p2)
	{
		return static_cast<double>((p2 - p1).count()) / std::chrono::high_resolution_clock::period::den;
	};

	return tm_type(std::chrono::high_resolution_clock::now, get_sec);
}

struct custom_hash
{
	vector<long long unsigned int> rnd;

	custom_hash()
	{
		for (int i = 0; i < 1000000; ++i)
			rnd.push_back(rand());
	}

	long long unsigned int operator()(int i) const
	{
		return rnd[i];
	}
};

TEST_CASE("_stress")
{
	auto tm = create_tm();
	hash_map<int, int, custom_hash> m(1000000);
	m.max_load_factor(0.1f);
	for (int i = 0; i < 1000000; ++i)
	{
		m.insert({ i, i * 3 });
	}
	for (int i = 100; i < 999999; ++i)
	{
		m.erase(i);
	}
	for (int i = 0; i < 1000000; ++i)
	{
		m.insert({ i, i * 3 });
	}
	for (int i = 0; i < 1000000; ++i)
	{
		m.insert({ i, i * 3 });
	}
	for (int i = 100; i < 999999; ++i)
	{
		m.erase(i);
	}
	for (int i = 100; i < 999999; ++i)
	{
		m.erase(i);
	}
	for (int i = 0; i < 100; ++i)
		REQUIRE(m[i] == i * 3);
	REQUIRE(m[999999] == 2999997);
	REQUIRE(m.size() == 101);
	for (int i = 0; i < 1000; ++i)
		m.insert({ rand() % 1000000, rand() });
	cout << "**********\n" << "STRESS TIME = " << tm.seconds() << "\n**********" << endl;
}

© 2019 GitHub, Inc.
Terms
Privacy
Security
Status
Help
Contact GitHub
Pricing
API
Training
Blog
About
