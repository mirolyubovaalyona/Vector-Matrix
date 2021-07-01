#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <type_traits>

//#include <utility>
#include <cstdint>
#include <cstring>

namespace fefu
{
	const static size_t D			= 3;
	const static size_t INIT_LENGTH = 0;
	const static size_t MAX_LOOP	= 100;
	const static double STEP		= 1.2;


template<typename T>
class allocator {
public:
    using size_type			= std::size_t;
    using difference_type	= std::ptrdiff_t;
    using pointer			= T*;
    using const_pointer		= const T*;
    using reference			= typename std::add_lvalue_reference<T>::type;
    using const_reference	= typename std::add_lvalue_reference<const T>::type;
    using value_type		= T;

	allocator() noexcept = default;

	allocator(const allocator&) noexcept{}
	
    template <class U>
	allocator(const allocator<U>&) noexcept {}
	
	~allocator() = default;

	pointer allocate(size_type n) {
		return new T[n];
	}
	
	void deallocate(pointer p, size_type n) noexcept {
		delete[] p;
	}
};

template<typename K, typename T,
	typename Hash = std::hash<K>,
	typename Pred = std::equal_to<K>,
	typename Alloc = allocator<std::pair<const K, T>>>
	class hash_map;

template<typename K, typename T,
	typename Hash,
	typename Pred,
	typename Alloc>
class hash_map_iterator {
	using this_type = hash_map_iterator<K, T, Hash, Pred, Alloc>;
	using map_type = hash_map<K, T, Hash, Pred, Alloc>;
	size_t pos;
	map_type* hash;
	hash_map_iterator(size_t p, class map_type* h) : pos(p), hash(h) {}

public:
    using iterator_category = std::forward_iterator_tag;
	using value_type = std::pair<const K, T>;

    using difference_type	= std::ptrdiff_t;
    using reference			= value_type&;
    using pointer			= value_type*;


    hash_map_iterator() noexcept : pos(0), hash(NULL) {}
	hash_map_iterator(const hash_map_iterator& other) noexcept {
		*this = other;
	}

	reference operator*() const {
		return (*hash).data_from(pos);
	}
	pointer operator->() const {
		return &((*hash).data_from(pos));
	}

    // prefix ++
	hash_map_iterator& operator++() {
		assert(hash != NULL);
		if (pos != hash->len_) ++pos;
		while ((pos < hash->len_) && (!(hash->get_exists(pos)))) {
			++pos;
		}
		return *this;
	}
    // postfix ++
	hash_map_iterator operator++(int) {
		hash_map_iterator res = *this;
		this->operator++();
		return res;
	}

	friend bool operator==(const this_type& it, const this_type& other) {
		return (other.pos == it.pos) && (other.hash == it.hash);
	}
	friend bool operator!=(const this_type& it, const this_type& other) {
		return !(other == it);
	}
};

template<typename K, typename T,
	typename Hash,
	typename Pred,
	typename Alloc>
class hash_map_const_iterator {
// Shouldn't give non const references on value
	using this_type = hash_map_iterator<K, T, Hash, Pred, Alloc>;
	using map_type = hash_map<K, T, Hash, Pred, Alloc>;
	size_t pos;
	const map_type* hash;
	hash_map_const_iterator(const  size_t p, const  class map_type* h) : pos(p), hash(h) {}

public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = std::pair<const K, T>;
    using difference_type	= std::ptrdiff_t;
    using reference			= const value_type&;
    using pointer			= const value_type*;

    hash_map_const_iterator() noexcept : pos(0), hash(NULL) {}

	hash_map_const_iterator(const hash_map_const_iterator& other) noexcept {
		*this = other;
	}
	hash_map_const_iterator(const hash_map_iterator<K, T, Hash, Pred, Alloc>& other) noexcept {
		*this = other;
	}

	reference operator*() const {
		return (*hash).data_from(pos);
	}
	pointer operator->() const {
		return &((*hash).data_from(pos));
	}

    // prefix ++
	hash_map_const_iterator& operator++() {
		assert(hash != NULL);
		if (pos != hash->len_) ++pos;
		while ((pos < hash->len_) && (!(hash->get_exists(pos)))) {
			++pos;
		}
		return *this;
	}
    // postfix ++
	hash_map_const_iterator operator++(int) {
		hash_map_const_iterator res = *this;
		this->operator++();
		return res;
	}

