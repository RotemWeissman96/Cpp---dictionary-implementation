/**
 * Created by Ron Deitch
 *
 *@Updates: [VERSION 1.9] (15/06/22)
 *
 * 0. Added test_const_correctness()
 *
 * TODO: Check with course staff
 * 0. Resize to the smallest capacity possible or to the first
 * valid one?
 *
 * @IMPORTANT: Check the tests forum for updates.
 * If you have any questions, found a mistake or a missing test,
 * feel free to contact me at ron.deitch@mail.huji.ac.il
 */

// includes
#include "HashMap.hpp"
#include "Dictionary.hpp"
#include <iostream>
#include <utility>
#include "sstream"

using namespace std;

// macros
#define START_TEST cout << __func__ << " "
#define assert(condition) if(!(condition)) throw runtime_error(format_what(#condition, \
__LINE__))

// helpers
string format_what (const string &condition, int line)
{
  std::ostringstream stream;
  stream << "assert(" << condition << ")" << ", line: " << line;
  return stream.str ();
}

struct key_struct {
    string x;
    int y;
    key_struct() : x(string()), y(int()) { }
    bool operator==(const key_struct& ks) const {
      return (this->x == ks.x && this->y == ks.y);
    }
    key_struct(string xp, int yp) : x(std::move(xp)),y(yp) {}
};


template <>
struct std::hash<key_struct> {
    size_t operator ()(const key_struct& value) const {
      return value.y;
    }
};

// tests
/**
 * @Tests:
 * 0. nothing, really.
 */
void test_constructor_default ()
{
  START_TEST;
  HashMap<int, int> h1;
  assert(h1.empty ());
  assert(h1.capacity () == 16); // init capacity is 16
  HashMap<string, int> h2;
  assert(h2.empty ());
  assert(h2.capacity () == 16);
  HashMap<string, string> h3;
  assert(h3.empty ());
  assert(h3.capacity () == 16);
  HashMap<char, float> h4;
  assert(h4.empty ());
  assert(h4.capacity () == 16);
  const HashMap<int, int> h7;
  HashMap<int, int> h8 (h7); // Your HashMap parameter should
  // be const in the copy constructor
  assert(h3.empty ());
  assert(h3.capacity () == 16);

  // You can add here more types.
}

/**
 * @tests:
 * 1. Throws exception for keys.size() != values.size()
 * 2. All keys and values are added in order when keys are unique
 * 3. Size and capacity are valid
 * 4. For each key, only it's last value is eventually inserted
 * 5. For same keys, create only one item
 * 6. Resize map according to load_factor
 */
void test_constructor_vectors ()
{
  START_TEST;
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  for (int i = 1; i <= 5; i++) assert(h1.at (i) == i * 10);

  HashMap<char, int> h2 (
      {'A', 'B', 'C', 'D', 'E'},
      {65, 66, 67, 68, 69});
  for (char i = 'A'; i <= 'E'; i++) assert(h2.at (i) == i);

  bool thrown = true;
  try
  {
    // Vectors should be of same size.
    HashMap<int, int> h3 ({1, 2}, {10});
    HashMap<int, int> h4 ({1}, {10, 20});
    thrown = false;
  }
  catch (exception &e)
  {
    assert(thrown);
  }

  HashMap<int, int> h5 (
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 20});
  assert(h5.size () == 1);
  assert(h5.at (1) == 20); // Should be the last value
  assert(h5.capacity () == 16); // There is only one item, no need to rehash

  HashMap<int, int> h6 ({1, 1, 1, 2, 2, 1}, {1, 2, 3, 4, 5, 6});
  assert(h6.size () == 2); // Only two unique keys
  assert(h6.at (1) == 6); // Should be the last value
  assert(h6.at (2) == 5); // Should be the last value

  HashMap<int, int> h7 (
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
      {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130});
  assert(h7.size () == 13);
  assert(h7.capacity () == 32); // Should've rehashed

}

/**
 * @tests:
 * 0. size and capacity are the same for both maps
 * 1. all items copied properly
 * 2. changing one map doesn't change the other map
 */
