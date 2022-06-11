#include "Board.h"
#include "AI.h"

//--- AI�𐶐����� ---//
AI* AI::createAi(type type)
{
	switch (type) {
	case TYPE_ORDERED:
		return new AI_ordered();
	case TYPE_NEGA_MAX:
		return new AI_nega_max();
	case TYPE_ALPHA_BETA:
		return new AI_alpha_beta();
	case TYPE_NEGA_SCOUT:
		return new AI_nega_scout();
	case TYPE_MONTE_CARLO:
		return new AI_monte_carlo();
	case TYPE_MONTE_CARLO_TREE:
		return new AI_monte_carlo_tree();
	default:
		return new AI_ordered();
		break;
	}

	return nullptr;
}


//--- ���Ԃɑł��Ă݂� ---//
bool AI_ordered::think(Board& b)
{
	for (int y = 0; y < Board::BOARD_SIZE; y++) {
		for (int x = 0; x < Board::BOARD_SIZE; x++) {
			if (b.mass_[y][x].put(Mass::ENEMY)) {
				return true;
			}
		}
	}
	return false;
}


//--- Nega-max�헪AI ---//
int AI_nega_max::evaluate(Board& b, Mass::status current, int& best_x, int& best_y)
{
	Mass::status next = (current == Mass::ENEMY) ? Mass::PLAYER : Mass::ENEMY;
	// ��������
	int r = b.calc_result();
	// ���ڕ]��
	if (r == current) return +10000;   // �Ăяo�����̏���
	if (r == next) return -10000;      // �Ăяo�����̕���
	if (r == Board::DRAW) return 0;    // ��������

	int score_max = -10001;   // �ł��Ȃ��͍̂ň�

	for (int y = 0; y < Board::BOARD_SIZE; y++)
	{
		for (int x = 0; x < Board::BOARD_SIZE; x++)
		{
			Mass& m = b.mass_[y][x];
			if (m.getStatus() != Mass::BLANK) continue;

			m.setStatus(current);   // ���̎��ł�
			int dummy;   // �ŏ�ʈȊO�͑ł킯�ł͂Ȃ��̂Ń_�~�[�����Ă��܂���
			int score = -evaluate(b, next, dummy, dummy);
			m.setStatus(Mass::BLANK);   //���߂�

			// �ő�l�̊m�ۂƎ�̋L�^
			if (score_max < score)
			{
				score_max = score;
				best_x = x;
				best_y = y;
			}
		}
	}
	return score_max;
}

bool AI_nega_max::think(Board& b)
{
	int best_x = -1, best_y;

	evaluate(b, Mass::ENEMY, best_x, best_y);

	if (best_x < 0) return false;   // �łĂ��͂Ȃ�����

	return b.mass_[best_y][best_x].put(Mass::ENEMY);
}


//--- alpha-beta�@AI ---//
int AI_alpha_beta::evaluate(int alpha, int beta, Board& b, Mass::status current, int& best_x, int& best_y)
{
	Mass::status next = (current == Mass::ENEMY) ? Mass::PLAYER : Mass::ENEMY;
	// ��������
	int r = b.calc_result();
	if (r == current) return +10000;   // �Ăяo�����̏���
	if (r == next) return -10000;      // �Ăяo�����̕���
	if (r == Board::DRAW) return 0;    // ��������

	int score_max = -9999;   // �ł��Ȃ��œ���

	for (int y = 0; y < Board::BOARD_SIZE; y++)
	{
		for (int x = 0; x < Board::BOARD_SIZE; x++)
		{
			Mass& m = b.mass_[y][x];
			if (m.getStatus() != Mass::BLANK) continue;

			m.setStatus(current);   // ���̎��ł�
			int dummy;
			// ���ʂł̓v���C���[�̎�ނƍő�E�ŏ�������ւ��B�����𔽓]���ē���ւ���Ɛ������]���\
			int score = -evaluate(-beta, -alpha, b, next, dummy, dummy);
			m.setStatus(Mass::BLANK);   //���߂�

			// ��ʂōň��̒l��舫��������i����̕]�����ǂ��Ȃ�����j�A�����K�w�͂���ȏ�T�����Ȃ�
			if (beta < score)
			{
				return (score_max < score) ? score : score_max;   // �ň��̒l��舫��
			}

			// �ő�l�̊m�ۂƎ�̋L�^
			if (score_max < score)
			{
				score_max = score;
				alpha = (alpha < score_max) ? score_max : alpha;   // ���̒l���X�V
				best_x = x;
				best_y = y;
			}
		}
	}
	return score_max;
}

