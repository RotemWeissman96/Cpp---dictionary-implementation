#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP



#include "HashMap.hpp"
#include <stdexcept>



using std::string; using std::vector; using std::pair;
class InvalidKey : public std::invalid_argument
    /**
     * exception declaration based on project demands
     */
{
  public:
  explicit InvalidKey(const string &msg = "") :std::invalid_argument(msg) {}
};


class Dictionary : public HashMap<string, string>
{
  public:

    Dictionary() = default;
    Dictionary(const vector<string>& keys, const vector<string>& values ) :
            HashMap<string, string>(keys, values)
    {    }

  template<typename IteratorT>
  void update(IteratorT begin, const IteratorT &end)
      /**
       * update the dictionary with all pairs of keys and valuse between the
       * iterator begin and end
       * @param begin
       * @param end
       */
      {
        while (begin != end)
        {
          this->insert_run_over_for_update(begin->first, begin->second);
          begin ++;
        }
      }

  void  insert_run_over_for_update(const string &key, const string &value)
  /**
   * add a single pair to map, will override an already inserted value if
   * needed
   * @param key
   * @param value
   */
  {
      if (this->contains_key (key))
      {
        (*this)[key] = value;
      }
      else
      {
        this->_size++;
        if (this->get_load_factor () > UP_BOUND)
        {
          resize_map (this->_capacity * 2);
          this->_capacity *= 2;
        }
        std::hash<string> hasher;
        size_t vec_index = (int) hasher (key) & (this->_capacity - 1);
        this->hash_list[vec_index].emplace_back (key, value);
      }
  }

    bool erase(const string &key) override
    /**
     * override the erase function in order for it to throw exception when
     * key is not found
     * @param key
     * @return true upon success else false
     */
    {
      std::hash<string> hasher;
      size_t vec_index = hasher(key) & (this->_capacity - 1);
      size_t new_cap = this->_capacity;
      auto itr = this->hash_list[vec_index].cbegin();
      for (size_t i = 0; i < this->hash_list[vec_index].size(); i++)
      { // find the right key in the right vector
        if (itr->first == key)
        { // key found
          this->hash_list[vec_index].erase (itr);
          this->_size--;
          if (this->get_load_factor() < LOW_BOUND)
          { // check if resize is needed
            while ((double)this->_size/(double)new_cap < LOW_BOUND
                   && new_cap != 1)
            { // find the right capacity for map
              new_cap /=2;
            }
            this->resize_map (new_cap);
            this->_capacity = new_cap;
          }
          return true;
        }
        itr ++;
      }
      throw InvalidKey(NO_KEY);
    }
};



#endif //DICTIONARY_HPP