	friend bool operator==(const this_type& it, const this_type& other) {
		return (other.pos == it.pos) && (other.hash == it.hash);
	}
	friend bool operator!=(const this_type& it, const this_type& other) {
		return !(other == it);
	}
};



template<typename K, typename T,
	   typename Hash,
	   typename Pred,
	   typename Alloc>
class hash_map
{
public:
    using key_type			= K;
    using mapped_type		= T;
    using hasher			= Hash;
    using key_equal			= Pred;
    using allocator_type	= Alloc;
    using value_type		= std::pair<const key_type, mapped_type>;
    using reference			= value_type&;
    using const_reference	= const value_type&;
    using iterator			= hash_map_iterator<K, T, Hash, Pred, Alloc>;
    using const_iterator	= hash_map_const_iterator<K, T, Hash, Pred, Alloc>;
    using size_type			= std::size_t;

	friend class const_iterator;
	friend class iterator;


private:
	typedef value_type Data;
	size_t d_;
	size_t init_length_;
	size_t max_loop_;
	double step_; //Коэффициент увеличения размера хэша во время
	Data** data_;
	char* exists_;//Массив флагов, указывающих на наличие элемента в хэше.
	size_t len_; //Общая длина всех хеш-массивов.
	size_t len_part_;//Длина каждого хеш-массива.
	size_t size_;//Фактическое количество элементов в хеше 
	bool is_rehashed_;
	hasher hasher_;
	key_equal key_equal_;
	allocator_type allocator_type_;

	void init() {
		len_part_ = init_length_ / d_ + 1;
		len_part_ = ((len_part_ + 7) >> 3) << 3;
		len_ = len_part_ * d_;
		data_ = (Data**)(malloc(d_ * sizeof(Data*)));
		for (size_t i = 0; i < d_; ++i) {
			data_[i] = (Data*)(malloc(len_part_ * sizeof(Data)));
		}
		//data_ = allocate(len_);
		exists_ = (char*)(malloc((len_ >> 3) * sizeof(char)));
		for (size_t i = 0; i < len_ >> 3; ++i) exists_[i] = 0;
		size_ = 0;
		is_rehashed_ = false;
	}


	void clear_all() {//деалокейт
		for (size_t i = 0; i < d_; ++i) {
			free(data_[i]);
		}
		free(data_);
		free(exists_);
	}


	/*Добавить новый элемент данных.
	* p Новый элемент.
	* return 0, чтобы завершить рекурсию, если это было необходимо.*/
	size_type add_new(Data p) {
		for (size_type i = 0; i < max_loop_; ++i) {
			for (size_type j = 0; j < d_; ++j) {
				size_type pos = hash_function(p.first, j);
				std::swap(p, data_[j][pos]);
				bool exists = get_exists(j * len_part_ + pos);
				set_exists(j * len_part_ + pos);
				if (!exists) {
					++size_;
					return 0;
				}
			}
		}
		rehash();
		return add_new(p);
	}

	//удалить элеент
	iterator remove(iterator& it) {
		unset_exists(it.pos);
		--size_;
		return ++it;
	}

	 Data& data_from(size_t pos) const {
		return data_[pos / len_part_][pos % len_part_];
	}

	 //есть ли эд-т в масиве
	 bool get_exists(size_t pos) const {
		 return (bool)(exists_[pos >> 3] & (1 << (pos - ((pos >> 3) << 3))));
	 }

	 //Установить флаг существования элемента дата в пос
	 void set_exists(size_t pos) {
		 exists_[pos >> 3] = exists_[pos >> 3] | (1 << (pos - ((pos >> 3) << 3)));
	 }

	 //отзываеит флаг существования
	 void unset_exists(size_t pos) {
		 exists_[pos >> 3] = exists_[pos >> 3] & (~(1 << (pos - ((pos >> 3) << 3))));
	 }


	 //Проверьте, есть ли у элемента в pos ключ k
	  bool is_here(const key_type& k, size_t pos) const {
		 return get_exists(pos) && key_equal_(data_from(pos).first, k);
	 }

public:
    /// Default constructor.
	hash_map() {
		d_ = D;
		init_length_ = INIT_LENGTH;
		max_loop_ = MAX_LOOP;
		step_ = STEP;
		hasher_ = hasher();
		key_equal_ = key_equal();
		allocator_type_ = get_allocator();
		init();
	}
    /**
     *  @brief  Default constructor creates no elements.
     *  @param n  Minimal initial number of buckets.
     */
	explicit hash_map(size_type n) {
		d_ = D;
		init_length_ = n;
		max_loop_ = MAX_LOOP;
		step_ = STEP;
		hasher_ = hasher();
		key_equal_ = key_equal();
		allocator_type_ = get_allocator();
		init();
	}

