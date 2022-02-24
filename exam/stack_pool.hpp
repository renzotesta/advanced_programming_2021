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
        
    };

    std::vector<node_t> pool;
    using stack_type = N;
    using value_type = T;
    using size_type = typename std::vector<node_t>::size_type;
    stack_type free_nodes; // at the beginning, it is empty

    node_t &node(stack_type x) noexcept { return pool[x - 1]; }
    const node_t &node(stack_type x) const noexcept { return pool[x - 1]; }
    
    stack_type head_to_freenode(stack_type head) {
    	auto tmp = head;
    	head = free_nodes;
    	next(head) = tmp;
    	free_nodes = next(free_nodes);
    	return head;
    }
    
    template<typename V>
    stack_type _push(V&& val, stack_type head) {
    	if (free_nodes != end() ){
    	    head = head_to_freenode(head);
    	    value(head) = std::forward<V>(val);
    	} else {
    	    pool.push_back(node_t{std::forward<V>(val), head});
    	    head = pool.size();
    	}
    	return head;
    }
    
public:
    stack_pool() noexcept : pool{0}, free_nodes {stack_type{0}} {}
    explicit stack_pool(size_type n) { // reserve n nodes in the pool
        reserve(n);
        free_nodes = stack_type{0}; 
    }
    


    using iterator = _iterator<stack_pool, value_type, stack_type>;
    using const_iterator = _iterator<const stack_pool, const value_type, stack_type>;

    iterator begin(stack_type x) noexcept { return iterator(x, *this); };
    iterator end(stack_type) noexcept{ return iterator(end(), *this); }; // this is not a typo

    const_iterator begin(stack_type x) const noexcept { return const_iterator(x, *this); }
    const_iterator end(stack_type) const noexcept{ return const_iterator(end(), *this); }

    const_iterator cbegin(stack_type x) const noexcept { return const_iterator(x, *this); }
    const_iterator cend(stack_type) const noexcept { return const_iterator(end(), *this); }



    stack_type new_stack() noexcept { // return an empty stack
        return end();
    }

    void reserve(const size_type n) { // reserve n nodes in the pool 
        pool.reserve(n);
    }

    size_type capacity() const noexcept { // the capacity of the pool
        return pool.capacity();
    }

    bool empty(const stack_type x) const noexcept {
        return x == end();
    }

    stack_type end() const noexcept { return stack_type(0); }

    T &value(const stack_type x) noexcept {					
        return node(x).value;
    }
    const T &value(const stack_type x) const noexcept {
        return node(x).value;
    }

    stack_type &next(const stack_type x) noexcept {				
        return node(x).next;
    }
    const stack_type &next(const stack_type x) const noexcept {
        return node(x).next;
    }
    
    stack_type push(const T &val, stack_type head) {		
	return _push(val, head);
    }

    stack_type push(T &&val, stack_type head) {		
	return _push(std::forward<T>(val), head);
    }

    stack_type pop(stack_type x) { // delete first node	
        auto tmp = free_nodes;
        free_nodes = x;
        x = next(x);
        next(free_nodes) = tmp;
        return x;
    }

    stack_type free_stack(stack_type x) { // free entire stack
        while ( x != end() )
            x = pop(x);
        return x;
    }
};

