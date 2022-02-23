#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

template <typename stack_pool, typename T, typename N = std::size_t>
class _iterator {
    N current_stack;
    stack_pool *pool;

public:
    using stack_type = N;

    using value_type = T;
    using reference = value_type &;
    using pointer = value_type *;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    _iterator(stack_type x, stack_pool &pool) : current_stack{x}, pool{&pool} {}
    reference operator*() const {
        return pool->value(current_stack);
    }
    _iterator &operator++() { 			// pre-increment
        current_stack = pool->next(current_stack);
        return *this;
    }

    friend bool operator==(const _iterator &x, const _iterator &y) {
        return x.current_stack == y.current_stack;
    }

    friend bool operator!=(const _iterator &x, const _iterator &y) {
        return !(x == y);
    }
};

template <typename T, typename N = std::size_t>
class stack_pool {
    struct node_t {
        T value;
        N next;

        node_t(const T &v, N &n) : value{v}, next{n} {}
    };

    std::vector<node_t> pool;
    using stack_type = N;
    using value_type = T;
    using size_type = typename std::vector<node_t>::size_type;
    stack_type free_nodes; // at the beginning, it is empty

    node_t &node(stack_type x) noexcept { return pool[x - 1]; }
    const node_t &node(stack_type x) const noexcept { return pool[x - 1]; }
    
    stack_type head_to_freenode(T&& val, stack_type head) {	/// cannot be noexcept: the auto = acquire resources
    	auto tmp = head;
    	head = free_nodes;
    	value(head) = std::forward<T>(val);
    	next(head) = tmp;
    	free_nodes = next(free_nodes);
    	return head;
    }
    
    template<typename V>
    stack_type _push(V&& val, stack_type head) {		/// cannot be noexcept because uses a fucntion that is not noexcept
    	if (free_nodes != end() ){
    	    head = head_to_freenode(std::forward<V>(val), head);
    	} else {
    	    pool.push_back(node_t(std::forward<V>(val), head));
    	    head = pool.size();
    	}
    	return head;
    }
    
public:
    stack_pool() {
        reserve(1024);
        free_nodes = stack_type(0); // or maybe is better to use free_nodes = end(); ?
    }
    explicit stack_pool(size_type n) { // reserve n nodes in the pool
        reserve(n);
        free_nodes = stack_type(0); // or maybe is better to use free_nodes = end(); ?
    }
    


    using iterator = _iterator<stack_pool, value_type, stack_type>;
    using const_iterator = _iterator<stack_pool, const value_type, stack_type>;

    iterator begin(stack_type x) { return iterator(x, *this); };
    iterator end(stack_type) { return iterator(end(), *this); }; // this is not a typo

    const_iterator begin(stack_type x) const { return const_iterator(x, *this); }
    const_iterator end(stack_type) const { return const_iterator(end(), *this); }

    const_iterator cbegin(stack_type x) const { return const_iterator(x, *this); }
    const_iterator cend(stack_type) const { return const_iterator(end(), *this); }



    stack_type new_stack() noexcept { // return an empty stack
        return end();
    }

    void reserve(size_type n) { // reserve n nodes in the pool /// cannot be noexcept: acquires resources 
        pool.reserve(n);
    }

    size_type capacity() const noexcept { // the capacity of the pool
        return pool.capacity();
    }

    bool empty(stack_type x) const {
        return x == end();
    }

    stack_type end() const noexcept { return stack_type(0); }

    T &value(stack_type x) {					/// cannot be noexcept: allows to set value: it could be of wrong type
        return node(x).value;
    }
    const T &value(stack_type x) const noexcept {
        return node(x).value;
    }

    stack_type &next(stack_type x) {				/// cannot be noexcept: allows to set next: it could be of wrong type
        return node(x).next;
    }
    const stack_type &next(stack_type x) const noexcept {
        return node(x).next;
    }
    
    stack_type push(const T &val, stack_type head) {		/// cannot be noexcept because uses a fucntion that is not noexcept
	stack_type h = _push(val, head);
        return h;
    }

    stack_type push(T &&val, stack_type head) {		/// cannot be noexcept because uses a fucntion that is not noexcept
	stack_type h = _push(std::forward<T>(val), head);
        return h;
    }

    stack_type pop(stack_type x) { // delete first node	/// cannot be noexcept because auto = acquires resources
        auto tmp = free_nodes;
        free_nodes = x;
        x = next(x);
        next(free_nodes) = tmp;
        return x;
    }

    stack_type free_stack(stack_type x) { // free entire stack
        if (not empty(x))
            while ( x != end() )
                x = pop(x);
        return x;
    }
};

