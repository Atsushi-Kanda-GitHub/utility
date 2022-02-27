#include "CoordinateDescent.h"
#include <fstream>
#include <iostream>
#include <random>
#include <math.h>
#include <stdlib.h>

using namespace std;


/* ����                                                         */
/* @param d_result �Z�o����                                     */
/* @param inputs   ���͏��                                     */
/* @return I_POSITIVE_EXAMPLE : ����  I_NEGATIVE_EXAMPLE : ���� */
int CoordinateDescent::judge(
  double& d_result,
  const FeatureValues& inputs) const
{
  d_result = 0.0;
  for (const auto& input : inputs) {
    d_result += feature_values_[input.first] * input.second;
  }

  if (d_result >= 0.0)
    return I_POSITIVE;

  return I_NEGATIVE;
}

/* CoordinateDescent�ɂ��w�K         */
/* @param learn_data    �w�K�f�[�^     */
/* @param d_soft_margin �\�t�g�}�[�W�� */
void CoordinateDescent::learn(
  const vector<LearnData>& learn_data,
  const double d_soft_margin)
{
  const int i_learn_size(learn_data.size());	/* �w�K�R�[�p�X�� */

  int i_max_feature_index;
  getMaxFeatureIndex(i_max_feature_index, learn_data);
  feature_values_.resize(i_max_feature_index + 1, 0.0); /* �T�C�Y�Ȃ̂�+1 */

  vector<double> values(i_learn_size, 0.0);

  /* 1vector���ɑS�l��2������Z�����l��ێ� */
  auto value = values.begin();
  for (const auto& learn : learn_data) {
    double d_add(d_soft_margin);
    const auto& feature_values = learn.feature_values_;
    for (const auto feature_value : feature_values) {
      d_add += feature_value.second * feature_value.second;
    }
    *value = d_add;
    ++value;
  }

  coordinateDescentSolve(d_soft_margin, values, learn_data);
}


/* �ő�f��Index������                      */
/* @param i_max_feature_index �ő�f��Index */
/* @param learn_data          �w�K�f�[�^    */
void CoordinateDescent::getMaxFeatureIndex(
  int& i_max_feature_index,
  const vector<LearnData>& learn_data) const
{
  i_max_feature_index = 0;
  for (const auto& learn : learn_data) {
    const auto& feature_values = learn.feature_values_;
    for (const auto& feature_value : feature_values) {
      if (i_max_feature_index < feature_value.first) {
        i_max_feature_index = feature_value.first;
      }
    }
  }
}


/* CoordinateDescent�v�Z                */
/* @param i_learn_size  �w�K�R�[�p�X��  */
/* @param d_soft_margin �\�t�g�}�[�W��  */
/* @param values        �S�l��2����Z�l */
/* @param learn_data    �w�K�f�[�^      */
void CoordinateDescent::coordinateDescentSolve(
  const double& d_soft_margin,
  const vector<double>& values,
  const vector<LearnData>& learn_data)
{
  const int i_learn_size(learn_data.size());
  vector<double> alphas(i_learn_size, 0.0);

  int i_value(0);
  vector<int> indexes(i_learn_size);
  for (auto index = indexes.begin(), end = indexes.end(); index != end; ++index, ++i_value) {
    *index = i_value;
  }

  const double d_eps(0.1);
  const int i_max_roop(2000);
  int i_change_learn_size(i_learn_size);
  double d_min_old(-HUGE_VAL), d_max_old(HUGE_VAL);
  for (int i = 0; i < i_max_roop; ++i) {
    shuffle(indexes, i_learn_size);
    double d_min_value( HUGE_VAL);
    double d_max_value(-HUGE_VAL);

    for (int n = 0; n < i_change_learn_size; ++n) {
      if (coordinateUpdate(indexes, alphas, d_min_value, d_max_value, n, i_change_learn_size-1, d_min_old, d_max_old, d_soft_margin, values, learn_data)) {
        --i_change_learn_size;
        --n;
      }
    }

    if (d_max_value - d_min_value <= d_eps) {
      if (i_change_learn_size == i_learn_size) {
        break;
      }
      i_change_learn_size = i_learn_size;
      d_max_old =  HUGE_VAL;
      d_min_old = -HUGE_VAL;
    }
    else {
      d_max_old = d_max_value;
      d_min_old = d_min_value;
      if (d_max_old <= 0) d_max_old =  HUGE_VAL;
      if (d_min_old >= 0) d_min_old = -HUGE_VAL;
    }
  }
}


