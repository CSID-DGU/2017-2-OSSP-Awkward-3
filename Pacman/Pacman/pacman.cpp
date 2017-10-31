#include <iostream>
#include <array>
#include <windows.h>
#include <conio.h> 

using namespace std;

///
///
//		���� �м� : �ܼ��� �̿��� �Ѹ� �������� �� ȭ�� ���� �׷����� �׸��带 �̿��� ������ �����Ѵ�.
//					��Ģ�� �⺻ �Ѹǰ� �����ϸ�, ���� ���� �������� �����Ǿ� ���� ���� �����̴�.
//					��� ���� �� ������ ������ Ŭ����Ǹ�, ���ѷ����� ������ ���� �ö󰡴� �����̴�. 
//					����� ���� ������ ���� ���� �Ǹ�, ���� ���� �� �ִ� ���·� ���Ѵ�. �̰� �׳� ����
///
///


// Keyboard ASCII codes
#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27

// Window handle
static HANDLE hStdout;
static CONSOLE_SCREEN_BUFFER_INFO csbi;
DWORD dummy;

class Pacman
{
public:
	bool mouthOpen = false;		// �Ѹ� �Ը��
	bool godMode = false;		// ������� (���� ��Ƹ��� �� �ִ� ����)
	int godModeMaxMoves = 40;	// ������� ���¿��� �̵��� �� �ִ� Ƚ��
	int godModeMovesLeft = 40;	// ������� �̵� ���� Ƚ��

								// Reset
	void Reset();
};

void Pacman::Reset()		// �Ѹ� ����
{
	mouthOpen = false;	// ���� ������ �ݾҴ� �ϴ� ����
	godMode = false;	// ���� ��� On / Off
	godModeMaxMoves = 40;
	godModeMovesLeft = 40;
}

class Game
{
public:
	int lives = 3;		// ���
	int score = 0;		// ����
	int dotsRemaining = 0;			// ���� �� ����
	int teleporterLeft = 0;			// ���� �ڷ���Ʈ ����
	int teleporterRight = 0;		// ������ �ڷ���Ʈ ����
	int status = 0;
	int themeMusicPlayed = 0;		// �׸� ���� �÷��� �Ǿ��°�
	bool exit = false;		// ���� ���� ����

							// Reset
	void Reset();
};

void Game::Reset()			// ���� ����
{
	lives = 3;		// ���
	score = 0;		// ����
	dotsRemaining = 0;		// ���� �� ����
	teleporterLeft = 0;
	teleporterRight = 0;
	status = 0;
	themeMusicPlayed = 0;
}

class Grid			// �׸��� (���� Status)
{
public:
	// Grid numbers			 
	static const int nothing = 0;			// �� ����
	static const int wall = 1;				// ��
	static const int white_dot = 2;			// �Դ� ��
	static const int yellow_dot = 3;		// ������ ���� ���
	static const int blinky = 4;			// ���� 1
	static const int inky = 5;				// ���� 2
	static const int pinky = 6;				// ���� 3
	static const int clyde = 7;				// ���� 4
	static const int pacman = 8;			// �Ѹ�
	static const int teleport = 9;			// �ڷ���Ʈ

											// Grid char equivalents			// �� ���°� �ֿܼ��� ǥ�õ� ���ڿ�
	static const char nothing_char = ' ';
	static const char wall_char = '#';
	static const char white_dot_char = '.';
	static const char yellow_dot_char = '.';
	static const char blinky_char = 'o';
	static const char inky_char = 'o';
	static const char pinky_char = 'o';
	static const char clyde_char = 'o';
	static const char pacman_char = 'O';
	static const char pacman_mouth_open_char = 'C';

	// Grid colors
	static const int nothing_color = 0; // Black
	static const int wall_color = 1; // Blue
	static const int white_dot_color = 15; // White
	static const int yellow_dot_color = 14; // Yellow
	static const int blinky_color = 4; // 
	static const int inky_color = 5; // 
	static const int pinky_color = 6; // 
	static const int clyde_color = 7; // 
	static const int pacman_color = 14; // Yellow
	static const int pacman_flash_color = 15; // White
	static const int pacman_godmode_color = 13; // Pink

												// Actual grid
	array<int, 575> originalGrid, currentGrid;
	static const int columns = 23;		// ���� 23
	static const int rows = 25;			// ���� 25

										// Locations of characters
	int pacmanLocation = 0;		// �� ĳ������ �׸��� �� ��ġ
	int blinkyLocation = 0;
	int inkyLocation = 0;
	int pinkyLocation = 0;
	int clydeLocation = 0;
	int leftTeleporterLocation = -1;
	int rightTeleporterLocation = -1;

