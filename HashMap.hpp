#ifndef HASHMAP_HPP
#define HASHMAP_HPP



#include <utility>
#include <vector>
#include <stdexcept>
#include <iostream>




/***********************************************************************
 *                                   macro
 ***********************************************************************/

#define UP_BOUND 0.75
#define LOW_BOUND 0.25
#define DEFAULT_CAPACITY 16
#define NO_KEY "Error: Key not found in dictionary"

using std::vector; using std::pair; using std::string;

template <typename KeyT, typename ValueT>
/***************************************************************************/

class HashMap
{
 protected:
  vector <pair<KeyT,ValueT>> *hash_list;
    size_t _capacity = DEFAULT_CAPACITY;
    size_t _size = 0;


 public:
  /***********************************************************************
   *                     constructors and rule of 3
   ***********************************************************************/

    HashMap(){
      /**
       * defualt constructor
       */
      this->hash_list = new vector<pair<KeyT,ValueT>>[DEFAULT_CAPACITY];
    };

    HashMap(const vector<KeyT> &keys, const vector<ValueT> &values )
    /**
     * constructor by keys and values
     * @param keys
     * @param values
     */
    {
        size_t obj_num = keys.size();
        if (obj_num != values.size())
        {
          throw std::length_error("ERROR: vectors must be of the same size!");
        }
        this->hash_list = new vector < pair <KeyT,ValueT> >[DEFAULT_CAPACITY];
        for (size_t i = 0; i < obj_num; i++)
        {
          if (this->contains_key(keys[i]))
          {
            (*this)[keys[i]] = values[i];
          }
          this->insert(keys[i], values[i]);
        }
    }

    HashMap(const HashMap &other)
    /**
     * copy constructor
     * @param other
     */
    {
      this->_capacity = other._capacity;
      this->_size = other._size;
      this->hash_list = new vector <pair<KeyT,ValueT>>[other._capacity];
      for (size_t i = 0; i < this->_capacity; i++)
      {
        this->hash_list[i] = other.hash_list[i];
      }
    }

    HashMap &operator=(const HashMap &other)
    /**
     * operator =
     * @param other
     * @return reference to the left side of equation
     */
    {
      if (&other == this) // sanity check
      {
        return *this;
      }
      this->_capacity = other._capacity;
      delete [] this->hash_list;
      this->hash_list = new vector <pair<KeyT,ValueT>>[other._capacity];
      for (size_t i = 0; i < other._capacity; i ++)
      { // go add all other keys and values
        for (auto &itr : other.hash_list[i])
        {
          this->insert (itr.first, itr.second);
        }
      }
      return *this;
    }

    virtual ~HashMap()
    {
      delete [] this->hash_list;
    }

  /***********************************************************************
   *                              operators
   ***********************************************************************/
  bool operator==(const HashMap &other) const
  /**
   * operator equals
   * @param other
   * @return true if equals, false else
   */
  {
    if (this->_size != other._size)
    { // check if size is the  same
      return false;
    }
    for (size_t i = 0; i < other._capacity; i ++)
    { // check if all vectors in other are in this as well
      for (auto &itr : other.hash_list[i])
      {
        if (!this->contains_key (itr.first))
        {
          return false;
        }
        if (this->at (itr.first) != other.at (itr.first))
        {
          return false;
        }
      }
    }
    return true;
  }

  bool operator!=(const HashMap &other) const
  /**
   * operator != not equals
   * @param other
   * @return false if equals else true
   */
  {
    return !(*this == other);
  }

  ValueT operator[](const KeyT &key) const
  /**
   * operator [] for read only
   * @param key
   * @return const value of key in map
   */
  {
    std::hash<KeyT> hasher;
    size_t vec_index = hasher(key) & (this->_capacity - 1);
    for (auto &itr : this->hash_list[vec_index])
    { // search for the right key
      if (itr.first == key)
      {
        return itr.second;
      }
    }
    return this->cbegin()->second; // return value undefined needed to
    // return some valid value...
  }

  ValueT &operator[](const KeyT &key)
  /**
   * operator [] for assignment
   * @param key
   * @return reference to the value of key - can change it
   */
    {
    std::hash<KeyT> hasher;
    size_t vec_index = hasher(key) & (this->_capacity - 1);
    for (auto &itr : this->hash_list[vec_index])
    { // search for the right key
      if (itr.first == key)
      {
        return itr.second;
      }
    }
    this->insert (key, ValueT());
    return this->at (key);
  }

  /***********************************************************************
   *                              functions
   ***********************************************************************/
    int size() const {return (int)(this->_size);}
    int capacity() const {return (int)(this->_capacity);}