    /**
     *  @brief  Builds an %hash_map from a range.
     *  @param  first  An input iterator.
     *  @param  last  An input iterator.
     *  @param  n  Minimal initial number of buckets.
     *
     *  Create an %hash_map consisting of copies of the elements from
     *  [first,last).  This is linear in N (where N is
     *  distance(first,last)).
     */
    template<typename InputIterator>
	hash_map(InputIterator first, InputIterator last,
		size_type n = 0) {
		d_ = D;
		init_length_ = INIT_LENGTH;
		max_loop_ = MAX_LOOP;
		step_ = STEP;
		hasher_ = hasher();
		key_equal_ = key_equal();
		allocator_type_ = get_allocator();
		init();
		size_t last_pos = last.pos; //TODO insert
		for (iterator it = first; it.pos != last_pos; ++it) {
			add_new(*it);
		}
	}

    /// Copy constructor.
	hash_map(const hash_map& other) {
		d_ = other.d_;
		init_length_ = other.init_length_;
		max_loop_ = other.max_loop_;
		step_ = other.step_;
		hasher_ = other.hasher_;
		key_equal_ = other.key_equal_;
		allocator_type_ = other.allocator_type_;
		init();
		const_iterator it = other.begin();
		size_type final_len = other.length(); // TODO insert
		while (it.pos != final_len) {
			insert(*it);
			++it;
		}
	}

    /// Move constructor.
	hash_map(hash_map&& other) {
		this.swap(other);
	}

    /**
     *  @brief Creates an %hash_map with no elements.
     *  @param a An allocator object.
     */
	explicit hash_map(const allocator_type& a) {
		d_ = D;
		init_length_ = INIT_LENGTH;
		max_loop_ = MAX_LOOP;
		step_ = STEP;
		hasher_ = hasher();
		key_equal_ = key_equal();
		allocator_type_ = a;
		init();

	}

    /*
    *  @brief Copy constructor with allocator argument.
    * @param  uset  Input %hash_map to copy.
    * @param  a  An allocator object.
    */
	hash_map(const hash_map& umap,
		const allocator_type& a) {
		d_ = umap.d_;
		init_length_ = umap.init_length_;
		max_loop_ = umap.max_loop_;
		step_ = umap.step_;
		hasher_ = umap.hasher_;
		key_equal_ = umap.key_equal_;
		allocator_type_ = a;
		init();
		const_iterator it = umap.begin();
		size_type final_len = umap.length();
		while (it.pos != final_len) {
			insert(*it);
			++it;
		}
	}

    /*
    *  @brief  Move constructor with allocator argument.
    *  @param  uset Input %hash_map to move.
    *  @param  a    An allocator object.
    */
	hash_map(hash_map&& umap,
		const allocator_type& a) {
		std::swap(d_, umap.d_);
		std::swap(init_length_, umap.init_length_);
		std::swap(max_loop_, umap.max_loop_);
		std::swap(step_, umap.step_);
		std::swap(hasher_, umap.hasher_);
		std::swap(key_equal_, umap.key_equal_);
		std::swap(data_, umap.data_);
		std::swap(exists_, umap.exists_);
		std::swap(len_, umap.len_);
		std::swap(len_part_, umap.len_part_);
		std::swap(size_, umap.size_);
		allocator_type_ = a;
		init();
		const_iterator it = umap.begin();
		size_type final_len = umap.length();
		while (it.pos != final_len) {
			insert(*it);
			++it;
		}
	}

    /**
     *  @brief  Builds an %hash_map from an initializer_list.
     *  @param  l  An initializer_list.
     *  @param n  Minimal initial number of buckets.
     *
     *  Create an %hash_map consisting of copies of the elements in the
     *  list. This is linear in N (where N is @a l.size()).
     */
	hash_map(std::initializer_list<value_type> l,
		size_type n = 0) {
	}