void test_constructor_copy ()
{
  START_TEST;
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  HashMap<int, int> h2 (h1);
  assert(h2.size () == h1.size ());
  assert(h2.capacity () == h1.capacity ());
  for (int i = 1; i <= 5; i++) assert(h2.at (i) == i * 10);
  h2.erase (1);
  h2.insert (1, 2);
  assert(h1.at (1) == 10);
  assert(h2.at (1) == 2);
  h1.insert (6, 60);
  assert(h1.at (6) == 60);
  assert(h2.contains_key (6) == false);
  h1.at (6) = 70;
  assert(h1.at (6) == 70);
}

/**
 * @tests:
 * 0. Check empty, size, load_factor, capacity
 */
void test_getters ()
{
  START_TEST;
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  assert(h1.empty () == false);
  assert(h1.size () == 5);
  assert(h1.capacity () == 16);
  assert(h1.get_load_factor () == (double) (5.f / 16.f));

  HashMap<int, int> h2;
  assert(h2.empty ());
  assert(h2.capacity () == 16);
  assert(h2.get_load_factor () == 0.f);
}

/**
 * @tests:
 * 0. contains key works properly after erasing and inserting items
 */
void test_contains_key ()
{
  START_TEST;
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  for (int i = 1; i <= 5; i++) assert(h1.contains_key (i));
  h1.erase (1);
  assert(!h1.contains_key (1));
  h1.insert (6, 60);
  assert(h1.contains_key (6));
  h1.insert (1, 10);
  assert(h1.contains_key (1));

}

/**
 * @tests:
 * 0. Throws exception if key doesn't exists
 * 1. Inserted keys have bucket_size > 0
 */
void test_bucket_size ()
{
  START_TEST;
  // std::hash() isn't consistent on different computers, so I can't
  // check for collisions.
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  for (int i = 1; i <= 5; i++) assert(h1.bucket_size (i) > 0);
  bool thrown = true;
  try
  {
    h1.bucket_size (6); // Should throw exception
    thrown = false;
  }
  catch (exception &e)
  {
    assert(thrown);
  }

  HashMap<int, int> h2 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  int prev_bucket_size = h2.bucket_size (2);
  h2.erase (2);
  h2.insert (2, 2);
  assert(h2.bucket_size (2) == prev_bucket_size);

}

/**
 * @tests:
 * 0. Throws exception if key doesn't exists
 * 1. Inserted keys have valid bucket_index
 */
void test_bucket_index ()
{
  START_TEST;
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  // std::hash() isn't consistent on different computers, so I can't
  // check for the exact bucket, only for the correct range (size of the map).
  for (int i = 1; i <= 5; i++)
    assert(0 <= h1.bucket_index (i) && h1.bucket_index (i) <= 15);
  bool thrown = true;
  try
  {
    h1.bucket_index (6); // Should throw exception
  }
  catch (exception &e)
  {
    assert(thrown);
  }

  HashMap<int, int> h2 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  int prev_bucket_index = h2.bucket_index (2);
  h2.erase (2);
  h2.insert (2, 2);
  assert(h2.bucket_index (2) == prev_bucket_index);
}

/**
 * @Tests:
 * 0. Throw exception if key doesn't exist
 * 1. Return by value
 */
void test_at ()
{
  START_TEST;
  HashMap<string, int> h1 (
      {"A", "BC", "DEF", "G", "H"},
      {10, 20, 30, 40, 50});
  assert(h1.at ("G") == 40);
  assert(h1.at ("DEF") == 30);
  assert(h1.at ("H") == 50);

  h1.at ("H") = 60;
  assert(h1.at ("H") == 60);

  h1.at ("G") = h1.at ("DEF") = 70;
  assert(h1.at ("G") == 70 && h1.at ("DEF") == 70);

  bool thrown = true;
  try
  {
    h1.at ("M");
    thrown = false; // Should've thrown an error
  }
  catch (exception &e)
  {
    assert(thrown);
  }
}

/**
 * @tests:
 * 0. Resize after item inserted if needed.
 * 1. Return true if key inserted successfully, false otherwise
 * 2. Change map size only upon success.
 * 3. Change value only for non-existing keys, otherwise do nothing.
 */
