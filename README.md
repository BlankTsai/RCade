# RCade

以 C++ 與 raylib 製作的四軌音樂遊戲，起源於大學課程期末專題，持續整理為研究所推甄作品集。

## 作者與素材

- 本專案為作者獨立製作的作品。
- 遊戲收錄音樂由作者自行製作。
- 圖片由 ChatGPT 生成。
- 各次變更及驗證結果會記錄於 Git 歷史與 CHANGELOG.md。
- 音效與其他素材的詳細來源見 ASSETS.md；本儲存庫尚未授予開源授權。

## 目前功能

- 開場、主選單、五首歌曲選擇、倒數與成績畫面。
- A、S、D、F 對應四條軌道。
- Perfect、Good、Miss 與錯誤按鍵計分。
- 依歌曲 BPM 產生音符，軌道目前為隨機配置。
- 成績畫面按 Enter 返回主選單。

## 建置與執行

已於 Windows 使用 MSYS2 MinGW64 GCC 14.2.0 與 raylib 5.5 重新編譯成功。執行 `./build.ps1` 建置，或用 `-Toolchain` 指定其他相容的工具鏈目錄。新版位於 `build/music_game.exe`，本機雙擊 `Play.cmd` 即可從正確的素材目錄啟動。原始執行檔仍保留於根目錄。

在專案根目錄執行原始編譯指令（需先安裝並設定相容的編譯器及 raylib）：

```sh
g++ src/*.cpp -o music_game -I include -lraylib -lopengl32 -lgdi32 -lwinmm
```

請從專案根目錄啟動遊戲，讓相對路徑 resources/ 能正確載入。Windows 動態連結建置亦需相容的 raylib 與編譯器執行階段 DLL。

執行檔、DLL、備份 ZIP 與個人編輯器設定不納入 Git；本機既有檔案仍保留。建置腳本會複製配套 DLL 到 build；可攜式發行包將另行整理。

## 程式結構

| 路徑 | 用途 |
| --- | --- |
| src/main.cpp | 主迴圈與場景分派 |
| src/Game.cpp | 遊戲生命週期與歌曲結束流程 |
| src/Block.cpp | 音符生成、移動及 Miss |
| src/Input.cpp | 按鍵、判定及計分 |
| src/Audio.cpp | 音樂串流 |
| src/Render.cpp | 畫面、選單及轉場 |
| src/SongDatabase.cpp | 歌曲資料與 BPM |
| include/ | 結構、介面與參數 |
| resources/ | 音樂、圖片及其他素材 |
| readme.pdf | 原始專案文件，保留作為歷史資料 |

## 優化進度

第一次優化已將預設日誌降為警告與錯誤，並加入可選的遊玩幀耗時量測。`--verbose` 可恢復原本日誌量，`--profile output.csv` 會在正常退出後保存資料。尚未完成實際遊玩 A/B 量測，改善幅度未定。輸入、判定與拍點邏輯尚未修改。

第一階段已獲作者核准：確認編譯環境、量測幀耗時並比較降低日誌輸出前後的結果。後續階段先提出具體方案，再由作者核准。

原始基準標籤：`baseline-original`。更新摘要見 [CHANGELOG.md](CHANGELOG.md)，驗證規劃見 [docs/OPTIMIZATION.md](docs/OPTIMIZATION.md)。