    bool insert(const KeyT &key, const ValueT &value)
    /**
     * insert a new value to map, only if key is not alread in map
     * @param key
     * @param value
     * @return true apon success else false
     */
    {
      if (this->contains_key(key))
      {
        return false;
      }
      this->_size ++;
      if (this->get_load_factor() > UP_BOUND)
      { // if size changed see if a resize is needed
        resize_map (this->_capacity * 2);
        this->_capacity *= 2;
      }
      std::hash<KeyT> hasher;
      size_t vec_index = (int)hasher(key) & (this->_capacity - 1);
      this->hash_list[vec_index].emplace_back (key, value);
      return true;
    }

    bool empty() const
    /**
     * check if map is empy
     * @return true if so else false
     */
    {
      return (this->_size == 0);
    }

    bool contains_key (const KeyT &key) const
    /**
     * checks if a key is inside the map
     * @param key
     * @return true if so else false
     */
    {
      std::hash<KeyT> hasher;
      size_t vec_index = hasher(key) & (this->_capacity - 1);
      for (auto &itr : this->hash_list[vec_index])
      { // search for the right key
        if (itr.first == key)
        {
          return true;
        }
      }
      return false;
    }

    ValueT &at(const KeyT &key)
    /**
     * returns the value of the key, can change value
     * @param key
     * @return changable reference of value
     */
    {
      std::hash<KeyT> hasher;
      size_t vec_index = hasher(key) & (this->_capacity - 1);
      for (auto &itr : this->hash_list[vec_index])
      { // search for the right key
        if (itr.first == key)
        {
          return itr.second;
        }
      }
      throw std::out_of_range(NO_KEY);
    }

  const ValueT &at(const KeyT &key) const
  /**
   * returns the value of the key, can not change value
   * @param key
   * @return const reference of value
   */
  {
    std::hash<KeyT> hasher;
    size_t vec_index = hasher(key) & (this->_capacity - 1);
    for (auto &itr : this->hash_list[vec_index])
    { // search for the right key
      if (itr.first == key)
      {
        return itr.second;
      }
    }
    throw std::out_of_range(NO_KEY);
  }

    virtual bool erase(const KeyT &key)
    /**
     * erase key and value from map
     * @param key
     * @return true upon success else false
     */
    {
      std::hash<KeyT> hasher;
      size_t vec_index = hasher(key) & (this->_capacity - 1);
      size_t new_cap = this->_capacity;
      auto itr = this->hash_list[vec_index].cbegin();
      for (size_t i = 0; i < this->hash_list[vec_index].size(); i++)
      { // search for key in the correct vector
        if (itr->first == key)
        {
          this->hash_list[vec_index].erase (itr);
          this->_size--;
          if (this->get_load_factor() < LOW_BOUND)
          { // if size changed, check if resize is needed
            while ((double)this->_size/(double)new_cap < LOW_BOUND
                    && new_cap != 1)
            { // find the right capacity value
              new_cap /=2;
            }
            this->resize_map (new_cap);
            this->_capacity = new_cap;
          }
          return true;
        }
        itr ++;
      }
      return false;
    }

    double get_load_factor() const
    /**
     * calculate the load factor
     * @return the load factor
     */

    {
      return  (double)this->_size/(double)this->_capacity;
    }

    int bucket_size(const KeyT &key) const
    /**
     * @param key
     * @return the size of the bucket of the bucket containing key
     */
    {
      std::hash<KeyT> hasher;
      size_t vec_index = hasher(key) & (this->_capacity - 1);
      size_t vec_len = this->hash_list[vec_index].size();
      for (auto &itr : this->hash_list[vec_index])
      { // search for the right key
        if (itr.first == key)
        {
          return (int)vec_len;
        }
      }
      throw std::out_of_range(NO_KEY);
    }

    int bucket_index(const KeyT &key) const
    /**
     * @param key
     * @return the index of the bucket of the bucket containing key
     */
    {
      std::hash<KeyT> hasher;
      size_t vec_index = hasher(key) & (this->_capacity - 1);
      for (auto &itr : this->hash_list[vec_index])
      { // search for the right key
        if (itr.first == key)
        {
          return (int)vec_index;
        }
      }
      throw std::out_of_range(NO_KEY);
    }

    void clear()
    /**
     * erase all keys and values from map
     */
    {
      for (size_t i = 0; i < this->_capacity; i ++)
      {
        this->hash_list[i].clear();
      }
      this->_size = 0;
    }

    void resize_map(size_t new_capacity)
    /**
     * resize and re hash all pairs in map
     * @param new_capacity
     */
    {
      auto ret_map = new vector<pair<KeyT,ValueT>> [new_capacity];
      size_t vec_index = 0;
      std::hash<KeyT> hasher;
      for (size_t i = 0; i < this->_capacity; i++)
      { // insert all old pairs with the new hash function
        for (auto &itr : this->hash_list[i])
        {
          vec_index = hasher (itr.first) & (new_capacity - 1);
          ret_map[vec_index].emplace_back (itr.first, itr.second);
        }
      }
      delete[](this->hash_list);
      this->hash_list = ret_map;
    }
  /***********************************************************************
   *                              iterator
   ***********************************************************************/
    typedef vector <pair<KeyT,ValueT>> *vec_ptr;
    typedef typename vector <pair<KeyT,ValueT>>::const_iterator vec_iter;