bool AI_alpha_beta::think(Board& b)
{
	int best_x, best_y;

	if (evaluate(-10000, 10000, b, Mass::ENEMY, best_x, best_y) <= -9999)
		return false;   // �łĂ��͂Ȃ�����

	return b.mass_[best_y][best_x].put(Mass::ENEMY);
}


//--- Nega-Scout�@AI ---//
int AI_nega_scout::evaluate(int limit, int alpha, int beta, Board& board, Mass::status current, int& best_x, int& best_y)
{
	if (limit-- == 0) return 0;   // �[�������ɒB�����B���������ɂ��Ă���

	Mass::status next = (current == Mass::ENEMY) ? Mass::PLAYER : Mass::ENEMY;
	// ��������
	int r = board.calc_result();
	if (r == current) return +10000;   // �Ăяo�����̏���
	if (r == next) return -10000;      // �Ăяo�����̕���
	if (r == Board::DRAW) return 0;    // ��������

	int a = alpha, b = beta;

	for (int y = 0; y < Board::BOARD_SIZE; y++)
	{
		for (int x = 0; x < Board::BOARD_SIZE; x++)
		{
			Mass& m = board.mass_[y][x];
			if (m.getStatus() != Mass::BLANK) continue;

			m.setStatus(current);   // ���̎��ł�
			int dummy;
			int score = -evaluate(limit, -b, -a, board, next, dummy, dummy);   // ���j�͕��ʂɒT��
			if (a < score && score < beta && !(x == 0 && y == 0) && limit <= 2)
			{
				a = -evaluate(limit, -beta, -score, board, next, dummy, dummy);
			}
			m.setStatus(Mass::BLANK);   //���߂��B�@�@�@���j�ȍ~�͕]�����ǂ���΂�����ƒT��

			if (a < score)
			{
				a = score;
				best_x = x;
				best_y = y;
			}

			// ������
			if (beta <= a)
			{
				return a;
			}

			b = a + 1;   // �k���E�B���h�E�̍X�V
		}
	}
	return a;
}

bool AI_nega_scout::think(Board& b)
{
	int best_x, best_y;

	if (evaluate(5, -10000, 10000, b, Mass::ENEMY, best_x, best_y) <= -9999)
		return false;   // �łĂ��͂Ȃ�����

	return b.mass_[best_y][best_x].put(Mass::ENEMY);
}


