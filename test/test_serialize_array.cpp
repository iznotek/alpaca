#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Serialize array<char, 3>" * test_suite("array")) {
  struct my_struct {
    std::array<char, 3> value;
  };

  my_struct s{{'x', 'y', 'z'}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 3);
  REQUIRE(bytes[0] == 'x');
  REQUIRE(bytes[1] == 'y');
  REQUIRE(bytes[2] == 'z');
}

TEST_CASE("Serialize array<uint64_t>" * test_suite("array")) {
  struct my_struct {
    std::array<uint64_t, 5> value;
  };

  my_struct s{{1, 2, 3, 4, 5}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 5);
  // values
  REQUIRE(bytes[0] == static_cast<uint8_t>(1));
  REQUIRE(bytes[1] == static_cast<uint8_t>(2));
  REQUIRE(bytes[2] == static_cast<uint8_t>(3));
  REQUIRE(bytes[3] == static_cast<uint8_t>(4));
  REQUIRE(bytes[4] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize array<int, 5>" * test_suite("array")) {
  struct my_struct {
    std::array<int, 5> value;
  };

  my_struct s{{1, 2, 3, 4, 5}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 5);
  // values
  REQUIRE(bytes[0] == static_cast<uint8_t>(1));
  REQUIRE(bytes[1] == static_cast<uint8_t>(2));
  REQUIRE(bytes[2] == static_cast<uint8_t>(3));
  REQUIRE(bytes[3] == static_cast<uint8_t>(4));
  REQUIRE(bytes[4] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize array<float, 5>" * test_suite("array")) {
  struct my_struct {
    std::array<float, 5> value;
  };

  my_struct s{{1.1, 2.2, 3.3, 4.4, 5.5}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 20);
  // values
  float current_value = 1.1;
  for (std::size_t i = 0; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(float, current_value);
    current_value += 1.1;
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize array<bool, 5>" * test_suite("array")) {
  struct my_struct {
    std::array<bool, 5> value;
  };

  my_struct s{{true, false, true, false, true}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 5);
  // values
  bool current_value = true;
  for (std::size_t i = 0; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(bool, current_value);
    current_value = !current_value;
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize array<std::string, 5>" * test_suite("array")) {
  struct my_struct {
    std::array<std::string, 5> value;
  };

  my_struct s{{"a", "b", "c", "d", "e"}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 10);
  // values
  char current_value = 'a';
  for (std::size_t i = 0; i < bytes.size();) {
    REQUIRE(bytes[i++] == static_cast<uint8_t>(1));

    CONSTRUCT_EXPECTED_VALUE(char, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize array<vector<int>, 2>" * test_suite("array")) {
  struct my_struct {
    std::array<std::vector<int>, 2> value;
  };

  my_struct s{{{{1, 2, 3}, {4, 5, 6}}}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 8);

  // first sub-vector
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(3));
  // values
  REQUIRE(bytes[1] == static_cast<uint8_t>(1));
  REQUIRE(bytes[2] == static_cast<uint8_t>(2));
  REQUIRE(bytes[3] == static_cast<uint8_t>(3));

  // second sub-vector
  // size
  REQUIRE(bytes[4] == static_cast<uint8_t>(3));
  // values
  REQUIRE(bytes[5] == static_cast<uint8_t>(4));
  REQUIRE(bytes[6] == static_cast<uint8_t>(5));
  REQUIRE(bytes[7] == static_cast<uint8_t>(6));
}

TEST_CASE("Serialize array<tuple>" * test_suite("array")) {
  struct my_struct {
    std::array<std::tuple<bool, int, float, std::string, char>, 2> values;
  };

  my_struct s;
  s.values[0] = std::make_tuple(true, 5, 3.14, "Hello", 'a');
  s.values[1] = std::make_tuple(false, -15, 2.718, "World", 'z');
  auto bytes = serialize(s);

  REQUIRE(bytes.size() == 26);
  // vector of tuple

  // start array values

  // array[0]
  REQUIRE(bytes[0] == static_cast<uint8_t>(true));
  REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[2 + i] == expected[i]);
    }
  }
  {
    REQUIRE(bytes[6] == static_cast<uint8_t>(5));
    REQUIRE(bytes[7] == static_cast<uint8_t>('H'));
    REQUIRE(bytes[8] == static_cast<uint8_t>('e'));
    REQUIRE(bytes[9] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[10] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[11] == static_cast<uint8_t>('o'));
  }
  REQUIRE(bytes[12] == static_cast<uint8_t>('a'));

  // array[1]
  REQUIRE(bytes[13] == static_cast<uint8_t>(false));
  REQUIRE(bytes[14] == static_cast<uint8_t>(0b10001111));
  {
    CONSTRUCT_EXPECTED_VALUE(float, 2.718f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[15 + i] == expected[i]);
    }
  }
  {
    REQUIRE(bytes[19] == static_cast<uint8_t>(5));
    REQUIRE(bytes[20] == static_cast<uint8_t>('W'));
    REQUIRE(bytes[21] == static_cast<uint8_t>('o'));
    REQUIRE(bytes[22] == static_cast<uint8_t>('r'));
    REQUIRE(bytes[23] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[24] == static_cast<uint8_t>('d'));
  }
  REQUIRE(bytes[25] == static_cast<uint8_t>('z'));
}