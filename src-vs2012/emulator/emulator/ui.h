#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_SELECT 2
#define BUTTON_START 3
#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_LEFT 6
#define BUTTON_RIGHT 7
#define BUTTON_COUNT 8

namespace ui
{
	// global functions
	void init();
	void deinit();
	void reset();

	void blt32(const uint32_t buffer[], const int width, const int height);

	bool hasInput(const int player);
	void resetInput();
	int readInput(const int player);
	int readInput(const int player, const int button);

	bool isForeground();

	bool forceTerminate();

	void limitFPS();

	// global event callbacks
	void onGameStart();
	void onGameEnd();
	
	void onFrameBegin();
	void onFrameEnd();

	void doEvents();
}