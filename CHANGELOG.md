# 更新紀錄

## 2026-09-19 — 版本管理與文件

- 保留原始遊戲程式、素材與課程文件，建立 baseline-original 標籤。
- 排除執行檔、DLL、備份 ZIP 與本機編輯器設定，保留本機檔案。
- 新增專案介紹、素材來源與優化驗證計畫。
- 本次由 Codex 協助整理版本管理與文件；遊戲程式尚未修改。
- 驗證：原始 src/ 與 include/ 將與課程版本逐檔比對；尚未重新編譯或進行效能量測。

## 2026-09-22 — 第一次優化：日誌與量測

- README 移除 Codex 名稱，保留變更紀錄連結。
- 預設 LOG_WARNING，保留 --verbose 開啟原始 LOG_ALL 日誌量。
- --profile 收集正式遊玩幀（不含倒數及結束淡出）；計時包含幀率等待與繪圖提交，並非純 CPU 或 GPU 耗時。
- 資料先存記憶體，正常退出後才寫 CSV，列印平均、p95、p99、最大值及超過 25/50ms 的幀數；多首歌曲會合併，比較時請每次只玩一首。
- 新增 MSYS2 建置腳本及一般／安靜量測／詳細日誌量測啟動器。
- 驗證：GCC 14.2.0、raylib 5.5 建置成功；統計測試與錯誤參數處理通過。原有初始化及未使用變數警告仍存在。
- 尚未驗證：完整遊玩、音訊輸出、實際卡頓改善幅度。不得將編譯或單元測試通過視為效能改善證據。

## README review and countdown correction

- Shortened the English README into a project entry point and moved implementation details to docs/DESIGN.md.
- Corrected build/launch instructions, the flat resources layout, input versus automatic Miss, and profiling semantics.
- Explicitly distinguished verified build/statistics checks from the still-pending gameplay A/B experiment.
- Corrected countdown text from 4-3-2-GO to 3-2-1-GO without changing its four-second duration or music start timing.
- Validation: rebuilt with the existing MSYS2 toolchain; full visual/audio playthrough remains pending.

## 2026-09-23 — Gameplay screenshots and play-test feedback

- Added three unmodified author-provided screenshots to the README: main menu, song selection and gameplay.
- Stored screenshots under docs/screenshots with portable filenames and repository-relative image links.
- The author reported noticeably smoother gameplay after trying the updated version. This is qualitative feedback; no controlled A/B result or measured improvement percentage is available.
- Validation: screenshot copies match the supplied files by SHA256; README image paths resolve locally. No game code or executable changed.