void test_insert ()
{
  START_TEST;
  HashMap<int, int> h1;
  assert(h1.insert (1, 10) == true); // excepts true if key wasn't in the map
  assert(h1.size () == 1);
  assert(h1.at (1) == 10);
  assert(h1.insert (1, 20) == false); // excepts false if key wasn't in the map
  assert(h1.size () == 1); // Shouldn't add another item
  assert(h1.at (1) == 10); // Shouldn't change existing item

  HashMap<int, int> h2;
  for (int i = 1; i <= 13; i++) assert(h2.insert (i, i * 10) == true);
  for (int i = 1; i <= 13; i++) assert(h2.at (i) == i * 10);
  assert(h2.capacity () == 32); // Should've rehashed
  for (int i = 14; i <= 48; i++) assert(h2.insert (i, i * 10) == true);
  assert(h2.capacity () == 64); // Should've rehashed
  for (int i = 1; i <= 100; i++) assert(h2.insert (48, i) == false);
  assert(h2.at (48) == 480); // Existing key, don't change value
  assert(h2.capacity () == 64); // Existing key, don't rehash

  HashMap<string, string> h3;
  for (int i = 1; i <= 13; i++)
    assert(h3.insert (to_string (i), to_string (i * 10)));
  assert(h3.size () == 13);
  assert(h3.capacity () == 32);
  assert(h3.at ("3") == "30");

}
/**
 * @tests:
 * 0. Resize after item erased if needed.
 * 1. Return true if key erased successfully, false otherwise
 * 2. Change map size only upon success.
 */
void test_erase ()
{
  START_TEST;
  HashMap<int, int> h1;
  assert(h1.erase (1) == false); // Key doesn't exist
  assert(h1.empty ()); // Don't change size if erase wasn't successful
  for (int i = 1; i <= 13; i++) assert(h1.insert (i, i * 10) == true);
  assert(h1.capacity () == 32);
  assert(h1.erase (1) == true); // Should return true upon success
  for (int i = 2; i <= 5; i++) assert(h1.erase (i) == true);
  assert(h1.size () == 8);
  assert(h1.capacity () == 32); // Shouldn't resize when load_factor == 0.25
  h1.erase (6);
  assert(h1.size () == 7); // load_factor < 0.25
  assert(h1.capacity () == 16); // Should've resized

  HashMap<string, string> h2;
  for (int i = 1; i <= 13; i++)
    assert(h2.insert (to_string (i), to_string (i * 10)));
  assert(h2.erase ("2") == true);
  assert(h2.size () == 12);

}

/**
 * @tests:
 * 0. Don't throw error when map is empty
 * 1. Don't change capacity after clear(), only size()
 * 2. Resize to the CORRECT capacity after clear() and then insert()
 */
void test_clear ()
{
  START_TEST;
  HashMap<int, int> h1;
  h1.clear (); // Should do nothing
  HashMap<int, int> h2;
  for (int i = 0; i < 1024; i++) h2.insert (i, i * 10);
  assert(h2.size () == 1024);
  assert(h2.capacity () == 2048);
  h2.clear ();
  assert(h2.empty ());
  assert(h2.capacity () == 2048); // Don't change capacity
  for (int i = 0; i < 1024; i++) assert(!h2.contains_key (i)); // All keys
  // were deleted
  h2.insert (1, 10);
  assert(h2.size () == 1);
  assert(h2.at (1) == 10);
  assert(h2.capacity () == 2048); // Capacity wasn't changed after insert.
  assert(h2.erase (1) == true);
  assert(h2.capacity () == 1); // Now it should be resized
  assert(h2.insert (1, 10) == true);
  assert(h2.capacity () == 2);
}

/**
 *  @tests:
 * 0. Don't throw exception for non-existing key
 * 1. Return the value of the key by reference.
 * 2. hash_map[key] = value works.
 * 3. hash_map[key]++ and hash_map[key] *= c works.
 * 4. hash_map[key1] == hash_map[key2] works.
 * 5. hash_map[key1] = hash_map[key2] = value works.
 */
