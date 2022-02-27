#include "DoubleArray.h"

using namespace std;

/* init only */
DoubleArray::DoubleArray() : i_base_(nullptr), i_check_(nullptr), c_tail_(nullptr), i_result_(nullptr),
                             i_array_size_(I_DEFAULT_ARRAY_SIZE),
                             i_tail_size_(I_DEFAULT_ARRAY_SIZE),
                             i_result_size_(I_DEFAULT_ARRAY_SIZE)
{
}


/* delete memory */
DoubleArray::~DoubleArray()
{
  deleteMemory();
}


/* メモリ確保                              */
/* @param b_init_size サイズを初期化するか */
/* @return Error Code                      */
int DoubleArray::keepMemory(
  const bool b_init_size) noexcept
{
  deleteMemory(b_init_size);  /* 既存のデータ構造を破棄 */

  try {
    i_base_  = new int[i_array_size_];
    i_check_ = new int[i_array_size_];
    c_tail_  = new char[i_tail_size_];
    if (i_result_size_) {
      i_result_ = new int64_t[i_result_size_];
    }

    memset(i_base_,  0,               sizeof(i_base_[0])  * i_array_size_);
    memset(i_check_, I_ARRAY_NO_DATA, sizeof(i_check_[0]) * i_array_size_);
    memset(c_tail_,  0,               sizeof(c_tail_[0])  * i_tail_size_);
    if (i_result_size_) {
      memset(i_result_, 0, sizeof(i_result_[0]) * i_result_size_);
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* メモリ破棄                              */
/* @param b_init_size サイズを初期化するか */
void DoubleArray::deleteMemory(
  const bool b_init_size) noexcept
{
  if (i_base_)   delete[] i_base_;
  if (i_check_)  delete[] i_check_;
  if (c_tail_)   delete[] c_tail_;
  if (i_result_) delete[] i_result_;

  i_base_   = nullptr;
  i_check_  = nullptr;
  c_tail_   = nullptr;
  i_result_ = nullptr;

  if (b_init_size) {
    i_array_size_  = I_DEFAULT_ARRAY_SIZE;
    i_tail_size_   = I_DEFAULT_ARRAY_SIZE;
    i_result_size_ = I_DEFAULT_ARRAY_SIZE;
  }
}


/* データが作成されているかチェック              */
/* @return true : データ作成されてる  false : 空 */
bool DoubleArray::checkInit() const noexcept
{
  return i_base_ != nullptr;
}


/* BaseCheckのメモリ拡張             */
/* @param i_lower_limit 拡張最低領域 */
/* @return Error Code                */
int DoubleArray::baseCheckExtendMemory(
  const uint64_t i_lower_limit) noexcept
{
  try {
    uint64_t i_new_array_size(i_array_size_);
    do {
      i_new_array_size *= I_EXTEND_MEMORY;
    } while (i_new_array_size < i_lower_limit);

    int* i_new_base  = new int[i_new_array_size];
    int* i_new_check = new int[i_new_array_size];
    memset(i_new_base,  0,               sizeof(i_new_base[0])  * i_new_array_size);
    memset(i_new_check, I_ARRAY_NO_DATA, sizeof(i_new_check[0]) * i_new_array_size);
    memcpy(i_new_base,  i_base_,         sizeof(i_new_base[0])  * i_array_size_);
    memcpy(i_new_check, i_check_,        sizeof(i_new_check[0]) * i_array_size_);

    delete[] i_base_;
    delete[] i_check_;
    i_base_       = i_new_base;
    i_check_      = i_new_check;
    i_array_size_ = i_new_array_size;
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* Tailのメモリ拡張                  */
/* @param i_lower_limit 拡張最低領域 */
/* @return Error Code                */
int DoubleArray::tailExtendMemory(
  const uint64_t i_lower_limit) noexcept
{
  try {
    uint64_t i_new_tail_size(i_tail_size_);
    do {
      i_new_tail_size *= I_EXTEND_MEMORY;
    } while (i_new_tail_size < i_lower_limit);

    char* c_tail      = new char[i_new_tail_size];
    int64_t* i_result = new int64_t[i_new_tail_size];
    memset(c_tail,   0,         sizeof(c_tail[0])   * i_new_tail_size);
    memset(i_result, 0,         sizeof(i_result[0]) * i_new_tail_size);
    memcpy(c_tail,   c_tail_,   sizeof(c_tail[0])   * i_tail_size_);
    memcpy(i_result, i_result_, sizeof(i_result[0]) * i_result_size_);

    delete[] c_tail_;
    delete[] i_result_;
    c_tail_        = c_tail;
    i_result_      = i_result;
    i_tail_size_   = i_new_tail_size;
    i_result_size_ = i_new_tail_size;
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* Memoryサイズを最適化する                                           */
/* @param i_tail_last_index Tail配列のデータが格納されている最終Index */
/* @return Error Code                                                 */
int DoubleArray::optimizeMemory(
  const int i_tail_last_index) noexcept
{
  if (i_array_size_ == 0)
    return I_NO_ERROR;

  for (int64_t i = i_array_size_ - 1; i >= 0; --i) {
    if (i_base_[i]) {
      i_array_size_ = i + 1;
      break;
    }
  }

  i_tail_size_ = i_tail_last_index;
  i_array_size_ += static_cast<int>(0xff);  /* 検索時に不正領域を参照しない対策 */

  try {
    /* 配列情報 */
    int* i_new_base  = new int[i_array_size_];
    int* i_new_check = new int[i_array_size_];
    memcpy(i_new_base,  i_base_,  sizeof(i_new_base[0])  * i_array_size_);
    memcpy(i_new_check, i_check_, sizeof(i_new_check[0]) * i_array_size_);
    delete[] i_base_;
    delete[] i_check_;
    i_base_  = i_new_base;
    i_check_ = i_new_check;

    /* Tail文字列情報 */
    char* c_new_tail = new char[i_tail_size_];
    memcpy(c_new_tail, c_tail_, sizeof(c_new_tail[0]) * i_tail_size_);
    delete[] c_tail_;
    c_tail_ = c_new_tail;

    if (i_result_size_) {
      i_result_size_ = i_tail_size_;  /* 最適化サイズに書き換え */
      int64_t* i_new_result = new int64_t[i_result_size_];
      memcpy(i_new_result, i_result_, sizeof(i_new_result[0]) * i_result_size_);
      delete[] i_result_;
      i_result_ = i_new_result;
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* DoubleArrayを構築する                  */
/* @param add_datas DoubleArray構築データ */
/* @param i_option  構築オプション        */
/* @return 0 : 正常終了  0以外 : 異常終了 */
int DoubleArray::createDoubleArray(
  ByteArrays& add_datas,
  const int i_option) noexcept
{
  if (add_datas.empty())
    return I_NO_ERROR;

  add_datas.sort(); /* Sort */

  /* Trie構築 */
  TrieNode* root_node = nullptr;
  createTrie(root_node, add_datas);
  if (keepMemory(true)) { /* メモリ確保 */
    return I_FAILED_MEMORY;
  }

  /* DoubleArray構築 */
  unsigned int base_array[256] = { 1 }; /* 分岐するNodeのBaseで全ての分岐先に適応する値を探すのに使用 */
  int i_tail_index(1), i_base_index(0); /* BaseとTailの初期値 */
  if (recursiveCreateDoubleArray(i_tail_index, base_array, root_node, i_base_index)) {
    return I_FAILED_MEMORY;
  }

  if (i_option & I_TAIL_UNITY) {
    delete[] i_result_;
    i_result_      = nullptr;
    i_result_size_ = 0;
  }

  return optimizeMemory(i_tail_index);
}


/* 入力データからTRIE構造を構築する          */
/* @param root_node   構築したTrie Root Node */
/* @param byte_arrays 基にするデータ         */
void DoubleArray::createTrie(
  TrieNode*& root_node,
  const ByteArrays& byte_arrays) const noexcept
{
  uint64_t i_max_length;
  vector<pair<uint64_t, uint64_t>> positions(byte_arrays.size(), make_pair(0, 0)); /* TailPosition */
  createOverlapPositions(i_max_length, positions, byte_arrays); /* TailPositionデータ作成 */

  vector<TrieNode*> trie_nodes(i_max_length, nullptr);
  constexpr uint64_t i_max_value = numeric_limits<uint64_t>::max();
  auto position = positions.cbegin();
  for (const auto& byte_array : byte_arrays) {
    const auto i_start(position->first);
    const auto i_tail(position->second);
    ++position;
    if (i_start == i_max_value) /* データが重複かチェック */
      continue;

    char* c_tail(nullptr);
    int64_t i_tail_size(0);
    if (i_tail < byte_array.i_byte_length_ - 1) {
      i_tail_size = byte_array.i_byte_length_ - i_tail - 1;
      c_tail = new char[i_tail_size];
      memcpy(c_tail, (byte_array.c_byte_ + i_tail + 1), i_tail_size);
    }

    auto branch_node = trie_nodes[i_start];
    auto node_parts  = new NodeParts(c_tail, i_tail_size, byte_array.result_);
    auto next_node   = new TrieNode(byte_array.c_byte_[i_tail], nullptr, nullptr, node_parts);
    trie_nodes[i_tail] = next_node;

    for (int64_t i = i_tail - 1; i >= static_cast<int64_t>(i_start); --i) {
      trie_nodes[i] = next_node = new TrieNode(byte_array.c_byte_[i], nullptr, next_node, nullptr);
    }

    if (root_node == nullptr) {
      root_node = trie_nodes[i_start];
    }

    if (branch_node != nullptr) {
      branch_node->p_same_layer_ = trie_nodes[i_start];
    }
  }
}


/* @param i_tail_index 書き込み開始TailIndex           */
/* @param base_array   BaseValueの値を決定するのに使用 */
/* @param trie_node    Current TrieNode                */
/* @param i_base_index 基準のBaseCheckIndex            */
/* @return Error Code                                  */
int DoubleArray::recursiveCreateDoubleArray(
  int& i_tail_index,
  unsigned int* base_array,
  TrieNode* trie_node,
  const int i_base_index) noexcept
{
  unsigned int i_base_value;
  if (getBaseValue(i_base_value, base_array, trie_node)) {
    return I_FAILED_MEMORY; /* 構築失敗 */
  }
  i_base_[i_base_index] = i_base_value;

  /* 再帰処理する前にcheckを設定 */
  auto current_node = trie_node;
  while (current_node != nullptr) {
    i_check_[current_node->c_byte_ + i_base_value] = i_base_index;
    current_node = current_node->p_same_layer_;
  }

  current_node = trie_node;
  while (current_node != nullptr) {
    int i_insert(current_node->c_byte_ + i_base_value);
    if (current_node->node_parts_) {
      i_base_[i_insert] = (-i_tail_index);  /* TailIndexはマイナス値 */

      if (setTailInfo(i_tail_index, current_node->node_parts_)) {
        return I_FAILED_MEMORY; /* 構築失敗 */
      }
      delete current_node->node_parts_;
    }
    if (current_node->p_child_top_ != nullptr) {
      if (recursiveCreateDoubleArray(i_tail_index, base_array, current_node->p_child_top_, i_insert)) {
        return I_FAILED_MEMORY; /* 構築失敗 */
      }
    }
    auto delete_node = current_node;
    current_node = current_node->p_same_layer_;
    delete delete_node;
  }

  return I_NO_ERROR;
}


/* Baseの値を求める ついでに同LayerLastIndexも求める    */
/* @param i_base_value  求めたBase値                    */
/* @param base_array    BaseValueの値を決定するのに使用 */
/* @param trie_node     Current Trie Node               */
/* @return Error Code                                   */
int DoubleArray::getBaseValue(
  unsigned int& i_base_value,
  unsigned int* base_array,
  const TrieNode* trie_node) noexcept
{
  /* 既存のバイト情報の最大Base値を検索 */
  i_base_value = 0;

  unsigned char c_byte_max(0x00);
  auto current_node = trie_node;
  do {
    if (base_array[current_node->c_byte_] > i_base_value) {
      i_base_value = base_array[current_node->c_byte_];
    }
    c_byte_max   = current_node->c_byte_;
    current_node = current_node->p_same_layer_;
  } while (current_node != nullptr);

  ++i_base_value;
  bool b_success(false);
  do {
    if (i_array_size_ <= (c_byte_max + i_base_value)) {
      if (baseCheckExtendMemory(c_byte_max + i_base_value)) {
        return I_FAILED_MEMORY;
      }
    }

    uint64_t i_limit = i_array_size_ - c_byte_max;
    do {
      b_success = true;
      current_node = trie_node;
      while (current_node != nullptr) {
        if (i_check_[current_node->c_byte_ + i_base_value] != I_ARRAY_NO_DATA) {
          b_success = false;
          break;
        }
        current_node = current_node->p_same_layer_;
      }
    } while (b_success == false && ++i_base_value < i_limit);
  } while (b_success == false);

  current_node = trie_node;
  do {
    base_array[current_node->c_byte_] = i_base_value;
    current_node = current_node->p_same_layer_;
  } while (current_node != nullptr);

  return I_NO_ERROR;
}


/* Tailに情報を設定する                 */
/* @param i_tail_index Tail格納開始位置 */
/* @param node_parts   NodeParts        */
/* @return Error code                   */
int DoubleArray::setTailInfo(
  int& i_tail_index,
  const NodeParts* node_parts) noexcept
{
  if ((i_tail_size_ <= i_tail_index + node_parts->i_tail_size_)
  &&  (tailExtendMemory(i_tail_index + node_parts->i_tail_size_))) { /* メモリが不足したので拡張 */
    return I_FAILED_MEMORY;
  }

  if (node_parts->c_tail_) {
    memcpy(&c_tail_[i_tail_index], node_parts->c_tail_, node_parts->i_tail_size_);
    i_tail_index += static_cast<int>(node_parts->i_tail_size_);
    --i_tail_index;
  } else {
    c_tail_[i_tail_index] = C_TAIL_CHAR;
  }

  i_result_[i_tail_index++] = node_parts->i_result_;

  return I_NO_ERROR;
}


/* 重複Index情報作成                    */
/* @param i_max_length 最長データ長     */
/* @param positions    start,tail Index */
/* @param datas        全追加データ     */
void DoubleArray::createOverlapPositions(
  uint64_t& i_max_length,
  vector<pair<uint64_t, uint64_t>>& positions,
  const ByteArrays& datas) const noexcept
{
  if (datas.size() == 1) {
    i_max_length = datas.front().i_byte_length_;
    return;
  }

  auto sameIndex = [](uint64_t& i_result, const uint64_t i_length, const char* c_first, const char* c_second) {
    for (uint64_t i = 0; i < i_length; ++i) {
      if (c_first[i] != c_second[i]) {
        i_result = i;
        break;
      }
    }
  };

  /* 先頭 */
  auto top  = datas.cbegin();
  auto next = top;
  ++next;
  i_max_length = top->i_byte_length_;
  uint64_t i_top_same_index(top->i_byte_length_);
  sameIndex(i_top_same_index, top->i_byte_length_, top->c_byte_, next->c_byte_);
  if (i_top_same_index == next->i_byte_length_) positions.begin()->first = numeric_limits<uint64_t>::max();
  else                                          positions.begin()->second = i_top_same_index;

  /* 末尾 */
  auto last    = datas.crbegin();
  auto one_ago = last;
  ++one_ago;
  uint64_t i_last_same_index = 0;
  sameIndex(i_last_same_index, min(last->i_byte_length_, one_ago->i_byte_length_), last->c_byte_, one_ago->c_byte_);
  auto& last_position  = *positions.rbegin();
  last_position.first  = i_last_same_index;
  last_position.second = i_last_same_index;
  if (i_max_length < last->i_byte_length_) {
    i_max_length = last->i_byte_length_;
  }

  /* 先頭末尾を除く */
  uint64_t i_before_same_index(i_top_same_index);
  for (uint64_t i = 1, i_last = datas.size() - 1; i < i_last; ++i) {
    const auto& current = datas[i];
    const auto& after   = datas[i+1];
    const char* c_current_byte = current.c_byte_;
    uint64_t i_after_same_index(current.i_byte_length_);
    if (i_max_length < i_after_same_index) {
      i_max_length = i_after_same_index;
    }

    sameIndex(i_after_same_index, current.i_byte_length_, c_current_byte, after.c_byte_);
    if (i_after_same_index == after.i_byte_length_) {
      positions[i].first = numeric_limits<uint64_t>::max();  /* 同一データ混在 */
    } else {
      positions[i].first  = i_before_same_index;
      positions[i].second = (i_before_same_index < i_after_same_index ? i_after_same_index : i_before_same_index);
      i_before_same_index = i_after_same_index;
    }
  }
}


/* 検索する                                       */
/* c_byteにはNULLが途中に含まれる可能性がある為、 */
/* バイト長も渡さないとダメ。                     */
/* 引数のバイト列末尾のNULLも使用する             */
/* @param c_byte        search bytes              */
/* @param i_byte_length search data length        */
/* @return search result                          */
int64_t DoubleArray::search(
  const char* c_byte,
  uint64_t i_byte_length) const noexcept
{
  uint64_t i(0);
  int i_base_index(0), i_check_index(0);
  for (; i <= i_byte_length; ++i) { /* 終端記号の分があるので<=とする */
    i_check_index  = i_base_[i_base_index];
    i_check_index += static_cast<unsigned char>(c_byte[i]);
    if (i_check_[i_check_index] != i_base_index) {
      return I_SEARCH_NOHIT;  /* データが存在しない */
    }

    if (i_base_[i_check_index] < 0) {
      break;
    }
    i_base_index = i_check_index;
  }

  /* Tail処理 */
  uint64_t i_tail_index(-i_base_[i_check_index]); /* Tail突入契機のマイナス値をプラスに変換 */
  if (i < i_byte_length) {
    ++i;
    uint64_t i_compare_length(i_byte_length - i);
    if (memcmp(&c_tail_[i_tail_index], &c_byte[i], i_compare_length + 1) == 0) {
      return (i_result_ == nullptr ? I_HIT_DEFAULT : i_result_[i_tail_index + i_compare_length]);
    }
  } else if (i == i_byte_length) {
    return (i_result_ == nullptr ? I_HIT_DEFAULT : i_result_[i_tail_index]);
  }

  return I_SEARCH_NOHIT;
}


/* 途中経過状態を取得しながら検索する            */
/* @param sarch_parts   search position          */
/* @param result        search result            */
/* @param c_byte        search bytes             */
/* @param i_byte_length search data length       */
/* @return true : 続きがある  false : 続きが無い */
bool DoubleArray::searchContinue(
  DASearchParts& search_parts,
  int64_t& result,
  const char* c_byte,
  const uint64_t i_byte_length) const noexcept
{
  result = I_SEARCH_NOHIT;

  uint64_t i(0), i_byte_index(0);
  if (search_parts.i_tail_ == 0) {  /* Tailまで進んでいない */
    for (; i < i_byte_length; ++i) {
      search_parts.i_check_ = i_base_[search_parts.i_base_] + static_cast<unsigned char>(c_byte[i]);
      if (i_check_[search_parts.i_check_] != search_parts.i_base_) {
        return false; /* データが存在しない */
      }

      if (i_base_[search_parts.i_check_] < 0) {
        i_byte_index = i + 1;
        search_parts.i_tail_ = -i_base_[search_parts.i_check_]; /* マイナス値をプラスに変換 */
        break;
      }
      search_parts.i_base_ = search_parts.i_check_;
    }

    if (i >= i_byte_length) {
      if (i_check_[i_base_[search_parts.i_base_]] == search_parts.i_base_) {
        const int i_tail_index(i_base_[i_base_[search_parts.i_base_]]);
        if (i_tail_index < 0) {
          if (i_result_) result = i_result_[-i_tail_index]; /* ヒット */
          else           result = I_HIT_DEFAULT;
        }
        return true;
      }
    }
  }

  /* Tail処理 */
  uint64_t i_compare_size(i_byte_length - i_byte_index);
  if (memcmp(&c_tail_[search_parts.i_tail_], &c_byte[i_byte_index], i_compare_size) != 0) {
    return false;  /* データが存在しない　Tailの途中で不一致 */
  }

  i_byte_index         += i_compare_size;
  search_parts.i_tail_ += static_cast<int>(i_compare_size);

  if (i_byte_index >= i_byte_length) {
    if (c_tail_[search_parts.i_tail_] == C_TAIL_CHAR) {
      if (i_result_) result = i_result_[search_parts.i_tail_];  /* ヒット */
      else           result = I_HIT_DEFAULT;
    } else {
      return true;
    }
  }

  return false;
}


/* DoubleArray情報を書き込む                         */
/* @param i_write_size 書き込んだデータサイズ        */
/* @param fp           OutputFileStream              */
/* @return I_DA_NO_ERROR : 正常終了 0以外 : 異常終了 */
int DoubleArray::writeBinary(
  int64_t& i_write_size,
  FILE* fp) const noexcept
{
  try {
    if (checkInit()) {
      fwrite(&i_array_size_,  sizeof(i_array_size_),  1,             fp);
      fwrite(&i_tail_size_,   sizeof(i_tail_size_),   1,             fp);
      fwrite(&i_result_size_, sizeof(i_result_size_), 1,             fp);
      fwrite(i_base_,         sizeof(i_base_[0]),     i_array_size_, fp);
      fwrite(i_check_,        sizeof(i_check_[0]),    i_array_size_, fp);
      fwrite(c_tail_,         sizeof(c_tail_[0]),     i_tail_size_,  fp);
      if (i_result_size_) {
        fwrite(i_result_, sizeof(i_result_[0]), i_result_size_, fp);
      }

      i_write_size += sizeof(i_array_size_);
      i_write_size += sizeof(i_tail_size_);
      i_write_size += sizeof(i_result_size_);
      i_write_size += sizeof(i_base_[0])  * i_array_size_;
      i_write_size += sizeof(i_check_[0]) * i_array_size_;
      i_write_size += sizeof(c_tail_[0])  * i_tail_size_;
      if (i_result_size_) {
        i_write_size += sizeof(i_result_[0]) * i_result_size_;
      }
    } else {
      int i_zero(0);
      fwrite(&i_zero, sizeof(i_zero), 1, fp);
      i_write_size += sizeof(i_zero);
    }
  }
  catch (...) {
    return I_FAIELD_FILE_IO;
  }

  return I_NO_ERROR;
}


/* DoubleArray情報を読み込む                 */
/* @param i_read_size 読み込んだデータサイズ */
/* @param fp InputFileStream                 */
/* @return 0 : 正常終了 0以外 : 異常終了     */
int DoubleArray::readBinary(
  int64_t& i_read_size,
  FILE* fp) noexcept
{
  try {
    if (1 != fread(&i_array_size_, sizeof(i_array_size_), 1, fp)) /* 配列サイズ */
      throw;

    if (i_array_size_) {
      if ((1 != fread(&i_tail_size_,   sizeof(i_tail_size_),   1, fp)) /* Tail文字列サイズ */
      ||  (1 != fread(&i_result_size_, sizeof(i_result_size_), 1, fp)) /* Tail結果サイズ   */
      ||  (keepMemory())) /* 配列サイズが確定したのでメモリ確保 */
        throw;

      if ((static_cast<size_t>(i_array_size_) != fread(i_base_,  sizeof(i_base_[0]),  i_array_size_, fp))   /* Base     */
      ||  (static_cast<size_t>(i_array_size_) != fread(i_check_, sizeof(i_check_[0]), i_array_size_, fp))   /* Check    */
      ||  (static_cast<size_t>(i_tail_size_)  != fread(c_tail_,  sizeof(c_tail_[0]),  i_tail_size_,  fp)))  /* Tail文字 */
        throw;

      if (i_result_size_) {
        if (static_cast<size_t>(i_result_size_) != fread(i_result_, sizeof(i_result_[0]), i_result_size_, fp)) /* Tail結果 */
          throw;
      } else {
        i_result_ = nullptr;
      }

      i_read_size += sizeof(i_tail_size_);
      i_read_size += sizeof(i_result_size_);
      i_read_size += sizeof(i_base_[0])  * i_array_size_;
      i_read_size += sizeof(i_check_[0]) * i_array_size_;
      if (i_result_size_) {
        i_read_size += sizeof(i_result_[0]) * i_result_size_;
      }
    } else {
      i_read_size += sizeof(i_array_size_);
    }
  } catch (...) {
    deleteMemory();
    return I_FAIELD_FILE_IO;
  }

  return I_NO_ERROR;
}


/* 内部データを取得する                  */
/* @param i_array_size  配列サイズ       */
/* @param i_tail_size   Tail文字列サイズ */
/* @param i_result_size Tail結果サイズ   */
/* @param i_base        Base配列         */
/* @param i_check       Check配列        */
/* @param i_result      Tail結果配列     */
/* @param c_tail        Tail文字配列     */
void DoubleArray::getDoubleArrayData(
  uint64_t& i_array_size,
  uint64_t& i_tail_size,
  uint64_t& i_result_size,
  const int*& i_base,
  const int*& i_check,
  const int64_t*& i_result,
  const char*& c_tail) const noexcept
{
  i_array_size  = i_array_size_;
  i_tail_size   = i_tail_size_;
  i_result_size = i_result_size_;
  i_base        = i_base_;
  i_check       = i_check_;
  i_result      = i_result_;
  c_tail        = c_tail_;
}


/* 内部データを外部から設定する          */
/* @param i_array_size  配列サイズ       */
/* @param i_tail_size   Tail文字列サイズ */
/* @param i_result_size Tail結果サイズ   */
/* @param i_base        Base配列         */
/* @param i_check       Check配列        */
/* @param i_result      Tail結果配列     */
/* @param c_tail        Tail文字配列     */
int DoubleArray::setDoubleArrayData(
  int i_array_size,
  int i_tail_size,
  int i_result_size,
  const int* i_base,
  const int* i_check,
  const int64_t* i_result,
  const char* c_tail) noexcept
{
  i_array_size_  = i_array_size;  /* 配列サイズ       */
  i_tail_size_   = i_tail_size;   /* Tail文字列サイズ */
  i_result_size_ = i_result_size; /* Tail結果サイズ   */

  if (keepMemory()) /* 配列サイズが確定したのでメモリ確保 */
    return I_FAILED_MEMORY;

  try {
    memcpy(i_base_,  i_base,  sizeof(i_base_[0])  * i_array_size_);
    memcpy(i_check_, i_check, sizeof(i_check_[0]) * i_array_size_);
    memcpy(c_tail_,  c_tail,  sizeof(c_tail_[0])  * i_tail_size_);
    if (i_result) {
      memcpy(i_result_, i_result, sizeof(i_result_[0]) * i_result_size_);
    } else {
      i_result_ = nullptr;
    }
  } catch (...) {
    deleteMemory();
    return I_FAILED_MEMORY;
  }

  return I_NO_ERROR;
}


/* 結果IndexからByte情報を復元する                               */
/* @param c_info         復元したByte情報 呼び出し側でdeleteする */
/* @param i_reuslt_index 復元する結果Index                       */
void DoubleArray::reproductionFromIndex(
  char*& c_info,
  const int64_t i_result_index) const noexcept
{
  c_info = nullptr;
  uint64_t i_last(0);
  for (uint64_t i = 0; i < i_result_size_; ++i) {
    if (i_result_[i] == i_result_index) {
      i_last = i;
    }
  }

  if (i_last == 0) {
    return; /* 指定のIndexは無し */
  }

  vector<char> datas;
  int64_t i_tail_index(--i_last);  /* 終端記号を指しているので一つ前にする */
  while (c_tail_[i_tail_index]) {
    datas.push_back(c_tail_[i_tail_index--]);  /* Tail部分のデータ */
  }
  ++i_tail_index; /* Tail先頭の位置にするのでIncrement */

  uint64_t i_array_index(0); /* base位置特定 */
  for (uint64_t i = 0; i < i_array_size_; ++i) {
    if ((i_base_[i] < 0)
    &&  ((-i_base_[i]) == i_tail_index)) {
      i_array_index = i;
      break;
    }
  }

  while (i_array_index != 0) {
    uint64_t i_char = (i_array_index - i_base_[i_check_[i_array_index]]) & 0xff;
    datas.push_back(static_cast<char>(i_char));
    i_array_index = i_check_[i_array_index];
  }

  int i_index(0);
  c_info = new char[datas.size() + 1];
  for (auto data = datas.crbegin(), data_end = datas.crend(); data != data_end; ++data) {
    c_info[i_index++] = *data;
  }
  c_info[i_index] = 0x00; /* 終端記号 */
}

