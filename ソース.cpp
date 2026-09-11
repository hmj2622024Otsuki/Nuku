#include "DxLib.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "Nuku.h" // ヘッダーファイルをインクルード

// 定数の宣言
const int WIDTH = 960, HEIGHT = 640; // ウィンドウの幅と高さのピクセル数
const int WHITE = GetColor(255, 255, 255); // 白
const int RED = GetColor(255, 0, 0); // 赤
const int BLACK = GetColor(0, 0, 0); // 黒
const int GRAY = GetColor(128, 128, 128); // グレー

enum { TITLE, PLAY, OVER }; // シーンを分けるための列挙定数

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

	// 変数の宣言
	int score = 0; // スコア
	int timer = 0; // 経過時間を数える変数
	int scrollSpeed = 24; //WaitTimer用

	int scene = TITLE; // 起動時に最初に遷移されるシーン

	// 画像の用意↓
	
	// モグラの画像
	int imgMole = LoadGraphWithCheck("image/mogura.png");

	// 背景画像
	int imgBG = LoadGraphWithCheck("image/bg.png");

	// 荷物の画像
	int imgBag[8] = {
		LoadGraphWithCheck("image/bag1.png"),
		LoadGraphWithCheck("image/bag2.png"),
		LoadGraphWithCheck("image/bag3.png"),
		LoadGraphWithCheck("image/bag4.png"),
		LoadGraphWithCheck("image/bag5.png"),
		LoadGraphWithCheck("image/bag6.png"),
		LoadGraphWithCheck("image/bag7.png"),
		LoadGraphWithCheck("image/scull_bag.png")
	};

	// ネコチャンの画像
	int imgNeko[4] = {
		LoadGraphWithCheck("image/neko1.png"),
		LoadGraphWithCheck("image/neko2.png"),
		LoadGraphWithCheck("image/neko3.png"),
		LoadGraphWithCheck("image/neko4.png")
	};

	// 泣いているネコチャンの画像
	int imgCryNeko = LoadGraphWithCheck("image/neko5.png");

	// バツマークの画像
	int imgX = LoadGraphWithCheck("image/X.png");

	// 効果音・BGMを読み込む
	int bgm = LoadSoundMemWithCheck("sound/bgm.mp3"); // BGM(仮)
	int decideSE = LoadSoundMemWithCheck("sound/decide.mp3"); // 決定時の効果音
	int backSE = LoadSoundMemWithCheck("sound/back.mp3"); // タイトルシーン遷移時の効果音

	// 荷物の座標用の変数
	int bagX = 980;
	int bagY = HEIGHT / 2 - 100;

	// 荷物の画像をランダムに表示するための変数
	int currentBag = imgBag[GetRand(8 - 1)];

	// ネコちゃんの座標用の変数
	int nekoX = WIDTH / 2 - 73;
	int nekoY = 40;

	// 乱数の初期化
	srand((unsigned int)time(NULL));

	// スペースキーが押されたことを確認するための仮変数
	int currentSpace = 0;

	// ゲームの骨組みとなる処理を、ここに記述
	while (1)
	{
		ClearDrawScreen(); // 画面をクリアする
		timer++;

		//// 軸を描く
		//DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY); // y軸
		//DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY); // x軸

		// 背景
		static int bgX;
		int spd = 12;
		bgX = (bgX - spd * 1) % WIDTH;
		DrawGraph(bgX + WIDTH, 0, imgBG, false); // 背景の表示
		DrawGraph(bgX, 0, imgBG, false); //

		// ネコチャンを表示
		DrawGraph(nekoX, nekoY, imgNeko[(timer / 6) % 4], true);

		// 当たり判定
		int x1 = bagX + 94, y1 = bagY + 100, r1 = 50, col1 = GetColor(0, 0, 255);
		int x2 = WIDTH / 2, y2 = HEIGHT / 2, r2 = 120;
		int d = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

		// シーンごとに処理を分岐
		switch (scene)
		{

		// タイトル画面の処理
		case TITLE:

			DrawTextC(WIDTH * 0.5, HEIGHT * 0.3 + 68, "CHECK-IN!", 0x00ffff, 105);

			if (timer % 50 < 25)
			{
				DrawTextC(WIDTH * 0.5, HEIGHT / 2 + 85, "スペースキーを押してスタート", 0xffffff, 42);
			}

			// スペースキーが押された時の処理
			if (CheckHitKey(KEY_INPUT_SPACE))
			{
				scene = PLAY;
				PlaySoundMem(decideSE, DX_PLAYTYPE_BACK);
			}
			break;

		// ゲームプレイ画面の処理
		case PLAY:

			// 荷物を動かす処理
			bagX = bagX - spd * 2;
			
			if (bagX < 0 - 200)
			{
				bagX = 980;

				if (currentBag == 69992458) // なにこれ
				{
					score -= 10;
				}
				else
				{
					score += 10;
				}
				currentBag = imgBag[GetRand(8 - 1)];
			}

			if (CheckHitKey(KEY_INPUT_SPACE) == 1 && d <= r1 + r2)
			{

			}
			else
			{
				DrawGraph(bagX, bagY, currentBag, true);
			}

			// 当たり判定を描画
			DrawCircle(x1, y1, r1, col1, true);

			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawCircle(x2, y2, r2, RED, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			// スコアを表示
			DrawTextB(68, 20, "スコア：%d", score, 0xffffff, 35);
			//DrawTextB(120, 50, "currentBag：%d", currentBag, 0xffffff, 35);

			// スペースキーが押されたことを確認するための仮テキスト
			DrawTextB(180, 60, "キーが押された：%d", currentSpace, 0xffffff, 20);

			// スペースキーが押されたことを確認するための仮コード
			if (CheckHitKey(KEY_INPUT_SPACE) == 1)
			{
				currentSpace = 1;
			}
			else
			{
				currentSpace = 0;
			}
		}

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(scrollSpeed); // 一定時間待つ 初期値は24
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}

// ここから下は自作した関数を記述するスペース↓

// 画像の読み込み。読み込み失敗時はエラーウィンドウを表示
int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);

	// エラーウィンドウの中に文字列を表示するためのchar型の変数
	char msg[256];

	// 変数指定によって変換された文字列を、指定文字数以下だけ出力する + ファイル名
	snprintf(msg, sizeof(msg), "画像ファイルが読み込めなかったよ\nこの名前のファイルが存在するか確認してね\n\n%s", file);

	if (res == -1) { MessageBox(GetMainWindowHandle(), msg, "動作記録", MB_OK | MB_ICONSTOP); }
	return res;
}

// 音声の読み込み。読み込み失敗時はエラーウィンドウを表示
int LoadSoundMemWithCheck(const char* file)
{
	int res = LoadSoundMem(file);

	// エラーウィンドウの中に文字列を表示するためのchar型の変数
	char msg[256];

	// 変数指定によって変換された文字列を、指定文字数以下だけ出力する + ファイル名
	snprintf(msg, sizeof(msg), "音声ファイルが読み込めなかったよ\nこの名前のファイルが存在するか確認してね\n\n%s", file);

	if (res == -1) { MessageBox(GetMainWindowHandle(), msg, "動作記録", MB_OK | MB_ICONSTOP); }
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
