#ifndef COORDINATEDESCENT_H
#define COORDINATEDESCENT_H

/**
* CoordinateDescent
*
* @brief  CoordinateDescent
* @file   CoordinateDescent.h
* @author dev.atsushi.kanda@gmail.com
* @date   2018/06/12
*/

#include <vector>
#include <utility>


/* first : �f��Index second : �l */
using FeatureValues = std::vector<std::pair<int, double>>;

class LearnData;

class CoordinateDescent
{
public:
  static constexpr int    I_POSITIVE            =  1;
  static constexpr int    I_NEGATIVE            = -1;
  static constexpr double D_DEFAULT_SOFT_MARGIN =  1.0;

public:
  /** �R���X�g���N�^ */
  CoordinateDescent() {}

  /** �f�X�g���N�^ */
  ~CoordinateDescent() {};

  /** CoordinateDescent�ɂ��w�K
  * @param learn_data    �w�K�f�[�^
  * @param d_soft_margin �\�t�g�}�[�W��
  * @return
  */
  void learn(
    const std::vector<LearnData>& learn_data,
    const double d_soft_margin = D_DEFAULT_SOFT_MARGIN);

  /** ����
  * @param d_result �Z�o����
  * @param inputs   ���͏��
  * @return I_POSITIVE_EXAMPLE : ����  I_NEGATIVE_EXAMPLE : ����
  */
  int judge(
    double& d_result,
    const FeatureValues& inputs) const;

  /** CoordinateDescent�f�[�^���t�@�C������ǂݍ���
  * @param c_file_path �t�@�C���p�X
  * @return false : ����I��  true : �ُ�
  */
  bool readCoordinateDescentData(
    const char* c_file_path);

  /** CoordinateDescent�f�[�^���t�@�C���o��
  * @param c_file_path �t�@�C���p�X
  * @return false : ����I��  true : �ُ�
  */
  bool writeCoordinateDescentData(
    const char* c_file_path) const;

  /** CoordinateDescent�f�[�^��ݒ�
  * @param i_feature_size �f����
  * @param feature_values �f���l
  * @return
  */
  void setCoordinateDescentData(
    const std::vector<double>& feature_values);

  /** CoordinateDescent�f�[�^���擾
  * @return �f���l
  */
  const std::vector<double>& getCoordinateDescentData() const;

  /** �w��̑f��Index����f���l���擾����
  * @param d_feature_value �f���l
  * @param i_feature_index �f��Index
  * @return
  */
  void getFeatureValue(
    double& d_featrue_value,
    const int i_feature_index) const;

private:
  /** �ő�f��Index������
  * @param i_max_feature_index �ő�f��Index
  * @param learn_data          �w�K�f�[�^
  * @return
  */
  void getMaxFeatureIndex(
    int& i_feature_size,
    const std::vector<LearnData>& learn_data) const;

  /** CoordinateDescent�v�Z
  * @param d_soft_margin �\�t�g�}�[�W��
  * @param values     �S�l��2����Z�l
  * @param learn_data �w�K�f�[�^
  * @return
  */
  void coordinateDescentSolve(
    const double& d_soft_margin,
    const std::vector<double>& values,
    const std::vector<LearnData>& learn_data);

  /** CoordinateDescent�X�V
  * @param indexes      Class�z��
  * @param alphas       �X�V���Ɏg�p����̈�
  * @param d_min_value  ���z�ŏ��l
  * @param d_max_value  ���z�ő�l
  * @param i_update     �X�V����Index
  * @param i_last_index �X�V�͈͂̍ŏIIndex
  * @param d_min_old    ���O�̌��z�ŏ��l
  * @param d_max_old    ���O�̌��z�ő�l
  * @param values       �S�l��2����Z�l
  * @param learn_data   �w�K�f�[�^
  * @return
  */
  bool coordinateUpdate(
    std::vector<int>& indexes,
    std::vector<double>& alphas,
    double& d_min_value,
    double& d_max_value,
    const int i_update,
    const int i_last_index,
    const double& d_min_old,
    const double& d_max_old,
    const double& d_soft_margin,
    const std::vector<double>& values,
    const std::vector<LearnData>& learn_data);

  /** �����̔z��̓��e�������_���ɓ���ւ���
  * @param indexesc     �z��f�[�^
  * @param i_learn_size �w�K�R�[�p�X��
  * @return
  */
  void shuffle(
    std::vector<int>& indexes,
    const int i_learn_size) const;

private:
  /** �f���l */
  std::vector<double> feature_values_;
};


/** �s����\������f�[�^ */
class LearnData
{
public:
  /** �R���X�g���N�^ */
  LearnData() : i_class_(0) {}

  /** �f�X�g���N�^ */
  ~LearnData() {};

  /** ���ᕉ�� */
  int i_class_;

  /** first : �f��Index second : �l */
  FeatureValues feature_values_;
};

#endif
