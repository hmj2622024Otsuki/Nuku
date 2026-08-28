#include "DxLib.h"

// 定数の宣言
const int WIDTH = 960, HEIGHT = 640; // ウィンドウの幅と高さのピクセル数
const int WHITE = GetColor(255, 255, 255); // 白
const int RED = GetColor(255, 0, 0); // 赤
const int BLACK = GetColor(0, 0, 0); // 黒

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance, // 現在のインスタンスのハンドル
	_In_opt_ HINSTANCE hPrevInstance, // 前のインスタンスのハンドル
	_In_ LPSTR lpCmdLine, // コマンドライン引数
	_In_ int nCmdShow // ウィンドウの表示状態
)

{
	SetWindowText("Nuku"); // ウィンドウのタイトル

	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 255, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	// 関数の宣言
	int timer = 0; // 経過時間を数える変数
	int scrollSpeed = 24;

	while(1)
	{
		ClearDrawScreen(); // 画面をクリアする
		timer++;

		int size = 200;
		SetFontSize(size);
		DrawString(WIDTH / 2 - size - 150, HEIGHT / 2 - size + 100, "バーカ！", RED);

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(scrollSpeed); // 一定時間待つ 初期値は24
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}

// ここから下は自作した関数を記述するスペース

// 画像の読み込み。読み込み失敗時はエラーウィンドウを表示
int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "log", MB_OK | MB_ICONSTOP); }
	return res;
}

// 音声の読み込み。読み込み失敗時はエラーウィンドウを表示
int LoadSoundMemWithCheck(const char* file)
{
	int res = LoadSoundMem(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "log", MB_OK | MB_ICONSTOP); }
	return res;
}

// 文字列と値をセンタリングして表示する関数
void DrawTextA(int x, int y, const char* txt, int val, int col, int siz)
{
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	x -= strWidth / 2;
	y -= siz / 2;
	SetFontSize(siz);
	DrawFormatString(x, y, col, txt, val);
}

// 影をつけた文字列と値をセンタリングして表示する関数
void DrawTextB(int x, int y, const char* txt, int val, int col, int siz)
{
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	x -= strWidth / 2;
	y -= siz / 2;
	SetFontSize(siz);
	DrawFormatString(x + 1, y + 1, 0x000000, txt, val);
	DrawFormatString(x, y, col, txt, val);
}

// 影をつけた文字列をセンタリングして表示する関数
void DrawTextC(int x, int y, const char* txt, int col, int siz)
{
	SetFontSize(siz);
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	x -= strWidth / 2;
	y -= siz / 2;
	DrawString(x + 1, y + 1, txt, 0x000000);
	DrawString(x, y, txt, col);
}