	// Dots count
	int dotsRemaining = 0;

	// Initialize grid
	Grid();

	// Flash screen when all dots are eaten
	void Flash();

	// Reset grid
	void Reset();

	// Draw grid
	void Draw(Game& game, Pacman& pacman);
};

void Grid::Reset()
{
	// Copy grid
	currentGrid = originalGrid = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 3, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1,
		1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1,
		1, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 1,
		1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		9, 0, 0, 0, 0, 2, 0, 0, 1, 4, 5, 0, 6, 7, 1, 0, 0, 2, 0, 0, 0, 0, 9,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1,
		1, 3, 2, 2, 1, 2, 2, 2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 1, 2, 2, 3, 1,
		1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1,
		1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
		1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1,
		1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};

	// Find out where characters are
	int dotsCounter = 0;

	for (int i = 0; i < originalGrid.size(); ++i) {		// ���� 1ĭ�� Ȯ���ϸ� �׸��带 �����.
		switch (originalGrid[i]) {
		case pacman:
			pacmanLocation = i;			// �Ѹ� ��ġ ȹ��
			break;
		case white_dot: case yellow_dot:		// ���� �� ����
			dotsCounter++;
			break;
		case teleport:			// �ڷ���Ʈ
			if (leftTeleporterLocation == -1) {
				leftTeleporterLocation = i;		// ���� �ڷ���Ʈ ��ġ ȹ��
			}
			else {
				rightTeleporterLocation = i;	// ������ �ڷ���Ʈ ��ġ ȹ��
			}

			break;
		case blinky:			// ���� ��ġ ȹ��
			blinkyLocation = i;
			break;
		case inky:
			inkyLocation = i;
			break;
		case pinky:
			pinkyLocation = i;
			break;
		case clyde:
			clydeLocation = i;
			break;
		default:
			break;
		}
	}

	dotsRemaining = dotsCounter;
}

void Grid::Flash()	// Ŭ���� �� ��¦��¦ ȿ��
{
	int alternate = 1;	// ��ü�� ����
	int color = 0;

	for (int k = 0; k < 10; k++) {
		if (alternate == 0) {
			color = 1;		// �Ķ���
			alternate = 1;
		}
		else {
			color = 15;		// ���
			alternate = 0;
		}

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				switch (currentGrid[(i * columns) + j]) {
				case wall:			//   ���̶��
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);		// ���� ����
					SetConsoleCursorPosition(hStdout, { (short)j, (short)i });			// Ŀ�� ��ġ ����		
					cout << "#";
					break;
				default:
					break;
				}
			}
		}

		Sleep(200);
	}
}

Grid::Grid()
{
	Reset();
}

void Grid::Draw(Game& game, Pacman& pacmanObj)		// �׸��带 ������ �׸��� �۾�
{
	char character, color;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			switch (currentGrid[(i * columns) + j]) {
			case wall:		// ��
				character = wall_char; color = wall_color; break;
			case white_dot:			// �Դ� ��
				character = white_dot_char; color = white_dot_color; break;
			case yellow_dot:		// ���� ���� ���ϴ� ��
				character = yellow_dot_char; color = yellow_dot_color; break;
			case blinky:			// �� 1, 2, 3, 4
				character = blinky_char; color = blinky_color; break;
			case inky:
				character = inky_char; color = inky_color; break;
			case pinky:
				character = pinky_char; color = pinky_color; break;
			case clyde:
				character = clyde_char; color = clyde_color; break;
			case pacman:		// �Ѹ�
								// Make pacman chomp
				if (pacmanObj.mouthOpen) {		// ������ �� ���� ������ �ݾҴ�
					character = pacman_mouth_open_char;
				}
				else {
					character = pacman_char;
				}

				// Flash pacman for last 10 moves of godmode
				if (pacmanObj.godMode) {		// ���� ��尡 Ǯ���� 10 ���� �� ��¦�̴� ȿ��
					if ((pacmanObj.godModeMovesLeft <= 10) && (pacmanObj.godModeMovesLeft % 2 == 1)) {
						color = pacman_flash_color;
					}
					else {
						color = pacman_godmode_color;
					}
				}
				else {
					color = pacman_color;
				}

				break;
				// Nothing & Teleport
			default:
				character = nothing_char; color = nothing_color; break;
			}

			// character�� �׸� (character�� �Ѹ��� �ƴ� ������ �׸��� data�� ����)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
			SetConsoleCursorPosition(hStdout, { (short)j, (short)i });
			cout << character;
		}
	}

	// Print score and lives
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetConsoleCursorPosition(hStdout, { 0, 26 });

	int spacing;		// ���� �ڸ����� ���� ���� �з����Ƿ� ������ �޲ٴ� �۾�

	if (game.score > 9999) {
		spacing = 3;
	}
	else if (game.score > 999) {
		spacing = 4;
	}
	else if (game.score > 99) {
		spacing = 5;
	}
	else if (game.score > 9) {
		spacing = 6;
	}
	else {
		spacing = 7;
	}

	cout << "Score: " << game.score;		// ������ ���� ��� ǥ��

	for (int i = 0; i < spacing; i++) {		// ����
		cout << " ";
	}

	cout << "Lives: " << game.lives;
}