void test_operator_brackets ()
{
  START_TEST;
  HashMap<int, int> h1;
  h1[1]; // Doesn't throw an exception.
  h1[1] = 10;
  assert(h1.at (1) == 10);
  h1[1] = 20;
  assert(h1.at (1) == 20);
  h1[2] = h1[1];
  assert(h1.at (2) == 20);
  h1[2] = 30;
  assert(h1.at (1) == 20);
  assert(h1.at (2) == 30);
  assert(h1.insert (2, 30) == false); // Shouldn't change the value
  assert(h1[2] == 30);

  h1[3] = 1;
  h1[3]++;
  assert(h1.at (3) == 2);
  h1[3] *= 10;
  assert(h1.at (3) == 20);
  h1[10] = h1[11] = 5;
  assert(h1[10] == 5 && h1[11] == 5);

  HashMap<int, int> h2 ({1, 2, 3}, {10, 20, 30});
  for (int i = 1; i <= 3; i++) assert(h2[i] == i * 10);

  HashMap<int, int> h3;
  for (int i = 1; i <= 10; i++) h2[i] = i * 10;
  for (int i = 1; i <= 10; i++) assert(h2.at (i) == i * 10);
  const int x = h3[1];

  HashMap<string, int> h4;
  for (int i = 1; i <= 10; i++) h4[to_string (i)] = i * 10;
  for (int i = 1; i <= 10; i++) assert(h4.at (to_string (i)) == i * 10);


  // check if you handle non-existing keys correctly
  h4["A"]; // this should add default value of int
  assert(h4.at("A") == int());

  HashMap<int, string> h5;
  bool thrown=true;
  try {
    assert(h5.at(1) == string());
    thrown=false;
  }
  catch(exception &e) {
    assert(thrown);
  }
  h5[1]; // this should add deafult value of string
  assert(h5.at(1) == string()); // this won't throw an execption because 1 is a key now
  h5.at(1) = "A";
  assert(h5.at(1) == "A");
}

/**
 *  @tests:
 *  0. size, capacity and items are the same on both maps.
 *  1. changes made to one HashMap doesn't impact the other.
 */
void test_operator_assignment ()
{
  START_TEST;
  HashMap<int, int> h1 ({1, 2, 3, 4, 5}, {10, 20, 30, 40, 50});
  HashMap<int, int> h2;
  h2 = h1;
  assert(h2.size () == h1.size ());
  assert(h2.capacity () == h1.capacity ());
  for (int i = 1; i <= 5; i++) assert(h2.at (i) == i * 10);
  h2.erase (1);
  h2.insert (1, 2);
  // Check if maps aren't entangled with each other
  assert(h1.at (1) == 10);
  assert(h2.at (1) == 2);
  h1.insert (6, 60);
  assert(h1.at (6) == 60);
  assert(h2.contains_key (6) == false);

  HashMap<string, int> h3, h4;
  for (int i = 1; i <= 10; i++) h3[to_string (i)] = i * 10;
  h4 = h3;
  for (int i = 1; i <= 10; i++)
    assert(h3[to_string (i)] == h4[to_string (i)]);
  assert(h3.size () == h4.size ());
  assert(h3.capacity () == h4.capacity ());
  assert(h3 == h4);

  HashMap<int, int> h5;
  h5.insert (1, 10);
  int *ptr = &h5.at (1);
  h5 = h5; // shouldn't change h5 because it's the same map
  *ptr = 2; // would seg fault if changed
  assert(h5[1] == 2);

}

/**
 *  @tests:
 *  0. Comparison by keys and values (not only by keys)
 *  1. operator == and != works as expected
 *  2. HashMaps with the equal items and different capacities are equal.
 */
void test_operator_comparison ()
{
  START_TEST;
  HashMap<int, int> h1, h2;
  assert(h1 == h2); // Both empty hence equal
  h1[1] = 10;
  assert(!(h1 == h2));
  assert(h1 != h2);
  h2[1] = 9;
  assert(!(h1 == h2));
  h2[1] = 10;
  assert(h1 == h2);
  for (int i = 0; i < 1000; i++) h1[i] = i * 10;
  h1.clear ();
  h2.clear ();
  assert(h1.capacity () != h2.capacity ());
  for (int i = 0; i < 3; i++) h1[i] = h2[i] = i * 10;
  assert(h1 == h2); // Capacities differ but both maps have the same values
}

/**
 *  @tests:
 *  0. begin == end when empty map
 *  1. begin == cbegin && end == cend
 */
void test_iterator_begin_end ()
{
  START_TEST;
  HashMap<int, int> h1;
  assert(typeid (h1.begin ()) == typeid (HashMap<int, int>::ConstIterator));
  assert(h1.begin () == h1.cbegin ());
  assert(h1.end () == h1.cend ());
  assert(h1.begin () == h1.end ()); // map is empty so begin == end
  for (auto it: h1)
  {
    assert(false); // Shouldn't enter this for loop
  }
}

/**
 * @tests:
 * 0. If for loop works with operator ++
 */
