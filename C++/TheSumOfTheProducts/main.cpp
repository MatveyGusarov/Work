#include <iostream>

bool* IndexTable(int length) {
  bool* table_of_index = new bool[length];
  for (int i = 0; i < length; ++i) {
    table_of_index[i] = false;
  }
  return table_of_index;
}

int StrToInt(const char* str) {
  static const int kNumberBase = 10;
  int res = 0;
  int i = 0;
  int sign = 1;

  if (str[i] == '-') {
    sign = -1;
    ++i;
  }

  for (; str[i] != '\0'; ++i) {
    res = res * kNumberBase + str[i] - '0';
  }

  return res * sign;
}

int* GetSize(int number_of_arrays, char* arrays_size[]) {
  int* arrays_sizes = new int[number_of_arrays + 1];
  arrays_sizes[0] = 0;
  for (int i = 0; i < number_of_arrays; ++i) {
    arrays_sizes[i + 1] = StrToInt(arrays_size[i]);
    if (arrays_sizes[i + 1] > arrays_sizes[0]) {
      arrays_sizes[0] = arrays_sizes[i + 1];
    }
  }
  return arrays_sizes;
}

int** GetArrays(int number_of_arrays, const int* size_of_arrays) {
  int** arrays = new int*[number_of_arrays];

  for (int i = 0; i < number_of_arrays; ++i) {
    arrays[i] = new int[size_of_arrays[i]];
    for (int j = 0; j < size_of_arrays[i]; ++j) {
      std::cin >> arrays[i][j];
    }
  }

  return arrays;
}

int64_t Multiply(int** arrays, int* size_of_arrays, bool* table_of_index,
                 int index_array, int cur_res) {
  int64_t res = 0;
  for (int i = 0; i < size_of_arrays[index_array + 1]; ++i) {
    if (!table_of_index[i]) {
      if (index_array == size_of_arrays[0] - 1) {
        res += cur_res * arrays[index_array][i];
      } else {
        table_of_index[i] = true;
        res += Multiply(arrays, size_of_arrays, table_of_index, index_array + 1,
                        cur_res * arrays[index_array][i]);
        table_of_index[i] = false;
      }
    }
  }
  return res;
}

int main(int argc, char* argv[]) {
  int* size_of_arrays = GetSize(argc - 1, argv + 1);
  int max_size = size_of_arrays[0];
  int** arrays = GetArrays(argc - 1, size_of_arrays + 1);
  bool* table_of_index = IndexTable(max_size);
  size_of_arrays[0] = argc - 1;

  std::cout << Multiply(arrays, size_of_arrays, table_of_index, 0, 1) << '\n';

  delete[] size_of_arrays;
  delete[] table_of_index;
  for (int i = 0; i < argc - 1; i++) {
    delete[] arrays[i];
  }
  delete[] arrays;
  return 0;
}