void movePacman(Game& game, Grid& grid, Pacman& pacman, int newLocation) {		// �Ѹ� �̵�
	pacman.mouthOpen = !pacman.mouthOpen;		// ���� ���� ����

	int locationOffset = newLocation;
	int oldLocation = grid.pacmanLocation;		// �׸��忡 �ִ� �Ѹ��� ��ġ�� oldLocation�� �ִ´�.
	newLocation = oldLocation + newLocation;

	int oldCharacter = grid.currentGrid[oldLocation];		// ���� ĳ���� ��ġ
	int newCharacter = grid.currentGrid[newLocation];		// �̵��� ĳ���� ��ġ

															// Process godmode
	if (pacman.godMode) {
		if (pacman.godModeMovesLeft - 1 == 0) {		// ���� �̵�Ƚ���� �ٵǸ�
			pacman.godMode = false;
			pacman.godModeMovesLeft = pacman.godModeMaxMoves;
		}
		else {
			pacman.godModeMovesLeft--;
		}
	}

	if (oldLocation == grid.leftTeleporterLocation && locationOffset == -1) { // ���� �ڷ���Ʈ �������� �������� ����
		grid.currentGrid[oldLocation] = grid.teleport;		// �̵� �� ��ġ �׸��带 teleport�� �ٲ� (�Ѹ� ����)
		grid.pacmanLocation = grid.rightTeleporterLocation;		// �Ѹ��� ��ǥ�� ������ �ڷ���Ʈ ��ǥ���� �̵�
		grid.currentGrid[grid.rightTeleporterLocation] = grid.pacman;	// ������ �ڷ���Ʈ �׸��带 �Ѹ����� �ٲ�
	}
	else if (oldLocation == grid.rightTeleporterLocation && locationOffset == 1) { // ������ �ڷ���Ʈ ���� -> ������ �̵�
		grid.currentGrid[oldLocation] = grid.teleport;
		grid.pacmanLocation = grid.leftTeleporterLocation;
		grid.currentGrid[grid.leftTeleporterLocation] = grid.pacman;
	}
	else if (newLocation == grid.leftTeleporterLocation || newLocation == grid.rightTeleporterLocation) {		// �ڷ���Ʈ ��
		grid.currentGrid[oldLocation] = grid.nothing;		// �̵� �� ��ġ �׸��带 �� ��������
		grid.pacmanLocation = newLocation;					// �Ѹ��� ��ǥ�� �̵� �� ��ǥ��
		grid.currentGrid[newLocation] = grid.pacman;		// �̵� �� ��ġ�� �Ѹ����� �ٲ� 
	}
	else if (newCharacter == grid.white_dot) {		// ĳ���� �̵� �� ��ġ�� �Դ� ���� ��ġ���
		game.score++;				// ���� ����
		grid.dotsRemaining--;				// ���� �� ���� --
		grid.currentGrid[grid.pacmanLocation] = grid.nothing;		// �Ѹ� �̵�
		grid.pacmanLocation = newLocation;
		grid.currentGrid[newLocation] = grid.pacman;

		if (grid.dotsRemaining == 0) {	// ��� ���� �� �Ծ��ٸ�
			grid.Draw(game, pacman);		// �׷�����
			grid.Flash();					// ��¦ ��¦ ȿ��
			grid.Reset();					// ���� ����
			pacman.Reset();					// �Ѹ� ����
		}
	}
	else if (newCharacter == grid.yellow_dot) {		// ���� ��� �ߵ� �̺�Ʈ
		Beep((DWORD) 261.63 * 2, 200); // C
		Beep((DWORD) 329.63 * 2, 200); // E
		grid.dotsRemaining--;
		grid.currentGrid[grid.pacmanLocation] = grid.nothing;
		grid.pacmanLocation = newLocation;
		grid.currentGrid[newLocation] = grid.pacman;
		pacman.godMode = true;		// ������� Ȱ��ȭ
		pacman.godModeMovesLeft = pacman.godModeMaxMoves;		// ������� ���� �̵� = ������� �ִ� �̵���

		if (grid.dotsRemaining == 0) {		// ��� �� �� �Ծ��� ��(Yellow_dot)
			grid.Draw(game, pacman);
			grid.Flash();
			grid.Reset();
			pacman.Reset();
		}
	}
	else if (newCharacter == grid.nothing) {	// �� ������ �̵��� ���
		grid.currentGrid[oldLocation] = grid.nothing;
		grid.pacmanLocation = newLocation;
		grid.currentGrid[newLocation] = grid.pacman;
	}
}