    /// Copy assignment operator.
	hash_map& operator=(const hash_map& other) {
		clear_all();
		d_ = other.d_;
		init_length_ = other.init_length_;
		max_loop_ = other.max_loop_;
		step_ = other.step_;
		hasher_ = other.hasher_;
		key_equal_ = other.key_equal_;
	    allocator_type_ = other.allocator_type_;
		init();
		const_iterator it = other.begin();
		size_type final_len = other.length();
		while (it.pos != final_len) {
			insert(*it);
			++it;
		}
		return *this;
	}

    /// Move assignment operator.
	hash_map& operator=(hash_map&& other) {
		return *this.swap(other);
	}

    /**
     *  @brief  %hash_map list assignment operator.
     *  @param  l  An initializer_list.
     *
     *  This function fills an %hash_map with copies of the elements in
     *  the initializer list @a l.
     *
     *  Note that the assignment completely changes the %hash_map and
     *  that the resulting %hash_map's size is the same as the number
     *  of elements assigned.
     */
	hash_map& operator=(std::initializer_list<value_type> l) {

	}

    ///  Returns the allocator object used by the %hash_map.
	allocator_type get_allocator() const noexcept {
		return this.allocator();
	}

    // size and capacity:

    ///  Returns true if the %hash_map is empty.
	bool empty() const noexcept {
		return size_ == 0;
	}

    ///  Returns the size of the %hash_map.
	size_type size() const noexcept {
		return len_;
	}

    ///  Returns the maximum size of the %hash_map.
	size_type max_size() const noexcept {
		return std::numeric_limits<int>::max();
	}

    // iterators.

    /**
     *  Returns a read/write iterator that points to the first element in the
     *  %hash_map.
     */
	iterator begin() noexcept {
		iterator it = iterator(0, this);
		if (!get_exists(it.pos)) ++it;
		return it;
	}

    //@{
    /**
     *  Returns a read-only (constant) iterator that points to the first
     *  element in the %hash_map.
     */
	const_iterator begin() const noexcept {
		const_iterator it = const_iterator(0, this);
		if (!get_exists(it.pos)) ++it;
		return it;
	}

	const_iterator cbegin() const noexcept {
		return begin();
	}

    /**
     *  Returns a read/write iterator that points one past the last element in
     *  the %hash_map.
     */
	iterator end() noexcept {
		return iterator(len_, this);
	}

    //@{
    /**
     *  Returns a read-only (constant) iterator that points one past the last
     *  element in the %hash_map.
     */
	const_iterator end() const noexcept {
		return const_iterator(len_, this);
	}

	const_iterator cend() const noexcept {
		return end();
	}
    //@}

    // modifiers.

    /**
     *  @brief Attempts to build and insert a std::pair into the
     *  %hash_map.
     *
     *  @param args  Arguments used to generate a new pair instance (see
     *	        std::piecewise_contruct for passing arguments to each
    *	        part of the pair constructor).
    *
    *  @return  A pair, of which the first element is an iterator that points
    *           to the possibly inserted pair, and the second is a bool that
    *           is true if the pair was actually inserted.
    *
    *  This function attempts to build and insert a (key, value) %pair into
    *  the %hash_map.
    *  An %hash_map relies on unique keys and thus a %pair is only
    *  inserted if its first element (the key) is not already present in the
    *  %hash_map.
    *
    *  Insertion requires amortized constant time.
    */
    template<typename... _Args>
	std::pair<iterator, bool> emplace(_Args&&... args) {

	}

    /**
     *  @brief Attempts to build and insert a std::pair into the
     *  %hash_map.
     *
     *  @param k    Key to use for finding a possibly existing pair in
     *                the hash_map.
     *  @param args  Arguments used to generate the .second for a
     *                new pair instance.
     *
     *  @return  A pair, of which the first element is an iterator that points
     *           to the possibly inserted pair, and the second is a bool that
     *           is true if the pair was actually inserted.
     *
     *  This function attempts to build and insert a (key, value) %pair into
     *  the %hash_map.
     *  An %hash_map relies on unique keys and thus a %pair is only
     *  inserted if its first element (the key) is not already present in the
     *  %hash_map.
     *  If a %pair is not inserted, this function has no effect.
     *
     *  Insertion requires amortized constant time.
     */
    template <typename... _Args>
	std::pair<iterator, bool> try_emplace(const key_type& k, _Args&&... args) {

	}