//--- �����e�J�����@AI ---//
int AI_monte_carlo::evaluate(bool fiest_time, Board& board, Mass::status current, int& best_x, int& best_y)
{
	Mass::status next = (current == Mass::ENEMY) ? Mass::PLAYER : Mass::ENEMY;
	///���[
	// ��������
	int r = board.calc_result();
	if (r == current) return +10000;   // �Ăяo�����̏���
	if (r == next) return -10000;      // �Ăяo�����̕���
	if (r == Board::DRAW) return 0;    // ��������

	/// �󂢂Ă���}�X�𒲂ׂ�
	char x_table[Board::BOARD_SIZE * Board::BOARD_SIZE];
	char y_table[Board::BOARD_SIZE * Board::BOARD_SIZE];
	int wins[Board::BOARD_SIZE * Board::BOARD_SIZE];   // ������
	int loses[Board::BOARD_SIZE * Board::BOARD_SIZE];  // �s�k��
	int blank_mass_num = 0;
	// �󂢂Ă���}�X�𐔂��z��Ƃ��Ĉʒu���m��
	for (int y = 0; y < Board::BOARD_SIZE; y++)
	{
		for (int x = 0; x < Board::BOARD_SIZE; x++)
		{
			Mass& m = board.mass_[y][x];
			if (m.getStatus() == Mass::BLANK)
			{
				x_table[blank_mass_num] = x;
				y_table[blank_mass_num] = y;
				wins[blank_mass_num] = loses[blank_mass_num] = 0;
				blank_mass_num++;
			}
		}
	}

	if (fiest_time)
	{
		/// �����_���ɋ󂫃}�X��I�сA�������𐔂���
		// ��ԏ�̊K�w�Ń����_���Ɏw���̂��J��Ԃ�
		for (int i = 0; i < 10000; i++)
		{
			int idx = rand() % blank_mass_num;
			Mass& m = board.mass_[y_table[idx]][x_table[idx]];

			m.setStatus(current);   // ���̎��ł�
			int dummy;
			int score = -evaluate(false, board, next, dummy, dummy);
			m.setStatus(Mass::BLANK);   //���߂�

			if (0 <= score)
			{
				wins[idx]++;
			}
			else
			{
				loses[idx]++;
			}
		}
		/// �������̍ł������}�X��I��
		int score_max = -9999;
		for (int idx = 0; idx < blank_mass_num; idx++)
		{
			int score = wins[idx] + loses[idx];
			if (0 != score)
			{
				score = 100 * wins[idx] / score;   // ����
			}
			if (score_max < score)
			{
				score_max = score;
				best_x = x_table[idx];
				best_y = y_table[idx];
			}
			std::cout << x_table[idx] + 1 << (char)('a' + y_table[idx]) << " " << score
				<< "% �iwin�F" << wins[idx] << ", lose�F" << loses[idx] << "�j" << std::endl;
		}

		return score_max;
	}

	/// �ŏ�ʂłȂ���΍Ō�܂Ń����_���ɑI��
	// ��ʂłȂ��w�͂ǂ�ǂ�K���ɑł��Ă���
	int idx = rand() % blank_mass_num;
	Mass& m = board.mass_[y_table[idx]][x_table[idx]];
	m.setStatus(current);   // ���̎��ł�
	int dummy;
	int score = -evaluate(false, board, next, dummy, dummy);
	m.setStatus(Mass::BLANK);   // ���߂�

	return score;
}

bool AI_monte_carlo::think(Board& b)
{
	int best_x = -1, best_y;

	if (evaluate(true, b, Mass::ENEMY, best_x, best_y) <= -9999)
		return false;   // �łĂ��͂Ȃ�����

	return b.mass_[best_y][best_x].put(Mass::ENEMY);
}


//--- �����e�J�����ؒT��AI ---//
// �����������قǑ������蓖�Ă�
int AI_monte_carlo_tree::select_mass(int n, int* a_count, int* a_wins)
{
	int total = 0;
	for (int i = 0; i < n; i++)
	{
		total += 10000 * (a_wins[i] + 1) / (a_count[i] + 1);   // 0�̎��ɂ��m�����オ��悤��+1����
	}
	if (total <= 0) return -1;

	int r = rand() % total;
	for (int i = 0; i < n; i++)
	{
		r -= 10000 * (a_wins[i] + 1) / (a_count[i] + 1);
		if (r < 0)
		{
			return i;
		}
	}
	return -1;
}

