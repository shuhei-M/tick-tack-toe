#pragma once

#include "Board.h"

//=== ���N���X ===//
class AI
{
public:
	AI() {}
	virtual ~AI() {}

	virtual bool think(Board& b) = 0;

public:
	enum type {
		TYPE_ORDERED = 0,
		TYPE_NEGA_MAX,
		TYPE_ALPHA_BETA,
		TYPE_NEGA_SCOUT,
		TYPE_MONTE_CARLO,
		TYPE_MONTE_CARLO_TREE,
	};

	static AI* createAi(type type);
};

//=== ���Ԃɑł��Ă݂� ===//
class AI_ordered : public AI
{
public:
	AI_ordered() {}
	~AI_ordered() {}

	bool think(Board& b);
};


//=== Nega-max�헪AI ===//
class AI_nega_max : public AI
{
private:
	int evaluate(Board& b, Mass::status current, int& best_x, int& best_y);
public:
	AI_nega_max() {}
	~AI_nega_max() {}

	bool think(Board& b);
};


//=== alpha-beta�@AI ===//
class AI_alpha_beta : public AI
{
private:
	int evaluate(int alpha, int beta, Board& b, Mass::status current, int& best_x, int& best_y);
public:
	AI_alpha_beta() {}
	~AI_alpha_beta() {}

	bool think(Board& b);
};


//=== Nega-Scout�@AI ===//
class AI_nega_scout : public AI
{
private:
	// limit�F�Ȃ�Đ�܂œǂނ�
	int evaluate(int limit, int alpha, int beta, Board& board, Mass::status current, int& best_x, int& best_y);
public:
	AI_nega_scout() {}
	~AI_nega_scout() {}

	bool think(Board& b);
};


//=== �����e�J�����@AI ===//
class AI_monte_carlo : public AI
{
private:
	int evaluate(bool fiest_time, Board& board, Mass::status current, int& best_x, int& best_y);
public:
	AI_monte_carlo() {}
	~AI_monte_carlo() {}

	bool think(Board& b);
};

//=== �����e�J�����ؒT��AI ===//
class AI_monte_carlo_tree : public AI
{
private:
	static int select_mass(int n, int* a_count, int* a_wins);
	int evaluate(bool all_search, int sim_count, Board& board, Mass::status current, int& best_x, int& best_y);
public:
	AI_monte_carlo_tree() {}
	~AI_monte_carlo_tree() {}

	bool think(Board& b);
};