void test_iterator_for ()
{
  START_TEST;
  HashMap<int, int> h2;
  for (int i = 0; i < 12; i++) h2[i] = i * 10;

  for (auto it = h2.begin (); it != h2.end (); it++)
  {
    assert(it->second == it->first * 10);
  }

  for (auto it = h2.begin (); it != h2.end (); ++it)
  {
    assert(it->second == it->first * 10);
  }

  HashMap<string, int> h3;
  for (int i = 1; i <= 12; i++) h3[to_string (i)] = i * 10;

  for (auto it = h3.begin (); it != h3.end (); ++it)
  {
    assert(h3.at (it->first) == it->second);
  }

}

/**
 * @tests:
 * 0. If for_each loop works
 */
void test_iterator_for_each ()
{
  START_TEST;
  HashMap<int, int> h3;
  for (int i = 0; i < 12; i++) h3[i] = i * 10;
  for (auto &item: h3)
  {
    assert(item.second == item.first * 10);
  }
  HashMap<string, int> h4;
  for (int i = 1; i <= 12; i++) h4[to_string (i)] = i * 10;
  for (auto &item: h4)
  {} // Just checking it doesn't crash

}

/**
 * @tests:
 * 0. Test
 * if operators work as expected
 */

void test_iterator_operators ()
{
  START_TEST;
  HashMap<int, string> h4 ({1, 2, 3}, {"a", "b", "c"});
  for (auto it1 = h4.begin (), it2 = h4.begin ();
       it1 != h4.end ();)
  {
    assert(it1->first == (*it1).first);
    assert(it1->second == (*it1).second);
    assert(it1 == it2);
    it1++;
    ++it2;
  }
  auto it = h4.begin ();

  HashMap<int, string> h5 ({1, 2, 3}, {"a", "b", "c"});
  for (auto it1 = h4.begin (), it2 = h5.begin ();
       it1 != h4.end (); it1++, it2++)
  {
    assert(*it1 == *it2); // Same keys and values
    assert(it1 != it2); // Doesn't point to same HashMap, hence not equal
    assert(!(it1 == it2));
  }

}

/**
 * @tests:
 * 0. Default constructor
 * 1. Vector constructor
 * 2. Copy constructor
 */
void test_dictionary_constructors ()
{
  START_TEST;
  // Default Constructor
  Dictionary d1;
  assert(d1.empty ());
  assert(d1.capacity () == 16);
  // Vector constructor
  Dictionary d2 ({"a", "b", "c"}, {"A", "B", "C"});
  assert(d2.size () == 3);
  assert(d2.capacity () == 16);
  assert(d2.at ("a") == "A");
  assert(d2.at ("b") == "B");
  assert(d2.at ("c") == "C");
  // Copy constructor
  Dictionary d3 (d2);
  assert(d3.size () == 3);
  assert(d3.capacity () == 16);
  assert(d3 == d2);
  assert(d3.at ("a") == "A");
  d3.erase ("b");
  assert(d3 != d2);
}

/**
 * @tests:
 * 0. erase() throws invalid_key when given non-existing key
 * 1. erase() doesn't change the size when failed
 * 2. erase() resizes map according to load_factor
 */
void test_dictionary_erase ()
{
  START_TEST;
  Dictionary d1;
  bool thrown = true;
  try
  {
    d1.erase ("A"); // need to throw InvalidKey
    thrown = false;
  }
  catch (InvalidKey &e)
  {
    assert(thrown);
  }
  assert(d1.empty ()); // Don't change size if erase wasn't successful
  for (int i = 0; i < 13; i++)
    d1.insert (to_string (i), to_string (i * 10));
  assert(d1.size () == 13);
  assert(d1.capacity () == 32);
  assert(d1.erase (to_string (1)) == true); // Should return true upon success
  for (int i = 2; i <= 5; i++) assert(d1.erase (to_string (i)) == true);
  assert(d1.size () == 8);
  assert(d1.capacity () == 32); // Shouldn't resize when load_factor == 0.25
  d1.erase (to_string (6));
  assert(d1.size () == 7); // load_factor < 0.25
  assert(d1.capacity () == 16); // Should've resized
}

/**
 * @tests:
 * 0. adds key and values properly
 * 1. changes values of existing key
 * 2. works with empty iterator
 * 3. resizes dictionary according to load_factor
 */
