#include "DxLib.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "Nuku.h" // ヘッダーファイルをインクルード

// 定数の宣言
const int WIDTH = 960, HEIGHT = 640;		// ウィンドウの幅と高さのピクセル数
const int WHITE = GetColor(255, 255, 255);	// 白
const int RED = GetColor(255, 0, 0);		// 赤
const int BLACK = GetColor(0, 0, 0);		// 黒
const int GRAY = GetColor(128, 128, 128);	// グレー
enum { TITLE, PLAY, OVER, MANUAL };			// シーンを分けるための列挙定数

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,			// 現在のインスタンスのハンドル
	_In_opt_ HINSTANCE hPrevInstance,	// 前のインスタンスのハンドル
	_In_ LPSTR lpCmdLine,				// コマンドライン引数
	_In_ int nCmdShow					// ウィンドウの表示状態
)

{
	SetWindowText("Nuku");				// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);				// ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1;	// ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 255, 0);		// 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK);		// 描画面を裏画面にする

	// 変数の宣言
	int score = 0;			// スコア
	int highScore = 12000;	// ハイスコア
	int timer = 0;			// 経過時間を数える変数
	int scrollSpeed = 16;	// WaitTimer用
	int scene = TITLE;		// 起動時に最初に遷移されるシーン

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

	// マニュアル画像
	int imgManual = LoadGraphWithCheck("image/manual.png");

	// 効果音・BGMを読み込む
	int bgm = LoadSoundMemWithCheck("sound/bgm.mp3");			// BGM(仮)
	int decideSE = LoadSoundMemWithCheck("sound/decide.mp3");	// 決定時の効果音
	int backSE = LoadSoundMemWithCheck("sound/back.mp3");		// タイトルシーン遷移時の効果音
	int manualSE = LoadSoundMemWithCheck("sound/manual.mp3");	// マニュアルシーン遷移時の効果音
	int pullSE = LoadSoundMemWithCheck("sound/pull2.mp3");		// よく使う効果音
	int expSE = LoadSoundMemWithCheck("sound/explosion.mp3");	// 爆発の効果音
	int overSE = LoadSoundMemWithCheck("sound/over.mp3");		// ゲームオーバー時の効果音
	//int pullSE = LoadSoundMemWithCheck("sound/pull.mp3");		//
	ChangeVolumeSoundMem(255, bgm);
	ChangeVolumeSoundMem(180, pullSE);

	// 荷物の座標用の変数
	int bagX = 980;
	int bagY = HEIGHT / 2 - 100;

	// 荷物の画像をランダムに表示するための変数
	int currentBag = imgBag[GetRand(8 - 1)];

	// ネコちゃんの座標用の変数
	int nekoX = WIDTH / 2 - 73;
	int nekoY = 40;

	// コンベアの速度(荷物もこの速度で動く)
	int spd = 5;

	// キャッチ判定の移動用
	int cx = WIDTH / 2;

	// 残機用の変数
	int life = 3;

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
		bgX = (bgX - spd * 1) % WIDTH;
		DrawGraph(bgX + WIDTH, 0, imgBG, false);	// 背景の表示
		DrawGraph(bgX, 0, imgBG, false);			//

		// ネコチャンを表示
		if (scene == OVER)
		{
			DrawGraph(nekoX, nekoY, imgCryNeko, true);
		}
		else
		{
			DrawGraph(nekoX, nekoY, imgNeko[(timer / 6) % 4], true);
		}

		// 荷物の当たり判定
		int x1 = bagX + 94, y1 = bagY + 100, r1 = 25;

		// キャッチ判定
		int x2 = cx, y2 = HEIGHT / 2, r2 = 80;

		// ↑上2つの接触判定
		int d = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

		// シーンごとに処理を分岐
		switch (scene)
		{
		// タイトル画面の処理
		case TITLE:

			// タイトルテキスト表示
			DrawTextC(WIDTH * 0.5, HEIGHT * 0.3 + 68, "CHECK-IN!", 0x00ffff, 105);

			if (timer % 50 < 25)
			{
				DrawTextC(WIDTH * 0.5, HEIGHT / 2 + 85, "スペースキーを押してスタート", 0xffffff, 42);
			}

			DrawTextC(WIDTH * 0.5 + 280, HEIGHT * 0.3 + 420, "Mキーでマニュアルを開く", 0xffffff, 30);

			// スペースキーが押された時の処理
			if (CheckHitKey(KEY_INPUT_SPACE) == 1)
			{
				scene = PLAY;
				cx = WIDTH / 2;
				PlaySoundMem(decideSE, DX_PLAYTYPE_BACK);
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP); // BGM再生開始(ループ)
				ChangeVolumeSoundMem(255, bgm);
			}
			else if (CheckHitKey(KEY_INPUT_M) == 1)
			{
				scene = MANUAL;
				PlaySoundMem(manualSE, DX_PLAYTYPE_BACK);
			}
			break;

		// ゲームプレイ画面の処理
		case PLAY:
			
			// 荷物を動かす処理
			bagX = bagX - spd * 2;
			
			if (bagX < 0 - 200)
			{
				bagX = 980;
				spd = GetRand(2) + 11;
				cx = GetRand(WIDTH / 2 - 300) + 400;

				// 荷物ごとのスコアの振り分け
				if (currentBag == 69992458) // なにこれ
				{
					score += 20;
					spd = GetRand(2) + 10;
					cx = GetRand(WIDTH / 2 - 250) + 400;
				}
				else
				{
					//score -= 10;
					life -= 1;
					spd = GetRand(2) + 10;
					cx = GetRand(WIDTH / 2 - 250) + 400;
					PlaySoundMem(expSE, DX_PLAYTYPE_BACK);
				}
				currentBag = imgBag[GetRand(8 - 1)];
			}

			// スペースキーが押されたかつ、キャッチ判定と荷物の当たり判定が触れた時の処理
			if (CheckHitKey(KEY_INPUT_SPACE) == 1 && d <= r1 + r2)
			{
				if (currentBag == 69992458)
				{
					bagX = 980;
					life -= 1;
					currentBag = imgBag[GetRand(8 - 1)];
					spd = GetRand(2) + 10;
					cx = GetRand(WIDTH / 2 - 250) + 400;
					PlaySoundMem(pullSE, DX_PLAYTYPE_BACK);
					PlaySoundMem(expSE, DX_PLAYTYPE_BACK);
				}
				else
				{
					bagX = 980;
					score += 20;
					currentBag = imgBag[GetRand(8 - 1)];
					spd = GetRand(2) + 10;
					cx = GetRand(WIDTH / 2 - 250) + 400;
					PlaySoundMem(pullSE, DX_PLAYTYPE_BACK);
				}
			}
			else
			{
				DrawGraph(bagX, bagY, currentBag, true);
			}

			// 当たり安定を描画
			SetDrawBlendMode(DX_BLENDMODE_ADD, 64);
			//DrawCircle(x1, y1, r1, RED, true);
			DrawCircle(x2, y2, r2, WHITE, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			// スコアを表示
			DrawTextB(100, 20, "スコア：%d", score, 0xffffff, 35);

			// ハイスコアの表示
			if (score > highScore)
			{
				highScore = score; // ハイスコアを超えた時の処理
				DrawTextB(747, 20, "ハイスコア：%d", highScore, 0xffff00, 35);
			}
			else
			{
				DrawTextB(747, 20, "ハイスコア：%d", highScore, 0xffffff, 35);
			}

			// 残機を表示
			if (life > 1)
			{
				DrawTextB(153, 600, "残りライフ：%d", life, 0xffffff, 33);
			}
			else
			{
				DrawTextB(153, 600, "残りライフ：%d", life, 0xff0000, 33);
			}

			if (life == 0)
			{
				scene = OVER;
				timer = 0;
				PlaySoundMem(overSE, DX_PLAYTYPE_BACK);
			}
			break;

		// ゲームオーバー画面の処理
		case(OVER):

			// 一時的に消音
			ChangeVolumeSoundMem(0, bgm);
			spd = 0;

			// ゲームオーバーテキスト表示
			DrawTextC(WIDTH * 0.5, HEIGHT * 0.3 + 60 , "ゲームオーバー", 0xff0000, 80);

			// 選択肢の表示
			DrawTextC(WIDTH * 0.5, HEIGHT / 2 + 170, "Tキーでタイトルに戻る", 0xffffff, 35);
			DrawTextC(WIDTH * 0.5, HEIGHT / 2 + 220, "Rキーでもう一度やり直す", 0xffffff, 35);

			// スコア別ランク付け
			if (score <= 100)
			{
				DrawTextB(WIDTH * 0.5 + 40, HEIGHT / 2 + 20, "スコア：%d　やる気あんの？", score, 0xffffff, 30);
			}
			else if (score <= 300)
			{
				DrawTextB(WIDTH * 0.5 + 20, HEIGHT / 2 + 20, "スコア：%d　新入社員級", score, 0xffffff, 30);
			}
			else if (score <= 800)
			{
				DrawTextB(WIDTH * 0.5 + 20, HEIGHT / 2 + 20, "スコア：%d　入社1年目級", score, 0xffffff, 30);
			}
			else if (score <= 1500)
			{
				DrawTextB(WIDTH * 0.5 + 30, HEIGHT / 2 + 20, "スコア：%d　カスミソウ級", score, 0xffffff, 30);
			}
			else if (score <= 3000)
			{
				DrawTextB(WIDTH * 0.5 + 30, HEIGHT / 2 + 20, "スコア：%d　クレマチス級", score, 0xffffff, 30);
			}
			else if (score <= 5000)
			{
				DrawTextB(WIDTH * 0.5 + 30, HEIGHT / 2 + 20, "スコア：%d　アマリリス級", score, 0xffffff, 30);
			}
			else if (score <= 10000)
			{
				DrawTextB(WIDTH * 0.5 + 30, HEIGHT / 2 + 20, "スコア：%d　テランセラ級", score, 0xffffff, 30);
			}
			else if (score >= 10000)
			{
				DrawTextB(WIDTH * 0.5 + 40, HEIGHT / 2 + 20, "スコア：%d　キバナコスモス級", score, 0xffffff, 30);
			}

			// Tキーが押されたらタイトルシーンへ遷移する
			if (CheckHitKey(KEY_INPUT_T) == 1)
			{
				// ここで値を初期化
				timer = 0;
				cx = WIDTH / 2;
				spd = 5;
				scene = TITLE;
				life = 3;
				score = 0;
				PlaySoundMem(backSE, DX_PLAYTYPE_BACK);
				StopSoundMem(bgm); //BGM再生停止
			}

			// Rキーが押されたら再びゲームシーンへ遷移する
			else if (CheckHitKey(KEY_INPUT_R) == 1)
			{
				// ここで値を初期化
				scene = PLAY;
				cx = WIDTH / 2;
				spd = 5;
				life = 3;
				score = 0;
				PlaySoundMem(decideSE, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(255, bgm);
			}
			break;

		// マニュアル画面の処理
		case(MANUAL):

			if (CheckHitKey(KEY_INPUT_T) == 1)
			{
				// ここで値を初期化
				timer = 0;
				cx = WIDTH / 2;
				spd = 5;
				scene = TITLE;
				life = 3;
				score = 0;
				PlaySoundMem(backSE, DX_PLAYTYPE_BACK);
			}
			else
			{
				DrawGraph(0, 0, imgManual, true);
				DrawTextC(WIDTH * 0.5 + 250, HEIGHT * 0.3 + 420, "Tキーでマニュアルを閉じるよ", 0xffffff, 30);
			}
		}

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(scrollSpeed); // 一定時間待つ 初期値は24
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1 && scene == TITLE) break; // ESCキーが押されたら終了
	}

	DxLib_End();	// DXライブラリ使用の終了処理
	return 0;		// ソフトの終了
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