    // move-capable overload
    template <typename... _Args>
	std::pair<iterator, bool> try_emplace(key_type&& k, _Args&&... args) {

	}

    //@{
    /**
     *  @brief Attempts to insert a std::pair into the %hash_map.
     *  @param x Pair to be inserted (see std::make_pair for easy
     *	     creation of pairs).
    *
    *  @return  A pair, of which the first element is an iterator that
    *           points to the possibly inserted pair, and the second is
    *           a bool that is true if the pair was actually inserted.
    *
    *  This function attempts to insert a (key, value) %pair into the
    *  %hash_map. An %hash_map relies on unique keys and thus a
    *  %pair is only inserted if its first element (the key) is not already
    *  present in the %hash_map.
    *
    *  Insertion requires amortized constant time.
    */
	std::pair<iterator, bool> insert(const value_type& x) {
		iterator res = find(x.first);
		if (res.pos != len_) {
			return make_pair(res, false);
		}
		add_new(x);
		return make_pair(iterator(hash_function(x.first, 0), this), true);
	}

	std::pair<iterator, bool> insert(value_type&& x) {
		insert(x);
	}

    //@}

    /**
     *  @brief A template function that attempts to insert a range of
     *  elements.
     *  @param  first  Iterator pointing to the start of the range to be
     *                   inserted.
     *  @param  last  Iterator pointing to the end of the range.
     *
     *  Complexity similar to that of the range constructor.
     */
    template<typename _InputIterator>
	void insert(_InputIterator first, _InputIterator last) {
		size_t last_pos = last.pos;
		for (iterator it = first; it.pos != last_pos; ++it) {
			insert(*it);
		}
	}

    /**
     *  @brief Attempts to insert a list of elements into the %hash_map.
     *  @param  l  A std::initializer_list<value_type> of elements
     *               to be inserted.
     *
     *  Complexity similar to that of the range constructor.
     */
	void insert(std::initializer_list<value_type> l) {
		for (auto val : l) {};
	}


    /**
     *  @brief Attempts to insert a std::pair into the %hash_map.
     *  @param k    Key to use for finding a possibly existing pair in
     *                the map.
     *  @param obj  Argument used to generate the .second for a pair
     *                instance.
     *
     *  @return  A pair, of which the first element is an iterator that
     *           points to the possibly inserted pair, and the second is
     *           a bool that is true if the pair was actually inserted.
     *
     *  This function attempts to insert a (key, value) %pair into the
     *  %hash_map. An %hash_map relies on unique keys and thus a
     *  %pair is only inserted if its first element (the key) is not already
     *  present in the %hash_map.
     *  If the %pair was already in the %hash_map, the .second of
     *  the %pair is assigned from obj.
     *
     *  Insertion requires amortized constant time.
     */
    template <typename _Obj>
	std::pair<iterator, bool> insert_or_assign(const key_type& k, _Obj&& obj) {

	}

    // move-capable overload
    template <typename _Obj>
	std::pair<iterator, bool> insert_or_assign(key_type&& k, _Obj&& obj) {

	}

    //@{
    /**
     *  @brief Erases an element from an %hash_map.
     *  @param  position  An iterator pointing to the element to be erased.
     *  @return An iterator pointing to the element immediately following
     *          @a position prior to the element being erased. If no such
     *          element exists, end() is returned.
     *
     *  This function erases an element, pointed to by the given iterator,
     *  from an %hash_map.
     *  Note that this function only erases the element, and that if the
     *  element is itself a pointer, the pointed-to memory is not touched in
     *  any way.  Managing the pointer is the user's responsibility.
     */
	iterator erase(const_iterator position) {
		remove(position);
	}

    // LWG 2059.
	iterator erase(iterator position) {
		remove(position);
	}
    //@}

    /**
     *  @brief Erases elements according to the provided key.
     *  @param  x  Key of element to be erased.
     *  @return  The number of elements erased.
     *
     *  This function erases all the elements located by the given key from
     *  an %hash_map. For an %hash_map the result of this function
     *  can only be 0 (not present) or 1 (present).
     *  Note that this function only erases the element, and that if the
     *  element is itself a pointer, the pointed-to memory is not touched in
     *  any way.  Managing the pointer is the user's responsibility.
     */
	size_type erase(const key_type& x) {
		iterator it = find(x);
		if (it.pos != len_) {
			remove(it);
			return 1;
		}
		return 0;
	}