void moveLeft(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, -1);			// ����
}
void moveRight(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, 1);			// ������
}
void moveUp(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, -23);		// ���� ����ŭ ����, �׸��� ������ ���� ��
}
void moveDown(Game& game, Grid& grid, Pacman& pacman) {
	movePacman(game, grid, pacman, 23);			// ���� ����ŭ ����, �׸��� ������ ���� ��
}

void themeMusic() {		// ���� �׸� ����, ���踦 �̿��ؼ� ����
	Beep((DWORD) 245.94, 200); // B
	Beep((DWORD) 245.94 * 2, 200); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 200); // Eb
	Beep((DWORD) 245.94 * 2, 100); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 400); // Eb
	Sleep(100);

	Beep((DWORD) 261.63, 200); // C 
	Beep((DWORD) 261.63 * 2, 200); // C
	Beep((DWORD) 391.00, 200); // G
	Beep((DWORD) 329.63, 200); // E
	Beep((DWORD) 261.63 * 2, 100); // C
	Beep((DWORD) 391.00, 200); // G
	Beep((DWORD) 329.63, 400); // E
	Sleep(100);

	Beep((DWORD) 245.94, 200); // B
	Beep((DWORD) 245.94 * 2, 200); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 200); // Eb
	Beep((DWORD) 245.94 * 2, 100); // B
	Beep((DWORD) 185.00 * 2, 200); // F#
	Beep((DWORD) 155.56 * 2, 400); // Eb
	Sleep(100);

	Beep((DWORD) 155.56 * 2, 100); // Eb
	Beep((DWORD) 329.63, 100); // E
	Beep((DWORD) 349.23, 200); // F

	Beep((DWORD) 349.23, 100); // F
	Beep((DWORD) 369.99, 100); // F#
	Beep((DWORD) 391.00, 200); // G

	Beep((DWORD) 391.00, 100); // G
	Beep((DWORD) 415.30, 100); // Ab
	Beep((DWORD) 440.00, 200); // A
	Beep((DWORD) 245.00 * 2, 400); //B
}

int main()
{
	// Center the console to the center of the screen
	HWND console = GetConsoleWindow();
	int consoleWidth = 800;		// ���� ȭ�� ũ��
	int consoleHeight = 600;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);	// ��ǻ�� ��ũ�� ȭ�� ũ��
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(console, (screenWidth - consoleWidth) / 2, (screenHeight - consoleHeight) / 2, consoleWidth, consoleHeight, TRUE);
	// �ܼ��� ȭ�� �߾����� ��ġ�ϴ� �۾�

	// Get stdout handle
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);		// Ŀ�� ��ġ ��ȯ

													// Hide cursor
	CONSOLE_CURSOR_INFO CURSOR;		// Ŀ�� ����� �ڵ�
	CURSOR.dwSize = 1;
	CURSOR.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &CURSOR);

	// Set up game
	Game game;
	Pacman pacman;
	Grid grid;

	// Main game loop
	while (!game.exit) {
		if (!game.themeMusicPlayed) {
			grid.Draw(game, pacman);		// �׸� ������ Ʋ�� �׸��带 �׸�.
			themeMusic();
			game.themeMusicPlayed = 1;
		}
		else {
			if (_kbhit()) {
				switch (_getch()) {		// Ű���� Ű�Է�
				case KB_ESCAPE:
					game.exit = true;
					break;
				case KB_LEFT:
					moveLeft(game, grid, pacman);
					break;
				case KB_RIGHT:
					moveRight(game, grid, pacman);
					break;
				case KB_UP:
					moveUp(game, grid, pacman);
					break;
				case KB_DOWN:
					moveDown(game, grid, pacman);
					break;
				}
			}

			grid.Draw(game, pacman);
		}
	}

	return 0;
}