void test_dictionary_update ()
{
  START_TEST;
  typedef pair<string, string> string_pair;
  Dictionary d1;
  vector<string_pair> vec1 ({
                                string_pair ("a", "A"),
                                string_pair ("b", "B"),
                                string_pair ("c", "C"),
                                string_pair ("d", "D")
                            });
  d1.update (vec1.begin (), vec1.end ());
  assert(d1.size () == 4);
  assert(d1.at ("a") == "A");
  assert(d1.at ("b") == "B");
  assert(d1.at ("c") == "C");
  assert(d1.at ("d") == "D");

  vector<string_pair> vec2 ({
                                string_pair ("a", "AA"),
                                string_pair ("e", "E")
                            });
  d1.update (vec2.begin (), vec2.end ());
  assert(d1.size () == 5);
  assert(d1.at ("a") == "AA");

  // test with empty vector
  Dictionary d2;
  vector<string_pair> vec3;
  d1.update (vec3.begin (), vec3.end ());
  assert(d2.empty ());

  // test resize of map
  Dictionary d3;
  vector<string_pair> vec4;
  for (int i = 0; i < 13; i++) d3[to_string (i)] = to_string (i * 10);
  assert(d3.size () == 13);
  assert(d3.capacity () == 32);
}

void test_dictionary_iterator ()
{
  START_TEST;
  Dictionary d1;
  for (auto it = d1.begin (); it != d1.end (); it++)
  {
    assert(false); // dict is empty then should enter the for loop
  }
  Dictionary d2 ({"a", "b", "c", "d", "e"}, {"A", "B", "C", "D", "E"});
  int counter = 0;
  for (auto it = d2.begin (); it != d2.end (); it++) counter++;
  assert(counter == 5);
  counter = 0;
  for (auto it: d2) counter++;
  assert(counter == 5);
}

void test_dictionary_base_functionality ()
{
  START_TEST;
  Dictionary d1;
  assert(d1.empty ()); // Empty works
  assert(d1.insert ("a", "A") == true); // Insert works
  assert(d1.size () == 1); // Size works
  assert(d1.capacity () == 16);
  assert(d1.bucket_index ("a") >= 0 && d1.bucket_index ("a") <= 15);
  assert(d1.bucket_size ("a") > 0);
  assert(d1.erase ("a"));
  for (int i = 0; i < 13; i++) d1[to_string (i)] = to_string (i * 10);
  for (int i = 0; i < 13; i++) assert(d1.contains_key (to_string (i)));
  assert(d1.get_load_factor () == 13.f / 32.f);
  assert(d1.at ("12") == "120");
  assert(d1["11"] == "110");
  Dictionary d2 (d1);
  assert(d1 == d2);
  d1.clear (); // clear works
  assert(d1.empty ());
  assert(d1.capacity () == 32);
  d1["A"] = "a";
  assert(d1.at ("A") == "a" && d1.size () == 1);
  d1["A"] = "b";
  assert(d1.at ("A") == "b" && d1.size () == 1);
}

void test_dictionary_slicing ()
{
  START_TEST;
  Dictionary d1 ({"a", "b", "c"}, {"A", "B", "C"});
  HashMap<string, string> h1 (d1);
  assert(h1.size () == 3);
  assert(h1.at ("a") == "A");
  assert(h1.erase ("d") == false); // Discards override erase
}

void test_invalid_key_exception ()
{
  START_TEST;
  try
  {
    throw InvalidKey (); // Should support default constructor
  }
  catch (InvalidKey &e)
  {}

  try
  {
    throw InvalidKey ("what_argument");
  }
  catch (invalid_argument &e)
  {
    assert((const string) e.what () == "what_argument"); // what function
    // works properly
  }
  catch (exception &e)
  {
    assert(false); // invalid_key should be derived from invalid argument
  }
}

/**
 * @tests:
 * 0. tries to mess up the capacity
 * 1. tests iterator on edge cases
 */