/* CoordinateDescent�X�V                    */
/* @param indexes      Class�z��            */
/* @param alphas       �X�V���Ɏg�p����̈� */
/* @param d_min_value  ���z�ŏ��l           */
/* @param d_max_value  ���z�ő�l           */
/* @param i_update     �X�V����Index        */
/* @param i_last_index �X�V�͈͂̍ŏIIndex  */
/* @param d_min_old    ���O�̌��z�ŏ��l     */
/* @param d_max_old    ���O�̌��z�ő�l     */
/* @param values       �S�l��2����Z�l      */
/* @param learn_data   �w�K�f�[�^           */
bool CoordinateDescent::coordinateUpdate(
  vector<int>& indexes,
  vector<double>& alphas,
  double& d_min_value,
  double& d_max_value,
  const int i_update,
  const int i_last_index,
  const double& d_min_old,
  const double& d_max_old,
  const double& d_soft_margin,
  const vector<double>& values,
  const vector<LearnData>& learn_data)
{
  const int i_current(indexes[i_update]);
  const int i_class(learn_data[i_current].i_class_);
  const LearnData& learn = learn_data[i_current];

  double d_gradient(0.0);
  const auto& feature_values = learn.feature_values_;
  for (const auto& feature_value : feature_values) {
    d_gradient += feature_values_[feature_value.first] * feature_value.second;
  }
  d_gradient = (d_gradient * i_class - 1) + alphas[i_current] * d_soft_margin;

  double d_update_value(0.0);
  if (alphas[i_current] == 0) {
    if (d_gradient > d_max_old) {
      indexes[i_update]     = indexes[i_last_index];
      indexes[i_last_index] = i_current;
      return true;
    }
    else if (d_gradient < 0) {
      d_update_value = d_gradient;
    }
  }
  else if (alphas[i_current] == HUGE_VAL) {
    if (d_gradient < d_min_old) {
      indexes[i_update]     = indexes[i_last_index];
      indexes[i_last_index] = i_current;
      return true;
    }
    else if (d_gradient > 0) {
      d_update_value = d_gradient;
    }
  }
  else {
    d_update_value = d_gradient;
  }
  if (d_max_value < d_update_value) d_max_value = d_update_value;
  if (d_min_value > d_update_value) d_min_value = d_update_value;

  double d_compare(d_update_value);
  if (d_compare < 0) {
    d_compare *= (-1);
  }	

  if (d_compare > 1.0e-12) {
    double d_alpha_old = alphas[i_current];
    double d_temp(alphas[i_current] - d_gradient / values[i_current]);
    double d_max(d_temp > 0.0 ? d_temp : 0.0);
    alphas[i_current] = (d_max < HUGE_VAL ? d_max : HUGE_VAL);
    double d_update((alphas[i_current] - d_alpha_old) * i_class);

    const auto& feature_values = learn.feature_values_;
    for (const auto& feature_value : feature_values) {
      feature_values_[feature_value.first] += d_update * feature_value.second;
    }
  }

  return false;
}


/* �����̔z��̓��e�������_���ɓ���ւ���   */
/* @param indexes      �z��f�[�^           */
/* @param i_learn_size �w�K�R�[�p�X��       */
void CoordinateDescent::shuffle(
  vector<int>& indexes,
  const int i_learn_size) const
{
  random_device rd;
  mt19937 mt(rd());
  for (int i = 0 ; i < i_learn_size ; ++i) {
    uniform_int_distribution<int> random_value(0, i_learn_size - i);
    int i_other = i + random_value(mt);

    int i_tmp        = indexes[i];
    indexes[i]       = indexes[i_other];
    indexes[i_other] = i_tmp;
  }
}


/* CoordinateDescent�f�[�^��ݒ� */
/* @param feature_values �f���l  */
void CoordinateDescent::setCoordinateDescentData(
  const vector<double>& feature_values)
{
  feature_values_ = feature_values;
}


/* CoordinateDescent�f�[�^���擾 */
const vector<double>& CoordinateDescent::getCoordinateDescentData() const
{
  return feature_values_;
}


/* CoordinateDescent�f�[�^���t�@�C������ǂݍ��� */
/* @param c_file_path �t�@�C���p�X               */
/* @return false : ����I��  true : �ُ�         */
bool CoordinateDescent::readCoordinateDescentData(
  const char* c_file_path)
{
  bool b_result(false);

  FILE* fin = NULL;
  try {
    fin = fopen(c_file_path, "rb");
    if (fin == NULL) {
      return true;
    }

    unsigned int i_size;
    if (1 != fread(&i_size, sizeof(i_size), 1, fin)) {
      throw;
    }

    if (i_size) {
      feature_values_.resize(i_size);
      if (i_size != fread(&feature_values_[0], sizeof(feature_values_[0]), i_size, fin)) {
        throw;
      }
    }
  }
  catch (...) {
    b_result = true;
  }
  if (fin) {
    fclose(fin);
  }

  return b_result;
}


/* CoordinateDescent�f�[�^���t�@�C���o�� */
/* @param c_file_path �t�@�C���p�X       */
/* @return false : ����I��  true : �ُ� */
bool CoordinateDescent::writeCoordinateDescentData(
  const char* c_file_path) const
{
  bool b_result(false);

  FILE* fout = NULL;
  try {
    fout = fopen(c_file_path, "wb");
    if (fout == NULL) {
      return true;
    }

    unsigned int i_size(feature_values_.size());
    fwrite(&i_size, sizeof(i_size), 1, fout);
    if (i_size) {
      fwrite(&feature_values_[0], sizeof(feature_values_[0]), i_size, fout);
    }
  }
  catch (...) {
    b_result = true;
  }
  if (fout) {
    fclose(fout);
  }

  return b_result;
}


/* �w��̑f��Index����f���l���擾���� */
/* @param d_feature_value �f���l       */
/* @param i_feature_index �f��Index    */
void CoordinateDescent::getFeatureValue(
  double& d_featrue_value,
  const int i_feature_index) const
{
  d_featrue_value = feature_values_[i_feature_index];
}