    /**
     *  @brief Erases a [first,last) range of elements from an
     *  %hash_map.
     *  @param  first  Iterator pointing to the start of the range to be
     *                  erased.
     *  @param last  Iterator pointing to the end of the range to
     *                be erased.
     *  @return The iterator @a last.
     *
     *  This function erases a sequence of elements from an %hash_map.
     *  Note that this function only erases the elements, and that if
     *  the element is itself a pointer, the pointed-to memory is not touched
     *  in any way.  Managing the pointer is the user's responsibility.
     */
	iterator erase(const_iterator first, const_iterator last) {
		while (first.pos != last.pos) {
			first = remove(first);
		}
	}

    /**
     *  Erases all elements in an %hash_map.
     *  Note that this function only erases the elements, and that if the
     *  elements themselves are pointers, the pointed-to memory is not touched
     *  in any way.  Managing the pointer is the user's responsibility.
     */
	void clear() noexcept {
		clear_all();
		init();
	}

    /**
     *  @brief  Swaps data with another %hash_map.
     *  @param  x  An %hash_map of the same element and allocator
     *  types.
     *
     *  This exchanges the elements between two %hash_map in constant
     *  time.
     *  Note that the global std::swap() function is specialized such that
     *  std::swap(m1,m2) will feed to this function.
     */
	void swap(hash_map& x) {
		std::swap(d_, x.d_);
		std::swap(init_length_, x.init_length_);
		std::swap(max_loop_, x.max_loop_);
		std::swap(step_, x.step_);
		std::swap(hasher_, x.hasher_);
		std::swap(key_equal_, x.key_equal_);
		std::swap(allocator_type_, x.allocator_type_);
		std::swap(data_, x.data_);
		std::swap(exists_, x.exists_);
		std::swap(len_, x.len_);
		std::swap(len_part_, x.len_part_);
		std::swap(size_, x.size_);
	}

    template<typename _H2, typename _P2>
	void merge(hash_map<K, T, _H2, _P2, Alloc>& source) {

	}

    template<typename _H2, typename _P2>
	void merge(hash_map<K, T, _H2, _P2, Alloc>&& source) {

	}

    // observers.

    ///  Returns the hash functor object with which the %hash_map was
    ///  constructed.
	Hash hash_function(const key_type& k, size_type hash_num) const {
		return hasher_(k, hash_num) % len_part_;
	}

    ///  Returns the key comparison object with which the %hash_map was
    ///  constructed.
	

    // lookup.

    //@{
    /**
     *  @brief Tries to locate an element in an %hash_map.
     *  @param  x  Key to be located.
     *  @return  Iterator pointing to sought-after element, or end() if not
     *           found.
     *
     *  This function takes a key and tries to locate the element with which
     *  the key matches.  If successful the function returns an iterator
     *  pointing to the sought after element.  If unsuccessful it returns the
     *  past-the-end ( @c end() ) iterator.
     */
	iterator find(const key_type& x) {
		size_t dist = 0;
		for (size_t i = 0; i < d_; ++i) {
			size_t pos = hash_function(x, i);
			size_t position = pos + dist;
			if (key_equal_(data_[i][pos].first, x) && (get_exists(position))) {
				return iterator(position, this);
			}
			dist += len_part_;
		}
		return end();

	}

	const_iterator find(const key_type& x) const {
		size_t dist = 0;
		for (size_t i = 0; i < d_; ++i) {
			size_t pos = hash_function(x, i);
			size_t position = pos + dist;
			if (key_equal_(data_[i][pos].first, x) && (get_exists(position))) {
				return const_iterator(position, this);
			}
			dist += len_part_;
		}
		return end();
	}
    //@}

    /**
     *  @brief  Finds the number of elements.
     *  @param  x  Key to count.
     *  @return  Number of elements with specified key.
     *
     *  This function only makes sense for %unordered_multimap; for
     *  %hash_map the result will either be 0 (not present) or 1
     *  (present).
     */
	size_type count(const key_type& x) const {
		return (find(x)).pos != len_ ? 1 : 0;
	}

    /**
     *  @brief  Finds whether an element with the given key exists.
     *  @param  x  Key of elements to be located.
     *  @return  True if there is any element with the specified key.
     */
	bool contains(const key_type& x) const {
		iterator it = find(x);
		if (it.pos != len_) {
			return true;
		}
		return false;
	}