int AI_monte_carlo_tree::evaluate(bool all_search, int sim_count, Board& board, Mass::status current, int& best_x, int& best_y)
{
	Mass::status next = (current == Mass::ENEMY) ? Mass::PLAYER : Mass::ENEMY;
	///���[
	// ��������
	int r = board.calc_result();
	if (r == current) return +100;   // �Ăяo�����̏���
	if (r == next) return -100;      // �Ăяo�����̕���
	if (r == Board::DRAW) return 0;    // ��������

	/// �󂢂Ă���}�X�𒲂ׂ�
	char x_table[Board::BOARD_SIZE * Board::BOARD_SIZE];
	char y_table[Board::BOARD_SIZE * Board::BOARD_SIZE];
	int wins[Board::BOARD_SIZE * Board::BOARD_SIZE];    // ������
	int count[Board::BOARD_SIZE * Board::BOARD_SIZE];   // �s�k��
	int scores[Board::BOARD_SIZE * Board::BOARD_SIZE];
	int blank_mass_num = 0;
	// �󂢂Ă���}�X�𐔂��z��Ƃ��Ĉʒu���m��
	for (int y = 0; y < Board::BOARD_SIZE; y++)
	{
		for (int x = 0; x < Board::BOARD_SIZE; x++)
		{
			Mass& m = board.mass_[y][x];
			if (m.getStatus() == Mass::BLANK)
			{
				x_table[blank_mass_num] = x;
				y_table[blank_mass_num] = y;
				wins[blank_mass_num] = count[blank_mass_num] = 0;
				scores[blank_mass_num] = -1;
				blank_mass_num++;
			}
		}
	}

	if (all_search)
	{
		for (int i = 0; i < sim_count; i++)
		{
			int idx = select_mass(blank_mass_num, count, wins);   // �����ɉ������}�X�̑I��
			if (idx < 0) break;
			Mass& m = board.mass_[y_table[idx]][x_table[idx]];

			m.setStatus(current);   // ���̎��ł�
			int dummy;
			int score = -evaluate(false, 0, board, next, dummy, dummy);
			m.setStatus(Mass::BLANK);   //���߂�

			if (0 < score)
			{
				wins[idx]++;
				count[idx]++;
			}
			else
			{
				count[idx]++;
			}
			/// ���s�񐔂𒴂����ۂɖ؂𐬒��i�����ł�臒l�̓}�W�b�N�i���o�[�j
			// 臒l�𒴂���΁A�؂𐬒�������
			if (sim_count / 10 < count[idx]   // 臒l��10%�ȏ�̒T����
				&& 10 < sim_count)            // �񐔂����Ȃ����̓����_���̐��x��������̂ŁA���������Ȃ�
			{
				m.setStatus(current);   // ���̎��ł�
				scores[idx] = 100 - evaluate(true, (int)sqrt(sim_count), board, next, dummy, dummy);
				m.setStatus(Mass::BLANK);   //���߂�
				wins[idx] = -1;   // ���̎}�͗����ŌĂ΂�Ȃ��悤�ɂ���
			}
		}
		/// �������̍ł������}�X��I��
		int score_max = -9999;
		for (int idx = 0; idx < blank_mass_num; idx++)
		{
			int score;
			if (-1 == wins[idx])
			{
				score = scores[idx];   // �}�����ꂵ���B�����������ꍇ�͂��̕]���l���̗p�B
			}
			else if (0 == count[idx])
			{
				score = 0;   // ��x���ʂ�Ȃ�����
			}
			else
			{
				double c = 1. * sqrt(2 * log(sim_count) / count[idx]);
				score = 100 * wins[idx] / count[idx] + (int)(c);   // ����
			}

			if (score_max < score)
			{
				score_max = score;
				best_x = x_table[idx];
				best_y = y_table[idx];
			}
			std::cout << x_table[idx] + 1 << (char)('a' + y_table[idx]) << " " << score
				<< "% �iwin�F" << wins[idx] << ", count�F" << count[idx] << "�j" << std::endl;
		}

		return score_max;
	}

	/// �ŏ�ʂłȂ���΍Ō�܂Ń����_���ɑI��
	// ��ʂłȂ��w�͂ǂ�ǂ�K���ɑł��Ă���
	int idx = rand() % blank_mass_num;
	Mass& m = board.mass_[y_table[idx]][x_table[idx]];
	m.setStatus(current);   // ���̎��ł�
	int dummy;
	int score = -evaluate(false, 0, board, next, dummy, dummy);
	m.setStatus(Mass::BLANK);   // ���߂�

	return score;
}

bool AI_monte_carlo_tree::think(Board& b)
{
	int best_x = -1, best_y;

	evaluate(true, 10000, b, Mass::ENEMY, best_x, best_y);

	if (best_x < 0) return false;   // �łĂ��͂Ȃ�����

	return b.mass_[best_y][best_x].put(Mass::ENEMY);
}