void test_capacity_edge_cases ()
{
  START_TEST;
  HashMap<int, int> h1; // keys: {}
  for (auto it: h1) assert(false); // empty map
  assert(h1.capacity () == 16 && h1.empty ());
  assert(h1.insert (1, 10)); // keys: {1}
  for (auto it: h1) assert(it.first == 1 && it.second == 10);
  assert(h1.capacity () == 16 && h1.size () == 1);
  assert(h1.erase (1)); // keys: {}
  assert(h1.capacity () == 1 && h1.empty ());
  assert(h1.insert (1, 10)); // keys: {1}
  for (auto it: h1) assert(it.first == 1 && it.second == 10);
  assert(h1.capacity () == 2 && h1.size () == 1);
  assert(h1.insert (2, 20)); // keys: {1,2}
  int counter = 0;
  for (auto it: h1) counter++;
  assert(counter == 2);
  assert(h1.capacity () == 4 && h1.size () == 2);
  assert(h1.erase (1)); // keys: {2}
  for (auto it: h1) assert(it.first == 2);
  assert(h1.capacity () == 4 && h1.size () == 1);
  assert(h1.insert (1, 10)); // keys: {1,2}
  // test: iterator doesn't miss any item
  counter = 0;
  for (auto it: h1) counter++;
  assert(counter == 2);
  assert(h1.capacity () == 4 && h1.size () == 2);
  assert(h1.insert (3, 30)); // keys: {1,2,3}
  // test: iterator doesn't miss any item
  counter = 0;
  for (auto it: h1) counter++;
  assert(counter == 3);
  assert(h1.capacity () == 4 && h1.size () == 3);
  assert(h1.insert (4, 40)); // keys: {1,2,3,4}
  assert(h1.capacity () == 8 && h1.size () == 4);
  assert(h1.insert (5, 50));// keys: {1,2,3,4,5}
  assert(h1.insert (6, 60));// keys: {1,2,3,4,5,6}
  counter = 0;
  for (auto it: h1) counter++;
  assert(counter == 6);
  assert(h1.insert (7, 70));  // keys: {1,2,3,4,5,6,7}
  // test: iterator doesn't miss any item
  counter = 0;
  for (auto it: h1) counter++;
  assert(counter == 7);
  assert(h1.capacity () == 16 && h1.size () == 7);
  h1.clear (); // keys: {}
  for (auto it: h1) assert(false);
  assert(h1.capacity () == 16 && h1.empty ());
  assert(h1.insert (1, 10)); // keys: {1}
  assert(h1.insert (1, 20) == false); // keys: {1}
  assert(h1.insert (1, 30) == false); // keys: {1}
  assert(h1[1] == 10);
  assert(h1.capacity () == 16 && h1.size () == 1);
  assert(h1.erase (2) == false); // keys: {1}
  for (int i = 2; i <= 7; i++) h1.insert (i, i * 10); // keys: {1,2,3,4,5,6,7}
  counter = 0;
  for (auto it: h1) counter++;
  assert(counter == 7);
  assert(h1.capacity () == 16 && h1.size () == 7);
  assert(h1.erase (7)); // keys: {1,2,3,4,5,6}
  assert(h1.capacity () == 16 && h1.size () == 6);
  assert(h1.erase (6)); // keys: {1,2,3,4,5}
  assert(h1.erase (5));// keys: {1,2,3,4}
  assert(h1.erase (4));// keys: {1,2,3}
  assert(h1.capacity () == 8 && h1.size () == 3);
}

//
//void test_special_key_types ()
//{
//  START_TEST;
//  // Pointer to int
//  HashMap<int *, int > h1;
//  int * p = new int(1);
//  assert(h1.insert (p,10));
//  assert(h1.at(p) == 10);
//  auto temp = p;
//  p = new int(2);
//  assert(h1[p]!=10);
//  h1.clear();
//  assert(h1.empty());
//  delete p;
//  delete temp;
//
//  // simple struct declared at beginning of the file, including override
//  // std::hash method
//  HashMap<key_struct,int> h2;
//  for(int i=0; i<1024;i++) h2.insert ({to_string (i), i*10}, i*20);
//  for(const auto& it:h2)
//    assert(h2[it.first] == it.first.y *2);
//  assert(h2.size()==1024);
//  assert(h2.capacity() == 2048);
//  assert(h2.erase ({"1",10}) == true);
//  assert(h2.size() == 1023);
//
//  // Check all items in same bucket
//  HashMap<key_struct,int> h7;
//  for(int i=0; i<1024;i++) h7.insert ({to_string (i), 0}, i);
//  for(const auto& it:h7)
//    assert(h7.bucket_index(it.first) == 0);
//
//  //bool
//  HashMap<bool,int> h3;
//  bool j = true;
//  for(int i=0; i<128;i++) h3.insert (i%2==0,i);
//  assert(h3.size()==2);
//
//  //char
//  HashMap<char,int> h4;
//  for(int i=0; i<256;i++) h4.insert ((char)i,i);
//  assert(h4.size()==256);
//
//  //double
//  HashMap<double,int> h5;
//  for(int i=0; i<256;i++) h5.insert ((double)(i*0.3953),i);
//  assert(h5.size()==256);
//
//  // int64_t
//  HashMap<int64_t ,int> h6;
//  for(int64_t i=9223372036854775805; i>9223372036854774805;i--) h6.insert
//        (i,(int)(i%1000));
//  assert(h6.size()==1000);
//}