    class ConstIterator
        // all methods in iterator are pretty straight (const)forward pan
        // intended, just operators and their usual return values
    {
      private:
        const HashMap &_hash_map;
        vec_ptr _vec_ptr; // the current vector in the map
        vec_iter _vec_iter; // the current iterator in current vector in map
        size_t _map_index; // save the index of current vector in map
        size_t _vec_index; // saves the current index of the vec iterator

      public:
        typedef const pair<KeyT,ValueT> value_type;
        typedef const value_type &reference;
        typedef const value_type *pointer;
        typedef int difference_type;
        typedef std::forward_iterator_tag iterator_category;

        ConstIterator(const ConstIterator &other) :
          _hash_map(other._hash_map)
        {
          this->_map_index = other._map_index;
          this->_vec_index = other._vec_index;
          this->_vec_ptr = &(this->_hash_map.hash_list[this->_map_index]);
          this->_vec_iter = this->_hash_map.hash_list[_map_index].cbegin();
          for (size_t i = 0; i < this->_vec_index; i++)
          {
            this->_vec_iter++;
          }
        }

        ConstIterator(size_t map_index, size_t vec_index,
                      const HashMap &hash_map) : _hash_map(hash_map)
        {
          this->_map_index = map_index;
          this->_vec_index = vec_index;
          this->_vec_ptr = &(this->_hash_map.hash_list[this->_map_index]);
          this->_vec_iter = this->_vec_ptr->cbegin();
          for (size_t i = 0; i < this->_vec_index; i++)
          {
            this->_vec_iter++;
          }
        }
        bool operator==(const ConstIterator &other) const
        {

          return (this->_hash_map == other._hash_map &&
              this->_vec_iter == other._vec_iter);
        }

      ConstIterator& operator=(const ConstIterator& other)
      {
        this->_map_index = other._map_index;
        this->_vec_index = other._vec_index;
        this->_vec_ptr = &(this->_hash_map.hash_list[this->_map_index]);
        this->_vec_iter = this->_hash_map.hash_list[_map_index].cbegin();
        for (size_t i = 0; i < this->_vec_index; i++)
        {
          this->_vec_iter++;
        }
      }

      bool operator!=(const ConstIterator &other) const
      {
        return !(*this == other);
      }

      reference operator*() const {return *(this->_vec_iter);}
      pointer operator->() const {return this->_vec_iter.operator->();}

      ConstIterator& operator++()
      {
        this->_vec_iter ++;
        this->_vec_index ++;
        if (this->_vec_iter != this->_vec_ptr->cend())
        {
          return *this;
        }
        this->_vec_index = 0;
        while (this->_vec_iter == this->_vec_ptr->cend() &&
                this->_map_index < this->_hash_map._capacity - 1)
        {
          this->_vec_ptr ++;
          this->_map_index ++;
          this->_vec_iter = this->_vec_ptr->cbegin();
        }
          return *this;
      }

      ConstIterator operator++(int)
      {
          ConstIterator temp(*this);
          ++(*this);
          return temp;
      }
    };


    ConstIterator cbegin() const
    {
      if (this->_size == 0)
      {
        return this->cend();
      }
      int i = 0;
      while (this->hash_list[i].empty())
      {
        i++;
      }
      ConstIterator itr (i, 0, *this);
      return itr;
    }

    ConstIterator cend() const
    {
      ConstIterator itr(this->_capacity - 1,
                        this->hash_list[this->_capacity-1].size(),
                        *this);
      return itr;
    }

    ConstIterator begin() const{
      if (this->_size == 0)
      {
        return this->end();
      }
      int i = 0;
      while (this->hash_list[i].empty())
      {
        i++;
      }
      ConstIterator itr (i, 0, *this);
      return itr;
    }

    ConstIterator end() const
    {
      ConstIterator itr(this->_capacity - 1,
                        this->hash_list[this->_capacity-1].size(),
                        *this);
      return itr;
    }

//  void print_hash_no_itr()
//  {
//    std::cout << "capacity: " << _capacity << std::endl;
//    std::cout << "size: " << _size << std::endl<< std::endl;
//    for (size_t i = 0; i < _capacity; i++)
//    {
//      std::cout << "vector number: " << i << std::endl << std::endl;
//      for (auto &itr : hash_list[i])
//      {
//        std::cout << "    key: "<< itr.first << "   value: " << itr.second<<
//                  std::endl;
//      }
//    }
//  }
};


#endif //HASHMAP_HPP