    //@{
    /**
     *  @brief  Subscript ( @c [] ) access to %hash_map data.
     *  @param  k  The key for which data should be retrieved.
     *  @return  A reference to the data of the (key,data) %pair.
     *
     *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
     *  data associated with the key specified in subscript.  If the key does
     *  not exist, a pair with that key is created using default values, which
     *  is then returned.
     *
     *  Lookup requires constant time.
     */
	mapped_type& operator[](const key_type& k) {
		return (*((this->insert(make_pair(k, mapped_type()))).first)).second;
	}

	mapped_type& operator[](key_type&& k) {
		return (*((this->insert(make_pair(std::move(k), mapped_type()))).first)).second;
	}
    //@}

    //@{
    /**
     *  @brief  Access to %hash_map data.
     *  @param  k  The key for which data should be retrieved.
     *  @return  A reference to the data whose key is equal to @a k, if
     *           such a data is present in the %hash_map.
     *  @throw  std::out_of_range  If no such data is present.
     */
	mapped_type& at(const key_type& k) {
		return (operator[](k)).second;
			//throw std::invalid_argument();
	}

	const mapped_type& at(const key_type& k) const {

	}
    //@}

    // bucket interface.

    /// Returns the number of buckets of the %hash_map.
	size_type bucket_count() const noexcept {
		return size_;
	}

    /*
    * @brief  Returns the bucket index of a given element.
    * @param  _K  A key instance.
    * @return  The key bucket index.
    */
	size_type bucket(const key_type& _K) const {
		
	}

    // hash policy.

    /// Returns the average number of elements per bucket.
	float load_factor() const noexcept {
		return static_cast<float>(bucket_count()) / static_cast<float>(size());
	}

    /// Returns a positive number that the %hash_map tries to keep the
    /// load factor less than or equal to.
	float max_load_factor() const noexcept {

	}

    /**
     *  @brief  Change the %hash_map maximum load factor.
     *  @param  z The new maximum load factor.
     */
	void max_load_factor(float z) {

	}

    /**
     *  @brief  May rehash the %hash_map.
     *  @param  n The new number of buckets.
     *
     *  Rehash will occur only if the new number of buckets respect the
     *  %hash_map maximum load factor.
     */
	void rehash(size_type n) {
		size_type len_temp_ = len_part_;

		len_part_ = (size_type)(len_part_ * step_);
		len_part_ = ((len_part_ + 7) >> 3) << 3;
		len_ = len_part_ * d_;

		//update_exists(len_temp_);  Увеличьте размер exist_ до len_temp_.
		char* t = (char*)(malloc((len_ >> 3) * sizeof(char)));
		char* s = t;
		char* f = s + (len_ >> 3);
		for (; s < f; ++s) {
			*s = 0;
		}
		for (size_type i = 0; i < d_; ++i) {
			memcpy(t + (i * (len_part_ >> 3)), exists_ + (i * (len_temp_ >> 3)), (len_temp_ >> 3));
		}
		std::swap(t, exists_);
		free(t);
		//update_data(len_temp_);Увеличьте размер data_ до len_temp_.
		for (size_type i = 0; i < d_; ++i) {
			Data* t = (Data*)(malloc(len_part_ * sizeof(Data)));
			memcpy(t, data_[i], len_temp_ * sizeof(Data));
			std::swap(t, data_[i]);
			free(t);
		}

		size_type n = 0;
		iterator it = begin();
		while (it != end()) {
			size_t i = it.pos / len_part_;
			size_t j = it.pos % len_part_;
			++n;
			if (j != hash_function((*it).first, i)) {
				Data t = *it;
				remove(it);
				add_new(t);
				if (is_rehashed_) {
					it = begin();
					is_rehashed_ = false;
				}
			}
			else {
				++it;
			}
		}
		is_rehashed_ = true;
	}

    /**
     *  @brief  Prepare the %hash_map for a specified number of
     *          elements.
     *  @param  n Number of elements required.
     *
     *  Same as rehash(ceil(n / max_load_factor())).
     */
	void reserve(size_type n) {
		rehash(n);
	}

	bool operator==(const hash_map& other) const {
		return (*this).data_ == other.data_;
	}
};

} // namespace fefu