void test_const_correctness() {
  START_TEST;
  const HashMap<int, string> h1({1,2,3},{"A","B","C"});
  const HashMap<int, string> h2({4,5,6},{"E","F","G"});
  HashMap<int, string> h3;
  const Dictionary d1;

  // If one of the following lines show error it means that
  // the method isn't const when it should be
  h1.size();
  h1.capacity();
  h1.empty();
  h1.get_load_factor();
  h1.begin();
  h1.cbegin();
  h1.end();
  h1.cend();
  assert(!(h1 == h2));
  assert(h1 != h3);
  h1.contains_key (1);
  h1[1];
  try
  {
    h1.at (1);
    h1.bucket_index (1);
    h1.contains_key (1);
  }
  catch(exception & e) {}

  // The following line tests if you handle const parameters correctly
  // don't forget to set your argument const when possible
  HashMap<int, string> h4;
  const int a = 1;
  assert(h4.insert (a,"A"));
  assert(h4.at(a)=="A");
  assert(h4.contains_key (a));
  assert(h4.bucket_index (a) != -1);
  assert(h4.bucket_size (a) == 1);
  assert(h4[a] == "A");
  assert(h4.erase (a));
  const string b = "B";
  assert(h4.insert (1,b));

  const std::vector<int> v1 = {1,2,3};
  const std::vector<string> v2 = {"A","B","C"};
  const std::vector<string> v3 = {"a","b","c"};
  HashMap<int, string> h5(v1,v2); // vectors can be const
  Dictionary d2(v2,v3); // vectors can be const
  const string c = "A";
  d2.erase (c); // key can be const

  const std::vector<pair<string,string>> pv = {
      pair<string, string> ("A", "a"),
      pair<string, string> ("B", "b"),
      pair<string, string> ("C", "c"),
  };

  Dictionary d3;
  d3.update (pv.begin(),pv.end()); // pv can be const
  assert(d3.size()==3);
  Dictionary d4;
  d4.update (pv.cbegin(),pv.cend()); // pv iterator can be const
  assert(d4.size()==3);

  const HashMap<int, string> h6({1,2,3},{"A","B","C"});
  HashMap<int, string> h7(h6); // map copied should be const
  assert(h7.size()==3);
  assert(h7.insert(4,"D"));

  const HashMap<int, int> h8(
      {1,2,3,4,5,6,7,8,9,10,11,12,13},
      {1,2,3,4,5,6,7,8,9,10,11,12,13});
  assert(h8.size()==13);
  assert(h8.capacity()==32);
}

int main ()
{
  typedef void (*test_func) ();

  // Uncomment the tests you don't want to run, every test is independent.
  test_func tests[] = {
      test_constructor_default,
      test_constructor_vectors,
      test_constructor_copy,
      test_getters,
      test_contains_key,
      test_bucket_size,
      test_bucket_index,
      test_at,
      test_insert,
      test_erase,
      test_clear,
      test_operator_brackets,
      test_operator_assignment,
      test_operator_comparison,
      test_iterator_begin_end,
      test_iterator_for,
      test_iterator_for_each,
      test_iterator_operators,
      test_dictionary_constructors,
      test_dictionary_erase,
      test_dictionary_update,
      test_dictionary_iterator,
      test_dictionary_base_functionality,
      test_dictionary_slicing,
      test_invalid_key_exception,
      test_capacity_edge_cases,
//      test_special_key_types,
      test_const_correctness
  };

  int i = 0, passed = 0, counter = 0;
  for (auto &test: tests)
  {
    counter++;
    cout << "[" << i++ << "]: ";
    try
    {
      test ();
      cout << "PASSED" << endl;
      passed++;
    }
    catch (exception &e)
    {
      cout << "FAILED: " << e.what () << endl;
    }
  }
  cout << "========================================" << endl;
  cout << "Passed " << passed << " out of " << counter << " tests." << endl;
  cout << "========================================" << endl;
}